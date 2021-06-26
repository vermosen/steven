#include <steven/yieldtermstructure/penalizedcubicbsplinesfitting.h>

#include <ql/termstructures/yield/nonlinearfittingmethods.hpp>
#include <utility>

namespace ql = QuantLib;

namespace steven {

  PenalizedCubicBSplinesFitting::PenalizedCubicBSplinesFitting(
      const std::vector<ql::Time>& knots,
      bool constrainAtZero,
      const ql::Array& weights,
      const std::shared_ptr<ql::OptimizationMethod>& optimizationMethod,
      const ql::Array& l2,
      const ql::Real minCutoffTime,
      const ql::Real maxCutoffTime)
  : ql::FittedBondDiscountCurve::FittingMethod(
        constrainAtZero, weights, optimizationMethod, l2, minCutoffTime, maxCutoffTime),
    splines_(3, knots.size() - 5, knots) {

    QL_REQUIRE(knots.size() >= 8,
                "At least 8 knots are required" );
    ql::Size basisFunctions = knots.size() - 4;

    if (constrainAtZero) {
      size_ = basisFunctions-1;

      // Note: A small but nonzero N_th basis function at t=0 may
      // lead to an ill conditioned problem
      N_ = 1;

      QL_REQUIRE(std::abs(splines_(N_, 0.0)) > QL_EPSILON,
                  "N_th cubic B-spline must be nonzero at t=0");
    } else {
      size_ = basisFunctions;
      N_ = 0;
    }
  }

  ql::Real PenalizedCubicBSplinesFitting::basisFunction(ql::Integer i, ql::Time t) const {
    return splines_(i,t);
  }

  std::unique_ptr<ql::FittedBondDiscountCurve::FittingMethod>
  PenalizedCubicBSplinesFitting::clone() const {
    return std::unique_ptr<ql::FittedBondDiscountCurve::FittingMethod>(
      new PenalizedCubicBSplinesFitting(*this));
  }

  ql::Size PenalizedCubicBSplinesFitting::size() const {
    return size_;
  }

  ql::DiscountFactor PenalizedCubicBSplinesFitting::discountFunction(const ql::Array& x, ql::Time t) const {
    ql::DiscountFactor d = 0.0;

    if (!constrainAtZero_) {
      for (ql::Size i = 0; i < size_; ++i) {
        d += x[i] * splines_(i,t);
      }
    } else {
      const ql::Real T = 0.0;
      ql::Real sum = 0.0;
      for (ql::Size i = 0; i < size_; ++i) {
        if (i < N_) {
          d += x[i] * splines_(i,t);
          sum += x[i] * splines_(i,T);
        } else {
          d += x[i] * splines_(i+1,t);
          sum += x[i] * splines_(i+1,T);
        }
      }
      ql::Real coeff = 1.0 - sum;
      coeff /= splines_(N_,T);
      d += coeff * splines_(N_,t);
    }

    return d;
  }
}
