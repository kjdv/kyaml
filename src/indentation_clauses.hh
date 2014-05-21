#ifndef INDENDATION_CLAUSES_HH
#define INDENDATION_CLAUSES_HH

#include "clauses_base.hh"

namespace kyaml
{
  namespace clauses
  {
    // [63] 	s-indent(n) 	::= 	s-space × n 
    class indent_clause_eq : public clause
    {
    public:
      indent_clause_eq(context &c) : clause(c)
      {}

      bool parse(document_builder &builder);
    private:
      int level() const
      {
        return ctx().indent_level();
      }
    };

    namespace internal
    {
      // special case of [63] needed to fulfill clause [69]
      class indent_clause_ge : public clause
      {
      public:       
        indent_clause_ge(context &c) : clause(c)
        {}
        
        bool parse(document_builder &builder);
      private:
        int level() const
        {
          return ctx().indent_level();
        }
      };  
    }

    // [64] 	s-indent(<n) 	::= 	s-space × m Where m < n 
    class indent_clause_lt : public clause
    {
    public:
      indent_clause_lt(context &c) : 
        clause(c),
        d_m(0)
      {}

      bool parse(document_builder &builder);
    private:
      int lower_level() const
      {
        return d_m;
      }
      int higher_level() const
      {
        return ctx().indent_level();
      }
      
      int d_m;
    };

    // [65] 	s-indent(≤n) 	::= 	s-space × m /* Where m ≤ n */ 
    class indent_clause_le : public clause
    {
    public:
      indent_clause_le(context &c) :
        clause(c),
        d_m(0)
      {}

      bool parse(document_builder &builder);
    private:
      int lower_level() const
      {
        return d_m;
      }
      int higher_level() const
      {
        return ctx().indent_level();
      }
      int d_m;
    };
  }
}

#endif // INDENDATION_CLAUSES_HH
