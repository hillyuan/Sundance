/* @HEADER@ */
// ************************************************************************
// 
//                 Playa: Programmable Linear Algebra
//                 Copyright 2012 Sandia Corporation
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

#ifndef PLAYA_EIGENSOLVER_HPP
#define PLAYA_EIGENSOLVER_HPP

#include "PlayaDefs.hpp"
#include "PlayaVectorDecl.hpp" 
#include "PlayaSolverState.hpp"
#include "Teuchos_ParameterList.hpp"
#include "PlayaEigensolverBase.hpp"

namespace Playa
{
using Teuchos::ParameterList;

/**
 * Handle class for eigensolvers
 */
template <class Scalar>
class Eigensolver : public Playa::Handle<EigensolverBase<Scalar> >
{
public:
  /** */
  Eigensolver() : Handle<EigensolverBase<Scalar> >() {;}
  /** */
  Eigensolver( Playa::Handleable<EigensolverBase<Scalar> >* rawPtr) 
    : Handle<EigensolverBase<Scalar> >(rawPtr) {;}
  /** */
  Eigensolver(const RCP<EigensolverBase<Scalar> >& smartPtr)
    : Handle<EigensolverBase<Scalar> >(smartPtr) {;}
  

  /** */
  void solve(
    const LinearOperator<Scalar>& K,
    const LinearOperator<Scalar>& M,
    Array<Vector<Scalar> >& ev,
    Array<std::complex<Scalar> >& ew) const 
    {
      this->ptr()->solve(K, M, ev, ew);
    }

  /** */
  void solve(
    const LinearOperator<Scalar>& K,
    Array<Vector<Scalar> >& ev,
    Array<std::complex<Scalar> >& ew) const 
    {
      this->ptr()->solve(K, ev, ew);
    }

  /** */
  const ParameterList& params() const 
    {
      TEUCHOS_TEST_FOR_EXCEPTION(this->ptr().get()==0, std::runtime_error,
                       "null pointer in Eigensolver::parameters()");
      return this->ptr()->params();
    }
  
  /** */
  ParameterList& params() 
    {
      TEUCHOS_TEST_FOR_EXCEPTION(this->ptr().get()==0, std::runtime_error,
                       "null pointer in Eigensolver::parameters()");
      return this->ptr()->params();
    }
  

  static FancyOStream& os()
    {
      return Out::os();
    }
};


  
}


#endif
