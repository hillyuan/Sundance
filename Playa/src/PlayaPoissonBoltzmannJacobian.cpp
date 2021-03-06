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

#include "PlayaPoissonBoltzmannJacobian.hpp"
#include "PlayaEpetraMatrix.hpp"
#include "PlayaIncrementallyConfigurableMatrixFactory.hpp"
#include "PlayaTabs.hpp"


#ifndef HAVE_TEUCHOS_EXPLICIT_INSTANTIATION
#include "PlayaVectorImpl.hpp"
#include "PlayaLinearOperatorImpl.hpp"
#endif
using namespace Playa;
using namespace Teuchos;


PoissonBoltzmannJacobian
::PoissonBoltzmannJacobian(int nLocalRows, 
  const VectorType<double>& type)
  : OperatorBuilder<double>(nLocalRows, type), op_(), nLocalRows_(nLocalRows),
    h_(1.0)
{
  h_ = 1.0/((double) domain().dim() - 1);
}

void PoissonBoltzmannJacobian::setEvalPoint(const Vector<double>& x)
{
  Tabs tab;
  Out::os() << tab << "in PBJ::setEvalPoint()" << std::endl;
  int rank = MPIComm::world().getRank();
  int nProc = MPIComm::world().getNProc();
  RCP<MatrixFactory<double> > mFact 
    = vecType().createMatrixFactory(domain(), range());
  
  int lowestLocalRow = nLocalRows_ * rank;

  IncrementallyConfigurableMatrixFactory* icmf 
    = dynamic_cast<IncrementallyConfigurableMatrixFactory*>(mFact.get());
  for (int i=0; i<nLocalRows_; i++)
  {
    int row = lowestLocalRow + i;
    Array<int> colIndices;
    if ((rank==0 && i==0) || (rank==nProc-1 && i==nLocalRows_-1))
    {
      colIndices = tuple(row);
    }
    else
    {
      colIndices = tuple(row-1, row, row+1);
    }
    icmf->initializeNonzerosInRow(row, colIndices.size(),
      &(colIndices[0]));
  }
  icmf->finalize();
      
  op_ = mFact->createMatrix();
      
  RCP<LoadableMatrix<double> > mat = op_.matrix();

  /* fill in with the Laplacian operator plus exp(-x) */
  for (int i=0; i<nLocalRows_; i++)
  {
    int row = lowestLocalRow + i;
    Array<int> colIndices;
    Array<double> colVals;
    if ((rank==0 && i==0) || (rank==nProc-1 && i==nLocalRows_-1))
    {
      colIndices = tuple(row);
      colVals = tuple(1.0);
    }
    else
    {
      colIndices = tuple(row-1, row, row+1);
      colVals = tuple(1.0/h_/h_, 
        -2.0/h_/h_ + exp(-x[i]), 
        1.0/h_/h_);
    }
    mat->addToRow(row, colIndices.size(), 
      &(colIndices[0]), &(colVals[0]));
  }
  Out::os() << tab << "done PBJ::setEvalPoint()" << std::endl;
}
