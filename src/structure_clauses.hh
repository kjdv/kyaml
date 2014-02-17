#ifndef STRUCTURE_CLAUSES_HH
#define STRUCTURE_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [66] 	s-separate-in-line 	::= 	s-white+ | Start of line 
    namespace internal
    {
      class start_of_line : public void_clause
      {
      public:
        using void_clause::void_clause;
        
        bool try_clause();
      };
    }
    typedef internal::or_clause<void_result,
                                internal::one_or_more<void_result, white>,
                                internal::start_of_line> separate_in_line;
  }
}

#endif // INDENDATION_CLAUSES_HH
