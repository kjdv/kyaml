#ifndef CONTEXT_HH
#define CONTEXT_HH

#include "char_stream.hh"
#include "utils.hh"

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

      typedef enum
      {
        CLIP,
        STRIP,
        KEEP,
      } chomp_t;

      struct state
      {
        // note: the indent_level is intentionally signed. It is in fact legal to have a negavitve indent level.
        // In fact, it is -1 by default so that 0 counts as higher than the current.
        int indent_level;
        blockflow_t blockflow;
        chomp_t chomp;
        unsigned linenumber;

        state(int il, blockflow_t bf, chomp_t c, unsigned l) :
          indent_level(il),
          blockflow(bf),
          chomp(c),
          linenumber(l)
        {}
      };

      context(char_stream &str,
              int indent_level = -1,
              blockflow_t bf = NA,
              chomp_t c = CLIP,
              unsigned l = 1) :
        d_stream(str),
        d_state(indent_level, bf, c, l)
      {}

      char_stream const &stream() const
      {
        return d_stream;
      }

      char_stream &stream()
      {
        return d_stream;
      }

      int indent_level() const
      {
        return d_state.indent_level;
      }

      blockflow_t blockflow() const
      {
        return d_state.blockflow;
      }

      chomp_t chomp() const
      {
        return d_state.chomp;
      }

      void set_blockflow(blockflow_t bf)
      {
        d_state.blockflow = bf;
      }

      void set_indent(int il)
      {
        d_state.indent_level = il;
      }

      void set_chomp(chomp_t c)
      {
        d_state.chomp = c;
      }

      unsigned linenumber() const
      {
        return d_state.linenumber;
      }

      void newline()
      {
        ++d_state.linenumber;
      }

      state get_state() const
      {
        return d_state;
      }

      void set_state(state const &s)
      {
        d_state = s;
      }

    private:
      char_stream &d_stream;
      state d_state;
    };

    // scope-based state guard
    class state_guard : public no_copy
    {
    public:
      state_guard(context &ctx);
      ~state_guard();

      void release()
      {
        d_canceled = true;
      }

    private:
      context &d_ctx;
      const context::state d_state;
      bool d_canceled;
    };

    // stream guard
    class stream_guard : public no_copy
    {
    public:
      stream_guard(context &ctx);
      ~stream_guard();

      void release()
      {
        d_canceled = true;
      }

    private:
      context &d_ctx;
      const char_stream::mark_t d_mark;
      bool d_canceled;
    };

    // guard stream and state
    class context_guard : public no_copy
    {
    public:
      context_guard(context &ctx) :
        d_state_guard(ctx),
        d_stream_guard(ctx)
      {}

      void release();

      void release_state()
      {
        d_state_guard.release();
      }

      void release_stream()
      {
        d_stream_guard.release();
      }

    private:
      state_guard d_state_guard;
      stream_guard d_stream_guard;
    };
  }
}

#endif // CONTEXT_HH
