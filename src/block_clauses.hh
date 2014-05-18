#ifndef BLOCK_CLAUSES_HH
#define BLOCK_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "comment_clauses.hh"
#include "indentation_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [163] 	c-indentation-indicator(m) 	::= 	ns-dec-digit ⇒ m = ns-dec-digit - #x30
    //                                                  /* Empty */  ⇒ m = auto-detect() 
    class indentation_indicator : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    private:
      bool autodetect(document_builder &builder);
    };

    // [164] 	c-chomping-indicator(t) 	::= 	“-”         ⇒ t = strip
    //                                                  “+”         ⇒ t = keep
    //                                                  /* Empty */ ⇒ t = clip 
    class chomping_indicator : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [162] 	c-b-block-header(m,t) 	::= 	( ( c-indentation-indicator(m)
    //                                              c-chomping-indicator(t) )
    //                                          | ( c-chomping-indicator(t)
    //                                              c-indentation-indicator(m) ) )
    //                                          s-b-comment 
    typedef internal::and_clause<internal::or_clause<internal::and_clause<indentation_indicator,
                                                                          chomping_indicator>,
                                                     internal::and_clause<chomping_indicator,
                                                                          indentation_indicator> >,
                                 sbreak_comment> block_header;

    // [165] 	b-chomped-last(t) 	::= 	t = strip ⇒ b-non-content | /* End of file */
    //                                          t = clip  ⇒ b-as-line-feed | /* End of file */
    //                                          t = keep  ⇒ b-as-line-feed | /* End of file */ 
    class chomped_last : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [169] 	l-trail-comments(n) 	::= 	s-indent(<n) c-nb-comment-text b-comment
    //                                          l-comment* 
    typedef internal::all_of<indent_clause_lt,
                             non_break_comment_text,
                             break_comment,
                             internal::zero_or_more<line_comment> > trail_comments;

    // [168] 	l-keep-empty(n) 	::= 	l-empty(n,block-in)*
    //                                          l-trail-comments(n)? 
    typedef internal::and_clause<internal::zero_or_more<internal::flow_restriction<empty_line, context::BLOCK_IN> >,
                                 internal::zero_or_one<trail_comments> > keep_empty;

    // [167] 	l-strip-empty(n) 	::= 	( s-indent(≤n) b-non-content )*
    //                                            l-trail-comments(n)? 
    typedef internal::and_clause<internal::zero_or_more<internal::and_clause<indent_clause_le, non_content> >,
                                 trail_comments> strip_empty;

    // [166] 	l-chomped-empty(n,t) 	::= 	t = strip ⇒ l-strip-empty(n)
    //                                          t = clip  ⇒ l-strip-empty(n)
    //                                          t = keep  ⇒ l-keep-empty(n)
    class chomped_empty : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [171] 	l-nb-literal-text(n) 	::= 	l-empty(n,block-in)*
    //                                          s-indent(n) nb-char+ 	 
    typedef internal::all_of<internal::zero_or_more<internal::flow_restriction<empty_line, context::BLOCK_IN> >,
                             indent_clause_eq,
                             internal::one_or_more<non_break_char> > line_literal_text;

    // [172] 	b-nb-literal-next(n) 	::= 	b-as-line-feed
    //                                          l-nb-literal-text(n) 	 
    typedef internal::and_clause<as_line_feed,
                                 line_literal_text> break_literal_text;

    // [173] 	l-literal-content(n,t) 	::= 	( l-nb-literal-text(n) b-nb-literal-next(n)*
    //                                            b-chomped-last(t) )?
    //                                          l-chomped-empty(n,t)
    typedef internal::and_clause<internal::zero_or_one<internal::all_of<line_literal_text,
                                                                        internal::zero_or_more<break_literal_text>,
                                                                        chomped_last> >,
                                 chomped_empty> literal_content;

    // [170] 	c-l+literal(n) 	::= 	“|” c-b-block-header(m,t)
    //                                      l-literal-content(n+m,t)
    typedef internal::all_of<internal::simple_char_clause<'|', false>,
                             block_header,
                             literal_content> line_literal; // TODO: test indentation magic

    // [175] 	s-nb-folded-text(n) 	::= 	s-indent(n) ns-char nb-char* 
    typedef internal::all_of<indent_clause_eq,
                             non_white_char,
                             internal::zero_or_more<non_break_char> > folded_text;

    // [176] 	l-nb-folded-lines(n) 	::= 	s-nb-folded-text(n)
    //                                          ( b-l-folded(n,block-in) s-nb-folded-text(n) )* 
    typedef internal::and_clause<folded_text,
                                 internal::zero_or_more<internal::and_clause<internal::flow_restriction<line_folded, context::BLOCK_IN>, // TODO: reevaluate flow restriction
                                                                             folded_text> > > folded_lines;

    // [177] 	s-nb-spaced-text(n) 	::= 	s-indent(n) s-white nb-char* 
    typedef internal::all_of<indent_clause_eq,
                             white,
                             internal::zero_or_more<non_break_char> > spaced_text;

    // [178] 	b-l-spaced(n) 	::= 	b-as-line-feed
    //                                  l-empty(n,block-in)* 
    typedef internal::and_clause<as_line_feed,
                                 internal::flow_restriction<empty_line, context::BLOCK_IN> > spaced;

    // [179] 	l-nb-spaced-lines(n) 	::= 	s-nb-spaced-text(n)
    //                                          ( b-l-spaced(n) s-nb-spaced-text(n) )*
    typedef internal::and_clause<spaced_text,
                                 internal::zero_or_more<internal::and_clause<spaced,
                                                                             spaced_text> > > spaced_lines;

    // [180] 	l-nb-same-lines(n) 	::= 	l-empty(n,block-in)*
    //                                          ( l-nb-folded-lines(n) | l-nb-spaced-lines(n) ) 
    typedef internal::and_clause<internal::zero_or_more<internal::flow_restriction<empty_line, context::BLOCK_IN> >,
                                 internal::or_clause<folded_lines,
                                                     spaced_lines>
                                 > same_lines;

    // [181] 	l-nb-diff-lines(n) 	::= 	l-nb-same-lines(n)
    //                                          ( b-as-line-feed l-nb-same-lines(n) )* 
    typedef internal::and_clause<same_lines,
                                 internal::zero_or_more<internal::and_clause<as_line_feed,
                                                                             same_lines> >
                                 > diff_lines;

    // [182] 	l-folded-content(n,t) 	::= 	( l-nb-diff-lines(n) b-chomped-last(t) )?
    //                                            l-chomped-empty(n,t) 	 
    typedef internal::and_clause<internal::zero_or_one<internal::and_clause<diff_lines, chomped_last> >,
                                 chomped_empty> folded_content;

    // [174] 	c-l+folded(n) 	::= 	“>” c-b-block-header(m,t)
    //                                   l-folded-content(n+m,t)
    typedef internal::all_of<internal::simple_char_clause<'>', false>,
                             block_header,
                             folded_content> content_folded; // TODO: test indentation magic, and a better name
  }
}

#endif // BLOCK_CLAUSES_HH
