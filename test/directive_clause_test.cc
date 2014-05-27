#include "directive_clauses.hh"
#include "clause_test.hh"

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace 
{
  clause_testcase tc(string const &input, bool result, unsigned consume = 0)
  {
    unsigned consumed = consume ? consume : (result ? input.size() : 0);

    return
      testcase_builder(input, result).
      with_consumed(consumed).
      build();
  }
}

CLAUSE_TEST(reserved_directive,
            cases({tc("FOO bar baz", true),
                   tc("YAML", true)}))
// TODO make this work  tc("YAML\nnextline", true, 4)}))

CLAUSE_TEST(yaml_directive,
            cases({tc("YAML 1.2", true)}))

CLAUSE_TEST(tag_directive,
            cases({tc("TAG !yaml! tag:yaml.org,2002:", true)}))

CLAUSE_TEST(ldirective,
            cases({tc("%YAML 1.2\nnextline", true, 10),
                   tc("%TAG !yaml! tag:yaml.org,2002:\nnextline", true, 31),
                   tc("%YAML\n", true)}))
