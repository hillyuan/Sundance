#include "PlayaPCGSolver.hpp"
#include "PlayaOut.hpp"
#include "PlayaTabs.hpp"
#include "PlayaLinearCombinationImpl.hpp"
#include "PlayaSimpleComposedOpImpl.hpp"
#include <iomanip>

using std::setw;

namespace Playa
{

  SolverState<double> PCGSolver::solve(const LinearOperator<double>& A,
				       const Vector<double>& b,
				       Vector<double>& x) const
  {
    if (precFactory_.ptr().get() == 0)
      {
	return solveUnprec(A, b, x);
      }
    else
      {
	SolverState<double> rtn;
	Preconditioner<double> prec = precFactory_.createPreconditioner(A);
	if (prec.isIdentity())
	  {
	    rtn = solveUnprec(A, b, x);
	  }
	else if (prec.isTwoSided())
	  {
	    LinearOperator<double> P = prec.left();
	    LinearOperator<double> Q = prec.right();
	    LinearOperator<double> PAQ = P*A*Q;
	    Vector<double> Pb = P*b;
	    Vector<double> y;
	    rtn = solveUnprec(PAQ, Pb, y);
	    x = Q*y;
	  }
	else if (prec.hasRight())
	  {
	    LinearOperator<double> Q = prec.right();
	    LinearOperator<double> AQ = A*Q;
	    Vector<double> y;
	    rtn = solveUnprec(AQ, b, y);
	    x = Q*y;
	  }
	else if (prec.hasLeft())
	  {
	    LinearOperator<double> P = prec.left();
	    LinearOperator<double> PA = P*A;
	    Vector<double> Pb = P*b;
	    rtn = solveUnprec(PA, Pb, x);
	  }
	else
	  {
	    TEUCHOS_TEST_FOR_EXCEPTION(true, std::runtime_error,
				       "Inconsistent preconditioner state "
				       "in PGCSolver::solve()");
	  }
	return rtn;
      }
  }
  
  SolverState<double> PCGSolver::solveUnprec(const LinearOperator<double>& A,
					     const Vector<double>& b,
					     Vector<double>& x) const
  {
    Tabs tab0;
    const ParameterList& params = parameters();
    int verb = 0;
    if (params.isParameter("Verbosity"))
      {
	verb = getParameter<int>(params, "Verbosity");
      }

    int maxIters = getParameter<int>(params, "Max Iterations");
    double tol = getParameter<double>(params, "Tolerance");

    /* Display diagnostic information if needed */
    PLAYA_ROOT_MSG1(verb, tab0 << "Playa CG Solver");
    Tabs tab1;
    PLAYA_ROOT_MSG2(verb, tab0);
    PLAYA_ROOT_MSG2(verb, tab1 << "Verbosity      " << verb);
    PLAYA_ROOT_MSG2(verb, tab1 << "Max Iters      " << maxIters);
    PLAYA_ROOT_MSG2(verb, tab1 << "Tolerance      " << tol);

    /* if the solution vector isn't initialized, set to RHS */
    if (x.ptr().get()==0) x = b.copy();
    
    Vector<double> r = b - A*x;
    Vector<double> p = r.copy();
    double bNorm = b.norm2();
    PLAYA_ROOT_MSG2(verb, tab1 << "Problem size:  " << b.space().dim());
    PLAYA_ROOT_MSG2(verb, tab1 << "||rhs||        " << bNorm);

    /* Prepare the status object to be returned */
    SolverState<double> rtn;
    bool converged = false;
    bool shortcut = false;

    /* Check for zero rhs */
    if (bNorm==0.0)
      {
	PLAYA_ROOT_MSG2(verb, tab1 << "RHS is zero!");
	rtn = SolverState<double>(SolveConverged, "Woo-hoo! Zero RHS shortcut", 
				  0, 0.0);
	shortcut = true;
      }

    double rNorm = 0.0;
    double rtr = r*r;

    if (!shortcut)
      {
	PLAYA_ROOT_MSG2(verb, tab1 << endl << tab1 << "CG Loop");

	for (int k=0; k<maxIters; k++)
	  {
	    Tabs tab2;
	    Vector<double> Ap = A*p;
	    double alpha = rtr/(p*Ap);
	    x += alpha*p;
	    r -= alpha*Ap;
	    double rtrNew = r*r;
	    rNorm = sqrt(rtrNew);
	    PLAYA_ROOT_MSG2(verb, tab2 << "iter=" << setw(10) 
			    << k << setw(20) << "||r||=" << rNorm);
	    /* check relative residual */
	    if (rNorm < bNorm*tol) 
	      {
		rtn = SolverState<double>(SolveConverged, "Woo-hoo!", 
					  k, rNorm);
		converged = true;
		break;
	      }
	    double beta = rtrNew/rtr;
	    rtr = rtrNew;
	    p = r + beta*p;
	  }
      }

    if (!converged && !shortcut)
      {
	rtn = SolverState<double>(SolveFailedToConverge, 
				  "CG failed to converge", 
				  maxIters, rNorm);
      }

    PLAYA_ROOT_MSG2(verb, tab0 << endl
		    << tab0 << "CG finished with status " 
		    << rtn.stateDescription());
    return rtn;
  }


}
