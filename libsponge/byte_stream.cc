#include "byte_stream.hh"
#include <algorithm>
// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

ByteStream::ByteStream(const size_t capacity):_queue(),_capacity(capacity),_end_write(false),_end_read(false),_written_bytes(0),_read_bytes(0) {}

size_t ByteStream::write(const std::string &data) {
    size_t canWrite = _capacity - _queue.size();
    size_t realWrite = std::min(canWrite, data.size());
    for (size_t i = 0; i < realWrite; i++) {
        _queue.push_back(data[i]);
    }
    _written_bytes += realWrite;
    return realWrite;
}

//! \param[in] len bytes will be copied from the output side of the buffer
std::string ByteStream::peek_output(const size_t len) const {
    size_t canPeek = std::min(len, _queue.size());
    std::string out = "";
    for (size_t i = 0; i < canPeek; i++) {
        out += _queue[i];
    }
    return out;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > _queue.size()) {
        set_error();
        return;
    }
    for (size_t i = 0; i < len; i++) {
        _queue.pop_front();
    }
    _read_bytes += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string out = "";
    if (len > _queue.size()) {
        set_error();
        return out;
    }
    for (size_t i = 0; i < len; i++) {
        out += _queue.front();
        _queue.pop_front();
    }
    _read_bytes += len;
    return out;
}

void ByteStream::end_input() {_end_write = true;}

bool ByteStream::input_ended() const { return _end_write; }

size_t ByteStream::buffer_size() const { return _queue.size(); }

bool ByteStream::buffer_empty() const { return _queue.empty(); }

bool ByteStream::eof() const { return _queue.empty() && _end_write; }

size_t ByteStream::bytes_written() const { return _written_bytes; }

size_t ByteStream::bytes_read() const { return _read_bytes; }

size_t ByteStream::remaining_capacity() const { return _capacity - _queue.size(); }
