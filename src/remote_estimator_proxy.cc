#include "remote_estimator_proxy.h"
#include "transport_feedback.h"
#include <memory>

namespace cls {

int64_t back_window_ms = 500;
int kMaxNumberOfPackets = 1 << 15;

// lower_bound: 第一个>=key的元素

// @param: seq -> transportSequenceNumber
void RemoteEstimatorProxy::IncomingPacket(int64_t arrival_time_ms, size_t payload_size, uint32_t media_ssrc, int64_t seq) {
  media_ssrc_ = media_ssrc;

  // 如果所有元素的seq都 < periodic_window_start_seq_
  if (periodic_window_start_seq_ != -1 &&
      packet_arrival_times_.lower_bound(periodic_window_start_seq_) == packet_arrival_times_.end())
  {
    // 淘汰时间过久的，seq小于当前收到的元素
    for (auto it = packet_arrival_times_.begin();
        it != packet_arrival_times_.end() && it->first < seq && arrival_time_ms - it->second >= back_window_ms; )
    {
      it = packet_arrival_times_.erase(it);
    }
  }

  // 保存最小的seq至periodic_window_start_seq_
  if (periodic_window_start_seq_ == -1 || seq < periodic_window_start_seq_) {
    periodic_window_start_seq_ = seq;
  }

  // We are only interested in the first time a packet is received.
  if (packet_arrival_times_.find(seq) != packet_arrival_times_.end())
  return;

  packet_arrival_times_[seq] = arrival_time_ms;

  // 删除过期的元素
  // Limit the range of sequence numbers to send feedback for.
  auto first_arrival_time_to_keep = packet_arrival_times_.lower_bound(
      packet_arrival_times_.rbegin()->first - kMaxNumberOfPackets);
  if (first_arrival_time_to_keep != packet_arrival_times_.begin()) {
    packet_arrival_times_.erase(packet_arrival_times_.begin(), first_arrival_time_to_keep);
    // if (send_periodic_feedback_) {
      // |packet_arrival_times_| cannot be empty since we just added one
      // element and the last element is not deleted.
      // RTC_DCHECK(!packet_arrival_times_.empty());
    
    // 保存最小的seq至periodic_window_start_seq_
    periodic_window_start_seq_ = packet_arrival_times_.begin()->first;
    // }
  }
}

void RemoteEstimatorProxy::SendPeriodicFeedbacks() {
  // |periodic_window_start_seq_| is the first sequence number to include in the
  // current feedback packet. Some older may still be in the map, in case a
  // reordering happens and we need to retransmit them.
  if (periodic_window_start_seq_ == -1)
    return;

  for (auto begin_iterator = packet_arrival_times_.lower_bound(periodic_window_start_seq_);
      begin_iterator != packet_arrival_times_.cend();
      begin_iterator = packet_arrival_times_.lower_bound(periodic_window_start_seq_))
  {
    // auto feedback_packet = std::make_unique<rtcp::TransportFeedback>();
    auto feedback_packet = std::make_shared<cls::TransportFeedback>();
    periodic_window_start_seq_ = BuildFeedbackPacket(
        feedback_packet_count_++, media_ssrc_, periodic_window_start_seq_,
        begin_iterator, packet_arrival_times_.cend(), feedback_packet.get());

    // RTC_DCHECK(feedback_sender_ != nullptr);

    // std::vector<std::unique_ptr<rtcp::RtcpPacket>> packets;
    // if (remote_estimate) {
    //   packets.push_back(std::move(remote_estimate));
    // }
    // packets.push_back(std::move(feedback_packet));

    // TODO chef: 这里实际发送
    // feedback_sender_->SendCombinedRtcpPacket(std::move(packets));
    // Note: Don't erase items from packet_arrival_times_ after sending, in case
    // they need to be re-sent after a reordering. Removal will be handled
    // by OnPacketArrival once packets are too old.
  }
}

int64_t RemoteEstimatorProxy::BuildFeedbackPacket(
    uint8_t feedback_packet_count,
    uint32_t media_ssrc,
    int64_t base_sequence_number,
    std::map<int64_t, int64_t>::const_iterator begin_iterator,
    std::map<int64_t, int64_t>::const_iterator end_iterator,
    cls::TransportFeedback* feedback_packet) {
  // RTC_DCHECK(begin_iterator != end_iterator);

  // TODO(sprang): Measure receive times in microseconds and remove the
  // conversions below.
  feedback_packet->SetMediaSsrc(media_ssrc);
  // Base sequence number is the expected first sequence number. This is known,
  // but we might not have actually received it, so the base time shall be the
  // time of the first received packet in the feedback.
  feedback_packet->SetBase(static_cast<uint16_t>(base_sequence_number & 0xFFFF),
                           begin_iterator->second * 1000);
  feedback_packet->SetFeedbackSequenceNumber(feedback_packet_count);
  int64_t next_sequence_number = base_sequence_number;
  for (auto it = begin_iterator; it != end_iterator; ++it) {
    if (!feedback_packet->AddReceivedPacket(
            static_cast<uint16_t>(it->first & 0xFFFF), it->second * 1000)) {
    //   // If we can't even add the first seq to the feedback packet, we won't be
    //   // able to build it at all.
    //   // RTC_CHECK(begin_iterator != it);

    //   // Could not add timestamp, feedback packet might be full. Return and
    //   // try again with a fresh packet.
      break;
    }
    next_sequence_number = it->first + 1;
  }
  return next_sequence_number;
}

} // namespace cls