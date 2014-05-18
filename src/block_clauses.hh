#ifndef BLOCK_CLAUSES_HH
#define BLOCK_CLAUSES_HH          

#include "clauses_base.hh"
#include "char_clauses.hh"

namespace kyaml
{
  namespace clauses
  {
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
