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

bool non_white_char::parse(document_builder &builder)
{
  document_builder::child_t b = builder.child();

  white w(ctx());
  if(w.parse(*b))
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
    builder.add(name(), c);
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
      builder.add(name(), c);
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
    builder.add(name(), c);
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
    builder.add(name(), c);
    advance();
    return true;
  }
  return false;
}

bool uri_char::parse(document_builder &builder)
{
  word_char wc(ctx());
  if(wc.parse(builder))
    return true;

  char_t c;
  if(!stream().peek(c))
    return false;

  if(c == '%')
  {
    document_builder::child_t b = builder.child();
    b->add(name(), c);
    advance();

    hex_digit_char h1(ctx());
    hex_digit_char h2(ctx());
    if(h1.parse(*b) && h2.parse(*b))
    {
      builder.add(name(), move(b));
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
    builder.add(name(), c);
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

  document_builder::child_t b = builder.child();
  flow_indicator fi(ctx());
  if(fi.parse(*b))
  {
    unwind();
    return false;
  }

  uri_char u(ctx());
  return u.parse(builder);
}
