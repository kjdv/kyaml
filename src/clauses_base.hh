#ifndef CLAUSES_BASE_HH
#define CLAUSES_BASE_HH

#include <sstream>
#include "char_stream.hh"
#include "utils.hh"

namespace kyaml
{
  namespace clauses
  {
    class context : private no_copy
    {
    public:
      context(char_stream &str, unsigned indent_level = 0) :
        d_stream(str),
        d_indent_level(indent_level)
      {}

      char_stream const &stream() const
      {
        return d_stream;
      }

      char_stream &stream()
      {
        return d_stream;
      }

      unsigned indent_level() const
      {
        return d_indent_level;
      }

    private:
      char_stream &d_stream;
      unsigned const d_indent_level;
    };

    // mostly useful for testing purposes: construct a blank self-contained context from a string
    class context_wrap
    {
    public:
      context_wrap(std::string const &s, unsigned indent_level = 0):
        d_sstream(s),
        d_stream(d_sstream),
        d_ctx(d_stream, indent_level)
      {}

      context const &get() const
      {
        return d_ctx;
      }
      
      context &get()
      {
        return d_ctx;
      }
    private:
      std::stringstream d_sstream;
      char_stream d_stream;
      context d_ctx;
    };
    
    // base class is informational only: there are no virtual functions, its here only to document
    // the common signature
    class clause
    {
    public:
      clause(context &c) : 
        d_context(c),
        d_mark(d_context.stream().mark())
      {}

      void advance(size_t n = 1)
      {
        stream().advance(n);
      } 
  
      void unwind()
      {
        stream().unwind(d_mark);
      }  

    protected:
      // returns true if the head of the stream is of this type, should leave the stream unmodified
      bool try_clause(); // not implemented, don't call directly 
      
      context const &ctx() const
      {
        return d_context;
      }

      context &ctx()
      {
        return d_context;
      }
            
      char_stream &stream()
      {
        return ctx().stream();
      }
      
    private:
      context &d_context;
      char_stream::mark_t const d_mark;
    };

    class string_result : public std::string
    {
    public:
      string_result()
      {}
      string_result(std::string const &s) : std::string(s)
      {}
      string_result(char_t c)
      {
        append_utf8(*this, c);
      }
      
      void append(char_t c)
      {
        append_utf8(*this, c);
      }
    private:
      std::string d_val;
    };

    class void_result
    {
    public:
      void_result()
      {}
      template <typename T>
      void_result(T const &)
      {}
      template <typename T>
      void append(T const &)
      {}
    };

    namespace internal
    {
      class void_clause : public clause
      {
      public:
        typedef void_result value_t;

        using clause::clause;
        
        value_t value() const
        {
          return value_t();
        }
      };
    }

    namespace internal
    {
      template <typename result_t>
      class compound_clause : public clause
      {
      public:
        typedef result_t value_t;
        
        using clause::clause;

        value_t const &value() const
        {
          return d_value;
        }
        
      protected:
        void set(value_t const &v)
        {
          d_value = v;
        }

      private:
        value_t d_value;
      };

      template <typename result_t, typename left_t, typename right_t>
      class or_clause : public compound_clause<result_t>
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        or_clause(context &ctx) : compound_clause<result_t>(ctx)
        {}

        bool try_clause()
        {
          left_t l(compound_clause<result_t>::ctx());
          if(l.try_clause())
          {
            value_t v(l.value());
            compound_clause<result_t>::set(v);
            return true;
          }

          right_t r(compound_clause<result_t>::ctx());
          if(r.try_clause())
          {
            value_t v(r.value());
            compound_clause<result_t>::set(v);
            return true;
          }
          return false;
        }
      };

      template <typename result_t, typename left_t, typename right_t>
      class and_clause : public compound_clause<result_t>
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        and_clause(context &ctx) : compound_clause<result_t>(ctx)
        {}

        bool try_clause()
        {
          left_t l(compound_clause<result_t>::ctx());
          if(l.try_clause())
          {
            value_t v(l.value());
            
            right_t r(compound_clause<result_t>::ctx());
            if(r.try_clause())
            {
              v.append(r.value());
              compound_clause<result_t>::set(v);
              return true;
            }
            else
              compound_clause<result_t>::unwind();
          }
          return false;
        }
      };

      template <typename result_t, typename subclause_t>
      class one_or_more : public compound_clause<result_t>
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        one_or_more(context &ctx) : compound_clause<result_t>(ctx)
        {}

        bool try_clause()
        {
          subclause_t s(compound_clause<result_t>::ctx());
          if(s.try_clause())
          {
            value_t v;
            do
            {
              v.append(s.value());
            } while(s.try_clause());
            compound_clause<result_t>::set(v);
            return true;
          }
          return false;
        }
      };

      template <typename result_t, typename subclause_t>
      class zero_or_more : public compound_clause<result_t>
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        zero_or_more(context &ctx) : compound_clause<result_t>(ctx)
        {}

        bool try_clause()
        {
          subclause_t s(compound_clause<result_t>::ctx());
          value_t v;
          while(s.try_clause())
            v.append(s.value());
          set(v);
          return true;
        }
      };
    }
  }
}

#endif // CLAUSES_BASE_HH
