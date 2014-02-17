#ifndef CLAUSES_BASE_HH
#define CLAUSES_BASE_HH

#include "char_stream.hh"
#include "utils.hh"

namespace kyaml
{
  namespace clauses
  {
    // base class is informational only: there are no virtual functions, its here only to document
    // the common signature
    class clause
    {
    public:
      clause(char_stream &stream) : 
        d_stream(stream),
        d_mark(d_stream.mark())
      {}
      
      // returns true if the head of the stream is of this type, should leave the stream unmodified
      bool try_clause(); 
      
      // return clause-depentend value of this, should only be called if try_clause returned true
      // value_t value();

      void unwind()
      {
        d_stream.unwind(d_mark);
      }

      void advance(size_t n = 1)
      {
        d_stream.advance(n);
      }
            
      // note: public methods are not implemented, they should not be called directly
      
    protected:
      char_stream &stream()
      {
        return d_stream;
      }
      
    private:
      char_stream &d_stream;
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

        bool try_clause()
        {
          left_t l(compound_clause<result_t>::stream());
          if(l.try_clause())
          {
            value_t v(l.value());
            set(v);
            return true;
          }

          right_t r(compound_clause<result_t>::stream());
          if(r.try_clause())
          {
            value_t v(r.value());
            set(v);
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

        bool try_clause()
        {
          left_t l(compound_clause<result_t>::stream());
          if(l.try_clause())
          {
            value_t v(l.value());
            
            right_t r(compound_clause<result_t>::stream());
            if(r.try_clause())
            {
              v.append(r.value());
              return true;
            }
            else
              compound_clause<result_t>::unwind();
          }
          return false;
        }
      };

      template <typename result_t, typename subclause_t>
      class one_or_more : public clause
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        bool try_clause()
        {
          subclause_t s(compound_clause<result_t>::stream());
          if(s.try_clause())
          {
            value_t v;
            do
            {
              v.append(s.value());
            } while(s.try_clause());
            set(v);
            return true;
          }
          return false;
        }
      };

      template <typename result_t, typename subclause_t>
      class zero_or_more : public clause
      {
      public:
        typedef typename compound_clause<result_t>::value_t value_t;

        bool try_clause()
        {
          subclause_t s(compound_clause<result_t>::stream());
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
