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
      class start_of_line : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder);
      };
    }
    typedef internal::or_clause<internal::one_or_more<white>,
                                internal::start_of_line> separate_in_line;

    // [67] 	s-line-prefix(n,c) 	::= 	c = block-out ⇒ s-block-line-prefix(n)
    //                                          c = block-in  ⇒ s-block-line-prefix(n)
    //                                          c = flow-out  ⇒ s-flow-line-prefix(n)
    //                                          c = flow-in   ⇒ s-flow-line-prefix(n)
    class line_prefix : public clause
    {
    public:
      line_prefix(context &ctx);

      bool parse(document_builder &builder)
      {
        return d_dispatch ? (this->*d_dispatch)(builder) : false;
      }
    private:
      bool parse_block(document_builder &builder);
      bool parse_flow(document_builder &builder);

      typedef bool (line_prefix::*dispatch_f)(document_builder &);
      dispatch_f d_dispatch;
    };

    // [68] 	s-block-line-prefix(n) 	::= 	s-indent(n)
    typedef indent_clause_eq block_line_prefix;

    // [69] 	s-flow-line-prefix(n) 	::= 	s-indent(n) s-separate-in-line? 
    typedef internal::and_clause<internal::indent_clause_ge,
                                 internal::zero_or_one<separate_in_line> 
                                 > flow_line_prefix;

    // [70] 	l-empty(n,c) 	::= 	( s-line-prefix(n,c) | s-indent(<n) )
    //                                  b-as-line-feed
    typedef internal::and_clause<internal::or_clause<line_prefix,
                                                     indent_clause_lt>,
                                 as_line_feed> empty_line;

    // [71] 	b-l-trimmed(n,c) 	::= 	b-non-content l-empty(n,c)+ 
    typedef internal::and_clause<non_content,
                                 internal::one_or_more<empty_line> > trimmed;

    // [72] 	b-as-space 	::= 	b-break
    class as_space : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [73] 	b-l-folded(n,c) 	::= 	b-l-trimmed(n,c) | b-as-space
    typedef internal::or_clause<trimmed,
                                as_space> line_folded;

    // [74] 	s-flow-folded(n) 	::= 	s-separate-in-line? b-l-folded(n,flow-in)
    //                                          s-flow-line-prefix(n)
    typedef internal::all_of<internal::zero_or_one<separate_in_line>,
                             internal::state_scope<internal::flow_modifier<context::FLOW_IN>, line_folded>,
                             flow_line_prefix> flow_folded;
  }
}

#endif // INDENDATION_CLAUSES_HH
