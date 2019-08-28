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
    typedef internal::and_clause<internal::simple_char_clause<'#'>,                   
                                 internal::zero_or_more<non_break_char> > non_break_comment_text;
  
    namespace internal
    {
      class endoffile : public clause
      {
      public:
        using clause::clause;
        
        bool parse(document_builder &builder)
        {
          return ctx().stream().eof();
        }
      };
    }

    // [76] 	b-comment 	::= 	b-non-content |  End of file 
    typedef internal::or_clause<non_content,
                                internal::endoffile> break_comment;

    // [77] 	s-b-comment 	::= 	( s-separate-in-line c-nb-comment-text? )?
    //                                  b-comment
    typedef internal::and_clause<internal::zero_or_one<
                                   internal::and_clause<eating_separate_in_line,
                                                        internal::zero_or_one<non_break_comment_text> > >, // whoei
                                 break_comment> sbreak_comment;

    // [78] 	l-comment 	::= 	s-separate-in-line c-nb-comment-text? b-comment
    typedef internal::all_of<separate_in_line,
                             internal::zero_or_more<non_break_comment_text>,
                             break_comment> line_comment;

    // [79] 	s-l-comments 	::= 	( s-b-comment | Start of line )
    //                                    l-comment* 
    typedef internal::and_clause<internal::or_clause<sbreak_comment,
                                                     internal::eating_start_of_line>,
                                 internal::zero_or_more<line_comment> > sline_comment;
  }
}

#endif // COMMENT_CLAUSES_HH
