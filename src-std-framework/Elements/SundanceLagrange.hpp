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

#ifndef SUNDANCE_LAGRANGE_H
#define SUNDANCE_LAGRANGE_H

#include "SundanceDefs.hpp"
#include "Teuchos_RefCountPtr.hpp"
#include "SundanceScalarBasis.hpp"

#ifndef DOXYGEN_DEVELOPER_ONLY

namespace SundanceStdFwk
{
  using namespace SundanceUtils;
  using namespace SundanceStdMesh;
  using namespace SundanceStdMesh::Internal;
  using namespace Internal;
  using namespace SundanceCore;
  using namespace SundanceCore::Internal;

  /** 
   * Lagrange basis 
   */
  class Lagrange : public ScalarBasis
  {
  public:
    /** */
    Lagrange(int order);

    /** */
    virtual ~Lagrange(){;}

    /** */
    virtual void print(ostream& os) const ;

    /** */
    virtual int order() const {return order_;}

    /** return the number of nodes for this basis on the given cell type */
    virtual int nNodes(const CellType& cellType) const ;

    /** */
    virtual void getLocalDOFs(const CellType& cellType,
                              Array<Array<Array<int> > >& dofs) const ;

    /** */
    virtual void refEval(const CellType& cellType,
                         const Array<Point>& pts,
                         const MultiIndex& deriv,
                         Array<Array<double> >& result) const ;

    /* Handleable boilerplate */
    GET_RCP(BasisFamilyBase);

  private:
    int order_;

    static Array<int> makeRange(int low, int high);

    /** evaluate on a line cell  */
    void evalOnLine(const Point& pt,
                    const MultiIndex& deriv,
                    Array<double>& result) const ;
    
    /** evaluate on a triangle cell  */
    void evalOnTriangle(const Point& pt,
                        const MultiIndex& deriv,
                        Array<double>& result) const ;
    
    /** evaluate on a tet cell  */
    void evalOnTet(const Point& pt,
                   const MultiIndex& deriv,
                   Array<double>& result) const ;
  };
}

#endif  /* DOXYGEN_DEVELOPER_ONLY */

#endif
