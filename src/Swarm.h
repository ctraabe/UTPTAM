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
    void UpdateTracking(const TooN::SE3<> &se3BodyToWorld,
      const int bHasTracking, const HiResTimePoint &tpTime);

    void SetCallback( const std::function<void(char c)>& _callback);

  private:
    void ProcessIncoming();
    void SendTrackingPacket() const;
    uint16_t Checksum(const uint8_t* data, size_t length) const;

    bool mbDone;
    std::mutex mMutex;

    Serial mSerial;

    TooN::SE3<> mse3BodyToWorld;
    HiResTimePoint mtpTrackingTime;
    int mnTrackingStatus;
    bool mbTrackingUpdated;

    std::function<void(char c)> mFusionToPTAMCallback;

};

}

#endif
