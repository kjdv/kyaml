#ifndef PEEKABLE_STREAM_HH
#define PEEKABLE_STREAM_HH

#include <iostream>
#include <deque>
#include <string>

// a buffered and peekable stream
namespace kyaml
{
  class peekable_stream
  {
  public:
    peekable_stream(std::istream &stream) : d_stream(stream)
    {}

    int getc();

    std::string peek(size_t n);
    
    std::string read(size_t n);

    void ignore(size_t n);

    bool eof() const
    {
      return d_buffer.empty() && d_stream.eof();
    }

    bool good() const
    {
      return !d_buffer.empty() || d_stream.good();
    }
    
  private:
    std::istream &d_stream;
    std::deque<char> d_buffer;
  };
}

#endif // PEEKABLE_STREAM_HH
