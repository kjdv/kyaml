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
    builder.add_atom(c);
    advance();
    return true;
  }
  else if(c >= 0xff && is_valid_utf8(c))
  {
    builder.add_atom(c);
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
    builder.add_atom(c);
    advance();
    return true;
  }
  else if(c >= 0xff && is_valid_utf8(c))
  {
    builder.add_atom(c);
    advance();
    return true;
  }

  return false;
}

bool byte_order_mark::parse(document_builder &builder)
{
  inner ic(ctx());
  return internal::try_parse(ic, builder);
}

bool byte_order_mark::inner::parse(document_builder &builder)
{
  char_t c;
  if(!stream().peek(c))
    return false;
  if(c == 0x0000feff)
  {
    builder.add_atom(c);
    advance();
    return true;

    /*if(stream().peek(c) && c == static_cast<char_t>('\xff'))
    {
      builder.add_atom(c);
      advance();
      builder.add_atom(c);
      return true;
    }
    else
      unwind();*/
  }
  return false;
}

bool sequence_start::parse(document_builder &builder)
{
  internal::simple_char_clause<'[', false> d(ctx());
  null_builder b;
  if(d.parse(b))
  {
    builder.start_sequence();
    return true;
  }
  return false;
}

bool sequence_end::parse(document_builder &builder)
{
  internal::simple_char_clause<']', false> d(ctx());
  null_builder b;
  if(d.parse(b))
  {
    builder.end_sequence();
    return true;
  }
  return false;
}

bool mapping_start::parse(document_builder &builder)
{
  internal::simple_char_clause<'{', false> d(ctx());
  null_builder b;
  if(d.parse(b))
  {
    builder.start_mapping();
    return true;
  }
  return false;
}

bool mapping_end::parse(document_builder &builder)
{
  internal::simple_char_clause<'}', false> d(ctx());
  null_builder b;
  if(d.parse(b))
  {
    builder.end_mapping();
    return true;
  }
  return false;
}

bool reserved::parse(document_builder &builder)
{
  char_t c;
  if(stream().peek(c) &&
     ( c == '@' || c == '`' ))
  {
    builder.add_atom(c);
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
    builder.add_atom(c);
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
    builder.add_atom(c);
    advance();
    return true;

  default:
    return false;
  }
}

bool non_break_char::parse(document_builder &builder)
{
  null_builder dm;
  break_char bc(ctx());
  if(bc.parse(dm))
  {
    unwind();
    return false;
  }

  byte_order_mark bo(ctx());
  if(bo.parse(dm))
  {
    unwind();
    return false;
  }

  printable pr(ctx());
  return pr.parse(builder);
}

bool non_white_char::parse(document_builder &builder)
{
  null_builder dm;
  white w(ctx());
  if(w.parse(dm))
  {
    unwind();
    return false;
  }

  non_break_char nb(ctx());
  return nb.parse(builder);
}

bool dec_digit_char::parse(document_builder &builder)
{
  char_t c;
  if(stream().peek(c) && 
     (c >= '0' && c <= '9'))
  {
    builder.add_atom(c);
    advance();
    return true;
  }
  return false;
}

bool hex_digit_char::parse(document_builder &builder)
{
  dec_digit_char d(ctx());
  if(d.parse(builder))
    return true;
  else
  {
    char_t c;
    if(stream().peek(c) && 
       ((c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')))
    {
      builder.add_atom(c);
      advance();
      return true;
    }
  }
  return false;
}

bool ascii_letter::parse(document_builder &builder)
{
  char_t c;
  if(stream().peek(c) &&
     ((c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z')))
  {
    builder.add_atom(c);
    advance();
    return true;
  }
  return false;
}

bool word_char::parse(document_builder &builder)
{
  dec_digit_char d(ctx());
  if(d.parse(builder))
    return true;
  
  ascii_letter a(ctx());
  if(a.parse(builder))
    return true;
  
  char_t c;
  if(stream().peek(c) &&
     c == '-')
  {
    builder.add_atom(c);
    advance();
    return true;
  }
  return false;
}

bool uri_char::parse(document_builder &builder)
{
  inner ic(ctx());
  return internal::try_parse(ic, builder);
}

bool uri_char::inner::parse(document_builder &builder)
{
  word_char wc(ctx());
  if(wc.parse(builder))
    return true;

  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '%')
  {
    builder.add_atom(c);
    advance();

    hex_digit_char h1(ctx());
    hex_digit_char h2(ctx());
    return h1.parse(builder) && h2.parse(builder);
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
    builder.add_atom(c);
    advance();
    return true;

  default:
    return false;
  } 
}

bool tag_char::parse(document_builder &builder)
{
  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '!')
    return false;

  null_builder dm;
  flow_indicator fi(ctx());
  if(fi.parse(dm))
  {
    unwind();
    return false;
  }

  uri_char u(ctx());
  return u.parse(builder);
}
