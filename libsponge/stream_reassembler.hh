#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <deque>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.

//******************************************************

// 在lab0中实现了一个由deque构成的有序字节流管道 一端读，一端写。。但我们接收到的是乱序，丢失，重复的一系列字串

// 要在lab1中把他们组装好，再write到字节流管道中去，让接收方可以收到可靠的信息

// _unass_base : 管道中期待获得的下一个字节的序号，例如【1，2，3，4】，则变量值为5，6,7,8等无法进入管道

// _unass_size : 缓存中存放的，还不能进入管道的字节数量

// _buffer : 用来缓存刚到来的字节流，如果每次缓存过后发现了_unass_base，则把从_unass_base开始连续的字节流写入管道（此时是有序的）

// _bitmap : 用来标记_buffer中每个位上是否有值

//******************************************************
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    uint64_t _unass_base;        //!< The index of the first unassembled byte
    size_t _unass_size;        //!< The number of bytes in the substrings stored but not yet reassembled
    bool _eof;                //!< The last byte has arrived
    std::deque<char> _buffer;  //!< The unassembled strings
    std::deque<bool> _bitmap;  //!< buffer bitmap

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes

    void check_contiguous();

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receives a substring and writes any newly contiguous bytes into the stream.
    //!
    //! If accepting all the data would overflow the `capacity` of this
    //! `StreamReassembler`, then only the part of the data that fits will be
    //! accepted. If the substring is only partially accepted, then the `eof`
    //! will be disregarded.
    //!
    //! \param data the string being added
    //! \param index the index of the first byte in `data`
    //! \param eof whether or not this segment ends with the end of the stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been submitted twice, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
