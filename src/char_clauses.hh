#ifndef CHAR_CLAUSES_HH
#define CHAR_CLAUSES_HH

#include <string>
#include "clauses_base.hh"

namespace kyaml
{
  namespace clauses
  {
    namespace internal
    {
      class single_char_clause : public clause
      {
      public:
        single_char_clause(char_stream &stream) : 
        clause(stream),
        d_value(0)
        {}
        
        char value()
      {
        return d_value;
      }
        
      protected:
        void consume(char c)
        {
          set(c);
          advance();
        }
        void set(char c)
        {
          d_value = c;
        }
      private:
        char d_value;
      };
    }
    
    // [1] 	c-printable 	::= 	  #x9 | #xA | #xD | [#x20-#x7E]
    class printable : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;

      bool try_clause();
    };
    
    // [2] 	nb-json 	::= 	#x9 | [#x20-#x10FFFF] 
    class json : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [3] 	c-byte-order-mark 	::= 	#xFEFF 
    // (we don't worry about utf for now)

    namespace internal
    {
      template <char char_value>
      class simple_char_clause : public internal::single_char_clause
      {
      public:
        using internal::single_char_clause::single_char_clause;
        
        bool try_clause()
        {
          if(stream().peek() == char_value)
        {
          consume(char_value);
          return true;
        }
          return false;
        }
      };
    }

    // [4]	c-sequence-entry	::=	“-” 
    typedef internal::simple_char_clause<'-'> sequence_entry;

    // [5]	c-mapping-key	::=	“?” 
    typedef internal::simple_char_clause<'?'> mapping_key;

    // [6]	c-mapping-value	::=	“:” 
    typedef internal::simple_char_clause<':'> mapping_value;
 
    // [7]	c-collect-entry	::=	“,” 
    typedef internal::simple_char_clause<','> collect_entry;

    // [8]	c-sequence-start	::=	“[”
    typedef internal::simple_char_clause<'['> sequence_start;

    // [9]	c-sequence-end	::=	“]” 
    typedef internal::simple_char_clause<']'> sequence_end;

    // [10]	c-mapping-start	::=	“{” 
    typedef internal::simple_char_clause<'{'> mapping_start;

    // [11]	c-mapping-end	::=	“}” 
    typedef internal::simple_char_clause<'}'> mapping_end;

    // [12]	c-comment	::=	“#” 
    typedef internal::simple_char_clause<'#'> comment;

    // [13]	c-anchor	::=	“&” 
    typedef internal::simple_char_clause<'&'> anchor;

    // [14]	c-alias	::=	“*” 
    typedef internal::simple_char_clause<'*'> alias;

    // [15]	c-tag	::=	“!” 
    typedef internal::simple_char_clause<'!'> tag;

    // [16]	c-literal	::=	“|” 
    typedef internal::simple_char_clause<'|'> literal;

    // [17]	c-folded	::=	“>” 
    typedef internal::simple_char_clause<'>'> folded;

    // [18]	c-single-quote	::=	“'” 
    typedef internal::simple_char_clause<'\''> single_quote;

    // [19]	c-double-quote	::=	“"” 
    typedef internal::simple_char_clause<'"'> double_quote;
    
    // [20]	c-directive	::=	“%” 
    typedef internal::simple_char_clause<'%'> directive;

    // [21]	c-reserved	::=	“@” | “`” 
    class reserved : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [22] 	c-indicator 	::= 	  “-” | “?” | “:” | “,” | “[” | “]” | “{” | “}”
    //                                  | “#” | “&” | “*” | “!” | “|” | “>” | “'” | “"”
    //                                  | “%” | “@” | “`”
    class indicator : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [23] 	c-flow-indicator 	::= 	“,” | “[” | “]” | “{” | “}”
    class flow_indicator : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [24] 	b-line-feed 	::= 	#xA    /* LF */ 
    typedef internal::simple_char_clause<'\n'> line_feed;

    // [25] 	b-carriage-return 	::= 	#xD    /* CR */ 
    typedef internal::simple_char_clause<'\r'> carriage_return;

    // [26] 	b-char 	::= 	b-line-feed | b-carriage-return
    class break_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [27] 	nb-char 	::= 	c-printable - b-char - c-byte-order-mark
    class non_break_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [28] 	b-break 	::= 	  ( b-carriage-return b-line-feed ) DOS, Windows 
    //                                  | b-carriage-return                 MacOS upto 9.x
    //                                  | b-line-feed                       UNIX, MacOS X
    class line_break : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
      
