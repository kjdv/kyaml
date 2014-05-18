#ifndef NODE_CLAUSES_HH
#define NODE_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "directive_clauses.hh"
#include "separation_line_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [102] 	ns-anchor-char 	::= 	ns-char - c-flow-indicator
    class anchor_char : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [103] 	ns-anchor-name 	::= 	ns-anchor-char+ 
    typedef internal::one_or_more<anchor_char> anchor_name;

    // [101] 	c-ns-anchor-property 	::= 	“&” ns-anchor-name
    class anchor_property : public clause
    {
    public:
      using clause::clause;
      
      bool parse(document_builder &builder);

      char const *name() const
      {
        return "c-ns-anchor-property";
      }
    };

    // [100] 	c-non-specific-tag 	::= 	“!”
    typedef internal::simple_char_clause<'!'> non_specific_tag;

    // [99] 	c-ns-shorthand-tag 	::= 	c-tag-handle ns-tag-char+ 
    typedef internal::and_clause<tag_handle,
                                 internal::one_or_more<tag_char> > shorthand_tag;

    // [98] 	c-verbatim-tag 	::= 	“!” “<” ns-uri-char+ “>” 
    class verbatim_tag : public clause
    {
    public:
      using clause::clause;
      
      bool parse(document_builder &builder);
      
      char const *name() const
      {
        return "c-verbatim-tag";
      }
    };

    // [97] 	c-ns-tag-property 	::= 	  c-verbatim-tag
    //                                          | c-ns-shorthand-tag
    //                                          | c-non-specific-tag
    typedef internal::any_of<verbatim_tag,
                             shorthand_tag,
                             non_specific_tag> tag_property;

    // [96] 	c-ns-properties(n,c) 	::= 	  ( c-ns-tag-property
    //                                                ( s-separate(n,c) c-ns-anchor-property )? )
    //                                            | ( c-ns-anchor-property
    //                                              ( s-separate(n,c) c-ns-tag-property )? ) 
    typedef internal::or_clause<internal::and_clause<tag_property,
                                                     internal::zero_or_one<
                                                       internal::and_clause<separate,
                                                                            anchor_property> > >,
                                internal::and_clause<anchor_property,
                                                     internal::zero_or_one<
                                                       internal::and_clause<separate,
                                                                            tag_property> > >
                                > properties;
    
    // [104] 	c-ns-alias-node 	::= 	“*” ns-anchor-name
    class alias_node : public clause
    {
    public:
      using clause::clause;
      
      bool parse(document_builder &builder);
      
      char const *name() const
      {
        return "c-ns-alias-node";
      }
    };

    // [105] 	e-scalar 	::= 	/* Empty */
    class empty : public clause
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
    typedef empty escalar;

    // [106] 	e-node 	::= 	e-scalar
    typedef escalar enode;

    // [107] 	nb-double-char 	::= 	c-ns-esc-char | ( nb-json - “\” - “"” )
    typedef internal::or_clause<esc_char,
                                internal::all_of<internal::not_clause<internal::simple_char_clause<'\\'> >,
                                                 internal::not_clause<internal::simple_char_clause<'"'> >,
                                                 json>
                               > nonbreak_double_char;

    // [108] 	ns-double-char 	::= 	nb-double-char - s-white
    typedef internal::and_clause<internal::not_clause<white>,
                                 nonbreak_double_char> nonspace_double_char;

    // [114] 	nb-ns-double-in-line 	::= 	( s-white* ns-double-char )*
    typedef internal::zero_or_more<internal::and_clause<internal::zero_or_more<white>,
                                                        nonspace_double_char>
                                  > nonbreak_nonspace_double_inline;

    // [112] 	s-double-escaped(n) 	::= 	s-white* “\” b-non-content
    //                                    l-empty(n,flow-in)* s-flow-line-prefix(n)
    typedef internal::all_of<internal::zero_or_more<white>,
                             internal::simple_char_clause<'\\'>,
                             non_content,
                             internal::zero_or_more<internal::flow_restriction<empty_line, context::FLOW_IN> >,
                             flow_line_prefix> double_escaped;

    // [113] 	s-double-break(n) 	::= 	s-double-escaped(n) | s-flow-folded(n)
    typedef internal::or_clause<double_escaped,
                                flow_folded> double_break;


    // [115] 	s-double-next-line(n) 	::= 	s-double-break(n)
    //                                      ( ns-double-char nb-ns-double-in-line
    //                                      ( s-double-next-line(n) | s-white* ) )?
    class double_next_line : public clause // not as fancy typedef to get around recursive definition
    {
    public:
      double_next_line(context &ctx) :
        clause(ctx),
        d_impl(ctx)
      {}

      bool parse(document_builder &builder)
      {
        return d_impl.parse(builder);
      }
    private:
      typedef internal::and_clause<double_break,
                                   internal::zero_or_one<
                                     internal::all_of<
                                       nonspace_double_char,
                                       nonbreak_nonspace_double_inline,
                                       internal::or_clause<
                                         double_next_line,
                                         internal::zero_or_more<white>
                                       >
                                     >
                                   > > impl_t;
      impl_t d_impl;
    };


    // [116] 	nb-double-multi-line(n) 	::= 	nb-ns-double-in-line
    //                                       ( s-double-next-line(n) | s-white* )
    typedef internal::and_clause<nonbreak_nonspace_double_inline,
                                 internal::or_clause<double_next_line,
                                                     internal::zero_or_more<white>
                                                    >
                                > double_multi_line;

    // [111] 	nb-double-one-line 	::= 	nb-double-char*
    typedef internal::zero_or_more<nonbreak_double_char> double_one_line;

    // [110] 	nb-double-text(n,c) 	::= 	c = flow-out  ⇒ nb-double-multi-line(n)
    //                                    c = flow-in   ⇒ nb-double-multi-line(n)
    //                                    c = block-key ⇒ nb-double-one-line
    //                                    c = flow-key  ⇒ nb-double-one-line
    class double_text : public clause
    {
    public:
      double_text(context &ctx);

      bool parse(document_builder &builder)
      {
        return d_dispatch ? (this->*d_dispatch)(builder) : false;
      }

    private:
      bool parse_multiline(document_builder &builder);
      bool parse_oneline(document_builder &builder);

      typedef bool (double_text::*dispatch_f)(document_builder &);
      dispatch_f d_dispatch;
    };

    // [109] 	c-double-quoted(n,c) 	::= 	“"” nb-double-text(n,c) “"”
    typedef internal::all_of<internal::simple_char_clause<'"', false>,
                             double_text,
                             internal::simple_char_clause<'"', false> > double_quoted;

    // [117] 	c-quoted-quote 	::= 	“'” “'”
    typedef internal::and_clause<internal::simple_char_clause<'\'', false>,
                                 internal::simple_char_clause<'\'', true> > quoted_quote;

    // [118] 	nb-single-char 	::= 	c-quoted-quote | ( nb-json - “'” )
    typedef internal::or_clause<quoted_quote,
                                internal::and_clause<internal::not_clause<internal::simple_char_clause<'\''> >,
                                                     json>
                               > nonbreak_single_char;

    // [119] 	ns-single-char 	::= 	nb-single-char - s-white
    typedef internal::and_clause<internal::not_clause<white>,
                                 nonbreak_single_char> nonspace_single_char;

    // [122] 	nb-single-one-line 	::= 	nb-single-char*
    typedef internal::zero_or_more<nonbreak_single_char> single_one_line;

    // [123] 	nb-ns-single-in-line 	::= 	( s-white* ns-single-char )*
    typedef internal::zero_or_more<internal::and_clause<internal::zero_or_more<white>,
                                                        nonspace_single_char>
                                  > nonbreak_nonspace_single_inline;

    // [124] 	s-single-next-line(n) 	::= 	s-flow-folded(n)
    //                                    ( ns-single-char nb-ns-single-in-line
    //                                    ( s-single-next-line(n) | s-white* ) )?
    class single_next_line : public clause // not as fancy typedef to get around recursive definition
    {
    public:
      single_next_line(context &ctx) :
        clause(ctx),
        d_impl(ctx)
      {}

      bool parse(document_builder &builder)
      {
        return d_impl.parse(builder);
      }
    private:
      typedef internal::and_clause<flow_folded,
                                   internal::zero_or_one<
                                     internal::all_of<
                                       nonspace_single_char,
                                       nonbreak_nonspace_single_inline,
                                       internal::or_clause<
                                         single_next_line,
                                         internal::zero_or_more<white>
                                       >
                                     >
                                   > > impl_t;
      impl_t d_impl;
    };

    // [125] 	nb-single-multi-line(n) 	::= 	nb-ns-single-in-line
    //                                        ( s-single-next-line(n) | s-white* )
    typedef internal::and_clause<nonbreak_nonspace_single_inline,
                                 internal::or_clause<single_next_line,
                                                     internal::zero_or_more<white>
                                                    >
                                > single_multi_line;

    // [121] 	nb-single-text(n,c) 	::= 	c = flow-out  ⇒ nb-single-multi-line(n)
    //                                    c = flow-in   ⇒ nb-single-multi-line(n)
    //                                    c = block-key ⇒ nb-single-one-line
    //                                    c = flow-key  ⇒ nb-single-one-line
    class single_text : public clause
    {
    public:
      single_text(context &ctx);

      bool parse(document_builder &builder)
      {
        return d_dispatch ? (this->*d_dispatch)(builder) : false;
      }

    private:
      bool parse_multiline(document_builder &builder);
      bool parse_oneline(document_builder &builder);

      typedef bool (single_text::*dispatch_f)(document_builder &);
      dispatch_f d_dispatch;
    };


    // [120] 	c-single-quoted(n,c) 	::= 	“'” nb-single-text(n,c) “'”
    typedef internal::all_of<internal::simple_char_clause<'\'', false>,
                             single_text,
                             internal::simple_char_clause<'\'', false> > single_quoted;

    // [129] 	ns-plain-safe-in 	::= 	ns-char - c-flow-indicator
    typedef internal::and_clause<internal::not_clause<flow_indicator>,
                                 non_white_char> plain_safe_in;

    // [128] 	ns-plain-safe-out 	::= 	ns-char
    typedef non_white_char plain_safe_out;

    // [127] 	ns-plain-safe(c) 	::= 	c = flow-out  ⇒ ns-plain-safe-out
    //                                c = flow-in   ⇒ ns-plain-safe-in
    //                                c = block-key ⇒ ns-plain-safe-out
    //                                c = flow-key  ⇒ ns-plain-safe-in
    class plain_safe : public clause
    {
    public:
      plain_safe(context &ctx);

      bool parse(document_builder &builder)
      {
        return d_dispatch ? (this->*d_dispatch)(builder) : false;
      }

    private:
      bool parse_safe_in(document_builder &builder);
      bool parse_safe_out(document_builder &builder);

      typedef bool (plain_safe::*dispatch_f)(document_builder &);
      dispatch_f d_dispatch;
    };


    // [126] 	ns-plain-first(c) 	::= 	  ( ns-char - c-indicator )
    //                                    | ( ( “?” | “:” | “-” )
    //                                      Followed by an ns-plain-safe(c)) )
    class plain_first : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);

    private:
      typedef internal::and_clause<internal::not_clause<indicator>,
                                   non_white_char> left_t;
      typedef internal::any_of<internal::simple_char_clause<'?'>,
                               internal::simple_char_clause<':'>,
                               internal::simple_char_clause<'-'> > right_t;
      typedef plain_safe tail_t;

      typedef internal::and_clause<right_t, tail_t> augmented_right_t;
    };

    // [130] 	ns-plain-char(c) 	::= 	  ( ns-plain-safe(c) - “:” - “#” )
    //                                  | ( An ns-char preceding “#” )
    //                                  | ( “:” Followed by an ns-plain-safe(c) )
    class plain_char : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    private:
      typedef internal::all_of<internal::not_clause<internal::simple_char_clause<':'> >,
                               internal::not_clause<internal::simple_char_clause<'#'> >,
                               plain_safe> clause1_t;
      typedef internal::simple_char_clause<'#'> clause2_t;
      typedef internal::simple_char_clause<':'> clause3_t;

      bool preceded_by_nschar(context &ctx);
      bool followed_by_plain_safe(context &ctx);
    };

    // [132] 	nb-ns-plain-in-line(c) 	::= 	( s-white* ns-plain-char(c) )*
    typedef internal::zero_or_more<internal::and_clause<internal::zero_or_more<white>,
                                                        plain_char> > plain_in_line;

    // [133] 	ns-plain-one-line(c) 	::= 	ns-plain-first(c) nb-ns-plain-in-line(c)
    typedef internal::and_clause<plain_first, plain_in_line> plain_one_line;                                 
   
    // [134] 	s-ns-plain-next-line(n,c) 	::= 	s-flow-folded(n)
    //                                                  ns-plain-char(c) nb-ns-plain-in-line(c)
    typedef internal::all_of<flow_folded,
                             plain_char,
                             plain_in_line> plain_next_line;

    // [135] 	ns-plain-multi-line(n,c) 	::= 	ns-plain-one-line(c)
    //                                                   s-ns-plain-next-line(n,c)* 
    typedef internal::and_clause<plain_one_line,
                                 internal::zero_or_more<plain_next_line> > plain_multi_line;

    // [131] 	ns-plain(n,c) 	::= 	c = flow-out  ⇒ ns-plain-multi-line(n,c)
    //                                  c = flow-in   ⇒ ns-plain-multi-line(n,c)
    //                                  c = block-key ⇒ ns-plain-one-line(c)
    //                                  c = flow-key  ⇒ ns-plain-one-line(c)
    class plain : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };
  }
}

#endif // NODE_CLAUSES_HH
