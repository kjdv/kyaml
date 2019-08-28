#ifndef DIRECTIVE_CLAUSES_HH
#define DIRECTIVE_CLAUSES_HH

#include "char_clauses.hh"
#include "structure_clauses.hh"
#include "comment_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [95] 	ns-global-tag-prefix 	::= 	ns-tag-char ns-uri-char* 
    typedef internal::and_clause<tag_char,
                                 internal::zero_or_more<uri_char> > global_tag_prefix;

    // [94] 	c-ns-local-tag-prefix 	::= 	“!” ns-uri-char* 
    typedef internal::and_clause<internal::simple_char_clause<'!'>,
                                 internal::zero_or_more<uri_char> > local_tag_prefix;

    // [93] 	ns-tag-prefix 	::= 	c-ns-local-tag-prefix | ns-global-tag-prefix
    typedef internal::or_clause<local_tag_prefix,
                                global_tag_prefix> tag_prefix;

    // [92] 	c-named-tag-handle 	::= 	“!” ns-word-char+ “!”
    typedef internal::all_of<internal::simple_char_clause<'!'>,
                             internal::one_or_more<word_char>,
                             internal::simple_char_clause<'!'> > named_tag_handle;

    // [91] 	c-secondary-tag-handle 	::= 	“!” “!”
    typedef internal::and_clause<internal::simple_char_clause<'!'>,
                                 internal::simple_char_clause<'!'> > secondary_tag_handle;

    // [90] 	c-primary-tag-handle 	::= 	“!”
    typedef internal::simple_char_clause<'!'> primary_tag_handle;

    // [89] 	c-tag-handle 	::= 	  c-named-tag-handle
    //                                  | c-secondary-tag-handle
    //                                  | c-primary-tag-handle
    typedef internal::any_of<named_tag_handle,
                             secondary_tag_handle,
                             primary_tag_handle> tag_handle;

    // [88] 	ns-tag-directive 	::= 	“T” “A” “G”
    //                                          s-separate-in-line c-tag-handle
    //                                          s-separate-in-line ns-tag-prefix  
    typedef internal::all_of<internal::simple_char_clause<'T'>,
                             internal::simple_char_clause<'A'>,
                             internal::simple_char_clause<'G'>,
                             separate_in_line,
                             tag_handle,
                             separate_in_line,
                             tag_prefix> tag_directive;
  

    // [87] 	ns-yaml-version 	::= 	ns-dec-digit+ “.” ns-dec-digit+ 
    typedef internal::all_of<internal::one_or_more<dec_digit_char>,
                             internal::simple_char_clause<'.'>,
                             internal::one_or_more<dec_digit_char> > yaml_version;

    // [86] 	ns-yaml-directive 	::= 	“Y” “A” “M” “L”
    //                                          s-separate-in-line ns-yaml-version
    typedef internal::all_of<internal::simple_char_clause<'Y'>,
                             internal::simple_char_clause<'A'>,
                             internal::simple_char_clause<'M'>,
                             internal::simple_char_clause<'L'>,
                             separate_in_line,
                             yaml_version> yaml_directive;

    // [85] 	ns-directive-parameter 	::= 	ns-char+ 
    typedef internal::one_or_more<non_white_char> directive_parameter;

    // [84] 	ns-directive-name 	::= 	ns-char+ 
    typedef internal::one_or_more<non_white_char> directive_name;

    // [83] 	ns-reserved-directive 	::= 	ns-directive-name 
    //                                           ( s-separate-in-line ns-directive-parameter )* 
  
    typedef internal::and_clause<directive_name,
                                 internal::zero_or_more<
                                   internal::and_clause<separate_in_line,
                                                        directive_parameter> >
                                 > reserved_directive;

    // [82] 	l-directive 	::= 	“%”
    //                                  ( ns-yaml-directive
    //                                    | ns-tag-directive
    //                                    | ns-reserved-directive )
    //                                      s-l-comments
    typedef internal::all_of<internal::simple_char_clause<'%'>,
                             internal::any_of<yaml_directive,
                                              tag_directive,
                                              reserved_directive>,
                             sline_comment> ldirective;
  }
}

#endif // DIRECTIVE_CLAUSES_HH
