/* @HEADER@ */
// ************************************************************************
// 
//                             Sundance
//                 Copyright 2011 Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Kevin Long (kevin.long@ttu.edu)
// 

/* @HEADER@ */

#include "SundanceEventDetector.hpp"
#include "SundanceDiscreteFunction.hpp"
#include "PlayaVectorImpl.hpp"
#include "PlayaOut.hpp"
#include "PlayaTabs.hpp"



namespace Sundance
{

bool ThresholdEventDetector::checkForEvent(
  const double& t1, const Expr& u1,
  const double& t2, const Expr& u2) 
{
  if (foundEvent()) return false;

  Vector<double> x1 = getDiscreteFunctionVector(u1);
  Vector<double> x2 = getDiscreteFunctionVector(u2);

  if (eventType_==AllAbove || eventType_==AnyBelow)
  {
    double a1 = x1.min()-threshold_;
    double a2 = x2.min()-threshold_;
    if (a1*a2 <= 0) 
    {
      double t = t1 - a1*(t2-t1)/(a2-a1);
      eventTime_ = t;
      gotIt_ = true;
      return true;
    }
  }
  else
  {
    double a1 = x1.max()-threshold_;
    double a2 = x2.max()-threshold_;
    if (a1*a2 <= 0) 
    {
      double t = t1 - a1*(t2-t1)/(a2-a1);
      eventTime_ = t;
      gotIt_ = true;
      return true;
    }
  }
  return false;
}

}
