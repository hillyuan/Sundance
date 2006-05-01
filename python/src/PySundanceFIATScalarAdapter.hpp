#ifndef PYSUNDANCE_FIAT_H
#define PYSUNDANCE_FIAT_H
#include "Python.h"
#include "SundanceScalarBasis.hpp"
#include "SundanceDefs.hpp"
#include "Teuchos_RefCountPtr.hpp"
#include <stack>

namespace SundanceStdFwk
{
  using namespace SundanceUtils;
  using namespace SundanceStdMesh;
  using namespace SundanceStdMesh::Internal;
  using namespace Internal;
  using namespace SundanceCore;
  using namespace SundanceCore::Internal;
  using namespace Teuchos;


  class FIATScalarAdapter : public ScalarBasis
  {
  public:
    FIATScalarAdapter( PyObject *pyfamilyclass , int order );
    
    ~FIATScalarAdapter();
    
    void getLocalDOFs(const CellType& cellType,
		      Array<Array<Array<int> > >& dofs) const;
    
    int nNodes(int spatialDim ,
	       const CellType& cellType ) const;
    
    void refEval(int spatialDim,
		 const CellType& cellType,
		 const Array<Point>& pts,
		 const MultiIndex& deriv,
		 Array<Array<double> >& result) const;
    
    int order() const { return order_; }
    
  private:
    // we instantiate the basis for each order
    int order_;
    PyObject *bases_[3];
  };
  
  
}
#endif
