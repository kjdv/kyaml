#ifndef FLOW_CLAUSES_HH
#define FLOW_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"

namespace kyaml
{
  namespace clauses
  {  
    // [105] 	e-scalar 	::= 	 Empty 
    class empty_scalar : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder)
      {
        return true;
      }

      char const *name() const
      {
        return "e-scalar";
      }
    };

    // [106] 	e-node 	::= 	e-scalar
    typedef empty_scalar empty_node;

    // [107] 	nb-double-char 	::= 	c-ns-esc-char | ( nb-json - “\” - “"” ) 
    typedef internal::or_clause<esc_char,
                                internal::all_of<
                                  internal::not_clause<internal::simple_char_clause<'\\'> >,
                                  internal::not_clause<internal::simple_char_clause<'"'> >,
                                  json > > b_double_char;

    // [108] 	ns-double-char 	::= 	nb-double-char - s-white
    typedef internal::and_clause<internal::not_clause<white>,
                                 b_double_char> s_double_char;
                            
  }
}

#endif // FLOW_CLAUSES_HH
