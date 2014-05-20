#ifndef FLOW_CLAUSES_HH
#define FLOW_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"
#include "node_clauses.hh"

namespace kyaml
{
  namespace clauses
  {  
    // [105] 	e-scalar 	::= 	 Empty 
    class empty_scalar : public clause
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

    // [106] 	e-node 	::= 	e-scalar
    typedef empty_scalar empty_node;

    // [107] 	nb-double-char 	::= 	c-ns-esc-char | ( nb-json - “\” - “"” ) 
    typedef internal::or_clause<esc_char,
                                internal::all_of<
                                  internal::not_clause<internal::simple_char_clause<'\\'> >,
                                  internal::not_clause<internal::simple_char_clause<'"'> >,
                                  json > > b_double_char;

    // [108] 	ns-double-char 	::= 	nb-double-char - s-white
    typedef internal::and_clause<internal::not_clause<white>,
                                 b_double_char> s_double_char;

    // [136] 	in-flow(c) 	::= 	c = flow-out  ⇒ flow-in
    //                                  c = flow-in   ⇒ flow-in
    //                                  c = block-key ⇒ flow-key
    //                                  c = flow-key  ⇒ flow-key 
    class in_flow : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [156] 	ns-flow-yaml-content(n,c) 	::= 	ns-plain(n,c)
    typedef plain flow_yaml_content;

    // [157] 	c-flow-json-content(n,c) 	::= 	  c-flow-sequence(n,c) | c-flow-mapping(n,c)
    //                                        | c-single-quoted(n,c) | c-double-quoted(n,c)
    class flow_json_content : public clause // not as typedef to break cyclic dependencies
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    };

    // [158] 	ns-flow-content(n,c) 	::= 	ns-flow-yaml-content(n,c) | c-flow-json-content(n,c)
    typedef internal::or_clause<flow_yaml_content, flow_json_content> flow_content;

    // [159] 	ns-flow-yaml-node(n,c) 	::= 	  c-ns-alias-node
    //                                          | ns-flow-yaml-content(n,c)
    //                                          | ( c-ns-properties(n,c)
    //                                              ( ( s-separate(n,c)
    //                                                  ns flow-yaml-content(n,c) 
    //                                                )
    //                                                | e-scalar 
    //                                              ) 
    //                                            )
    typedef internal::any_of<alias_node,
                             flow_yaml_content,
                             internal::and_clause<properties,
                                                  internal::or_clause<internal::and_clause<separate,
                                                                                           flow_yaml_content>,
                                                                      escalar
                                                                     >
                                                 >
                            > flow_yaml_node;    
    // [160] 	c-flow-json-node(n,c) 	::= 	( c-ns-properties(n,c) s-separate(n,c) )?
    //                                            c-flow-json-content(n,c)
    typedef internal::and_clause<internal::zero_or_one<internal::and_clause<properties,
                                                                            separate> >,
                                 flow_json_content> flow_json_node;

    // [161] 	ns-flow-node(n,c) 	::= 	  c-ns-alias-node
    //                                          | ns-flow-content(n,c)
    //                                          | ( c-ns-properties(n,c)
    //                                              ( ( s-separate(n,c)
    //                                                  ns-flow-content(n,c) 
    //                                                )
    //                                                | e-scalar 
    //                                              ) 
    //                                            )
    typedef internal::any_of<alias_node,
                             flow_content,
                             internal::and_clause<properties,
                                                  internal::or_clause<internal::and_clause<separate,
                                                                                           flow_content>,
                                                                      escalar
                                                                     >
                                                 >
                            > flow_node;
 
    // [149] 	c-ns-flow-map-adjacent-value(n,c) 	::= 	“:” ( ( s-separate(n,c)?
    //                                                                  ns-flow-node(n,c) 
    //                                                                )
    //                                                                | e-node 
    //                                                              ) /* Value */ 
    typedef internal::and_clause<internal::simple_char_clause<':', false>,
                                 internal::or_clause<internal::and_clause<internal::zero_or_one<separate>,
                                                                          flow_node>,
                                                     enode>
                                 > flow_map_adjacent_value;

