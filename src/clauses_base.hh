#ifndef CLAUSES_BASE_HH
#define CLAUSES_BASE_HH

#include "char_stream.hh"

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
  }
}

#endif // CLAUSES_BASE_HH
