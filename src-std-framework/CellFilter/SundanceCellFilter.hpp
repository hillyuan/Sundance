/* @HEADER@ */
/* @HEADER@ */

#ifndef SUNDANCE_CELLFILTER_H
#define SUNDANCE_CELLFILTER_H

#include "SundanceDefs.hpp"
#include "SundanceCellFilterBase.hpp"
#include "SundanceOrderedHandle.hpp"
#include "TSFHandle.hpp"
#include "Teuchos_RefCountPtr.hpp"

namespace SundanceStdFwk
{
 using namespace SundanceUtils;
using namespace SundanceStdMesh;
using namespace SundanceStdMesh::Internal;
  using namespace SundanceCore::Internal;
  using namespace Teuchos;
  using namespace TSFExtended;
  
  /** 
   * CellFilter is a user-level object representing a 
   * filter that selects from a mesh all cells that
   * satisfy some condition. CellFilters are used to identify subdomains
   * on which equations or boundary conditions apply. 
   * Examples of cell filters are to identify
   * all cells on the boundary of the mesh, or all cells whose node positions
   * satisfy some mathematical equation or inequality. 
   *
   * <h4> Use of CellFilter </h4>
   *
   * \code
   * // Define a filter that will pick out all maximal cells located 
   * // entirely in the left half-plane x < 0 
   * CellFilter elements = new MaximalCellFilter();
   * CellFilter leftHalf = elements.subset( x <= 0.0 );
   * 
   * // Apply the leftHalf filter to a mesh, thus enumerating
   * // all the cells of that mesh living in the left half-plane
   * CellSet leftCells = leftHalf.getCells(mesh);
   * \endcode
   *
   * <h4> Set operations with CellFilter objects </h4>
   *
   * Operations on cell filters can produce new filters. 
   *
   * The subset() and labeledSubset() operators
   * produce new CellFilters that pick out a subset of the cells
   * satisfying an additional condition given in the argument
   * to the subset methods. 
   *
   * Binary set operations can also produce new filters.
   * Suppose
   * <tt>a</tt> and <tt>b</tt> are CellFilters whose <tt>getCells()</tt>
   * methods produce
   * CellSets \f$\{A\}\f$ and \f$\{B\}\f$, respectively. There exist
   * operators for the following binary operations:
   * <ul>
   * <li> The <b>union</b> operator <tt>a+b.</tt> The result of a union
   * operation is a filter that will produce the union of the two operand's
   * cell sets, 
   * \f[{\tt a+b} \rightarrow \{A\} \cup \{B\}, \f]
   * i.e., all cells that are in either \f$\{A\}\f$ or \f$\{B\}\f$
   * <li> The <b>intersection </b> operator <tt>a.intersection(b)</tt> 
   * The result of an intersection
   * operation is a filter that will produce the intersection
   * of the two operand's
   * cell sets, 
   * \f[{\tt a.intersection(b)} \rightarrow \{A\} \cap \{B\}, \f]
   * i.e., all cells that are in both \f$\{A\}\f$ and \f$\{B\}\f$
   * <li> The <b>exclusion </b> operator <tt>a-b.</tt>  
   * The result of an exclusion
   * operation is a filter that will produce the exclusion
   * of the two operand's
   * cell sets, 
   * \f[{\tt a - b} \rightarrow \{A\} \setminus \{B\}, \f]
   * i.e., all cells that are in \f$\{A\}\f$ but not in \f$\{B\}\f$
   * </ul>
   * \code
   * CellFilter elements = new MaximalCellFilter();
   * CellFilter leftHalf = elements.subset( x <= 0.0 );
   * CellFilter topHalf = elements.subset( x >= 0.0 );
   * CellFilter topLeftQuarter = leftHalf.intersection(topHalf);
   * CellFilter 
   * \endcode
   *

   */
  class CellFilter : public OrderedHandle<CellFilterStub>
  {
  public:
    ORDERED_HANDLE_CTORS(CellFilter, CellFilterStub);

    /** Find the cells passing this filter on the given mesh */
    CellSet getCells(const Mesh& mesh) const ;

    /** Return the dimension of this cell set on the given mesh */
    int dimension(const Mesh& mesh) const ;

    /** Return a filter that returns the set union of the sets
     * produced by the two operand filters */
    CellFilter operator+(const CellFilter& other) const ;
    
    /** Return a filter that returns the set difference between the sets
     * produced by the two operand filters */
    CellFilter operator-(const CellFilter& other) const ;

    /** Return a filter that returns the set intersection of the sets
     * produced by the two operand filters */
    CellFilter intersection(const CellFilter& other) const ;

    // /** Return a filter that returns the
//      *  subset of cells for which the logical expression 
//      * is true */
//     CellFilter subset(const LogicalExpr& expr) const ;

    
    /** Return a filter that will return the subset of cells having
     * the given label */
    CellFilter labeledSubset(int label) const ;

    
    /** Return a filter that will return the subset of cells for which
     * the given predicate is true */
    CellFilter subset(const CellPredicate& test) const ;

    /** Indicate whether this is a null cell filter */
    bool isNullCellFilter() const ;

    /** */
    XMLObject toXML() const ;

#ifndef DOXYGEN_DEVELOPER_ONLY

  protected:
    /** */
    const CellFilterBase* cfbPtr() const ;
    
#endif  /* DOXYGEN_DEVELOPER_ONLY */

    };
}

#endif