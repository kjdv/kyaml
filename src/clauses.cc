#include "clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool printable::try_clause()
{
  int c = stream().peekc();
  return 
    c >=0 &&
    (
      c == '\x9' ||
      c == '\xa' ||
      c == '\xd' ||
      (c >= '\x20' && c <= '\x7e')
    );
}

bool json::try_clause()
{
  int c = stream().peekc();
  return 
    c >=0 &&
    (
      c == '\x9' ||
      c >= '\x20'
    );
}

bool reserved::try_clause()
{
  int c = stream().peekc();
  return 
    c >=0 &&
    ( c == '@' || c == '`' );
}
