#ifndef COMMENT_CLAUSES_HH
#define COMMENT_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [75] 	c-nb-comment-text 	::= 	“#” nb-char* 
    typedef internal::and_clause<string_result,
                                 internal::simple_char_clause<'#'>,                   
                                 internal::zero_or_more<string_result,
                                                        non_break_char> > non_break_comment_text;
  }
}

#endif // COMMENT_CLAUSES_HH
