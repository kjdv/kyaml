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

    class separate : public internal::void_clause
    {
    public:
      separate(context &ctx);

      bool try_clause() 
      {
        return d_dispatch ? (this->*d_dispatch)() : false;
      }

    private:
      bool try_lines();
      bool try_in_line();

      typedef bool (separate::*dispatch_f)();
      dispatch_f d_dispatch;
    };

    // [81] 	s-separate-lines(n) 	::= 	  ( s-l-comments s-flow-line-prefix(n) )
    //                                            | s-separate-in-line

    typedef internal::or_clause<void_result,
                                internal::and_clause<void_result, 
                                                     sline_comment,
                                                     flow_line_prefix>,
                                separate_in_line> separate_in_lines;
  }
}

#endif // SEPARATION_LINE_CLAUSES_HH
