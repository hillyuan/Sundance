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

#include "SundanceMap.hpp"
#include "SundanceTabs.hpp"
#include "SundanceOut.hpp"
#include "SundanceOrderedTuple.hpp"
#include "SundanceNodalDOFMapHN.hpp"
#include "SundanceLagrange.hpp"
#include "Teuchos_MPIContainerComm.hpp"
#include "Teuchos_TimeMonitor.hpp"

using namespace Sundance;
using namespace Teuchos;

NodalDOFMapHN::NodalDOFMapHN(const Mesh& mesh,
  int nFuncs, 
  const CellFilter& maxCellFilter,
  int setupVerb)
  : HNDoFMapBase(mesh, nFuncs, setupVerb),
    maxCellFilter_(maxCellFilter),
    dim_(mesh.spatialDim()),
    nFuncs_(nFuncs),
    nElems_(mesh.numCells(mesh.spatialDim())),
    nNodes_(mesh.numCells(0)),
    nNodesPerElem_(mesh.numFacets(mesh.spatialDim(),0,0)),
    elemDofs_(nElems_ * nFuncs * nNodesPerElem_),
    nodeDofs_(mesh.numCells(0)*nFuncs_, -1),
    structure_(rcp(new MapStructure(nFuncs_, rcp(new Lagrange(1))))),
    hasCellHanging_(nElems_),
    nodeIsHanging_(nElems_ * nFuncs * nNodesPerElem_),
    cellsWithHangingDoF_globalDoFs_(),
    cells_To_NodeLIDs_(),
    hangingNodeLID_to_NodesLIDs_(),
    hangindNodeLID_to_Coefs_(),
    facetLID_()
{
  init();
}

