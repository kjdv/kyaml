#ifndef CLAUSES_BASE_HH
#define CLAUSES_BASE_HH

#include <sstream>
#include "context.hh"
#include "document_builder.hh"

namespace kyaml
{
  namespace clauses
  {
    class clause
    {
    public:
      clause(context &c) :
        d_context(c)
      {}

      context const &ctx() const
      {
        return d_context;
      }

      context &ctx()
      {
        return d_context;
      }

    protected:
      // returns true if the head of the stream is of this type, should leave the stream unmodified
      bool parse(document_builder &builder); // not implemented, don't call directly

      // not mandatory, but adviced
      char const *name() const
      {
        return "(clause nos)";
      }

    private:
      context &d_context;
    };

    namespace internal
    {
      // +
      template <typename subclause_t>
      class one_or_more : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          if(parse_once(builder))
          {
            char_stream::mark_t before = clause::ctx().stream().mark();
            while(parse_once(builder))
            {
              char_stream::mark_t after = clause::ctx().stream().mark();
              if(after <= before)
                break;
              before = after;
            }

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

      // *
      template <typename subclause_t>
      class zero_or_more : public clause
      {
      public:
        using clause::clause;
        
        bool parse(document_builder &builder)
        {
          char_stream::mark_t before = clause::ctx().stream().mark();
          while(parse_once(builder))
          {
            char_stream::mark_t after = clause::ctx().stream().mark();
            if(after <= before)
              break;
            before = after;
          }

          return true;
        }

      private:
        bool parse_once(document_builder &builder)
        {
          subclause_t s(clause::ctx());
          return s.parse(builder);
        }          
      };

      // ?
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
          stream_guard sg(ctx());

          replay_builder rb;
          if(parse_recurse<clauses_t...>(rb))
          {
            rb.replay(builder);
            sg.release();
            return true;
          }
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

      template <typename clause_t>
      class not_clause : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          stream_guard sg(ctx());
          null_builder db;
          if(clause_t(ctx()).parse(db))
            return false;

          sg.release();
          return true;
        }
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
        context_guard cg(cl.ctx());

        replay_builder rb;
        if(cl.parse(rb))
        {
          rb.replay(builder);
          cg.release();
          return true;
        }

        return false;
      }
      
      template <typename state_modifier_t, typename base_clause_t>
      class state_scope : public clause
      {
      public:
        using clause::clause;
        
        bool parse(document_builder &builder)
        {
          context_guard cg(ctx());

          state_modifier_t sm(ctx());
          if(sm.parse(builder))
          {
            base_clause_t bc(ctx());
            if(bc.parse(builder))
            {
               cg.release_stream();
               return true;
            }
          }
          return false;
        }
      };

      template <context::blockflow_t blockflow_v>
      class flow_modifier : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          ctx().set_blockflow(blockflow_v);
          return true;
        }
      };

      class indent_inc_modifier : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          int i = ctx().indent_level();
          ctx().set_indent(++i);
          return true;
        }
      };

      template <int indent_v>
      class indent_modifier : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          ctx().set_indent(indent_v);
          return true;
        }
      };
    }
  }
}

#endif // CLAUSES_BASE_HH
