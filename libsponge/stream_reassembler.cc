#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _unass_base(0)
    , _unass_size(0)
    , _eof(false)
    , _buffer(capacity, '\0')
    , _bitmap(capacity, false)
    , _output(capacity)
    , _capacity(capacity) {}


// 每次都会检查期待的字符是否出现，如果出现，把它以及后面连续的字节都写进管道
void StreamReassembler::check_contiguous() {
    string tmp = "";
    while (_bitmap.front()) {
        // cout<<"check one more contiguous substring"<<endl;
        tmp += _buffer.front();
        _buffer.pop_front();
        _bitmap.pop_front();

        //保持缓冲区始终为capacity，因为不仅是两头插数据，还会根据下标来索引
        _buffer.push_back('\0');
        _bitmap.push_back(false);
    }
    if (tmp.length() > 0) {
        // cout << "push one contiguous substring with length " << tmp.length() << endl;
        _output.write(tmp);
        _unass_base += tmp.length();
        _unass_size -= tmp.length();
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // ignore invalid index
    //到来的子串序号过大，任何一个字节都存放不下，直接丢弃
    if (index >= _unass_base + _capacity) return;

    size_t len = data.length();

    // 对应于一开始就是空串的边界情况。
    if (len == 0 && eof && _unass_size == 0) {
        _eof = true;
        _output.end_input();
        return;
    }

    // 表示这是最后一个字符串，但也有可能是提前到达的最后一个子串。
    if (eof) {
        _eof = true;
    }

    //如果序号合理并且 大于等于期待的 
    if (index >= _unass_base) {
        //计算序号比期待的大多少
        int offset = index - _unass_base;
        //只能放得下这个串中，比管道容量-距离，小的那部分
        size_t real_len = min(len, _output.remaining_capacity() - offset);
        //如果串长度较大，超过了管道的可用空间说明除了期待的字节没到以外，后面还有更多
        if (real_len < len) {
            _eof = false;  //终止到的太早了，后面截取的数据没保留下来，只有_eof被改掉了，现在发现后面应该还有，所以改回来
        }
        //空offset个位置，把字节装进缓冲区
        for (size_t i = 0; i < real_len; i++) {
            if (_bitmap[i + offset])
                continue;
            _buffer[i + offset] = data[i];
            _bitmap[i + offset] = true;
            _unass_size++;
        }
    } else if (index + len > _unass_base) {  //如果序号合理并且 小于期待的，但是串中包含一部分老的和一部分新的，把新的拿到缓存区
        //offset表示新字节在串中的下标
        int offset = _unass_base - index;
        //同理
        size_t real_len = min(len - offset, _output.remaining_capacity());
        if (real_len < len - offset) {
            _eof = false;
        }
        //缓存区大小和管道相同
        for (size_t i = 0; i < real_len; i++) {
            if (_bitmap[i])
                continue;
            _buffer[i] = data[i + offset];
            _bitmap[i] = true;
            _unass_size++;
        }
    }
    check_contiguous();
    //写管道后，正常判断是否终止的入口
    if (_eof && _unass_size == 0) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unass_size; }

bool StreamReassembler::empty() const { return _unass_size == 0; }

uint64_t StreamReassembler::ack_index() const {return _unass_base;}