void NodalDOFMapHN::init()
{ 
  Tabs tab;

  SUNDANCE_MSG2(setupVerb(), tab << "NodalDOFMapHN initializing nodal DOF map nrFunc:"
		  << nFuncs_ << "  nNodes_:" << nNodes_ << " nElems_:" <<nElems_);

  Array<Array<int> > remoteNodes(mesh().comm().getNProc());
  Array<int> hasProcessedCell(nNodes_, 0);

  /* start the DOF count at zero. */
  int nextDOF = 0;
  int owner;

  nFacets_ = mesh().numFacets(dim_, 0, 0);
  Array<int> elemLID(nElems_);
  Array<int> facetOrientations(nFacets_*nElems_);

  /* Assign node DOFs for locally owned 0-cells */
  CellSet maxCells = maxCellFilter_.getCells(mesh());

  int cc = 0;
  for (CellIterator iter=maxCells.begin(); iter != maxCells.end(); iter++, cc++)
    {
      int c = *iter;
      elemLID[cc] = c;
    }
  /* look up the LIDs of the facets (points)*/
  /* This is important so that we have locality in the DOF numbering */
  mesh().getFacetLIDs(dim_, elemLID, 0, facetLID_, facetOrientations);
  
  for (int c=0; c<nElems_; c++) {
	  hasCellHanging_[c] = false;
      /* for each facet, process its DOFs */
      for (int f=0; f<nFacets_; f++) {
          /* if the facet's DOFs have been assigned already,
           * we're done */
          int fLID = facetLID_[c*nFacets_+f];
          SUNDANCE_MSG2(setupVerb(), "NodalDOFMapHN::init() CellLID:"<< c <<"Try point LID:" << fLID << " facet:" << f);
          if (hasProcessedCell[fLID] == 0) {
              /* the facet may be owned by another processor */
              if (isRemote(0, fLID, owner)) {
                  int facetGID 
                    = mesh().mapLIDToGID(0, fLID);
                  remoteNodes[owner].append(facetGID);
                }
              else {/* we can assign a DOF locally */
            	    SUNDANCE_MSG2(setupVerb(), "NodalDOFMapHN::init() Doing point LID:" << fLID << " facet:" << f);
                    // test if the node is not a hanging node
                    if ( mesh().isElementHangingNode(0,fLID) == false ){
                       /* assign DOFs , (for each function space) */
                       for (int i=0; i<nFuncs_; i++){
                          nodeDofs_[fLID*nFuncs_ + i] = nextDOF;
                          nextDOF++;
                       }
                    } else {
                       SUNDANCE_MSG2(setupVerb(), "NodalDOFMapHN::init() Hanging node found LID:" << fLID);
                  	   hasCellHanging_[c] = true;
                       for (int i=0; i<nFuncs_; i++){
                	       nodeDofs_[fLID*nFuncs_ + i] = -1; // this means that this is not golbal DoF
                       }
                       Array<int> pointsLIDs;
                       Array<int> facetIndex;
                       Array<double> coefs;
                       // get the global DoFs which contribute (what if that is not yet numbered?, then only the "fLIDs")
                       getPointLIDsForHN( fLID, f, c , pointsLIDs, coefs , facetIndex);

                	   // also store the corresponding coefficients
                       hangingNodeLID_to_NodesLIDs_.put( fLID , pointsLIDs );
                       hangindNodeLID_to_Coefs_.put(fLID,coefs);
                    }
                }
              hasProcessedCell[fLID] = 1;
            }
            // if this node is hanging then mark the cell as hanging
            if (mesh().isElementHangingNode(0,fLID) == true) {
        	          hasCellHanging_[c] = true;
            }
       }
  }
  
  /* Compute offsets for each processor */
  int localCount = nextDOF;
  computeOffsets(localCount);
  
  /* Resolve remote DOF numbers */
  shareRemoteDOFs(remoteNodes);


  /* Assign DOFs for elements */
  for (int c=0; c<nElems_; c++)
    {
	    if (hasCellHanging_[c]){
	    	Array<int> HNDoFs;
	    	Array<double> transMatrix;
	    	Array<double> coefs;

	  		HNDoFs.resize(4);
	    	transMatrix.resize(nFacets_*nFacets_);
	    	for (int ii = 0 ; ii < nFacets_*nFacets_ ; ii++) transMatrix[ii] = 0.0;

	    	for (int f=0; f<nFacets_; f++)
	    	{
	  		  int fLID = facetLID_[c*nFacets_+f];
	  		SUNDANCE_MSG2(setupVerb(), tab << "NodalDOFMapHN cell:" << c << " facetLID:" << fLID
	  				  << " hanging:"<< nodeDofs_[fLID*nFuncs_] << "  array:" << HNDoFs);
              if (nodeDofs_[fLID*nFuncs_] < 0)
              {
                  Array<int> pointsLIDs;
                  Array<int> facetIndex;
                  Array<double> coefs;
                  // get the composing points
                  getPointLIDsForHN( fLID, f, c , pointsLIDs, coefs , facetIndex);

                  for (int j=0 ; j < pointsLIDs.size() ; j++){
                	  // look for tmpArray[j] in the existing set, put there coefs[j]
                	  HNDoFs[facetIndex[j]] = pointsLIDs[j];
                	  transMatrix[f*nFacets_  + facetIndex[j]] = coefs[j];
                  }
              }
              else
              {
            	  // look for fLID in the actual set and put there 1.0, if not found then size+1
            	  HNDoFs[f] = fLID;
            	  transMatrix[f*nFacets_  + f] = 1.0;
              }
	    	}
	    	// store the matrix corresponding to this cell
	    	maxCellLIDwithHN_to_TrafoMatrix_.put( c , transMatrix );
	    	// store the point LID's which contribute to this cell
	    	cellsWithHangingDoF_globalDoFs_.put( c , HNDoFs );

	    	SUNDANCE_MSG2(setupVerb(), tab << "NodalDOFMapHN initializing cellLID:" << c << " array:" << HNDoFs);
	    	SUNDANCE_MSG2(setupVerb(), tab << "NodalDOFMapHN initializing cellLID:" << c << " Trafo array:" << transMatrix);

	    	// add the global DOFs to the array
	    	for (int f=0; f<nFacets_; f++)
	    	{
	    		int fLID = HNDoFs[f];
	    		for (int i=0; i<nFuncs_; i++)
	    		{
	    			elemDofs_[(c*nFuncs_+i)*nFacets_ + f] = nodeDofs_[fLID*nFuncs_ + i];
	    		}
	    	}
	    	SUNDANCE_MSG2(setupVerb(),tab << "NodalDOFMapHN initializing cellLID:" << c << " elemDofs_:" << elemDofs_);
	    }
	    else {
		/* set the element DOFs given the dofs of the facets */
	    	for (int f=0; f<nFacets_; f++)
	    	{
	    		int fLID = facetLID_[c*nFacets_+f];
	    		for (int i=0; i<nFuncs_; i++)
	    		{
	    			elemDofs_[(c*nFuncs_+i)*nFacets_ + f] = nodeDofs_[fLID*nFuncs_ + i];
	    		}
	    	}
	    	  SUNDANCE_MSG2(setupVerb(),tab << "NodalDOFMapHN initializing cellLID:" << c << " elemDofs_:" << elemDofs_);
	    }
    }
}

