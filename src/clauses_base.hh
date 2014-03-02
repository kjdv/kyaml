#ifndef CLAUSES_BASE_HH
#define CLAUSES_BASE_HH

#include <sstream>
#include "char_stream.hh"
#include "utils.hh"
#include "document_builder.hh"

namespace kyaml
{
  namespace clauses
  {
    class context : private no_copy
    {
    public:
      typedef enum
      {
        NA,
        BLOCK_OUT,
        BLOCK_IN,
        BLOCK_KEY,
        FLOW_OUT,
        FLOW_IN,
        FLOW_KEY,
      } blockflow_t;

      context(char_stream &str, 
              unsigned indent_level = 0,
              blockflow_t bf = NA) :
        d_stream(str),
        d_indent_level(indent_level),
        d_bf(bf)
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

      blockflow_t blockflow() const
      {
        return d_bf;
      }

    private:
      char_stream &d_stream;
      unsigned const d_indent_level;
      blockflow_t const d_bf;
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
      bool parse(document_builder &builder); // not implemented, don't call directly 
      
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

      // not mandatory, but advices
      char const *name() const
      {
        return "(clause nos)";
      }
      
    private:
      context &d_context;
      char_stream::mark_t const d_mark;
    };

    namespace internal
    {
      template <typename subclause_t>
      class one_or_more : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          if(parse_once(builder))
          {
            while(parse_once(builder));

            return true;
          }
          return false;            
        }

      private:
        bool parse_once(document_builder &builder)
        {
          subclause_t s(clause::ctx());
          return s.parse(builder);
        }          
      };

      template <typename subclause_t>
      class zero_or_more : public clause
      {
      public:
        using clause::clause;
        
        bool parse(document_builder &builder)
        {
          while(parse_once(builder));

          return true;
        }

      private:
        bool parse_once(document_builder &builder)
        {
          subclause_t s(clause::ctx());
          return s.parse(builder);
        }          
      };

      template <typename subclause_t>
      class zero_or_one : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          parse_once(builder);
          return true;
        }

      private:
        bool parse_once(document_builder &builder)
        {
          subclause_t s(clause::ctx());
          return s.parse(builder);
        }
      };
      
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
      {
      public:
        using clause::clause;
        
        bool parse(document_builder &builder)
        {
          // we need two passes, one for checking, on for collecting results
          // todo: find something smarter
          dummy_document_builder db;
          if(parse_recurse<clauses_t...>(db)) // pass 1
          {
            unwind();
            return parse_recurse<clauses_t...>(builder);
          }
          else
            unwind();
          return false;
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
            parse_recurse<head_t>(builder) &&
            parse_recurse<head2_t, tail_t...>(builder);
        }
      };

      template <typename left_t, typename right_t>
      class or_clause : public any_of<left_t, right_t>
      {
      public:
        or_clause(context &ctx) : any_of<left_t, right_t>(ctx)
        {}
      };

      template <typename left_t, typename right_t>
      class and_clause : public all_of<left_t, right_t>
      {
      public:
        and_clause(context &ctx) : all_of<left_t, right_t>(ctx)
        {}
      };

      template <typename clause_t, context::blockflow_t blockflow_v>
      class flow_restriction : public clause_t
      {
      public:
        using clause_t::clause_t;

        bool parse(document_builder &builder)
        {
          return
            clause_t::ctx().blockflow() == blockflow_v &&
            clause_t::parse(builder);
        }
      };

      template <typename clause_t>
      bool try_parse(clause_t &cl, document_builder &builder)
      {
        dummy_document_builder dm;
        if(cl.parse(dm))
        {
          cl.unwind();
          return cl.parse(builder);
        }
        else
          cl.unwind();
        
        return false;
      }
    }
  }
}

#endif // CLAUSES_BASE_HH
