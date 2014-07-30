#ifndef __SWARM_H
#define __SWARM_H

#include <TooN/TooN.h>
#include <TooN/se3.h>

#include <thread>
#include <chrono>
#include <mutex>

#include "Serial.h"

namespace PTAMM {

class SwarmLab {
  public:
    typedef std::chrono::high_resolution_clock::time_point HiResTimePoint;

    SwarmLab();
    void operator()();
    void StopThread() { mbDone = true; }
    void UpdatePose(const TooN::SE3<> &se3Pose,
		    int bHasTracking,
                    const HiResTimePoint &tpTime
		    );

    void SetCallback( const std::function<void(char c)>& _callback);
  private:
    void ProcessIncoming();
    void SendPosePacket();
    uint16_t Checksum(const uint8_t* data, size_t length) const;
  private:
    bool mbDone;
    std::mutex mMutex;

    Serial mSerial;

    TooN::SE3<> mse3CurrentPose;
    HiResTimePoint mtpPoseTime;
    int mbHasTracking;
    bool mbPoseUpdated;

    std::function<void(char c)> mFusionToPTAMCallback;

};

}

#endif
