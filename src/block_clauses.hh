#ifndef BLOCK_CLAUSES_HH
#define BLOCK_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "comment_clauses.hh"
#include "indentation_clauses.hh"
#include "separation_line_clauses.hh"
#include "node_clauses.hh"
#include "flow_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // generic helper for indendation
    namespace internal
    {
      int delta_indent(context &ctx);
      bool autodetect_indent(context &ctx, int minumum);
    }

    // [163] 	c-indentation-indicator(m) 	::= 	ns-dec-digit ⇒ m = ns-dec-digit - #x30
    //                                                  /* Empty */  ⇒ m = auto-detect() 
    class indentation_indicator : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    private:
      bool autodetect();
    };

    // [164] 	c-chomping-indicator(t) 	::= 	“-”         ⇒ t = strip
    //                                        “+”         ⇒ t = keep
    //                                         /* Empty */ ⇒ t = clip
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
                                 internal::or_clause<sbreak_comment,
                                                     break_comment // extension
                                                    >
                                > block_header;

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
    typedef internal::and_clause<internal::zero_or_more<internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, empty_line > >,
                                 internal::zero_or_one<trail_comments> > keep_empty;

    // [167] 	l-strip-empty(n) 	::= 	( s-indent(≤n) b-non-content )*
    //                                            l-trail-comments(n)? 
    typedef internal::and_clause<internal::zero_or_more<internal::and_clause<indent_clause_le, non_content> >,
                                 internal::zero_or_one<trail_comments> > strip_empty;

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
    //                                    s-indent(n) nb-char+
    typedef internal::all_of<internal::zero_or_more<internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, empty_line> >,
                             indent_clause_eq,
                             internal::one_or_more<non_break_char> > line_literal_text;

    // [172] 	b-nb-literal-next(n) 	::= 	b-as-line-feed
    //                                          l-nb-literal-text(n) 	 
    typedef internal::and_clause<as_line_feed,
                                 line_literal_text> break_literal_text;

    // [173] 	l-literal-content(n,t) 	::= 	( l-nb-literal-text(n) b-nb-literal-next(n)*
    //                                            b-chomped-last(t) )?
    //                                          l-chomped-empty(n,t)
    class literal_content : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [170] 	c-l+literal(n) 	::= 	“|” c-b-block-header(m,t)
    //                                  l-literal-content(n+m,t)
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
                                 internal::zero_or_more<internal::and_clause<internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, line_folded>,
                                                                             folded_text> > > folded_lines;

    // [177] 	s-nb-spaced-text(n) 	::= 	s-indent(n) s-white nb-char* 
    typedef internal::all_of<indent_clause_eq,
                             white,
                             internal::zero_or_more<non_break_char> > spaced_text;

    // [178] 	b-l-spaced(n) 	::= 	b-as-line-feed
    //                                  l-empty(n,block-in)* 
    typedef internal::and_clause<as_line_feed,
                                 internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, empty_line> > spaced;

    // [179] 	l-nb-spaced-lines(n) 	::= 	s-nb-spaced-text(n)
    //                                          ( b-l-spaced(n) s-nb-spaced-text(n) )*
    typedef internal::and_clause<spaced_text,
                                 internal::zero_or_more<internal::and_clause<spaced,
                                                                             spaced_text> > > spaced_lines;

    // [180] 	l-nb-same-lines(n) 	::= 	l-empty(n,block-in)*
    //                                          ( l-nb-folded-lines(n) | l-nb-spaced-lines(n) ) 
    typedef internal::and_clause<internal::zero_or_more<internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, empty_line> >,
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
    class folded_content : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [174] 	c-l+folded(n) 	::= 	“>” c-b-block-header(m,t)
    //                                   l-folded-content(n+m,t)
    typedef internal::all_of<internal::simple_char_clause<'>', false>,
                             block_header,
                             folded_content> content_folded; // TODO: test indentation magic, and a better name

    // [201] 	seq-spaces(n,c) 	::= 	c = block-out ⇒ n-1
    //                                          c = block-in  ⇒ n 
    class seq_spaces : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [200] 	s-l+block-collection(n,c) 	::= 	( s-separate(n+1,c) c-ns-properties(n+1,c) )?
    //                                          s-l-comments
    //                                          ( l+block-sequence(seq-spaces(n,c))
    //                                          | l+block-mapping(n) )
    class block_collection : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [199] 	s-l+block-scalar(n,c) 	::= 	s-separate(n+1,c)
    //                                          ( c-ns-properties(n+1,c) s-separate(n+1,c) )?
    //                                          ( c-l+literal(n) | c-l+folded(n) ) 	 
    typedef internal::all_of<internal::state_scope<internal::indent_inc_modifier, separate>,
                             internal::zero_or_one<internal::and_clause<
                                                     internal::state_scope<internal::indent_inc_modifier, properties >,
                                                     internal::state_scope<internal::indent_inc_modifier, separate> >
                                                   >,
                             internal::or_clause<line_literal,
                                                 content_folded> > block_scalar;

    // [198] 	s-l+block-in-block(n,c) 	::= 	s-l+block-scalar(n,c) | s-l+block-collection(n,c)
    typedef internal::or_clause<block_scalar, block_collection> block_in_block;

    // [197] 	s-l+flow-in-block(n) 	::= 	s-separate(n+1,flow-out)
    //                                          ns-flow-node(n+1,flow-out) s-l-comments 
    typedef internal::all_of<internal::state_scope<internal::indent_inc_modifier,
                                                   internal::state_scope<internal::flow_modifier<context::FLOW_OUT>, separate> >,
                             internal::state_scope<internal::indent_inc_modifier,
                                                   internal::state_scope<internal::flow_modifier<context::FLOW_OUT>, flow_node> >,
                             sline_comment> flow_in_block;

    // [196] 	s-l+block-node(n,c) 	::= 	s-l+block-in-block(n,c) | s-l+flow-in-block(n)
    typedef internal::or_clause<block_in_block, flow_in_block> block_node;

    // [195] 	ns-l-compact-mapping(n) 	::= 	ns-l-block-map-entry(n)
    //                                                  ( s-indent(n) ns-l-block-map-entry(n) )* 
    // not a typedef to break cyclic dependency
    class compact_mapping : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder); 
    };

    // [194] 	c-l-block-map-implicit-value(n) 	::= 	“:” ( s-l+block-node(n,block-out)
    //                                                              | ( e-node s-l-comments ) ) 
    typedef internal::and_clause<internal::simple_char_clause<':', false>,
                                 internal::or_clause<internal::state_scope<internal::flow_modifier<context::BLOCK_OUT>, block_node>,
                                                     internal::and_clause<enode, sline_comment> > > block_map_implicit_value;

    // [193] 	ns-s-block-map-implicit-key 	::= 	  c-s-implicit-json-key(block-key)
    //                                                  | ns-s-implicit-yaml-key(block-key)
    typedef internal::or_clause<internal::state_scope<internal::flow_modifier<context::BLOCK_KEY>, implicit_json_key>,
                                internal::state_scope<internal::flow_modifier<context::BLOCK_KEY>, implicit_yaml_key>
                                > block_map_implicit_key;

    // [192] 	ns-l-block-map-implicit-entry(n) 	::= 	( ns-s-block-map-implicit-key
    //                                                            | e-node )
    //                                                          c-l-block-map-implicit-value(n)
    typedef internal::and_clause<internal::or_clause<block_map_implicit_key,
                                                     enode>,
                                 block_map_implicit_value> block_map_implicit_entry;

    // [185] 	s-l+block-indented(n,c) 	::= 	  ( s-indent(m)
    //                                            (   ns-l-compact-sequence(n+1+m)
    //                                              | ns-l-compact-mapping(n+1+m)
    //                                            )
    //                                          )
    //                                          | s-l+block-node(n,c)
    //                                          | ( e-node s-l-comments )
    class block_indented : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder); 

    private:
      bool parse_funky_indent(document_builder &builder);
      bool parse_normal(document_builder &builder);
    };

    // [191] 	l-block-map-explicit-value(n) 	::= 	s-indent(n)
    //                                                  “:” s-l+block-indented(n,block-out)
    typedef internal::all_of<indent_clause_eq,
                             internal::simple_char_clause<':', false>,
                             internal::state_scope<internal::flow_modifier<context::BLOCK_OUT>, block_indented> > block_map_explicit_value;

    // [190] 	c-l-block-map-explicit-key(n) 	::= 	“?” s-l+block-indented(n,block-out)
    typedef internal::and_clause<internal::simple_char_clause<'?', false>,
                                 internal::state_scope<internal::flow_modifier<context::BLOCK_OUT>, block_indented> > block_map_explicit_key;

    // [189] 	c-l-block-map-explicit-entry(n) 	::= 	c-l-block-map-explicit-key(n)
    //                                                        ( l-block-map-explicit-value(n)
    //                                                          | e-node ) 
    typedef internal::and_clause<block_map_explicit_key,
                                 internal::or_clause<block_map_explicit_value, enode> > block_map_explicit_entry;

    // [188] 	ns-l-block-map-entry(n) 	::= 	  c-l-block-map-explicit-entry(n)
    //                                                  | ns-l-block-map-implicit-entry(n)
    typedef internal::or_clause<block_map_explicit_entry,
                                block_map_implicit_entry> block_map_entry;

    // [184] 	c-l-block-seq-entry(n) 	::= 	“-” /* Not followed by an ns-char */
    //                                          s-l+block-indented(n,block-in)
    typedef internal::all_of<internal::simple_char_clause<'-', false>,
                             internal::not_clause<non_white_char>,
                             internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, block_indented>
                            > block_seq_entry;

    // [186] 	ns-l-compact-sequence(n) 	::= 	c-l-block-seq-entry(n)
    //                                                  ( s-indent(n) c-l-block-seq-entry(n) )* 
    class compact_sequence : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [183] 	l+block-sequence(n) 	::= 	( s-indent(n+m) c-l-block-seq-entry(n+m) )+
    //                                          /* For some fixed auto-detected m > 0 */ 
    class block_sequence : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };
    
    // [187] 	l+block-mapping(n) 	::= 	( s-indent(n+m) ns-l-block-map-entry(n+m) )+
    //                                          /* For some fixed auto-detected m > 0 */ 
    class block_mapping : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };
  }
}

#endif // BLOCK_CLAUSES_HH
