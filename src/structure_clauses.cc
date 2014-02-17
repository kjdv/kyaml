#include "structure_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool internal::start_of_line::try_clause()
{
  char_t c;
  // the last character was a newline
  if(!stream().rpeek(c) ||
     (c == '\n' || c == '\r'))
  {
    // the current one is not
    if(stream().peek(c) &&
       (c != '\n' && c != '\r'))
      return true;
  }
  return false;
}