    // [148] 	c-ns-flow-map-json-key-entry(n,c) 	::= 	c-flow-json-node(n,c)
    //                                                          ( ( s-separate(n,c)?
    //                                                              c-ns-flow-map-adjacent-value(n,c) 
    //                                                            )
    //                                                            | e-node 
    //                                                          ) 
    typedef internal::and_clause<flow_json_node,
                                 internal::or_clause<internal::and_clause<internal::zero_or_one<separate>,
                                                                          flow_map_adjacent_value>,
                                                     enode>
                                 > flow_map_json_key_entry;

    // [147] 	c-ns-flow-map-separate-value(n,c) 	::= 	“:” /* Not followed by an ns-plain-safe(c) */
    //                                                              ( ( s-separate(n,c) ns-flow-node(n,c) )
    //                                                           | e-node /* Value */ ) 
    typedef internal::and_clause<internal::simple_char_clause<':', false>,
                                 internal::or_clause<internal::and_clause<separate,
                                                                          flow_node
                                                                         >,
                                                     enode>
                                > flow_map_separate_value;

    // [146] 	c-ns-flow-map-empty-key-entry(n,c) 	::= 	e-node /* Key */
    //                                                          c-ns-flow-map-separate-value(n,c)
    typedef internal::and_clause<enode, flow_map_separate_value> flow_map_empty_key_entry;

    // [145] 	ns-flow-map-yaml-key-entry(n,c) 	::= 	ns-flow-yaml-node(n,c)
    //                                                          ( ( s-separate(n,c)?
    //                                                              c-ns-flow-map-separate-value(n,c) 
    //                                                            )
    //                                                            | e-node 
    //                                                          ) 
    typedef internal::and_clause<flow_yaml_node,
                                 internal::or_clause<internal::and_clause<internal::zero_or_one<separate>,
                                                                          flow_map_separate_value>,
                                                     enode>
                                 > flow_map_yaml_key_entry;

    // [144] 	ns-flow-map-implicit-entry(n,c) 	::= 	  ns-flow-map-yaml-key-entry(n,c)
    //                                                          | c-ns-flow-map-empty-key-entry(n,c)
    //                                                          | c-ns-flow-map-json-key-entry(n,c)
    typedef internal::any_of<flow_map_yaml_key_entry,
                             flow_map_empty_key_entry,
                             flow_map_json_key_entry> flow_map_implicit_entry;

    // [143] 	ns-flow-map-explicit-entry(n,c) 	::= 	  ns-flow-map-implicit-entry(n,c)
    //                                                           | ( e-node  Key 
    //                                                              e-node  Value  ) 
    typedef internal::or_clause<flow_map_implicit_entry,
                                internal::and_clause<enode, enode> > flow_map_explicit_entry;

    // [142] 	ns-flow-map-entry(n,c) 	::= 	  ( “?” s-separate(n,c)
    //                                              ns-flow-map-explicit-entry(n,c) 
    //                                            )
    //                                            | ns-flow-map-implicit-entry(n,c)
    typedef internal::or_clause<internal::all_of<internal::simple_char_clause<'?', false>,
                                                 separate,
                                                 flow_map_explicit_entry>,
                                flow_map_implicit_entry> flow_map_entry;

    // [155] 	c-s-implicit-json-key(c) 	::= 	c-flow-json-node(n/a,c) s-separate-in-line?
    //                                               /* At most 1024 characters altogether */ 
    typedef internal::and_clause<flow_json_node,
                                 internal::zero_or_one<separate_in_line> > implicit_json_key; // TODO: "at most 1024 characters" not enforced
    
    // [154] 	ns-s-implicit-yaml-key(c) 	::= 	ns-flow-yaml-node(n/a,c) s-separate-in-line?
    //                                               /* At most 1024 characters altogether */ 
    typedef internal::and_clause<flow_yaml_node,
                                 internal::zero_or_one<separate_in_line> > implicit_yaml_key; // TODO: "at most 1024 characters" not enforced

    // [153] 	c-ns-flow-pair-json-key-entry(n,c) 	::= 	c-s-implicit-json-key(flow-key)
    //                                                          c-ns-flow-map-adjacent-value(n,c)
    typedef internal::and_clause<implicit_json_key,
                                 flow_map_adjacent_value> flow_pair_json_key_entry;