RCP<const MapStructure>
NodalDOFMapHN::getDOFsForCellBatch(int cellDim,
  const Array<int>& cellLID,
  const Set<int>& requestedFuncSet,
  Array<Array<int> >& dofs,
  Array<int>& nNodes,
  int verbosity) const
{
  TimeMonitor timer(batchedDofLookupTimer());

  Tabs tab;
  SUNDANCE_MSG2(verbosity,
               tab << "NodalDOFMapHN::getDOFsForCellBatch(): cellDim=" << cellDim
               << " requestedFuncSet:" << requestedFuncSet
               << " cellLID=" << cellLID);


  dofs.resize(1);
  nNodes.resize(1);

  int nCells = cellLID.size();


  if (cellDim == dim_)
    {
      int dofsPerElem = nFuncs_ * nNodesPerElem_;
      nNodes[0] = nNodesPerElem_;
      dofs[0].resize(nCells * dofsPerElem);
      Array<int>& dof0 = dofs[0];
      Array<int> tmpArray;

	  tmpArray.resize(dofsPerElem);

      for (int c=0; c<nCells; c++)
        {
    	    // here , nothing to do ... in elemDofs_ should be the proper DoFs
            for (int i=0; i<dofsPerElem; i++) {
               dof0[c*dofsPerElem + i] = elemDofs_[cellLID[c]*dofsPerElem+i];
               tmpArray[i] = elemDofs_[cellLID[c]*dofsPerElem+i];
            }
            SUNDANCE_MSG2(verbosity,tab << "NodalDOFMapHN::getDOFsForCellBatch cellDim:" <<
            		cellDim << " cellLID:" << c << " array:" << tmpArray);
        }
    }
  else if (cellDim == 0)
    { // point integrals are also mostly used for BCs
      nNodes[0] = 1;
      dofs[0].resize(nCells * nFuncs_);
      Array<int>& dof0 = dofs[0];
      Array<int> tmpArray;
	  tmpArray.resize(nFuncs_);

      for (int c=0; c<nCells; c++)
        {
          for (int i=0; i<nFuncs_; i++)
            {
              dof0[c*nFuncs_ + i] = nodeDofs_[cellLID[c]*nFuncs_+i];
              tmpArray[i] = nodeDofs_[cellLID[c]*nFuncs_+i];
            }
          SUNDANCE_MSG2(verbosity,tab << "NodalDOFMapHN::getDOFsForCellBatch cellDim:" <<
          		cellDim << " pointLID:" << cellLID[c] << " array:" << tmpArray);
        }
    }
  else
    {
	  // since edge or surface Integrals are mostly used for BCs, where are NO HN
      int nFacets = mesh().numFacets(cellDim, cellLID[0], 0);
      nNodes[0] = nFacets;
      int dofsPerCell = nFuncs_ * nNodes[0];
      dofs[0].resize(nCells * dofsPerCell);
      Array<int>& dof0 = dofs[0];
      Array<int> facetLIDs(nCells * nNodes[0]);
      Array<int> dummy(nCells * nNodes[0]);
      Array<int> tmpArray;
	  tmpArray.resize(nFuncs_*nFacets);

      mesh().getFacetLIDs(cellDim, cellLID, 0, facetLIDs, dummy);

      for (int c=0; c<nCells; c++)
        {
          for (int f=0; f<nFacets; f++)
            {
              int facetCellLID = facetLIDs[c*nFacets+f];
              for (int i=0; i<nFuncs_; i++)
                {
                  dof0[(c*nFuncs_+i)*nFacets + f]
                    = nodeDofs_[facetCellLID*nFuncs_+i];
                  tmpArray[f*nFuncs_+i] = nodeDofs_[facetCellLID*nFuncs_+i];
                }
            }
          SUNDANCE_MSG2(verbosity,tab << "NodalDOFMapHN::getDOFsForCellBatch cellDim:" <<
          		cellDim << " edgeLID:" << cellLID[c] << " array:" << tmpArray);
        }
    }
  SUNDANCE_MSG2(verbosity,
                tab << "NodalDOFMapHN::getDOFsForCellBatch(): DONE");
  return structure_;
}


