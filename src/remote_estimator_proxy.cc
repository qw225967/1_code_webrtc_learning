#include "../include/remote_estimator_proxy.h"
#include "../include/transport_feedback.h"
#include <iostream>
#include <memory>


namespace cls {

int64_t back_window_ms = 500;
int kMaxNumberOfPackets = 1 << 15;

// lower_bound: 第一个>=key的元素

// @param: seq -> transportSequenceNumber
void RemoteEstimatorProxy::IncomingPacket(int64_t arrival_time_ms, size_t payload_size, uint32_t media_ssrc, int64_t seq) {
  media_ssrc_ = media_ssrc;

  //packet_arrival_times_该map使用seq作为键值，包的到达时间作为实值。

  // 查找 大于 初始的序列号包的 第一个包 或者 本身的时间。并判断其是否是最后一个元素，保证容器里存在seq较小的包，以便清理
  if (periodic_window_start_seq_ != -1 &&
      packet_arrival_times_.lower_bound(periodic_window_start_seq_) == packet_arrival_times_.end())
  {
    // 淘汰时间过久的，seq小于当前收到的元素
    for (auto it = packet_arrival_times_.begin();
        it != packet_arrival_times_.end() && it->first < seq && arrival_time_ms - it->second >= back_window_ms; )  
        //判断容器里序列号都小于当前的序列号，而且时间都小于等于当前时间才移除
    {
      it = packet_arrival_times_.erase(it);
    }
  }

  // 保存最小的seq至periodic_window_start_seq_
  if (periodic_window_start_seq_ == -1 || seq < periodic_window_start_seq_) {
    periodic_window_start_seq_ = seq;
  }

  // We are only interested in the first time a packet is received.
  if (packet_arrival_times_.find(seq) != packet_arrival_times_.end())  //find 找不到元素时就回返回end（）此处为了防止收到重复的包
  return;

  packet_arrival_times_[seq] = arrival_time_ms;

  // 删除过期的元素
  // Limit the range of sequence numbers to send feedback for.
  //如果最后一个元素的序列号减去 最大包数限制 再进行lower_bound查找仍然大于 第一个序列号则证明存在多的包，则从头开始移除最大值的包。
  auto first_arrival_time_to_keep = packet_arrival_times_.lower_bound(
      packet_arrival_times_.rbegin()->first - kMaxNumberOfPackets);
  if (first_arrival_time_to_keep != packet_arrival_times_.begin()) {
    packet_arrival_times_.erase(packet_arrival_times_.begin(), first_arrival_time_to_keep);
    // if (send_periodic_feedback_) {
      // |packet_arrival_times_| cannot be empty since we just added one
      // element and the last element is not deleted.
      // RTC_DCHECK(!packet_arrival_times_.empty());
    
    //调整初始序列号记录
    // 保存最小的seq至periodic_window_start_seq_
    periodic_window_start_seq_ = packet_arrival_times_.begin()->first;
    // }
  }
      
      if(m_timer->sendfeedback == true){
        int Issendfeedback = (m_globalmtx)->mylock();
        if(Issendfeedback == 0){
          m_timer->sendfeedback = false;
          (m_globalmtx)->myunlock();
        }
        SendPeriodicFeedbacks();
      }
}

//void RemoteEstimatorProxy::SendPeriodicFeedbacks() {
  void RemoteEstimatorProxy::SendPeriodicFeedbacks() {
  // |periodic_window_start_seq_| is the first sequence number to include in the
  // current feedback packet. Some older may still be in the map, in case a
  // reordering happens and we need to retransmit them.
  if (periodic_window_start_seq_ == -1) //第一个序列号为-1就返回
    return;

  // 否则查找正好大于等于第一个序列号的第一个值（lower_bound（）函数返回值是一个
  //迭代器,返回指向大于等于key的第一个值的位置）
  //从大于或等于初始序列号开始查找，知道尾部结束
  for (auto begin_iterator = packet_arrival_times_.lower_bound(periodic_window_start_seq_);
      begin_iterator != packet_arrival_times_.cend();
      begin_iterator = packet_arrival_times_.lower_bound(periodic_window_start_seq_))
  {
    // auto feedback_packet = std::make_unique<rtcp::TransportFeedback>();
    //从头开始查，并从初始序列号开始建立回调包，回调包计数
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
    uint8_t* packet_temp = new uint8_t;
    size_t* position_temp ;
    *position_temp = 0;
    size_t max_length_temp = 1200;

    
    feedback_packet->Create(packet_temp,position_temp,max_length_temp);
    for(int i=0;packet_temp[i] != 0;i++)
      std::cout << packet_temp[i] ;

      std::cout << std::endl;
     
   
    delete packet_temp;
  }
  return;
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
