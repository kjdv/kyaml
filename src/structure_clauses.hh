#ifndef STRUCTURE_CLAUSES_HH
#define STRUCTURE_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "indentation_clauses.hh"

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

    // [67] 	s-line-prefix(n,c) 	::= 	c = block-out ⇒ s-block-line-prefix(n)
    //                                          c = block-in  ⇒ s-block-line-prefix(n)
    //                                          c = flow-out  ⇒ s-flow-line-prefix(n)
    //                                          c = flow-in   ⇒ s-flow-line-prefix(n)
    class line_prefix : public internal::void_clause
    {
    public:
      line_prefix(context &ctx);

      bool try_clause() 
      {
        return d_dispatch ? (this->*d_dispatch)() : false;
      }

    private:
      bool try_block();
      bool try_flow();

      typedef bool (line_prefix::*dispatch_f)();
      dispatch_f d_dispatch;
    };

    // [68] 	s-block-line-prefix(n) 	::= 	s-indent(n)
    typedef indent_clause_eq block_line_prefix;

    // [69] 	s-flow-line-prefix(n) 	::= 	s-indent(n) s-separate-in-line? 
    typedef internal::and_clause<void_result,
                                 internal::indent_clause_ge,
                                 internal::one_or_more<void_result, separate_in_line> 
                                 > flow_line_prefix;

    // [70] 	l-empty(n,c) 	::= 	( s-line-prefix(n,c) | s-indent(<n) )
    //                                  b-as-line-feed
    typedef internal::and_clause<void_result,
                                 internal::or_clause<void_result,
                                                     line_prefix,
                                                     indent_clause_lt>,
                                 as_line_feed> empty_line;

    // [71] 	b-l-trimmed(n,c) 	::= 	b-non-content l-empty(n,c)+ 
    typedef internal::and_clause<void_result,
                                 non_content,
                                 internal::one_or_more<void_result,
                                                       empty_line> > trimmed;

    // [72] 	b-as-space 	::= 	b-break
    typedef line_break as_space;

    // [73] 	b-l-folded(n,c) 	::= 	b-l-trimmed(n,c) | b-as-space
    typedef internal::or_clause<void_result,
                                trimmed,
                                as_space> line_folded;

    // [74] 	s-flow-folded(n) 	::= 	s-separate-in-line? b-l-folded(n,flow-in)
    //                                          s-flow-line-prefix(n)
    typedef internal::and_clause<void_result,
                                 internal::zero_or_one<void_result, 
                                                       separate_in_line>,
                                 internal::and_clause<void_result,
                                                      internal::flow_restriction<line_folded, context::FLOW_IN>,
                                                      flow_line_prefix> > flow_folded;

  }
}

#endif // INDENDATION_CLAUSES_HH
