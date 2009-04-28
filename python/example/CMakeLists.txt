INCLUDE(PackageCopyFilesToBinaryDir)
INCLUDE(PackageAddTest)

SET(TEST_LIST
  Poisson1D
)

PACKAGE_COPY_FILES_TO_BINARY_DIR(setpath.py
  SOURCE_FILES setpath.src
  DEST_FILES   setpath.py)

SET(SolverParamPath ${PACKAGE_SOURCE_DIR}/etc/SolverParameters)

PACKAGE_COPY_FILES_TO_BINARY_DIR(PySundanceTestCopyFiles
  DEST_FILES 
                   amesos.xml 
                   anasazi-ml.xml 
                   aztec-ifpack.xml 
                   aztec-ml.xml 
                   aztec-native.xml
                   aztec.xml
                   bicgstab.xml
                   gmres.xml
                   nox.xml            
                   poissonParams.xml
                   userPrecParams.xml
  SOURCE_DIR ${SolverParamPath}
  )

PACKAGE_COPY_FILES_TO_BINARY_DIR(PySundanceSolverCopyFiles
  SOURCE_FILES
    amesosSolver.py
    aztecSolver.py
    bicgstabSolver.py
    blockTriangularSolver.py
    noxSolver.py
  DEST_FILES 
    amesosSolver.py
    aztecSolver.py
    bicgstabSolver.py
    blockTriangularSolver.py
    noxSolver.py
  )



SET(PAR_TEST_LIST
  Poisson1D
  Poisson2D
  RadDiff1D
  Helmholtz1D
  VorticityStokes2D
)

SET(SERIAL_TEST_LIST
  Poisson3D
  ConvTest2D
  AToC
  NonlinearPartialDomain
  TwoZoneLaplace1D
)



FOREACH(TEST ${PAR_TEST_LIST})

  PACKAGE_COPY_FILES_TO_BINARY_DIR(${TEST}.py
    SOURCE_FILES ex${TEST}.py
    DEST_FILES   ${TEST}.py)
  
  PACKAGE_ADD_TEST(
    ${PYTHON_EXECUTABLE}
    NOEXEPREFIX
    NOEXESUFFIX
    NAME Py${TEST}
    ARGS "${TEST}.py"
    COMM serial mpi
    )
    

ENDFOREACH()


FOREACH(TEST ${SERIAL_TEST_LIST})

  PACKAGE_COPY_FILES_TO_BINARY_DIR(${TEST}.py
    SOURCE_FILES ex${TEST}.py
    DEST_FILES   ${TEST}.py)
  
  PACKAGE_ADD_TEST(
    ${PYTHON_EXECUTABLE}
    NOEXEPREFIX
    NOEXESUFFIX
    NAME Py${TEST}
    ARGS "${TEST}.py"
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
    

ENDFOREACH()
