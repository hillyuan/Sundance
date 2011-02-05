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

#include "SundanceBubble.hpp"
#include "SundanceSpatialDerivSpecifier.hpp"
#include "SundancePoint.hpp"
#include "SundanceADReal.hpp"
#include "PlayaExceptions.hpp"
#include "SundanceObjectWithVerbosity.hpp"
#include "SundanceOut.hpp"

using namespace Sundance;
using namespace Sundance;
using namespace Sundance;
using namespace Teuchos;




bool Bubble::supportsCellTypePair(
  const CellType& maximalCellType,
  const CellType& cellType
  ) const
{
  switch(maximalCellType)
  {
    case LineCell:
      switch(cellType)
      {
        case LineCell:
        case PointCell:
          return true;
        default:
          return false;
      }
    case TriangleCell:
      switch(cellType)
      {
        case TriangleCell:
        case LineCell:
        case PointCell:
          return true;
        default:
          return false;
      }
    case TetCell:
      switch(cellType)
      {
        case TetCell:
        case TriangleCell:
        case LineCell:
        case PointCell:
          return true;
        default:
          return false;
      }
    default:
      return false;
  }
}

void Bubble::print(std::ostream& os) const 
{
  os << "Bubble(" << order_ << ")";
}

int Bubble::nReferenceDOFsWithoutFacets(
  const CellType& maximalCellType,
  const CellType& cellType
  ) const
{
  if (maximalCellType==cellType) return 1;
  else return 0;
}

void Bubble::getReferenceDOFs(
  const CellType& maxCellType,
  const CellType& cellType,
  Array<Array<Array<int> > >& dofs) const 
{
  typedef Array<int> Aint;

  Aint z(1);
  z[0] = 0;

  switch(cellType)
    {
      case PointCell:
        switch(maxCellType)
        {
          case LineCell:
          case TriangleCell:
          case TetCell:
            dofs.resize(1);
            dofs[0].resize(1);
            return;
          default:
            TEST_FOR_EXCEPT(1);
        }
      case LineCell:
        dofs.resize(2);
        dofs[0].resize(2);
        dofs[1].resize(1);
        if (maxCellType==LineCell)
        {
          dofs[1] = tuple<Aint>(z);          
        }
        return;
      case TriangleCell:
        dofs.resize(3);
        dofs[0].resize(3);
        dofs[1].resize(3);
        dofs[2].resize(1);
        if (maxCellType==TriangleCell)
        {
          dofs[2]=tuple<Aint>(z);
        }
        return;
      case TetCell:
        dofs.resize(4);
        dofs[0].resize(4);
        dofs[1].resize(6);
        dofs[2].resize(4);
        dofs[3].resize(1);
        if (maxCellType==TetCell)
        {
          dofs[3]=tuple<Aint>(z);
        }
        return;
      default:
        TEST_FOR_EXCEPTION(true, std::runtime_error, "Cell type "
          << cellType << " not implemented in Bubble basis");
    }
}




void Bubble::refEval(
  const CellType& cellType,
  const Array<Point>& pts,
  const SpatialDerivSpecifier& sds,
  Array<Array<Array<double> > >& result,
  int verbosity) const
{
  TEST_FOR_EXCEPTION(!(sds.isPartial() || sds.isIdentity()), 
    std::runtime_error,
    "cannot evaluate spatial derivative " << sds << " on Bubble basis");
  const MultiIndex& deriv = sds.mi();
  typedef Array<double> Adouble;
  result.resize(1);
  result[0].resize(pts.length());

  switch(cellType)
    {
    case PointCell:
      result[0] = tuple<Adouble>(tuple(1.0));
      return;
    case LineCell:
      for (int i=0; i<pts.length(); i++)
        {
          evalOnLine(pts[i], deriv, result[0][i]);
        }
      break;
    case TriangleCell:
      for (int i=0; i<pts.length(); i++)
        {
          evalOnTriangle(pts[i], deriv, result[0][i]);
        }
      break;
    case TetCell:
      for (int i=0; i<pts.length(); i++)
        {
          evalOnTet(pts[i], deriv, result[0][i]);
        }
      break;
    default:
      TEST_FOR_EXCEPTION(true, std::runtime_error,
                         "Bubble::refEval() unimplemented for cell type "
                         << cellType);

    }
}

/* ---------- evaluation on different cell types -------------- */

void Bubble::evalOnLine(const Point& pt, 
  const MultiIndex& deriv,
  Array<double>& result) const
{
	ADReal x = ADReal(pt[0], 0, 1);
	ADReal one(1.0, 1);
	
	result.resize(1);
	Array<ADReal> tmp(result.length());

  int p = (order_+1)/2;
  if (order_==0) p=1;
  ADReal xp = one;
  for (int i=0; i<p; i++) xp = xp*x;
  
  tmp[0] = xp*(1.0-xp);

	for (int i=0; i<tmp.length(); i++)
		{
			if (deriv.order()==0) result[i] = tmp[i].value();
			else result[i] = tmp[i].gradient()[0];
		}
}

void Bubble::evalOnTriangle(const Point& pt, 
															const MultiIndex& deriv,
															Array<double>& result) const



{
	ADReal x = ADReal(pt[0], 0, 2);
	ADReal y = ADReal(pt[1], 1, 2);
	ADReal one(1.0, 2);

  Array<ADReal> tmp(1);
  result.resize(1);


  SUNDANCE_OUT(this->verb() > 3, "x=" << x.value() << " y="
               << y.value());

  int p = (order_+2)/3;
  if (order_==0) p=1;

  ADReal xp = one;
  ADReal yp = one;
  for (int i=0; i<p; i++) 
  {
    xp = xp*x;
    yp = yp*y;
  }

  tmp[0] = xp*yp*(1.0-xp-yp)*std::pow(2.0,3*p);

	for (int i=0; i<tmp.length(); i++)
		{
      SUNDANCE_OUT(this->verb() > 3,
                   "tmp[" << i << "]=" << tmp[i].value() 
                   << " grad=" << tmp[i].gradient());
			if (deriv.order()==0) result[i] = tmp[i].value();
			else 
          result[i] = tmp[i].gradient()[deriv.firstOrderDirection()];
		}
}


void Bubble::evalOnTet(const Point& pt, 
												 const MultiIndex& deriv,
												 Array<double>& result) const
{
	ADReal x = ADReal(pt[0], 0, 3);
	ADReal y = ADReal(pt[1], 1, 3);
	ADReal z = ADReal(pt[2], 2, 3);
	ADReal one(1.0, 3);

  result.resize(1);
	Array<ADReal> tmp(result.length());

  int p = (order_+3)/4;
  if (order_==0) p=1;

  ADReal xp = one;
  ADReal yp = one;
  ADReal zp = one;
  for (int i=0; i<p; i++) 
  {
    xp = xp*x;
    yp = yp*y;
    zp = zp*z;
  }

  tmp[0] = (1.0-xp-yp-zp)*xp*yp*zp*std::pow(2.0,4*p);

	for (int i=0; i<tmp.length(); i++)
		{
			if (deriv.order()==0) result[i] = tmp[i].value();
			else 
				result[i] = tmp[i].gradient()[deriv.firstOrderDirection()];
		}
}

