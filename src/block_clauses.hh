#ifndef BLOCK_CLAUSES_HH
#define BLOCK_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
    namespace internal
    {
      template <typename indent_modifier_t, typename base_clause_t>
      class indent_scope : public clause
      {
      public:
        using clause::clause;

        bool parse(document_builder &builder)
        {
          bool result = false;
          unsigned mem = ctx().blockflow();
          indent_modifier_t fm(ctx());
          if(fm.parse(builder))
          {
            base_clause_t bc(ctx());
            result = bc.parse(builder);
          }

          if(!result)
            unwind();

          ctx().set_indent_level(mem);
          return result;
        }
      };
    }


    // [163] 	c-indentation-indicator(m) 	::= 	ns-dec-digit ⇒ m = ns-dec-digit - #x30
    //                                                  /* Empty */  ⇒ m = auto-detect() 
    class indentation_indicator : public clause
    {
    public:
      using clause::clause;

      bool parse(document_builder &builder);
    private:
      bool autodetect(document_builder &builder);
    };

    // [162] 	c-b-block-header(m,t) 	::= 	( ( c-indentation-indicator(m)
    //                                              c-chomping-indicator(t) )
    //                                          | ( c-chomping-indicator(t)
    //                                              c-indentation-indicator(m) ) )
    //                                          s-b-comment 	 
  }
}

#endif // BLOCK_CLAUSES_HH
