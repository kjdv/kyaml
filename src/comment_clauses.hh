#ifndef COMMENT_CLAUSES_HH
#define COMMENT_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "structure_clauses.hh"

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
    typedef internal::or_clause<string_result,
                                non_content,
                                internal::endoffile> break_comment;

    // [77] 	s-b-comment 	::= 	( s-separate-in-line c-nb-comment-text? )?
    //                                  b-comment
    typedef internal::and_clause<string_result,
                                 internal::zero_or_more<string_result,
                                                        internal::and_clause<string_result,
                                                                             separate_in_line,
                                                                             internal::zero_or_more<string_result,
                                                                                                    non_break_comment_text> > >, // whoei
                                 break_comment> sbreak_comment;

    // [78] 	l-comment 	::= 	s-separate-in-line c-nb-comment-text? b-comment
    typedef internal::and_clause<string_result,
                                 separate_in_line,
                                 internal::and_clause<string_result,
                                                      internal::zero_or_more<string_result, 
                                                                             non_break_comment_text>,
                                                      break_comment> > line_comment;

    // [79] 	s-l-comments 	::= 	( s-b-comment | Start of line )
    //                                    l-comment* 
    typedef internal::and_clause<string_result,
                                 internal::or_clause<string_result,
                                                     sbreak_comment,
                                                     internal::start_of_line>,
                                 internal::zero_or_more<string_result,
                                                        line_comment> > sline_comment;
  }
}

#endif // COMMENT_CLAUSES_HH
