#include "char_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

static_assert(sizeof(char_t) == 4, "");

bool printable::parse(document_builder &builder)
{
  char_t c;
  if(!stream().peek(c))
    return false;
  if(c == '\x9' ||
     c == '\xa' ||
     c == '\xd' ||
     (c >= '\x20' && c <= '\x7e') ||
     c == static_cast<char_t>('\x85'))
  {
    builder.add(name(), c);
    advance();
    return true;
  }
  else if(c >= 0xff) // must be utf8
  {
    // no extensive checking done, we just assume all utf8 is printable (to improve)
    builder.add(name(), c);
    advance();
    return true;
  }

  return false;
}

bool json::parse(document_builder &builder)
{
  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '\x9' ||
     (c >= '\x20' && c <= '\x7f'))
  {
    builder.add(name(), c);
    advance();
    return true;
  }
  else if(c >= 0xff) // must be utf8
  {
    // no extensive checking done, we just assume all utf8 is printable (to improve)
    builder.add(name(), c);
    advance();
    return true;
  }

  return false;
}

bool byte_order_mark::parse(document_builder &builder)
{
  char_t c;
  if(!stream().peek(c))
    return false;
  if(c == 0x0000feff)
  {
    document_builder::child_t b = builder.child();

    b->add(name(), c);
    advance();
    return true;
    if(stream().peek(c) && c == static_cast<char_t>('\xff'))
    {
      b->add(name(), c);
      advance();
      builder.add(name(), c);
      return true;
    }
    else
      unwind();
  }
  return false;
}

bool reserved::parse(document_builder &builder)
{
  char_t c;
  if(stream().peek(c) &&
     ( c == '@' || c == '`' ))
  {
    builder.add(name(), c);
    advance();
    return true;
  }
  return false;
}

bool indicator::parse(document_builder &builder)
{
  char_t c = 0;
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
    builder.add(name(), c);
    advance();
    return true;

  default:
    return false;
  }
}

bool flow_indicator::parse(document_builder &builder)
{
  char_t c = 0;
  stream().peek(c);
  
  switch(c)
  {
  case ',':
  case '[':
  case ']':
  case '{':
  case '}':
    builder.add(name(), c);
    advance();
    return true;

  default:
    return false;
  }
}

bool non_break_char::parse(document_builder &builder)
{
  document_builder::child_t b = builder.child();

  break_char bc(ctx());
  if(bc.parse(*b))
  {
    unwind();
    return false;
  }

  byte_order_mark bo(ctx());
  if(bo.parse(*b))
  {
    unwind();
    return false;
  }

  printable pr(ctx());
  return pr.parse(builder);
}

#ifdef COMPILE_GUARD
bool line_break::try_clause()
{
  clear();

  carriage_return cr(ctx());
  line_feed lf(ctx());

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
    set(val);
    return true;
  }
  return false;
}

bool white::try_clause()
{
  space s(ctx());
  tab t(ctx());

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
  white w(ctx());
  if(w.try_clause())
  {
    unwind();
    return false;
  }

  non_break_char nb(ctx());
  if(nb.try_clause())
  {
    set(nb.value());
    return true;
  }

  return false;
}

bool dec_digit_char::try_clause()
{
  char_t c;
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
  dec_digit_char d(ctx());
  if(d.try_clause())
  {
    set(d.value());
    return true;
  }
  else
  {
    char_t c;
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
  char_t c;
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
  dec_digit_char d(ctx());
  if(d.try_clause())
  {
    set(d.value());
    return true;
  }

  ascii_letter a(ctx());
  if(a.try_clause())
  {
    set(a.value());
    return true;
  }
  
  char_t c;
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
  clear();

  word_char wc(ctx());
  if(wc.try_clause())
  {
    append(wc.value());
    return true;
  }

  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '%')
  {
    consume(c);

    hex_digit_char h1(ctx());
    hex_digit_char h2(ctx());
    if(h1.try_clause() && h2.try_clause())
    {
      append(h1.value());
      append(h2.value());
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
    append(c);
    advance();
    return true;

  default:
    return false;
  } 
}

bool tag_char::try_clause()
{
  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '!')
    return false;

  flow_indicator fi(ctx());
  if(fi.try_clause())
  {
    unwind();
    return false;
  }

  uri_char u(ctx());
  if(u.try_clause())
  {
    append(u.value());
    return true;
  }

  return false;
}

// helper for escape_char
template <typename C>
bool tryclause(context &ctx, string &value)
{
  C c(ctx);
  if(c.try_clause())
  {
    append_utf8(value, c.value());
    return true;
  }
  return false;
}

bool esc_char::try_clause()
{
  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '\\')
  {
    consume(c);

    string val;
    if(tryclause<esc_null>(ctx(), val) ||
       tryclause<esc_bell>(ctx(), val) ||
       tryclause<esc_backspace>(ctx(), val) ||
       tryclause<esc_htab>(ctx(), val) ||
       tryclause<esc_linefeed>(ctx(), val) ||
       tryclause<esc_vtab>(ctx(), val) ||
       tryclause<esc_form_feed>(ctx(), val) ||
       tryclause<esc_carriage_return>(ctx(), val) ||
       tryclause<esc_escape>(ctx(), val) ||
       tryclause<esc_space>(ctx(), val) ||
       tryclause<esc_slash>(ctx(), val) ||
       tryclause<esc_bslash>(ctx(), val) ||
       tryclause<esc_dquote>(ctx(), val) ||
       tryclause<esc_next_line>(ctx(), val) ||
       tryclause<esc_non_break_space>(ctx(), val) ||
       tryclause<esc_line_separator>(ctx(), val) ||       
       tryclause<esc_paragraph_separator>(ctx(), val) ||
       tryclause<esc_unicode_8b>(ctx(), val) ||
       tryclause<esc_unicode_16b>(ctx(), val) ||
       tryclause<esc_unicode_32b>(ctx(), val))
    {
      append(val);
      return true;
    }
    else
      unwind();
  }
  return false;
}

#endif // COMPILE_GUARD
