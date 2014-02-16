#include "char_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  bool is_utf8_lead_byte(char b)
  {
    return (b & 0xc0) == 0xc0;
  }

  bool is_utf8_cont_byte(char b)
  {
    return (b & 0xc0) == 0x80;
  }
}

bool printable::try_clause()
{
  char c;
  if(!stream().peek(c))
    return false;
  if(c >=0 &&
     (
       c == '\x9' ||
       c == '\xa' ||
       c == '\xd' ||
       (c >= '\x20' && c <= '\x7e') ||
       c == '\x85'
     ))
  {
    consume(c);
    return true;
  }
  else if(is_utf8_lead_byte(c))
  {
    consume(c);
    // no extensive checking done, we just assume all utf8 is printable (to improve)
    while(stream().peek(c) && is_utf8_cont_byte(c))
      consume(c);
    return true;
  }

  return false;
}

bool json::try_clause()
{
  char c;
  if(!stream().peek(c))
    return false;

  if(c >=0 &&
     (
       c == '\x9' ||
       (c >= '\x20' && c <= '\x7f')
     ))
  {
    consume(c);
    return true;
  }
  else if(is_utf8_lead_byte(c))
  {
    consume(c);
    // no extensive checking done, we just assume all utf8 is printable (to improve)
    while(stream().peek(c) && is_utf8_cont_byte(c))
      consume(c);
    return true;
  }

  return false;
}

bool reserved::try_clause()
{
  char c;
  if(stream().peek(c) &&
     c >=0 &&
     ( c == '@' || c == '`' ))
  {
    consume(c);
    return true;
  }
  return false;
}

bool indicator::try_clause()
{
  char c = 0;
  stream().peek(c);

  switch(c)
  {
  case '-':
  case '?':
  case ':':
  case ',':
  case '[':
  case ']':
  case '{':
  case '}':
  case '#':
  case '&':
  case '*':
  case '!':
  case '>':
  case '\'':
  case '"':
  case '%':
  case '@':
  case '`':
    consume(c);
    return true;

  default:
    return false;
  }
}

bool flow_indicator::try_clause()
{
  char c = 0;
  stream().peek(c);
  
  switch(c)
  {
  case ',':
  case '[':
  case ']':
  case '{':
  case '}':
    consume(c);
    return true;

  default:
    return false;
  }
}

bool break_char::try_clause()
{
  line_feed lf(stream());
  carriage_return cr(stream());

  if(lf.try_clause())
  {
    set(lf.value());
    return true;
  }
  else if(cr.try_clause())
  {
    set(cr.value());
    return true;
  }

  return false;
}

bool non_break_char::try_clause()
{
  break_char bc(stream());
  if(bc.try_clause())
  {
    bc.unwind();
    return false;
  }

  printable pr(stream());
  if(pr.try_clause())
  {
    set(pr.value());
    return true;
  }

  return false;
}

bool line_break::try_clause()
{
  d_value.clear();

  carriage_return cr(stream());
  line_feed lf(stream());

  string val;
  if(cr.try_clause())
  {
    val += cr.value();
    if(lf.try_clause())
      val += lf.value();
  }
  else if(lf.try_clause())
  {
    val += lf.value();
  }

  if(!val.empty())
  {
    d_value = val;
    return true;
  }
  return false;
}

bool white::try_clause()
{
  space s(stream());
  tab t(stream());

  if(s.try_clause())
  {
    set(s.value());
    return true;
  }
  else if(t.try_clause())
  {
    set(t.value());
    return true;
  }

  return false;
}

bool non_white_char::try_clause()
{
  white w(stream());
  if(w.try_clause())
  {
    w.unwind();
    return false;
  }

  non_break_char nb(stream());
  if(nb.try_clause())
  {
    set(nb.value());
    return true;
  }

  return false;
}

bool dec_digit_char::try_clause()
{
  char c;
  if(stream().peek(c) && 
     (c >= '0' && c <= '9'))
  {
    consume(c);
    return true;
  }
  return false;
}

