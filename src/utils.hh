#ifndef KYAML_UTILS_HH
#define KYAML_UTILS_HH

#include <string>
#include <iostream>
#include <stdexcept>

namespace kyaml
{
  class no_copy
  {
  protected:
    no_copy()
    {}
  private:
    no_copy(no_copy const &);
    no_copy &operator=(no_copy const &);
  };

  class invalid_utf8 : public std::exception
  {
  public:
    invalid_utf8(std::string const &msg, std::string const &sequence) :
      d_msg(construct(msg, sequence))
    {}

    char const *what() const throw() override
    {
      return d_msg.c_str();
    }
  private:
    static std::string construct(std::string const &msg, std::string const &sequence);
    std::string d_msg;
  };

  // return the number of utf8 bytes based on the first byte c (including c)
  size_t nr_utf8bytes(uint8_t c);

  inline bool is_lead_byte(uint8_t c)
  {
    return (c & 0xc0) == 0xc0;
  }

  inline bool is_continuation_byte(uint8_t c)
  {
    return (c & 0xc0) == 0x80;
  }

  bool extract_utf8(std::istream &stream, char32_t &result);
  bool extract_utf8(std::string const &str, char32_t &result);
 
  void append_utf8(std::string &str, char32_t ch);
  // just a small helper to support uniform appending
  inline void append_utf8(std::string &str, std::string const &s)
  {
    str.append(s);
  }

  // for debugging purposes: a (compile-time-switchable) logger for what is parsed
  template <bool enabled = false>
  class logger
  {
  public:
    logger(std::ostream &out = std::cerr)
    {}

    // does nothing, compiler should optimize out everything
    template<typename... args_t>
    void operator()(std::string const &, args_t...)
    {}
  };

  // specialided for enabled
  template<>
  class logger<true>
  {
  public:
    logger(std::ostream &out = std::cerr) :
      d_out(out)
    {}

    template<typename... args_t>
    void operator()(std::string const &tag, args_t... args)
    {
      d_out << "(" << tag << "):";
      log_recurse(args...);
    }
    
  private:
    template<typename first_t, typename... args_t>
    void log_recurse(first_t const &head, args_t... tail)
    {
      d_out << ' ' << head;
      log_recurse(tail...);
    }    
    void log_recurse()
    {
      d_out << '\n';
    }

    std::ostream &d_out;
  };
}

#endif // KYAML_UTILS_HH
