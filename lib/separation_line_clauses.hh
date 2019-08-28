#ifndef SEPARATION_LINE_CLAUSES_HH
#define SEPARATION_LINE_CLAUSES_HH

#include "structure_clauses.hh"
#include "comment_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [80] 	s-separate(n,c) 	::= 	c = block-out ⇒ s-separate-lines(n)
    //                                          c = block-in  ⇒ s-separate-lines(n)
    //                                          c = flow-out  ⇒ s-separate-lines(n)
    //                                          c = flow-in   ⇒ s-separate-lines(n)
    //                                          c = block-key ⇒ s-separate-in-line
    //                                          c = flow-key  ⇒ s-separate-in-line

    class separate : public clause
    {
    public:
      separate(context &ctx);

      bool parse(document_builder &builder)
      {
        return d_dispatch ? (this->*d_dispatch)(builder) : false;
      }

    private:
      bool parse_lines(document_builder &builder);
      bool parse_in_line(document_builder &builder);

      typedef bool (separate::*dispatch_f)(document_builder &);
      dispatch_f d_dispatch;
    };

    // [81] 	s-separate-lines(n) 	::= 	  ( s-l-comments s-flow-line-prefix(n) )
    //                                            | s-separate-in-line

    typedef internal::or_clause<internal::and_clause<sline_comment,
                                                     flow_line_prefix>,
                                eating_separate_in_line> separate_in_lines;
  }
}

#endif // SEPARATION_LINE_CLAUSES_HH