bool hex_digit_char::try_clause()
{
  dec_digit_char d(stream());
  if(d.try_clause())
  {
    set(d.value());
    return true;
  }
  else
  {
    char c;
    if(stream().peek(c) && 
       ((c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')))
    {
      consume(c);
      return true;
    }
  }
  return false;
}

bool ascii_letter::try_clause()
{
  char c;
  if(stream().peek(c) &&
     ((c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z')))
  {
    consume(c);
    return true;
  }
  return false;
}

bool word_char::try_clause()
{
  dec_digit_char d(stream());
  if(d.try_clause())
  {
    set(d.value());
    return true;
  }

  ascii_letter a(stream());
  if(a.try_clause())
  {
    set(a.value());
    return true;
  }
  
  char c;
  if(stream().peek(c) &&
     c == '-')
  {
    consume(c);
    return true;
  }
  return false;
}

bool uri_char::try_clause()
{
  d_value.clear();

  char c;
  if(!stream().peek(c))
    return false;

  if(c == '%')
  {
    d_value += (char)c;
    advance(1);

    hex_digit_char h1(stream());
    hex_digit_char h2(stream());
    if(h1.try_clause() && h2.try_clause())
    {
      d_value += h1.value();
      d_value += h2.value();
      return true;
    }
    else
    {
      unwind();
      return false;
    }
  }

  switch(c)
  {
  case '#':
  case ';':
  case '/':
  case '?':
  case ':':
  case '@':
  case '&':
  case '=':
  case '+':
  case '$':
  case ',':
  case '_':
  case '.':
  case '!':
  case '~':
  case '*':
  case '\'':
  case '(':
  case ')':
  case '[':
  case ']':
    d_value += (char)c;
    advance(1);
    return true;

  default:
    return false;
  } 
}

bool tag_char::try_clause()
{
  char c;
  if(!stream().peek(c))
    return false;

  if(c == '!')
    return false;

  flow_indicator fi(stream());
  if(fi.try_clause())
  {
    unwind();
    return false;
  }

  uri_char u(stream());
  if(u.try_clause())
  {
    d_value = u.value();
    return true;
  }

  return false;
}

// helper for escape_char
template <typename C>
bool tryclause(char_stream &str, string &value)
{
  C c(str);
  if(c.try_clause())
  {
    value += c.value();
    return true;
  }
  return false;
}

bool esc_char::try_clause()
{
  char c;
  if(!stream().peek(c))
    return false;

  if(c == '\\')
  {
    d_value += (char)c;
    advance(1);

    if(tryclause<esc_null>(stream(), d_value) ||
       tryclause<esc_bell>(stream(), d_value) ||
       tryclause<esc_backspace>(stream(), d_value) ||
       tryclause<esc_htab>(stream(), d_value) ||
       tryclause<esc_linefeed>(stream(), d_value) ||
       tryclause<esc_vtab>(stream(), d_value) ||
       tryclause<esc_form_feed>(stream(), d_value) ||
       tryclause<esc_carriage_return>(stream(), d_value) ||
       tryclause<esc_escape>(stream(), d_value) ||
       tryclause<esc_space>(stream(), d_value) ||
       tryclause<esc_slash>(stream(), d_value) ||
       tryclause<esc_bslash>(stream(), d_value) ||
       tryclause<esc_dquote>(stream(), d_value) ||
       tryclause<esc_next_line>(stream(), d_value) ||
       tryclause<esc_non_break_space>(stream(), d_value) ||
       tryclause<esc_line_separator>(stream(), d_value) ||       
       tryclause<esc_paragraph_separator>(stream(), d_value) ||
       tryclause<esc_unicode_8b>(stream(), d_value) ||
       tryclause<esc_unicode_16b>(stream(), d_value) ||
       tryclause<esc_unicode_32b>(stream(), d_value))
    {
      return true;
    }
    else
    {
      unwind();
    }
  }
  return false;
}
