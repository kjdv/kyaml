#ifndef DOCUMENT_CLAUSES_HH
#define DOCUMENT_CLAUSES_HH

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "comment_clauses.hh"
#include "directive_clauses.hh"
#include "block_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    // [202] 	l-document-prefix 	::= 	c-byte-order-mark? l-comment* 
    typedef internal::and_clause<internal::zero_or_one<byte_order_mark>,
                                 internal::zero_or_more<line_comment> > document_prefix;

                                                  
    // [203] 	c-directives-end 	::= 	“-” “-” “-” 
    typedef internal::all_of<internal::simple_char_clause<'-', false>,
                             internal::simple_char_clause<'-', false>,
                             internal::simple_char_clause<'-', false> > directives_end;

    // [204] 	c-document-end 	::= 	“.” “.” “.” 
    typedef internal::all_of<internal::simple_char_clause<'.', false>,
                             internal::simple_char_clause<'.', false>,
                             internal::simple_char_clause<'.', false> > document_end;

    // [205] 	l-document-suffix 	::= 	c-document-end s-l-comments
    typedef internal::and_clause<document_end,
                                 sline_comment> document_suffix;

    // [206] 	c-forbidden 	::= 	/* Start of line */
    //                                  ( c-directives-end | c-document-end )
    //                                  ( b-char | s-white | /* End of file */ ) 
    typedef internal::all_of<internal::start_of_line,
                             internal::or_clause<directives_end, document_end>,
                             internal::any_of<break_char,
                                              white,
                                              internal::endoffile
                                              >
                             > forbidden;

    // [207] 	l-bare-document 	::= 	s-l+block-node(-1,block-in)
                                                /* Excluding c-forbidden content */ 
    typedef internal::state_scope<
              internal::and_clause<internal::indent_modifier<-1>,
                                   internal::flow_modifier<context::BLOCK_IN>
                                  >,
              block_node
            > bare_document;


    // [208] 	l-explicit-document 	::= 	c-directives-end
    //                                          ( l-bare-document
    //                                          | ( e-node s-l-comments ) )
    typedef internal::and_clause<directives_end,
                                 internal::or_clause<bare_document,
                                                     internal::and_clause<enode,
                                                                          sline_comment>
                                                     >
                                 > explicit_document;

    // [209] 	l-directive-document 	::= 	l-directive+
    //                                          l-explicit-document 	 
    typedef internal::and_clause<internal::one_or_more<ldirective>,
                                 explicit_document> directive_document;

    // [210] 	l-any-document 	::= 	  l-directive-document
    //                                  | l-explicit-document
    //                                  | l-bare-document
    typedef internal::any_of<directive_document,
                             explicit_document,
                             bare_document> any_document;

    // [211] 	l-yaml-stream 	::= 	l-document-prefix* l-any-document?
    //                                  ( l-document-suffix+ l-document-prefix* l-any-document?
    //                                  | l-document-prefix* l-explicit-document? )* // TODO: resolve ambiguous |
    typedef internal::all_of<internal::zero_or_more<document_prefix>,
                             internal::zero_or_one<any_document>,
                             internal::zero_or_more<
                               internal::or_clause<
                                 internal::all_of<
                                   internal::one_or_more<document_suffix>,
                                   internal::zero_or_more<document_prefix>,
                                   internal::zero_or_one<any_document>
                                   >,
                                 internal::and_clause<
                                   internal::zero_or_more<document_prefix>,
                                   internal::zero_or_one<explicit_document>
                                   >
                                 >
                               >
                             > yaml_stream;
                                   
  }
}

#endif // DOCUMENT_CLAUSES_HH
