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
    class parse_error : public std::exception
    {
    public:
      parse_error(unsigned linenumber, std::string const &msg = "");

      char const *what() const throw() override
      {
        return d_msg.c_str();
      }

    private:
      unsigned d_linenumber;
      std::string d_msg;
    };

    parser(std::istream &input);
    ~parser();

    std::unique_ptr<const document> parse(); // may throw

  private:
    std::unique_ptr<parser_impl> d_pimpl; // trick to encapsulate dependencies
  };
}

#endif // KYAML_HH
