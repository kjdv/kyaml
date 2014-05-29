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
      void start_sequence(context const &ctx) override
      {
        start_sequence();
      }
      void end_sequence(context const &ctx) override
      {
        end_sequence();
      }
      void start_mapping(context const &ctx) override
      {
        start_mapping();
      }
      void end_mapping(context const &ctx) override
      {
        end_mapping();
      }

      void add_anchor(context const &ctx, std::string const &anchor) override
      {
        add_anchor(anchor);
      }
      void add_alias(context const &ctx, std::string const &alias) override
      {
        add_alias(alias);
      }
      void add_scalar(context const &ctx, std::string const &val) override
      {
        add_scalar(val);
      }
      void add_atom(context const &ctx, char32_t c) override
      {}
      void add_property(context const &ctx, std::string const &prop) override
      {
        add_property(prop);
      }

      MOCK_METHOD0(start_sequence, void());
      MOCK_METHOD0(end_sequence, void());
      MOCK_METHOD0(start_mapping, void());
      MOCK_METHOD0(end_mapping, void());
      MOCK_METHOD1(add_anchor, void(std::string const &));
      MOCK_METHOD1(add_alias, void(std::string const &));
      MOCK_METHOD1(add_scalar, void(std::string const &));
      MOCK_METHOD1(add_property, void(std::string const &));

      void expect_sequence(std::initializer_list<std::string> const &seq = {});\
      void expect_mapping(std::initializer_list<std::pair<std::string, std::string> > const &map = {});
    };
  }
}

#endif // MOCK_BUILDER_HH
