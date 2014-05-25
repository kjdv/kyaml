#ifndef MOCK_BUILDER_HH
#define MOCK_BUILDER_HH

#include "document_builder.hh"
#include <initializer_list>
#include <string>
#include <gmock/gmock.h>

namespace kyaml
{
  namespace test
  {
    class mock_builder : public document_builder
    {
    public:
      MOCK_METHOD0(start_sequence, void());
      MOCK_METHOD0(end_sequence, void());
      MOCK_METHOD0(start_mapping, void());
      MOCK_METHOD0(end_mapping, void());
      MOCK_METHOD1(add_anchor, void(std::string const &));
      MOCK_METHOD1(add_alias, void(std::string const &));
      MOCK_METHOD1(add_scalar, void(std::string const &));
      MOCK_METHOD1(add_property, void(std::string const &));
      MOCK_METHOD1(add_atom, void(char32_t));

      void expect_sequence(std::initializer_list<std::string> const &seq = {});\
      void expect_mapping(std::initializer_list<std::pair<std::string, std::string> > const &map = {});
    };
  }
}

#endif // MOCK_BUILDER_HH