    // [152] 	ns-flow-pair-yaml-key-entry(n,c) 	::= 	ns-s-implicit-yaml-key(flow-key)
    //                                                          c-ns-flow-map-separate-value(n,c)
    typedef internal::and_clause<implicit_yaml_key,
                                 flow_map_separate_value> flow_pair_yaml_key_entry;

    // [151] 	ns-flow-pair-entry(n,c) 	::= 	  ns-flow-pair-yaml-key-entry(n,c)
    //                                                  | c-ns-flow-map-empty-key-entry(n,c)
    //                                                  | c-ns-flow-pair-json-key-entry(n,c)
    typedef internal::any_of<flow_pair_yaml_key_entry,
                             flow_map_empty_key_entry,
                             flow_pair_json_key_entry> flow_pair_entry;

  
    // [150] 	ns-flow-pair(n,c) 	::= 	  ( “?” s-separate(n,c)
    //                                             ns-flow-map-explicit-entry(n,c) )
    //                                            | ns-flow-pair-entry(n,c)
    typedef internal::or_clause<internal::all_of<internal::simple_char_clause<'?', false>,
                                                 separate,
                                                 flow_map_explicit_entry>,
                                flow_pair_entry> flow_pair;
    // [141] 	ns-s-flow-map-entries(n,c) 	::= 	ns-flow-map-entry(n,c) s-separate(n,c)?
    //                                                  ( “,” s-separate(n,c)?
    //                                                    ns-s-flow-map-entries(n,c)? 
    //                                                  )
    class flow_map_entries : public clause
    {
    public:
      using clause::clause;
      
      bool parse(document_builder &builder)
      {
        i_flow_map_entries fme(ctx());
        return fme.parse(builder);
      }

    private:
      typedef internal::all_of<flow_map_entry,
                               internal::zero_or_one<separate>,
                               internal::all_of<internal::simple_char_clause<'?', false>,
                                                internal::zero_or_one<separate>,
                                                internal::zero_or_one<flow_map_entries> > > i_flow_map_entries;
    };

    // [140] 	c-flow-mapping(n,c) 	::= 	“{” s-separate(n,c)?
    //                                              ns-s-flow-map-entries(n,in-flow(c))? “}”
    typedef internal::all_of<mapping_start,
                             internal::zero_or_one<separate>,
                             internal::zero_or_one<internal::state_scope<in_flow, flow_map_entries> >,
                             mapping_end
                             > flow_mapping;

    // [139] 	ns-flow-seq-entry(n,c) 	::= 	ns-flow-pair(n,c) | ns-flow-node(n,c)
    typedef internal::or_clause<flow_pair, flow_node> flow_seq_entry;

    // [138] 	ns-s-flow-seq-entries(n,c) 	::= 	ns-flow-seq-entry(n,c) s-separate(n,c)?
    //                                                  ( “,” s-separate(n,c)?
    //                                                    ns-s-flow-seq-entries(n,c)? 
    //                                                  )? 
    class flow_seq_entries : public clause
    {
    public:
      using clause::clause;
      
      bool parse(document_builder &builder)
      {
        i_flow_seq_entries fse(ctx());
        return fse.parse(builder);
      }

    private:
      typedef internal::all_of<flow_seq_entry,
                               internal::zero_or_one<separate>,
                               internal::zero_or_one<internal::all_of<internal::simple_char_clause<',', false>,
                                                                      internal::zero_or_one<separate>,
                                                                      internal::zero_or_one<flow_seq_entries> >
                                                     >
                               > i_flow_seq_entries;
    };

    // [137] 	c-flow-sequence(n,c) 	::= 	“[” s-separate(n,c)?
    //                                              ns-s-flow-seq-entries(n,in-flow(c))? “]”
    typedef internal::all_of<sequence_start,
                             internal::zero_or_one<separate>,
                             internal::zero_or_one<internal::state_scope<in_flow, flow_seq_entries> >,
                             sequence_end> flow_sequence;
                                                             
                                
  }
}

#endif // FLOW_CLAUSES_HH
