SET(LIBRARY_NAME ${PROJECT_NAME})

SET(${LIBRARY_NAME}_SOURCES
  utils/Box.cc
  utils/ProblemConfig.cc
  utils/Printer.cc

  functions/BoxAbovePlan.cc
  functions/BoxAboveFixedPlan.cc
  functions/CostDistance.cc
  functions/FixedBoxPosition.cc
  functions/PlanBetweenBoxAndObstacle.cc

  solvers/QP.cc
  solvers/QPPlanesFixed.cc
  solvers/QPBoxesFixed.cc
  solvers/QPBoxesFixedIndividual.cc
  solvers/LPQPsolver.cc

  TrajectoryProblem.cc

)
SET(${LIBRARY_NAME}_HEADERS
  utils/Box.hh
)
ADD_LIBRARY(${LIBRARY_NAME}
  SHARED
  ${${LIBRARY_NAME}_SOURCES}
  )

#PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} qpOASES)

#add_executable(exp1 test.cc)
TARGET_LINK_LIBRARIES (${LIBRARY_NAME} ${Boost_LIBRARIES} ${QPOASES_LIBRARY})
INSTALL(TARGETS ${LIBRARY_NAME} DESTINATION lib)
