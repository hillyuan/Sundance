/* @HEADER@ */
/* @HEADER@ */

#ifndef SUNDANCE_UNARYFUNCTOR_H
#define SUNDANCE_UNARYFUNCTOR_H

#include "SundanceDefs.hpp"
#include "SundanceExceptions.hpp"
#include "SundanceFunctorDomain.hpp"
#include "Teuchos_ScalarTraits.hpp"
#include "Teuchos_RefCountPtr.hpp"

#ifndef DOXYGEN_DEVELOPER_ONLY

namespace SundanceUtils
{
  using namespace Teuchos;

  using std::string;
  using std::ostream;

  /**
   * 
   */
  class UnaryFunctor
  {
  public:
    /** ctor */
    UnaryFunctor(const string& name, 
                 const RefCountPtr<FunctorDomain>& domain 
                 = rcp(new UnboundedDomain())) 
      : name_(name), h_(fdStep()), domain_(domain) {;}

    /** */
    virtual ~UnaryFunctor(){;}

    /** */
    const string& name() const {return name_;}

    /** */
    virtual void eval0(const double* const x, 
                       int nx, 
                       double* f) const = 0 ;
    
    /** */
    virtual void eval1(const double* const x, 
                       int nx, 
                       double* f, 
                       double* df_dx) const ;
    
    /** */
    virtual void eval2(const double* const x, 
                       int nx, 
                       double* f, 
                       double* df_dx,
                       double* d2f_dxx) const ;

    

    /** */
    void evalFDDerivs1(const double* const x, 
                       int nx, 
                       double* f, 
                       double* df_dx) const ;
    /** */
    void evalFDDerivs2(const double* const x, 
                       int nx, 
                       double* f, 
                       double* df_dx,
                       double* d2f_dxx) const ;

    /** */
    bool testDerivs(const double& x, const double& tol) const ;

    /** */
    bool testInvalidValue(const double& xBad) const ;

    /** */
    bool test(int nx, const double& tol) const ;
    
    /** Specify whether we should test for NAN or INFINITE results. */
    static bool& checkResults() {static bool rtn = false; return rtn;}

    static double& fdStep() {static double rtn = 1.0e-6; return rtn;}

    const RefCountPtr<FunctorDomain>& domain() const 
    {return domain_;}
  private:
    string name_;

    double h_;

    RefCountPtr<FunctorDomain> domain_;
  };
}

/** */
#define SUNDANCE_UNARY_FUNCTOR(opName, functorName, description, domain,\
                     funcDefinition, firstDerivDefinition, \
                     secondDerivDefinition) \
  class functorName : public SundanceUtils::UnaryFunctor \
  {\
  public:\
    /** ctor for description functor */\
    functorName() : SundanceUtils::UnaryFunctor(#opName, rcp(new domain)) {;} \
    /** virtual dtor */\
  virtual ~functorName(){;}\
    /** Evaluate function at an array of values */ \
    void eval0(const double* const x, int nx, double* f) const ;\
    /** Evaluate function and first derivative at an array of values */\
    void eval1(const double* const x, \
              int nx, \
              double* f, \
              double* df) const ;\
    /** Evaluate function and first derivative at an array of values */\
    void eval2(const double* const x, \
              int nx, \
              double* f, \
              double* df, \
              double* d2f_dxx) const ;\
  };\
inline void functorName::eval0(const double* const x, int nx, double* f) const \
{\
   if (checkResults())\
     {\
        for (int i=0; i<nx; i++) \
          {\
             f[i] = funcDefinition;\
          TEST_FOR_EXCEPTION(Teuchos::ScalarTraits<double>::isnaninf(f[i]), RuntimeError, "Non-normal floating point result detected in evaluation of unary functor " << name() << " at argument " << x[i]);\
          }\
     }\
   else\
     {\
        for (int i=0; i<nx; i++) f[i] = funcDefinition;\
     }\
}\
inline void functorName::eval1(const double* const x, \
                              int nx, \
                              double* f, \
                              double* df) const \
{ \
    if (checkResults())\
      {\
         for (int i=0; i<nx; i++) \
           {\
             f[i] = funcDefinition;\
             df[i] = firstDerivDefinition;\
             TEST_FOR_EXCEPTION(Teuchos::ScalarTraits<double>::isnaninf(f[i])||Teuchos::ScalarTraits<double>::isnaninf(df[i]),\
                              RuntimeError,\
                              "Non-normal floating point result detected in "\
                              "evaluation of unary functor " \
                                << name() << " at argument " << x[i]);\
           }\
      }\
    else\
      {\
         for (int i=0; i<nx; i++) \
           { \
             f[i] = funcDefinition;\
             df[i] = firstDerivDefinition;\
           }\
      }\
}\
inline void functorName::eval2(const double* const x, \
                              int nx, \
                              double* f, \
                              double* df,\
                              double* d2f) const \
{ \
    if (checkResults())\
      {\
         for (int i=0; i<nx; i++) \
           {\
              f[i] = funcDefinition;\
              df[i] = firstDerivDefinition;\
              d2f[i] = secondDerivDefinition;\
              TEST_FOR_EXCEPTION(Teuchos::ScalarTraits<double>::isnaninf(f[i])||Teuchos::ScalarTraits<double>::isnaninf(df[i])||Teuchos::ScalarTraits<double>::isnaninf(d2f[i]),\
                              RuntimeError,\
                              "Non-normal floating point result detected in "\
                              "evaluation of unary functor " \
                                 << name() << " at argument " << x[i] );\
           }\
      }\
    else\
      {\
         for (int i=0; i<nx; i++) \
           { \
               f[i] = funcDefinition;\
               df[i] = firstDerivDefinition;\
               d2f[i] = secondDerivDefinition;\
           }\
      }\
}


                  
#endif  /* DOXYGEN_DEVELOPER_ONLY */  

#endif
