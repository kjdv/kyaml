# KYAML

Klaas' Yaml Library. A library that parses yaml documents for you.

This has to be mostly read as an expiroment in how to use C++'s template mechanism, specifically the extensions that became available in C++11, to write parsers. Yaml is an interesting if challenging vehicle for that as it is not easy to parse yet is a nice human format making it worth having a parser for.

I make no claims to completeness or correctness, and not to performance either.

Check `example/main.cc` for example usage.

## Internals

The idea is to express each clause in the [formal grammar](http://yaml.org/spec/1.2/spec.html) as a template deriving from the base clause (simplified):

```c++
class clause
{
public:
  // returns true if the head of the stream is of this type, should leave the stream unmodified
  bool parse(document_builder &builder);
};

// next, define some primitives

// +
template <typename subclause_t>
class one_or_more : public clause
{ /* ... */ };

// *
template <typename subclause_t>
class zero_or_more : public clause
{ /* ... */ };

// ?
template <typename subclause_t>
class zero_or_one : public clause
{ /* ... */ };

template <typename... clauses_t>
class any_of : public clause
{
public:
  using clause::clause;

  bool parse(document_builder &builder)
  {
    return parse_recurse<clauses_t...>(builder);
  }

private:
  template <typename head_t>
  bool parse_recurse(document_builder &builder)
  {
    head_t head(clause::ctx());
    return head.parse(builder);
  }

  template <typename head_t, typename head2_t, typename... tail_t>
  bool parse_recurse(document_builder &builder)
  {
    return
      parse_recurse<head_t>(builder) ||
      parse_recurse<head2_t, tail_t...>(builder);
  }        
};

template <typename... clauses_t>
class all_of : public clause
{ /* ... */ };

template <typename left_t, typename right_t>
class or_clause : public any_of<left_t, right_t>
{ /* ... */ };

template <typename left_t, typename right_t>
class and_clause : public all_of<left_t, right_t>
{ /* ... */ };
```

The above should give you enough rope to translate a rule from the formal grammar like

```
[76] 	b-comment 	::= 	b-non-content |  End of file
```
into
```c++
typedef internal::or_clause<non_content, endoffile> break_comment;
```

## Python

Check the `python` subdirectory for python bindings.
