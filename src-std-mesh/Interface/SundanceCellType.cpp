/* @HEADER@ */
/* @HEADER@ */

#include "SundanceCellTopologyCode.hpp"
#include "SundanceExceptions.hpp"

using namespace Sundance::StdMesh::Internal;

namespace Sundance
{

  string toString(const CellTopologyCode& cellType)
  {
    switch(cellType)
      {
      case NullCell:
        return "NullCell";
      case PointCell:
        return "PointCell";
      case LineCell:
        return "LineCell";
      case TriangleCell:
        return "TriangleCell";
      case QuadCell:
        return "QuadCell";
      case TetCell:
        return "TetCell";
      case BrickCell:
        return "BrickCell";
      case PrismCell:
        return "PrismCell";
      }

  }

  int dimension(const CellTopologyCode& cellType)
  {
    switch(cellType)
      {
      case NullCell:
        return -1;
      case PointCell:
        return 0;
      case LineCell:
        return 1;
      case TriangleCell:
      case QuadCell:
        return 2;
      case TetCell:
      case BrickCell:
      case PrismCell:
        return 3;
      }

  }
}
