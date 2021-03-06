#include <hpp/foot/solvers/QPPlanesFixed.hh>

namespace hpp{
namespace foot
{
QPPlanesFixed::QPPlanesFixed(const TrajectoryProblem& pb) : pb_(pb)
{
  // The problem's variables are the boxes variables plus the relaxation term(1
  // or 2)
  // The problem's constraint are one per pair fixed plan-box
  // plus one of dimension 2 per pair of successive mobile boxes, minus one for
  // the first pair because the first box is fixed
  // plus one of dimension 3 to lock the position of the final box
  size_t nRelaxationTerms = 1;
  if (pb_.hasVirtualObstacles()) nRelaxationTerms = 2;
  setDimensions(pb_.dimBoxes() + nRelaxationTerms,
                pb_.nFixedPlanes() * pb_.nBoxes() + 2 * pb_.nPlans() - 1 + 3);
}

QPPlanesFixed::~QPPlanesFixed() {}

//void QPPlanesFixed::addRelaxationTerm(const double& alpha)
//{
  //c_.tail(1) << alpha * 1;
  //C_.col(dimVar_ - 1).head(C_.cols()-3).setConstant(1);
  //lVar_.tail(1) << 0;
//}

void QPPlanesFixed::addRelaxationTerm(size_t cstrIndexBegin, size_t cstrSize,
                                      bool isVirtual)
{
  // virtual relaxation term is the last variable, normal relaxation term is the
  // one-to-last variable
  size_t colIndex = dimVar_ - 1;
  if (pb_.hasVirtualObstacles())
    colIndex = isVirtual ? dimVar_ - 1 : dimVar_ - 2;
  C_.col(colIndex).segment(cstrIndexBegin, cstrSize).setConstant(1);
}

void QPPlanesFixed::formQP(ConstRefVec xPlanes)
{
  reset();
  pb_.costFct().fillQuadCost(A_.topLeftCorner(pb_.dimBoxes(), pb_.dimBoxes()),
                             c_.head(pb_.dimBoxes()));
  // Adding relaxation terms to the cost function and lower bound of variables
  if (pb_.hasVirtualObstacles())
  {
    c_.tail(2) << alpha_, alphaVirtual_;
    lVar_.tail(2) << 0, 0;
  }
  else
  {
    c_.tail(1) << alpha_;
    lVar_.tail(1) << 0;
  }

  //A_.bottomRightCorner(1, 1) << 1;

  long cstrIndexBegin = 0;
  for (size_t i = 0; i < pb_.boxAboveFixedPlanFcts().size(); ++i)
  {
    long boxIndexBegin = 3 * pb_.boxAboveFixedPlanFcts().at(i).box().index();
    pb_.boxAboveFixedPlanFcts().at(i).fillLinCstr(
        l_(cstrIndexBegin), C_.block(cstrIndexBegin, boxIndexBegin, 1, 3));

    addRelaxationTerm(cstrIndexBegin, 1, false);

    cstrIndexBegin += 1;
  }
  for (size_t iPlan = 0; iPlan < pb_.nMobilePlanCstr(); ++iPlan)
  {
    const long iBox0Above(pb_.plans().at(iPlan).box0Above());
    const long iBox1Above(pb_.plans().at(iPlan).box1Above());
    const long iBoxBelow(pb_.plans().at(iPlan).boxBelow());
    Eigen::Vector3d planN;
    double planD(xPlanes(4 * iPlan));
    planN << xPlanes.segment(4 * iPlan + 1, 3);

    Eigen::Matrix<double, 1, 3> placeholderMatrix;

    Index box0IndexBegin = 3 * iBox0Above;
    Index box1IndexBegin = 3 * iBox1Above;

    if (!pb_.getBox(iBox0Above).fixed())
    {
      PlanBetweenBoxAndObstacle::fillLinCstr(
          pb_.getBox(iBox0Above), pb_.obstacles().at(iBoxBelow), planD, planN,
          l_.segment(cstrIndexBegin, 1),
          C_.block(cstrIndexBegin, box0IndexBegin, 1, 3),
          pb_.securityDistance());
      addRelaxationTerm(cstrIndexBegin, 1,
                        pb_.obstacles().at(iBoxBelow).isVirtual());
      cstrIndexBegin += 1;
    }
    if (!pb_.getBox(iBox1Above).fixed())
    {
      PlanBetweenBoxAndObstacle::fillLinCstr(
          pb_.getBox(iBox1Above), pb_.obstacles().at(iBoxBelow), planD, planN,
          l_.segment(cstrIndexBegin, 1),
          C_.block(cstrIndexBegin, box1IndexBegin, 1, 3),
          pb_.securityDistance());
      addRelaxationTerm(cstrIndexBegin, 1,
                        pb_.obstacles().at(iBoxBelow).isVirtual());
      cstrIndexBegin += 1;
    }
  }

  Index finalBoxIndex = 3 * (pb_.nBoxes() - 1);
  FixedBoxPosition::fillLinCstr(pb_.finalPos(), l_.segment(cstrIndexBegin, 3),
                                C_.block(cstrIndexBegin, finalBoxIndex, 3, 3),
                                u_.segment(cstrIndexBegin, 3));

}

void QPPlanesFixed::updatePlanD(RefVec xPlanes)
{
  for (size_t iPlan = 0; iPlan < pb_.nMobilePlanCstr(); ++iPlan)
  {
    const long iBoxBelow(pb_.plans().at(iPlan).boxBelow());
    PlanBetweenBoxAndObstacle::updatePlanD(
        pb_.obstacles().at(iBoxBelow), xPlanes(4 * iPlan),
        xPlanes.segment(4 * iPlan + 1, 3), pb_.securityDistance());
  }
}

} /* feettrajectory */
}