void NodalDOFMapHN::getTrafoMatrixForCell(
	    int cellLID,
	    int funcID,
	    int& trafoMatrixSize,
	    bool& doTransform,
	    Array<double>& transfMatrix ) const {

	trafoMatrixSize = nFacets_;

	if (cellsWithHangingDoF_globalDoFs_.containsKey(cellLID))
	{
		// return the transformation matrix from the Map
		transfMatrix = maxCellLIDwithHN_to_TrafoMatrix_.get( cellLID ); // this should return a valid array
		doTransform = true;
	}
	else  // no transformation needed, return false
	{
		doTransform = false;
	}
}

/** Function for nodal plotting */
void NodalDOFMapHN::getDOFsForHNCell(
	  int cellDim,
	  int cellLID,
      int funcID,
      Array<int>& dofs ,
      Array<double>& coefs ) const {

	// treat the cells only of dimension zero
	if (  (cellDim == 0) && (hangingNodeLID_to_NodesLIDs_.containsKey(cellLID))  )
	{
		Array<int> pointLIDs;
		Array<int> pointCoefs;
		pointLIDs = hangingNodeLID_to_NodesLIDs_.get( cellLID );
		dofs.resize(pointLIDs.size());
		// return the DoFs belonging to the points and function which collaborate to the hanging node
		for (int ind = 0 ; ind < pointLIDs.size() ; ind++){
			dofs[ind] = nodeDofs_[ pointLIDs[ind] *nFuncs_ + funcID ]; // return the DoF corresp. to function ID
		}
		// get the coefficients
		coefs = hangindNodeLID_to_Coefs_.get( cellLID );
	}
}

/** This function is only for TRISECTION implemented */
// we might use the method of BasisFunction->getConstraintForHN(), but this should be faster
void NodalDOFMapHN::getPointLIDsForHN( int pointLID , int facetIndex ,
		int maxCellIndex ,Array<int>& glbLIDs, Array<double>& coefsArray , Array<int>& nodeIndex){

	Array<int> facets;
    int indexInParent , parentLID , facetCase = 0;
    double divRatio = 0.5;
	switch (dim_){
	case 2:
		// todo: eventually implement bisection as well (if it will be needed)
		glbLIDs.resize(2);
		nodeIndex.resize(2);
		indexInParent = mesh().indexInParent(maxCellIndex);
		switch (indexInParent){
		  case 0: {facetCase = (facetIndex == 1)? 0 : 1;
		          divRatio = (1.0/3.0);  break;}
		  case 1: {facetCase = 0;
		          divRatio = (facetIndex == 0)? (1.0/3.0) : (2.0/3.0);  break;}
		  case 2: {facetCase = (facetIndex == 0)? 0 : 2;
                  divRatio = (facetIndex == 0)? (2.0/3.0) : (1.0/3.0);  break;}
		  case 5: {facetCase = 1;
                  divRatio = (facetIndex == 0)? (1.0/3.0) : (2.0/3.0);  break;}
		  case 3: {facetCase = 2;
                  divRatio = (facetIndex == 1)? (1.0/3.0) : (2.0/3.0);  break;}
		  case 6: {facetCase = (facetIndex == 0)? 1 : 3;
                  divRatio = (facetIndex == 0)? (2.0/3.0) : (1.0/3.0);  break;}
		  case 7: {facetCase = 3;
                  divRatio = (facetIndex == 2)? (1.0/3.0) : (2.0/3.0);  break;}
		  case 8: {facetCase = (facetIndex == 1)? 2 : 3;
                  divRatio = (2.0/3.0);  break;}
		}
		mesh().returnParentFacets(maxCellIndex , 0 ,facets , parentLID );
		switch (facetCase){
		   case 0: {glbLIDs[0] = facets[0]; glbLIDs[1] = facets[1];
		            nodeIndex[0] = 0;  nodeIndex[1] = 1;  break;}
		   case 1: {glbLIDs[0] = facets[0]; glbLIDs[1] = facets[2];
		            nodeIndex[0] = 0;  nodeIndex[1] = 2;  break;}
		   case 2: {glbLIDs[0] = facets[1]; glbLIDs[1] = facets[3];
		            nodeIndex[0] = 1;  nodeIndex[1] = 3;   break;}
		   case 3: {glbLIDs[0] = facets[2]; glbLIDs[1] = facets[3];
		            nodeIndex[0] = 2;  nodeIndex[1] = 3;  break;}
		}
		 coefsArray.resize(2); coefsArray[0] = 1 - divRatio; coefsArray[1] = divRatio;
		  SUNDANCE_MSG2(setupVerb(),"NodalDOFMapHN::getPointLIDsForHN() fc=" << facetCase << " R=" << divRatio
		               << " facetIndex:" << facetIndex <<   " indexInParent:" << indexInParent <<" glbLIDs:"
		               << glbLIDs << " maxCellIndex:" << maxCellIndex << " nodeIndex:" << nodeIndex);
		break;
	case 3:
		glbLIDs.resize(4);
        // Todo: implement this for 3D , this might get here much complicated ...
		break;
	}

}



