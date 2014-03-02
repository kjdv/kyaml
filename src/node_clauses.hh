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
//    typedef internal::and_clause<internal::simple_char_clause<'&'>,
    //                               anchor_name> anchor_property;

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
    typedef internal::all_of<internal::simple_char_clause<'!'>,
                             internal::simple_char_clause<'<'>,
                             internal::one_or_more<uri_char>,
                             internal::simple_char_clause<'>'> > verbatim_tag;

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
                                                   
  
  }
}

#endif // NODE_CLAUSES_HH
