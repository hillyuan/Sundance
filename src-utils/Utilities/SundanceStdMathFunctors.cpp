/* @HEADER@ */
/* @HEADER@ */

#include "SundanceStdMathFunctors.hpp"
#include "Teuchos_Utils.hpp"

using namespace SundanceUtils;
using namespace Teuchos;


PowerFunctor::PowerFunctor(const double& p) 
  : UnaryFunctor("pow("+Teuchos::toString(p)+")"), p_(p)
{;}

void PowerFunctor::eval1(const double* const x, 
                        int nx, 
                        double* f, 
                        double* df) const
{
  if (checkResults())
    {
      for (int i=0; i<nx; i++) 
        {
          double px = ::pow(x[i], p_-1);
          df[i] = p_*px;
          f[i] = x[i]*px;
          TEST_FOR_EXCEPTION(fpclassify(f[i]) != FP_NORMAL 
                             || fpclassify(df[i]) != FP_NORMAL,
                             RuntimeError,
                             "Non-normal floating point result detected in "
                             "evaluation of unary functor " << name());
        }
    }
  else
    {
      for (int i=0; i<nx; i++) 
        {
          double px = ::pow(x[i], p_-1);
          df[i] = p_*px;
          f[i] = x[i]*px;
        }
    }
}

void PowerFunctor::eval2(const double* const x, 
                        int nx, 
                        double* f, 
                        double* df,
                        double* d2f_dxx) const
{
  if (checkResults())
    {
      for (int i=0; i<nx; i++) 
        {
          double px = ::pow(x[i], p_-2);
          d2f_dxx[i] = p_ * (p_-1) * px;
          df[i] = p_*x[i]*px;
          f[i] = x[i]*x[i]*px;
          TEST_FOR_EXCEPTION(fpclassify(f[i]) != FP_NORMAL 
                             || fpclassify(df[i]) != FP_NORMAL,
                             RuntimeError,
                             "Non-normal floating point result detected in "
                             "evaluation of unary functor " << name());
        }
    }
  else
    {
      for (int i=0; i<nx; i++) 
        {
          double px = ::pow(x[i], p_-2);
          d2f_dxx[i] = p_ * (p_-1) * px;
          df[i] = p_*x[i]*px;
          f[i] = x[i]*x[i]*px;
        }
    }
}

void PowerFunctor::eval0(const double* const x, 
                        int nx, 
                        double* f) const
{
  if (checkResults())
    {
      for (int i=0; i<nx; i++) 
        {
          f[i] = ::pow(x[i], p_);
          TEST_FOR_EXCEPTION(fpclassify(f[i]) != FP_NORMAL, 
                             RuntimeError,
                             "Non-normal floating point result detected in "
                             "evaluation of unary functor " << name());
        }
    }
  else
    {
      for (int i=0; i<nx; i++) 
        {
          f[i] = ::pow(x[i], p_);
        }
    }
}