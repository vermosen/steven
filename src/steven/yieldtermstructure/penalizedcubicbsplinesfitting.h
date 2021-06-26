#pragma once
#ifndef STEVEN_YIELDTERMSTRUCTURE_PEMALIZEDCUBICBSPLINESFITTING_H
#define STEVEN_YIELDTERMSTRUCTURE_PEMALIZEDCUBICBSPLINESFITTING_H

#include <memory>

#include <ql/termstructures/yield/fittedbonddiscountcurve.hpp>
#include <ql/math/bspline.hpp>

namespace steven {
  class PenalizedCubicBSplinesFitting
    : public QuantLib::FittedBondDiscountCurve::FittingMethod {
  public:
    PenalizedCubicBSplinesFitting(
        const std::vector<QuantLib::Time>& knotVector,
        bool constrainAtZero = true,
        const QuantLib::Array& weights = QuantLib::Array(),
        const std::shared_ptr<QuantLib::OptimizationMethod>& optimizationMethod =
            std::shared_ptr<QuantLib::OptimizationMethod>(),
        const QuantLib::Array& l2 = QuantLib::Array(),
        QuantLib::Real minCutoffTime = 0.0,
        QuantLib::Real maxCutoffTime = QL_MAX_REAL);

  public:
    QuantLib::Real basisFunction(QuantLib::Integer i, QuantLib::Time t) const;
    std::unique_ptr<QuantLib::FittedBondDiscountCurve::FittingMethod> clone() const override;
  private:
    QuantLib::Size size() const override;
    QuantLib::DiscountFactor discountFunction(const QuantLib::Array& x, QuantLib::Time t) const override;

  private:
    QuantLib::BSpline splines_;
    QuantLib::Size size_;
    QuantLib::Natural N_;
  };
}

#endif // STEVEN_YIELDTERMSTRUCTURE_PEMALIZEDCUBICBSPLINESFITTING_H