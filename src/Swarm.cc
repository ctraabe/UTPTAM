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
  , mnTrackingStatus(0)
  , mbTrackingUpdated(false)
{
  // Read COM port settings
  string comport = GV3::get<string>("SwarmLab.ComPort", "/dev/ttyUSB0", SILENT);
  int baudrate = GV3::get<int>("SwarmLab.ComPortBaudrate", "38400", SILENT);

  mSerial = Serial(comport, baudrate);

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
      if (mbTrackingUpdated) {
        SendTrackingPacket();
        mbTrackingUpdated = false;
      }
    }

    rateLimiter.Limit(50.0); // Limit to 50 Hz
  }
}

void SwarmLab::SetCallback(const std::function<void(char c)> &_callback)
{
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

void SwarmLab::SendTrackingPacket() const
{
  // Build the packet
  struct TrackingInfoPacket_t {
    uint8_t Header;
    uint64_t Time; // In microseconds since epoch
    float Position[3];
    float Rotation[4];  // quaternion
    uint8_t TrackStatus;
    uint16_t Checksum;
  } __attribute__((packed));

  const uint8_t len = sizeof(TrackingInfoPacket_t);
  union bus {
    TrackingInfoPacket_t packet;
    uint8_t bytes[len];
  } uot;

  auto timestamp = std::chrono::duration_cast<
      std::chrono::microseconds>(mtpTrackingTime.time_since_epoch()).count();

  const Quaternion<> qBodyToWorld(mse3BodyToWorld);

  uot.packet.Header = 0xE5;
  uot.packet.Time = timestamp;
  uot.packet.Position[0] = (float)mse3BodyToWorld.get_translation()[0];
  uot.packet.Position[1] = (float)mse3BodyToWorld.get_translation()[1];
  uot.packet.Position[2] = (float)mse3BodyToWorld.get_translation()[2];
  uot.packet.Rotation[0] = (float)qBodyToWorld[0];
  uot.packet.Rotation[1] = (float)qBodyToWorld[1];
  uot.packet.Rotation[2] = (float)qBodyToWorld[2];
  uot.packet.Rotation[3] = (float)qBodyToWorld[3];
  uot.packet.TrackStatus = (uint8_t)mnTrackingStatus;
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

void SwarmLab::UpdateTracking(const TooN::SE3<> &se3BodyToWorld,
  const int nTrackingStatus, const HiResTimePoint &tpTime)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mse3BodyToWorld = se3BodyToWorld;
    mnTrackingStatus = (uint8_t)nTrackingStatus;
    mtpTrackingTime = tpTime;
    mbTrackingUpdated = true;
}

}
