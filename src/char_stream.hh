#ifndef CHAR_STREAM_HH
#define CHAR_STREAM_HH

#include <istream>
#include <deque>
#include <string>

namespace kyaml
{
  class char_stream
  {
  public:
    typedef size_t mark_t;

    char_stream(std::istream &base) : 
      d_base(base),
      d_pos(0),
      d_mark_valid(false)
    {}

    // get the next character, or EOF
    int get();

    // peek the next character without modifying the stream
    int peek();

    void advance(size_t n = 1);

    // mark the current position in the stream
    mark_t mark() const;

    // unwind the stream, setting the read pos back to m. 
    void unwind(mark_t m);

    // eat all charactes to the current read pos. Returns all from m to the current read pos. Note
    // that this invalidates all marks previously returned by mark().
    std::string consume(mark_t m = 0);

    // mostly for diagnostic purposes
    mark_t pos() const
    {
      return d_pos;
    }

  private:
    bool underflow();

    std::istream    &d_base;
    std::deque<char> d_buffer;
    mark_t           d_pos;
    mutable bool     d_mark_valid; // only for additional run-time error checking
  };
}

#endif // CHAR_STREAM_HH
