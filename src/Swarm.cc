#include "Swarm.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "MathUtils.h"
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

void SwarmLab::ProcessIncoming()
{
  assert(mSerial);

  const int RX_BUFFER_SIZE = 256;
  uint8_t buffer[RX_BUFFER_SIZE];
  int readBytes = 0;

  readBytes = mSerial.Read(buffer, RX_BUFFER_SIZE);
  if (readBytes)
    cout << "Received from serial: " << buffer[0] << endl;
}

void SwarmLab::SendPosePacket()
{
  // Build the packet
  struct PoseInfoPacket_t {
    uint8_t Header;
    uint64_t Time; // In microseconds since epoch
    float Position[3];
    float Yaw;
    float Pitch;
    float Roll;
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
  Vector<3> v3YawPitchRoll = So3ToYawPitchRoll(se3WorldPose.get_rotation());

  auto timestamp = std::chrono::duration_cast<
      std::chrono::microseconds>(mtpPoseTime.time_since_epoch()).count();

  uot.packet.Header = 0xE5;
  uot.packet.Time = timestamp;
  uot.packet.Position[0] = v3Pos[0];
  uot.packet.Position[1] = v3Pos[1];
  uot.packet.Position[2] = v3Pos[2];
  uot.packet.Yaw = v3YawPitchRoll[0];
  uot.packet.Pitch = v3YawPitchRoll[1];
  uot.packet.Roll = v3YawPitchRoll[2];
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

void SwarmLab::UpdatePose(const TooN::SE3<> &se3Pose, bool bHasTracking,
                          const HiResTimePoint &tpTime)
{
  std::unique_lock<std::mutex> lock(mMutex);
  mse3CurrentPose = se3Pose;
  mbHasTracking = bHasTracking;
  mtpPoseTime = tpTime;
  mbPoseUpdated = true;
}

}