void NodalDOFMapHN::computeOffsets(int localCount)
{
  Array<int> dofOffsets;
  int totalDOFCount;
  int myOffset = 0;
  int np = mesh().comm().getNProc();
  if (np > 1)
    {
      MPIContainerComm<int>::accumulate(localCount, dofOffsets, totalDOFCount,
                                        mesh().comm());
      myOffset = dofOffsets[mesh().comm().getRank()];

      int nDofs = nNodes_ * nFuncs_;
      for (int i=0; i<nDofs; i++)
        {
          if (nodeDofs_[i] >= 0) nodeDofs_[i] += myOffset;
        }
    }
  else
    {
      totalDOFCount = localCount;
    }
  
  setLowestLocalDOF(myOffset);
  setNumLocalDOFs(localCount);
  setTotalNumDOFs(totalDOFCount);
}


void NodalDOFMapHN::shareRemoteDOFs(const Array<Array<int> >& outgoingCellRequests)
{
  int np = mesh().comm().getNProc();
  if (np==1) return;
  int rank = mesh().comm().getRank();

  Array<Array<int> > incomingCellRequests;
  Array<Array<int> > outgoingDOFs(np);
  Array<Array<int> > incomingDOFs;

  SUNDANCE_MSG2(setupVerb(),
               "p=" << mesh().comm().getRank()
               << "synchronizing DOFs for cells of dimension 0");
  SUNDANCE_MSG2(setupVerb(),
               "p=" << mesh().comm().getRank()
               << " sending cell reqs d=0, GID=" 
               << outgoingCellRequests);

  /* share the cell requests */
  MPIContainerComm<int>::allToAll(outgoingCellRequests, 
                                  incomingCellRequests,
                                  mesh().comm());
  
  /* get DOF numbers for the zeroth function index on every node that's been 
   * requested by someone else */
  for (int p=0; p<np; p++)
    {
      if (p==rank) continue;
      const Array<int>& requestsFromProc = incomingCellRequests[p];
      int nReq = requestsFromProc.size();

      SUNDANCE_MSG4(setupVerb(), "p=" << mesh().comm().getRank() 
                            << " recv'd from proc=" << p
                            << " reqs for DOFs for cells " 
                            << requestsFromProc);

      outgoingDOFs[p].resize(nReq);

      for (int c=0; c<nReq; c++)
        {
          int GID = requestsFromProc[c];
          SUNDANCE_MSG2(setupVerb(),
                       "p=" << rank
                       << " processing zero-cell with GID=" << GID); 
          int LID = mesh().mapGIDToLID(0, GID);
          SUNDANCE_MSG2(setupVerb(),
                       "p=" << rank
                       << " LID=" << LID << " dofs=" 
                       << nodeDofs_[LID*nFuncs_]);
          outgoingDOFs[p][c] = nodeDofs_[LID*nFuncs_];
          SUNDANCE_MSG2(setupVerb(),
                       "p=" << rank
                       << " done processing cell with GID=" << GID);
        }
    }

  SUNDANCE_MSG2(setupVerb(),
               "p=" << mesh().comm().getRank()
               << "answering DOF requests for cells of dimension 0");

  /* share the DOF numbers */
  MPIContainerComm<int>::allToAll(outgoingDOFs,
                                  incomingDOFs,
                                  mesh().comm());

  SUNDANCE_MSG2(setupVerb(),
               "p=" << mesh().comm().getRank()
               << "communicated DOF answers for cells of dimension 0" );

  
  /* now assign the DOFs from the other procs */

  for (int p=0; p<mesh().comm().getNProc(); p++)
    {
      if (p==mesh().comm().getRank()) continue;
      const Array<int>& dofsFromProc = incomingDOFs[p];
      int numCells = dofsFromProc.size();
      for (int c=0; c<numCells; c++)
        {
          int cellGID = outgoingCellRequests[p][c];
          int cellLID = mesh().mapGIDToLID(0, cellGID);
          int dof = dofsFromProc[c];
          for (int i=0; i<nFuncs_; i++)
            {
              nodeDofs_[cellLID*nFuncs_ + i] = dof+i;
              addGhostIndex(dof+i);
            }
        }
    }
}
