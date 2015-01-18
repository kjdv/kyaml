#ifndef KYAML_HH
#define KYAML_HH

#include <memory>
#include <istream>
#include "node.hh"

namespace kyaml
{
  class parser_impl;
  class parser
  {   
  public:
    class error : public std::runtime_error
    {
    protected: // not to be used directly
      error(unsigned linenumber, std::string const &msg);
    public:
      unsigned linenumber() const
      {
        return d_linenumber;
      }

    private:
      unsigned d_linenumber;
    };

    class parse_error : public kyaml::parser::error
    {
    public:
      parse_error(unsigned linenumber, std::string const &msg = "");
    };

    class content_error : public kyaml::parser::error
    {
    public:
      content_error(unsigned linenumber, std::string const &msg = "");
    };

    parser(std::istream &input);
    ~parser();

    std::unique_ptr<const document> parse(); // may throw

    // intended for testing/debugging/error reporting, returns the next n characters of the stream
    std::string peek(size_t n) const;

    unsigned linenumber() const;

  private:
    std::unique_ptr<parser_impl> d_pimpl; // trick to encapsulate dependencies
  };
}

#endif // KYAML_HH
