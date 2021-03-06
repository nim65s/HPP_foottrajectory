#pragma once

#include <iostream>
#include <Eigen/Core>

#include <hpp/foot/utils/Box.hh>
#include <hpp/foot/utils/FixedPlan.hh>
#include <hpp/foot/functions/BoxAbovePlan.hh>

namespace hpp{
namespace foot
{
class BoxAboveFixedPlan : public BoxAbovePlan
{
 public:
  BoxAboveFixedPlan(const Box& c, const Eigen::Vector3d& normal,
                    const double& d);
  BoxAboveFixedPlan(const Box& c, const FixedPlan& p);
  virtual ~BoxAboveFixedPlan();

  void compute(Eigen::Ref<Eigen::Matrix<double, 8, 1>> res,
               const Eigen::Ref<const Eigen::Vector3d> trans,
               const Eigen::Ref<const Eigen::Vector4d> quat) const;
  void diffTrans(Eigen::Ref<Eigen::Matrix<double, 8, 3>> res,
                 const Eigen::Ref<const Eigen::Vector3d> trans,
                 const Eigen::Ref<const Eigen::Vector4d> quat) const;
  void diffQuat(Eigen::Ref<Eigen::Matrix<double, 8, 4>> res,
                const Eigen::Ref<const Eigen::Vector3d> trans,
                const Eigen::Ref<const Eigen::Vector4d> quat) const;

  void diffTrans(Eigen::Ref<Eigen::Matrix<double, 1, 3>> res,
                 const Eigen::Ref<const Eigen::Vector3d> trans,
                 const Eigen::Ref<const Eigen::Vector4d> quat,
                 const long& index) const;
  void diffQuat(Eigen::Ref<Eigen::Matrix<double, 1, 4>> res,
                const Eigen::Ref<const Eigen::Vector3d> trans,
                const Eigen::Ref<const Eigen::Vector4d> quat,
                const long& index) const;
  const Eigen::Vector3d& normal() const { return normal_; }
  const double& d() const { return d_; }

  void fillLinCstr(double& lb, Eigen::Ref<Eigen::MatrixXd> C) const;

  void setName();

 private:
  Eigen::Vector3d normal_;
  double d_;
};

}  // end of namespace feettrajectory
}
