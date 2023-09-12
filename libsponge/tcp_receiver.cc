#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    if(seg.header().syn){
        if(_syn_received){
            return false;
        }
        _syn_received = true;
        _isn = seg.header().seqno;
        if(seg.header().fin){
            _fin_received = true;
        }
        _reassembler.push_substring(seg.payload().copy(),0,_fin_received);
        return true;
    }
    if(_syn_received)
    {
        if(seg.header().fin){
            if(_fin_received)
                return false;
            _fin_received = true;
        }
        uint64_t checkpoint = _reassembler.ack_index();
        uint64_t abs_seqno = unwrap(seg.header().seqno,_isn,checkpoint);
        if(abs_seqno-1 >= _reassembler.ack_index() + window_size() || abs_seqno-1 + seg.length_in_sequence_space() <= checkpoint){
            return false;
        }
        _reassembler.push_substring(seg.payload().copy(),abs_seqno-1,_fin_received);
        return true;
    }
    return false;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_syn_received){
        return wrap(_reassembler.ack_index() + 1 + _reassembler.stream_out().input_ended(), _isn);
    }   
    else
        return std::nullopt;
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }