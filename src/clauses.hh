#ifndef CLAUSES_HH
#define CLAUSES_HH

#include <string>
#include "peekable_stream.hh"

namespace kyaml
{
  typedef peekable_stream char_stream;
  
  namespace clauses
  {
    // base class is informational only: there are no virtual functions, its here only to document
    // the common signature
    class clause
    {
    public:
      clause(char_stream &stream) : d_stream(stream)
      {}
      
      // returns true if the head of the stream is of this type, should leave the stream unmodified
      bool try_clause(); 
      
      // return clause-depentend value of this, should only be called if try_clause returned true
      // value_t consume();
      // void unwind(value_t)
      
      // note: public methods are not implemented, they should not be called directly
      
    protected:
      char_stream &stream()
      {
        return d_stream;
      }
      
    private:
      char_stream &d_stream;
    };
    
    // [1] 	c-printable 	::= 	  #x9 | #xA | #xD | [#x20-#x7E]
    class printable : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
      char consume()
      {
        return stream().getc();
      }

      void unwind(char c)
      {
        stream().putback(c);
      }
    };
    
    // [2] 	nb-json 	::= 	#x9 | [#x20-#x10FFFF] 
    class json : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
      
      char consume()
      {
        return stream().getc();
      }

      void unwind(char c)
      {
        stream().putback(c);
      }
    };

    // [3] 	c-byte-order-mark 	::= 	#xFEFF 
    // (we don't worry about utf for now)

    template <char value>
    class simple_char_clause : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause()
      {
        return stream().peekc() == value;
      }
      
      char consume()
      {
        return stream().getc();
      }

      void unwind(char c)
      {
        stream().putback(c);
      }
    };

    // [4]	c-sequence-entry	::=	“-” 
    typedef simple_char_clause<'-'> sequence_entry;

    // [5]	c-mapping-key	::=	“?” 
    typedef simple_char_clause<'?'> mapping_key;

    // [6]	c-mapping-value	::=	“:” 
    typedef simple_char_clause<':'> mapping_value;
 
    // [7]	c-collect-entry	::=	“,” 
    typedef simple_char_clause<','> collect_entry;

    // [8]	c-sequence-start	::=	“[”
    typedef simple_char_clause<'['> sequence_start;

    // [9]	c-sequence-end	::=	“]” 
    typedef simple_char_clause<']'> sequence_end;

    // [10]	c-mapping-start	::=	“{” 
    typedef simple_char_clause<'{'> mapping_start;

    // [11]	c-mapping-end	::=	“}” 
    typedef simple_char_clause<'}'> mapping_end;

    // [12]	c-comment	::=	“#” 
    typedef simple_char_clause<'#'> comment;

    // [13]	c-anchor	::=	“&” 
    typedef simple_char_clause<'&'> anchor;

    // [14]	c-alias	::=	“*” 
    typedef simple_char_clause<'*'> alias;

    // [15]	c-tag	::=	“!” 
    typedef simple_char_clause<'!'> tag;

    // [16]	c-literal	::=	“|” 
    typedef simple_char_clause<'|'> literal;

    // [17]	c-folded	::=	“>” 
    typedef simple_char_clause<'>'> folded;

    // [18]	c-single-quote	::=	“'” 
    typedef simple_char_clause<'\''> single_quote;

    // [19]	c-double-quote	::=	“"” 
    typedef simple_char_clause<'"'> double_quote;
    
    // [20]	c-directive	::=	“%” 
    typedef simple_char_clause<'%'> directive;

    // [21]	c-reserved	::=	“@” | “`” 
    class reserved : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
      
      char consume()
      {
        return stream().getc();
      }

      void unwind(char c)
      {
        stream().putback(c);
      }
    };

  }
}

#endif // CLAUSES_HH
