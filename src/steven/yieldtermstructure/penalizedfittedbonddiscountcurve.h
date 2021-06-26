/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Allen Kuo
 Copyright (C) 2009 Ferdinando Ametrano
 Copyright (C) 2015 Andres Hernandez
 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/
 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file fittedbonddiscountcurve.hpp
    \brief discount curve fitted to a set of bonds
*/
#pragma once
#ifndef STEVEN_YIELDTERMSTRUCTURE_PENALIZEDFITTEDBONDDISCOUNTCURVE_H
#define STEVEN_YIELDTERMSTRUCTURE_PENALIZEDFITTEDBONDDISCOUNTCURVE_H

#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/math/optimization/method.hpp>
#include <ql/patterns/lazyobject.hpp>
#include <ql/math/array.hpp>
#include <ql/utilities/clone.hpp>

namespace steven {

    class PenalizedFittedBondDiscountCurve 
              : public QuantLib::YieldTermStructure,
                public QuantLib::LazyObject {
      public:
        friend class PenalizedFittingMethod;

        PenalizedFittedBondDiscountCurve(
            QuantLib::Natural settlementDays,
            const QuantLib::Calendar& calendar,
            std::vector<std::shared_ptr<QuantLib::BondHelper> > bonds,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::FittingMethod& fittingMethod,
            QuantLib::Real accuracy = 1.0e-10,
            QuantLib::Size maxEvaluations = 10000,
            QuantLib::Array guess = QuantLib::Array(),
            QuantLib::Real simplexLambda = 1.0,
            QuantLib::Size maxStationaryStateIterations = 100);
            

        QuantLib::Size numberOfBonds() const;
        QuantLib::Date maxDate() const override;
        const QuantLib::FittingMethod& fitResults() const;
        void update() override;

      private:
        void setup();
        void performCalculations() const override;
        QuantLib::DiscountFactor discountImpl(QuantLib::Time) const override;

      private:
        QuantLib::Real accuracy_;
        QuantLib::Size maxEvaluations_;
        QuantLib::Real simplexLambda_;
        QuantLib::Size maxStationaryStateIterations_;
        QuantLib::Array guessSolution_;
        mutable QuantLib::Date maxDate_;
        std::vector<std::shared_ptr<QuantLib::BondHelper> > bondHelpers_;
        QuantLib::Clone<QuantLib::FittingMethod> fittingMethod_;
    };
}

#endif // STEVEN_YIELDTERMSTRUCTURE_PENALIZEDFITTEDBONDDISCOUNTCURVE_H