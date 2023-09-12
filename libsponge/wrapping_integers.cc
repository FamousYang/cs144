#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn + uint32_t(n);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t tmp_up = 0;
    uint64_t tmp_down = 0;
    if(n - isn >= 0){
        tmp_up =  uint64_t(n - isn);
    }else{
        tmp_up =  uint64_t(n - isn + (1L << 32));
    }
    //检查点就在32位之内
    if (tmp_up >= checkpoint)
        return tmp_up;
    //检查点高于当前值，可能在32位以内，也可能是2^32的若干倍，把它们放在同一个32位内进行比较
    tmp_up |= ((checkpoint >> 32) << 32);  //获得高32位
    while (tmp_up < checkpoint)
    {
        tmp_up += (1L << 32);
    }
    tmp_down = tmp_up - (1L << 32);
    return (tmp_up - checkpoint) < (checkpoint - tmp_down) ? tmp_up : tmp_down;
}
