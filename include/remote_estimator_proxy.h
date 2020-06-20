/**
 * @file   remote_estimator_proxy.h
 * @author pengrl
 *
 */

#pragma once

#include "transport_feedback.h"
#include <stdint.h>
#include <map>
#include "../include/timer.h"

namespace cls {

class RemoteEstimatorProxy {
  public:
  RemoteEstimatorProxy(mymutex *globalmtx,timer ** globaltimer){ 
        m_globalmtx = globalmtx;
        m_timer = *globaltimer;
    }
  ~RemoteEstimatorProxy(){}
  public:
    void IncomingPacket(int64_t arrival_time_ms, size_t payload_size, uint32_t media_ssrc, int64_t seq);
  private:
    void SendPeriodicFeedbacks();
  	//BACKCALLFUNC SendPeriodicFeedbacks();
  	static int64_t BuildFeedbackPacket(
      uint8_t feedback_packet_count,
      uint32_t media_ssrc,
      int64_t base_sequence_number,
      std::map<int64_t, int64_t>::const_iterator
          begin_iterator,  // |begin_iterator| is inclusive.
      std::map<int64_t, int64_t>::const_iterator
          end_iterator,  // |end_iterator| is exclusive.
      cls::TransportFeedback* feedback_packet);
  private:
    uint32_t media_ssrc_ = 0;
    uint8_t feedback_packet_count_ = 0;
    int64_t periodic_window_start_seq_ = -1;
    std::map<int64_t, int64_t> packet_arrival_times_; // seq -> time
  public:
    mymutex *m_globalmtx;
    timer *m_timer;
};

}
