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
      typedef unsigned value_t;

      indent_clause_eq(char_stream &s, unsigned n) : clause(s), d_n(n)
      {}

      bool try_clause();

      value_t value() const
      {
        return d_n;
      }
    private:
      const unsigned d_n;
    };

    // [64] 	s-indent(<n) 	::= 	s-space × m Where m < n 
    class indent_clause_lt : public clause
    {
    public:
      typedef unsigned value_t;

      indent_clause_lt(char_stream &s, unsigned n) : 
        clause(s),
        d_n(n),
        d_m(0)
      {}

      bool try_clause();

      value_t value() const
      {
        return d_m;
      }
    private:
      const unsigned d_n;
      unsigned d_m;
    };

    // [65] 	s-indent(≤n) 	::= 	s-space × m /* Where m ≤ n */ 
    class indent_clause_le : public clause
    {
    public:
      typedef unsigned value_t;

      indent_clause_le(char_stream &s, unsigned n) : 
        clause(s),
        d_n(n),
        d_m(0)
      {}

      bool try_clause();

      value_t value() const
      {
        return d_m;
      }
    private:
      const unsigned d_n;
      unsigned d_m;
    };
  }
}

#endif // INDENDATION_CLAUSES_HH