      std::string value()
      {
        return d_value;
      }
    private:
      std::string d_value;
    };

    // [29] 	b-as-line-feed 	::= 	b-break
    typedef line_break as_line_feed;

    // [30] 	b-non-content 	::= 	b-break
    typedef line_break non_content;

    // [31] 	s-space 	::= 	#x20 SP 
    typedef internal::simple_char_clause<' '> space;

    // [32] 	s-tab 	::= 	#x9  TAB  
    typedef internal::simple_char_clause<'\t'> tab;

    // [33] 	s-white 	::= 	s-space | s-tab
    class white : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [34] 	ns-char 	::= 	nb-char - s-white
    class non_white_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [35] 	ns-dec-digit 	::= 	[#x30-#x39] 0-9 
    class dec_digit_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [36] 	ns-hex-digit 	::= 	  ns-dec-digit
    //                                  | [#x41-#x46] A-F | [#x61-#x66]  a-f  
    class hex_digit_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [37] 	ns-ascii-letter 	::= 	[#x41-#x5A] A-Z  | [#x61-#x7A] a-z 
    class ascii_letter : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [38] 	ns-word-char 	::= 	ns-dec-digit | ns-ascii-letter | “-” 
    class word_char : public internal::single_char_clause
    {
    public:
      using internal::single_char_clause::single_char_clause;
      
      bool try_clause();
    };

    // [39] 	ns-uri-char 	::= 	  “%” ns-hex-digit ns-hex-digit | ns-word-char | “#”
    //                                  | “;” | “/” | “?” | “:” | “@” | “&” | “=” | “+” | “$” | “,”
    //                                  | “_” | “.” | “!” | “~” | “*” | “'” | “(” | “)” | “[” | “]” 
    class uri_char : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
       
      std::string value()
      {
        return d_value;
      }
    private:
      std::string d_value;
    };

    // [40] 	ns-tag-char 	::= 	ns-uri-char - “!” - c-flow-indicator
    class tag_char : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
       
      std::string value()
      {
        return d_value;
      }
    private:
      std::string d_value;
    };

    // [41] 	c-escape 	::= 	“\” 
    typedef internal::simple_char_clause<'\\'> escape;

    // [42]	ns-esc-null	::=	“0” 
    typedef internal::simple_char_clause<'0'> esc_null;

    // [43]	ns-esc-bell	::=	“a”
    typedef internal::simple_char_clause<'a'> esc_bell;

    // [44]	ns-esc-backspace	::=	“b” 
    typedef internal::simple_char_clause<'b'> esc_backspace;
    
    // [45]	ns-esc-horizontal-tab	::=	“t” | #x9 
    typedef internal::simple_char_clause<'t'> esc_htab;
    
    // [46]	ns-esc-line-feed	::=	“n” 
    typedef internal::simple_char_clause<'n'> esc_linefeed;

    // [47]	ns-esc-vertical-tab	::=	“v” 
    typedef internal::simple_char_clause<'v'> esc_vtab;

    // [48]	ns-esc-form-feed	::=	“f” 
    typedef internal::simple_char_clause<'f'> esc_form_feed;

    // [49]	ns-esc-carriage-return	::=	“r” 
    typedef internal::simple_char_clause<'r'> esc_carriage_return;

    // [50]	ns-esc-escape	::=	“e” 
    typedef internal::simple_char_clause<'e'> esc_escape;

    // [51]	ns-esc-space	::=	#x20 
    typedef internal::simple_char_clause<' '> esc_space;

    // [52]	ns-esc-double-quote	::=	“"”
    typedef internal::simple_char_clause<'"'> esc_dquote;

    // [53]	ns-esc-slash	::=	“/” 
    typedef internal::simple_char_clause<'/'> esc_slash;

    // [54]	ns-esc-backslash	::=	“\”
    typedef internal::simple_char_clause<'\\'> esc_bslash;

    // [55]	ns-esc-next-line	::=	“N” 
    typedef internal::simple_char_clause<'N'> esc_next_line;

    // [56]	ns-esc-non-breaking-space	::=	“_” 
    typedef internal::simple_char_clause<'_'> esc_non_break_space;

    // [57]	ns-esc-line-separator	::=	“L”
    typedef internal::simple_char_clause<'L'> esc_line_separator;

    // [58]	ns-esc-paragraph-separator	::=	“P” 
    typedef internal::simple_char_clause<'P'> esc_paragraph_separator;

    namespace internal
    {
      template <char escape, size_t size>
      class esc_unicode : public clause
      {
      public:
        using clause::clause;
        
        bool try_clause()
        {
          d_value.clear();
          int c = stream().peek();
          if(c == escape)
          {
            d_value += (char)c;
            advance(1);
            for(size_t i = 0; i < size; ++i)
            {
              hex_digit_char h(stream());
              if(h.try_clause())
                d_value += h.value();
              else
              {
                unwind();
              return false;
              }
            }
            return true;
          }
          return false;
        }
        
        std::string const &value()
        {
          return d_value;
        }
      private:
        std::string d_value;
      };
    }

    // [59]	ns-esc-8-bit	::=	“x”
    //                                 ( ns-hex-digit × 2 ) 
    typedef internal::esc_unicode<'x', 2> esc_unicode_8b;

    // [60]	ns-esc-16-bit	::=	“u”
    //                                 ( ns-hex-digit × 4 ) 
    typedef internal::esc_unicode<'u', 4> esc_unicode_16b;

    // [61]	ns-esc-32-bit	::=	“U”
    //                                 ( ns-hex-digit × 8 ) 
    typedef internal::esc_unicode<'U', 8> esc_unicode_32b;

    // [62] 	c-ns-esc-char 	::= 	“\”
    //                                ( ns-esc-null | ns-esc-bell | ns-esc-backspace
    //                                | ns-esc-horizontal-tab | ns-esc-line-feed
    //                                | ns-esc-vertical-tab | ns-esc-form-feed
    //                                | ns-esc-carriage-return | ns-esc-escape | ns-esc-space
    //                                | ns-esc-double-quote | ns-esc-slash | ns-esc-backslash
    //                                | ns-esc-next-line | ns-esc-non-breaking-space
    //                                | ns-esc-line-separator | ns-esc-paragraph-separator
    //                                | ns-esc-8-bit | ns-esc-16-bit | ns-esc-32-bit )
    class esc_char : public clause
    {
    public:
      using clause::clause;
      
      bool try_clause();
      
      std::string value()
      {
        return d_value;
      }
    private:
      std::string d_value;
    };
  }
}

#endif // CHAR_CLAUSES_HH
