#include "Swarm.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "MathUtils.h"
#include "Quaternion.h"
#include "Timing.h"

#include <gvars3/gvars3.h>

#include <iostream>

using namespace GVars3;
using namespace TooN;
using namespace std;

namespace PTAMM {

SwarmLab::SwarmLab()
  : mbDone(false)
  , mtpPoseTime()
  , mbHasTracking(false)
  , mbPoseUpdated(false)
{
  // Read COM port settings
  string comport = GV3::get<string>("SwarmLab.ComPort", "/dev/ttyUSB0", SILENT);
  int baudrate = GV3::get<int>("SwarmLab.ComPortBaudrate", "38400", SILENT);

  mSerial = Serial(comport, baudrate);

  Vector<4> v = makeVector(1, 0, 0, 0);
  // SO3<> test_SO3(v);
  // SE3<> test_SE3(test_SO3, v);
  Quaternion<> test(v);
  cout << "=== TEST AAA === "
    << test.get_array()[0]  << " "
    << test.get_array()[1]  << " "
    << test.get_array()[2]  << " "
    << test.get_array()[3]  << " "
    << endl;

  if (mSerial)
    cout << "Opened " << comport << " @ " << baudrate << " for SwarmLab" << endl;
}

void SwarmLab::operator()()
{
  RateLimiter rateLimiter;
  while (!mbDone && mSerial) {
    {
      ProcessIncoming();

      std::unique_lock<std::mutex> lock(mMutex);
      if (mbPoseUpdated) {
        SendPosePacket();
        mbPoseUpdated = false;
      }
    }

    rateLimiter.Limit(500.0); // Limit to 500 Hz
  }
}

  void SwarmLab::SetCallback( const std::function<void(char c)> &_callback) {
    mFusionToPTAMCallback = _callback;
  }

void SwarmLab::ProcessIncoming()
{
  assert(mSerial);

  const int RX_BUFFER_SIZE = 256;
  uint8_t buffer[RX_BUFFER_SIZE];
  int readBytes = 0;

  readBytes = mSerial.Read(buffer, RX_BUFFER_SIZE);
  if (readBytes) {
    cout << "Received from serial: " << buffer[0] << endl;
    mFusionToPTAMCallback(buffer[0]);
  }

}

void SwarmLab::SendPosePacket()
{
  // Build the packet
  struct PoseInfoPacket_t {
    uint8_t Header;
    uint64_t Time; // In microseconds since epoch
    float Position[3];
    float Rotation[4];  // quaternion
    uint8_t HasTracking;
    uint16_t Checksum;
  } __attribute__((packed));

  const uint8_t len = sizeof(PoseInfoPacket_t);
  union bus {
    PoseInfoPacket_t packet;
    uint8_t bytes[len];
  } uot;

  SE3<> se3WorldPose = mse3CurrentPose.inverse();
  Vector<3> v3Pos = se3WorldPose.get_translation();
  Quaternion<> qPose(se3WorldPose);

  auto timestamp = std::chrono::duration_cast<
      std::chrono::microseconds>(mtpPoseTime.time_since_epoch()).count();

  uot.packet.Header = 0xE5;
  uot.packet.Time = timestamp;
  uot.packet.Position[0] = v3Pos[0];
  uot.packet.Position[1] = v3Pos[1];
  uot.packet.Position[2] = v3Pos[2];
  uot.packet.Rotation[0] = qPose[0];
  uot.packet.Rotation[1] = qPose[1];
  uot.packet.Rotation[2] = qPose[2];
  uot.packet.Rotation[3] = qPose[3];
  uot.packet.HasTracking = (uint8_t)mbHasTracking;
  uot.packet.Checksum = 0;

  uot.packet.Checksum = Checksum(uot.bytes, sizeof(uot.packet));

  mSerial.SendBuffer(uot.bytes, sizeof(uot.packet));
}

uint16_t SwarmLab::Checksum(const uint8_t* data, size_t length) const
{
  uint16_t cs = 0;

  for (size_t i = 0; i < length; ++i) {
    cs += data[i];
  }

  return cs;
}

/*void SwarmLab::SendBuffer(uint8_t* data, int length)
{
  uint8_t count = 0;
  while (length > 0) {
    int sent = SendBuf(mnComPortId, data, length);
    length -= sent;
    data += sent;
    if (++count == 255) break;
  }
  }*/

    
void SwarmLab::UpdatePose(const TooN::SE3<> &se3Pose,
			  int bHasTracking,
                          const HiResTimePoint &tpTime
			  )
{
    std::unique_lock<std::mutex> lock(mMutex);
    mse3CurrentPose = se3Pose;
    mbHasTracking = (uint8_t)bHasTracking;
    mtpPoseTime = tpTime;
    mbPoseUpdated = true;
}

}
