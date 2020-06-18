/**
 * @file   remote_estimator_proxy.h
 * @author pengrl
 *
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <vector>

namespace cls {

class TransportFeedback {
public:
  class ReceivedPacket {
   public:
    ReceivedPacket(uint16_t sequence_number, int16_t delta_ticks)
        : sequence_number_(sequence_number),
          delta_ticks_(delta_ticks),
          received_(true) {}
    explicit ReceivedPacket(uint16_t sequence_number)
        : sequence_number_(sequence_number), received_(false) {}
    ReceivedPacket(const ReceivedPacket&) = default;
    ReceivedPacket& operator=(const ReceivedPacket&) = default;

    uint16_t sequence_number() const { return sequence_number_; }
    int16_t delta_ticks() const { return delta_ticks_; }
    int32_t delta_us() const { return delta_ticks_ * kDeltaScaleFactor; }
    // TimeDelta delta() const { return TimeDelta::Micros(delta_us()); }
    bool received() const { return received_; }

   private:
    uint16_t sequence_number_;
    int16_t delta_ticks_;
    bool received_;
  }; // TransportFeedback

  using DeltaSize = uint8_t;

  // Keeps DeltaSizes that can be encoded into single chunk if it is last chunk.
  class LastChunk {
   public:
    using DeltaSize = TransportFeedback::DeltaSize;

    LastChunk();

    bool Empty() const;
    void Clear();
    // Return if delta sizes still can be encoded into single chunk with added
    // |delta_size|.
    bool CanAdd(DeltaSize delta_size) const;
    // Add |delta_size|, assumes |CanAdd(delta_size)|,
    void Add(DeltaSize delta_size);

    // Encode chunk as large as possible removing encoded delta sizes.
    // Assume CanAdd() == false for some valid delta_size.
    uint16_t Emit();
    // Encode all stored delta_sizes into single chunk, pad with 0s if needed.
    uint16_t EncodeLast() const;

    // Decode up to |max_size| delta sizes from |chunk|.
    void Decode(uint16_t chunk, size_t max_size);
    // Appends content of the Lastchunk to |deltas|.
    void AppendTo(std::vector<DeltaSize>* deltas) const;

   private:
    static constexpr size_t kMaxRunLengthCapacity = 0x1fff;
    static constexpr size_t kMaxOneBitCapacity = 14;
    static constexpr size_t kMaxTwoBitCapacity = 7;
    static constexpr size_t kMaxVectorCapacity = kMaxOneBitCapacity;
    static constexpr DeltaSize kLarge = 2;

    uint16_t EncodeOneBit() const;
    void DecodeOneBit(uint16_t chunk, size_t max_size);

    uint16_t EncodeTwoBit(size_t size) const;
    void DecodeTwoBit(uint16_t chunk, size_t max_size);

    uint16_t EncodeRunLength() const;
    void DecodeRunLength(uint16_t chunk, size_t max_size);

    DeltaSize delta_sizes_[kMaxVectorCapacity];
    size_t size_;
    bool all_same_;
    bool has_large_delta_;
  }; // LastChunk

  TransportFeedback();

  // If |include_timestamps| is set to false, the created packet will not
  // contain the receive delta block.
  explicit TransportFeedback(bool include_timestamps,
                             bool include_lost = false);

  void SetMediaSsrc(uint32_t ssrc) { media_ssrc_ = ssrc; }
  void SetBase(uint16_t base_sequence,     // Seq# of first packet in this msg.
               	 int64_t ref_timestamp_us);  // Reference timestamp for this msg.
  void SetFeedbackSequenceNumber(uint8_t feedback_sequence);
  // NOTE: This method requires increasing sequence numbers (excepting wraps).
  bool AddReceivedPacket(uint16_t sequence_number, int64_t timestamp_us);

  // Creates packet in the given buffer at the given position.
  // Calls PacketReadyCallback::OnPacketReady if remaining buffer is too small
  // and assume buffer can be reused after OnPacketReady returns.
  bool Create(uint8_t* packet,
              size_t* index,
              size_t max_length) const;
              // PacketReadyCallback callback) const = 0;

private:
  // Get the reference time in microseconds, including any precision loss.
  int64_t GetBaseTimeUs() const;

  bool AddDeltaSize(DeltaSize delta_size);

  size_t BlockLength() const;
  size_t PaddingLength() const;

  // Size of the rtcp packet as written in header.
  size_t HeaderLength() const;

  static void CreateHeader(size_t count_or_format,
                           uint8_t packet_type,
                           size_t block_length,  // Payload size in 32bit words.
                           uint8_t* buffer,
                           size_t* pos);

  static void CreateHeader(size_t count_or_format,
                           uint8_t packet_type,
                           size_t block_length,  // Payload size in 32bit words.
                           bool padding,  // True if there are padding bytes.
                           uint8_t* buffer,
                           size_t* pos);

  void CreateCommonFeedback(uint8_t* payload) const;

  uint32_t media_ssrc() const { return media_ssrc_; }

  uint32_t sender_ssrc() const { return sender_ssrc_; }

public:
  static constexpr uint8_t kPacketType = 205;

  static constexpr size_t kCommonFeedbackLength = 8;

    // Size of the rtcp common header.
  static constexpr size_t kHeaderLength = 4;

  // TODO(sprang): IANA reg?
  static constexpr uint8_t kFeedbackMessageType = 15;
  // Convert to multiples of 0.25ms.
  static constexpr int kDeltaScaleFactor = 250;
  // Maximum number of packets (including missing) TransportFeedback can report.
  static constexpr size_t kMaxReportedPackets = 0xffff;

  uint32_t media_ssrc_ = 0;

  uint32_t sender_ssrc_ = 0;

  const bool include_lost_;
  uint16_t base_seq_no_;
  uint16_t num_seq_no_;
  int32_t base_time_ticks_;
  uint8_t feedback_seq_;
  bool include_timestamps_;

  int64_t last_timestamp_us_;
  std::vector<ReceivedPacket> received_packets_;
  std::vector<ReceivedPacket> all_packets_;
  // All but last encoded packet chunks.
  std::vector<uint16_t> encoded_chunks_;
  LastChunk last_chunk_;
  size_t size_bytes_;
};

} // namespace cls