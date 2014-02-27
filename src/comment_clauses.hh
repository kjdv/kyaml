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
  
    namespace internal
    {
      class endoffile : public void_clause
      {
      public:
        using void_clause::void_clause;
        
        bool try_clause()
        {
          return ctx().stream().eof();
        }
      };
    }

    // [76] 	b-comment 	::= 	b-non-content |  End of file 
    typedef internal::or_clause<void_result,
                                non_content,
                                internal::endoffile> break_comment;
  }
}

#endif // COMMENT_CLAUSES_HH
