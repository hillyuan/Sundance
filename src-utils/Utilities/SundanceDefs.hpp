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

#ifndef SUNDANCE_DEFS_H
#define SUNDANCE_DEFS_H

#include "Teuchos_ConfigDefs.hpp"
#include "Teuchos_TestForException.hpp"
#include "Teuchos_Array.hpp"

#ifndef __cplusplus
#define __cplusplus
#endif

#ifdef HAVE_CONFIG_H

/*
 * The macros PACKAGE, PACKAGE_NAME, etc, get defined for each package 
 * and need to be undef'd here to avoid warnings when this file is 
 * included from another package.
 * KL 11/25/02
 */   
#ifdef PACKAGE
#undef PACKAGE
#endif

#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif

#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif

#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif

#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif

#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif

#ifdef VERSION
#undef VERSION
#endif

#include "SundanceConfig.h"
#include "TSF_config.h"

#ifdef PACKAGE
#undef PACKAGE
#endif

#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif

#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif

#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif

#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif

#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif

#ifdef VERSION
#undef VERSION
#endif


#endif /* HAVE_CONFIG_H */

namespace Teuchos {}

namespace SundanceUtils {}

namespace SundanceCore 
{
  namespace Internal{}
}

namespace SundanceStdMesh
{
  namespace Internal{}
}

#ifdef TRILINOS_8

namespace Teuchos {

template<typename T>
std::vector<T>& createVector( Array<T> &a ) { return a; }

template<typename T>
const std::vector<T>& createVector( const Array<T> &a ) { return a; }

} // namespace Teuchos

#endif // TRILINOS_8

#endif
