/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                                 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA                                                                                
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_DISCRETEFUNCTION_H
#define SUNDANCE_DISCRETEFUNCTION_H

#include "SundanceDefs.hpp"
#include "SundanceExpr.hpp"
#include "SundanceDiscreteFunctionStub.hpp"
#include "SundanceFuncWithBasis.hpp"
#include "SundanceDiscreteSpace.hpp"
#include "TSFVector.hpp"

namespace SundanceStdFwk
{
  using namespace SundanceUtils;
  using namespace Teuchos;
  using namespace SundanceCore;
  using namespace SundanceCore::Internal;
  using namespace Internal;


  /** 
   * DiscreteFunction represents a function that is discretized
   * on a finite-element space.
   */
  class DiscreteFunction : public DiscreteFunctionStub,
                           public FuncWithBasis
  {
  public:
    /** */
    DiscreteFunction(const DiscreteSpace& space, const string& name="");

    /** */
    DiscreteFunction(const DiscreteSpace& space, const Vector<double>& vec, 
                     const string& name="");

    /** */
    DiscreteFunction(const DiscreteSpace& space, const double& constantValue,
                     const string& name="");
   

#ifndef DOXYGEN_DEVELOPER_ONLY
    /** virtual destructor */
    virtual ~DiscreteFunction() {;}

    /* boilerplate */
    GET_RCP(ExprBase);

    /** */
    void updateGhosts() const ;

    /** */
    void setVector(const Vector<double>& vec);

    /** */
    const Vector<double>& getVector() const {return vector_;}

    /** */
    const DiscreteSpace& discreteSpace() const {return space_;}

    /** */
    const Mesh& mesh() const {return space_.mesh();}

    /** */
    const RefCountPtr<DOFMapBase>& map() const {return space_.map();}

    /** */
    void getLocalValues(int cellDim, 
                        const Array<int>& cellLID,
                        Array<double>& localValues) const ;

    /** */
    static const DiscreteFunction* discFunc(const Expr& expr);


    /** */
    static DiscreteFunction* discFunc(Expr& expr);


    RefCountPtr<GhostView<double> > ghostView() const 
    {updateGhosts(); return ghostView_;}


  private:
    friend class NonlinearProblem;

    /** */
    const Vector<double>& vector() const {return vector_;}

    DiscreteSpace space_;

    Vector<double> vector_;

    mutable RefCountPtr<GhostView<double> > ghostView_;

    mutable bool ghostsAreValid_;

#endif /* DOXYGEN_DEVELOPER_ONLY */
  };

}



#endif
