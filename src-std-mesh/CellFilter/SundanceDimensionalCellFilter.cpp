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

#include "SundanceDimensionalCellFilter.hpp"
#include "SundanceImplicitCellSet.hpp"
#include "PlayaExceptions.hpp"

using namespace Sundance;
using namespace Sundance;
using namespace Sundance;
using namespace Teuchos;

DimensionalCellFilter::DimensionalCellFilter(int dim)
  : CellFilterBase(), dim_(dim)
{
  setName(Teuchos::toString(dim) + "-cells");
}

XMLObject DimensionalCellFilter::toXML() const 
{
  XMLObject rtn("DimensionalCellFilter");
  rtn.addAttribute("dim", Teuchos::toString(dim_));
  return rtn;
}


bool DimensionalCellFilter::lessThan(const CellFilterStub* other) const
{
  TEUCHOS_TEST_FOR_EXCEPTION(dynamic_cast<const DimensionalCellFilter*>(other) == 0,
                     std::logic_error,
                     "argument " << other->toXML() 
                     << " to DimensionalCellFilter::lessThan() should be "
                     "a DimensionalCellFilter pointer.");

  return dim_ < dynamic_cast<const DimensionalCellFilter*>(other)->dim_;
}

CellSet DimensionalCellFilter::internalGetCells(const Mesh& mesh) const
{
  return new ImplicitCellSet(mesh, dim_, 
                             mesh.cellType(dim_));
}
