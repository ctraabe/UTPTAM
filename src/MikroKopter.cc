#include "MikroKopter.h"
#include "PerformanceMonitor.h"
#include "Tracker.h"

#include <gvars3/instances.h>

#include <functional>
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::placeholders;
using namespace GVars3;

namespace PTAMM {

MikroKopter::MikroKopter(const Tracker* pTracker, PerformanceMonitor *pPerfMon)
  : mbDone(false)
  , mpTracker(pTracker)
  , mpPerfMon(pPerfMon)
  , mnTrackingStatus(0)
  , mbUpdateReady(false)
  , mbLogMKControl(false)
  , mbLogMKData(false)
  , mbLogMKDebug(false)
  , mMKDebugRequestTimeout(2.0)
  , mMKData()  // zeros the structure
  , mMKDebug()  // zeros the structure
  , mMKToPTAM()  // zeros the structure
{
  // Read COM port settings
  string comport = GV3::get<string>("MKFlightCtrl.ComPort", "/dev/ttyUSB1",
    SILENT);
  int baudrate = GV3::get<int>("MKFlightCtrl.ComPortBaudrate", "57600", SILENT);
  ConnectToFC(comport, baudrate);

  // Log the control data
  mbLogMKControl = GV3::get<int>("Log.MKControl", 0, SILENT);
  if (mbLogMKControl) {
    mMKControlLogFile.open("mk_control.log", ios::out | ios::trunc);
    if (!mMKControlLogFile) {
      cerr << "Failed to open mk_control.log" << endl;
    }
  }

  // Log the data stream from the FlightCtrl
  mbLogMKData = GV3::get<int>("Log.MKData", 0, SILENT);
  if (mbLogMKData) {
    mMKDataLogFile.open("mk_data.log", ios::out | ios::trunc);
    if (!mMKDataLogFile) {
      cerr << "Failed to open mk_data.log" << endl;
    }
  }

  // Log the debug output stream from the FlightCtrl
  mbLogMKDebug = GV3::get<int>("Log.MKDebug", 0, SILENT);
  if (mbLogMKDebug) {
    mMKDebugLogFile.open("mk_debug.log", ios::out | ios::trunc);
    if (!mMKDebugLogFile) {
      cerr << "Failed to open mk_debug.log" << endl;
    }
  }

  mStartTime = Clock::now();
}

void MikroKopter::operator()()
{
  RateLimiter rateLimiter;
  static int use = GV3::get<int>("MKFlightCtrl.Active", "0", SILENT);

  while (!mbDone) {
    if (mFCConn && use) {
      mFCConn.ProcessIncoming();

      // Don't try to send two commands on the same frame
      if (mbLogMKDebug && mMKDebugRequestTimeout.HasTimedOut()) {
        // Request debug data being sent from the MK, this has to be done every
        // few seconds or the MK will stop sending the data
        mFCConn.RequestMKDebugInterval(10);  // Request data every 100ms (10 Hz)
        mMKDebugRequestTimeout.Reset();
      } else if (mbUpdateReady) {
        std::unique_lock<std::mutex> lock(mMutex);

        mFCConn.SendPTAMToMK(mTargetController.GetControl(),
            mTargetController.GetEulerAngles(), mTargetController.GetConfig());

        mbUpdateReady = false;
      }
    }

    // Lock rate to 256 Hz
    rateLimiter.Limit(256.0);

    mpPerfMon->UpdateRateCounter("mk");
  }
}

void MikroKopter::UpdatePose(const TooN::SE3<> &se3Pose,
  const int nTrackingStatus)
{
  static int use = GV3::get<int>("MKFlightCtrl.Active", "0", SILENT);
  if (use) {
    std::unique_lock<std::mutex> lock(mMutex);
    mnTrackingStatus = nTrackingStatus;
    mTargetController.Update(se3Pose, nTrackingStatus, TargetController::Clock::now());
    mbUpdateReady = true;
    if (mbLogMKControl) LogMKControl();
  }
}

void MikroKopter::GoToLocation(TooN::Vector<2> v2LocInWorld)
{
  std::unique_lock<std::mutex> lock(mMutex);
  cout << "Go to location: " << v2LocInWorld << endl;
  mTargetController.SetTargetLocation(v2LocInWorld);
  mFCConn.SendNewTargetNotice();
}

void MikroKopter::SetTargetAltitude(double altitude)
{
  std::unique_lock<std::mutex> lock(mMutex);
  mTargetController.SetTargetAltitude(altitude);
}

void MikroKopter::ConnectToFC(const string &comport, const int baudrate)
{
  // Attempt connecting to the MK FlightCtrl
  mFCConn = MKConnection(comport, baudrate);
  if (!mFCConn) {
    cerr << "Failed to connect to MikroKopter FlightCtrl." << endl;
  } else {
    cout << "Opened " << comport << " @ " << baudrate << " for MikroKopter" << endl;
    // Hook up all the callback functions
    mFCConn.SetPositionHoldCallback(std::bind(&MikroKopter::RecvPositionHold, this));
    mFCConn.SetMKToPTAMCallback(std::bind(&MikroKopter::RecvMKToPTAM, this, _1));
    mFCConn.SetMKDebugCallback(std::bind(&MikroKopter::RecvMKDebug, this, _1));
  }
}

void MikroKopter::RecvPositionHold()
{
  cout << " >> Position hold requested." << endl;
  mTargetController.HoldCurrentLocation();
}

void MikroKopter::RecvMKToPTAM(const MKToPTAM_t& mkToPTAM)
{
  mTargetController.SetTargetAltitude((float)mkToPTAM.altitude * 0.01 + 0.45);
  mTargetController.RequestConfig(mkToPTAM.request);
  mMKToPTAM = mkToPTAM;
  if (mbLogMKData) LogMKData();
}

void MikroKopter::RecvMKDebug(const MKDebug_t& mkDebug)
{
  mMKDebug = mkDebug;
}

void MikroKopter::LogMKControl()
{
  mMKControlLogFile
      << std::chrono::duration_cast<RealSeconds>(mTargetController.GetTime() - mStartTime).count()
      << " " << mTargetController.GetPosInWorld()
      << mTargetController.GetTarget()
      << mTargetController.GetTargetOffset()
      << mTargetController.GetEulerAngles()
      << mTargetController.GetVelocity()
      << (int)mTargetController.GetConfig()
      << " " << (int)mTargetController.GetExperimentMode();
  for (size_t i = 0; i < 5; ++i) {
    mMKControlLogFile << " " << mTargetController.GetControl()[i];
  }
  mMKControlLogFile << endl;
}

void MikroKopter::LogMKData()
{
  mMKDataLogFile
      << std::chrono::duration_cast<RealSeconds>(Clock::now() - mStartTime).count()
      << " " << (int16_t)mMKToPTAM.count
      << " " << (int16_t)mMKToPTAM.request
      << " " << ((float)mMKToPTAM.altitude * 0.01);

  for (size_t i = 0; i < 6; ++i) {
    mMKDataLogFile << " " << mMKToPTAM.int16[i];
  }
  for (size_t i = 0; i < 3; ++i) {
    mMKDataLogFile << " " << mMKToPTAM.single[i];
  }
  mMKDataLogFile << endl;
}

void MikroKopter::LogMKDebug()
{
  for (size_t i = 0; i < 32; ++i) {
    mMKDebugLogFile << " " << mMKDebug.int16[i];
  }

  mMKDebugLogFile << endl;
}

}
