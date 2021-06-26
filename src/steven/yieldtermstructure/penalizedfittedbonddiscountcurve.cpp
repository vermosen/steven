
#include <steven/yieldtermstructure/penalizedfittedbonddiscountcurve.h>

namespace ql = QuantLib;

namespace steven {

  ql::Size PenalizedFittedBondDiscountCurve::numberOfBonds() const {
    return bondHelpers_.size();
  }

  ql::Date PenalizedFittedBondDiscountCurve::maxDate() const {
    calculate();
    return maxDate_;
  }

  const PenalizedFittedBondDiscountCurve::FittingMethod&
  FittedBondDiscountCurve::fitResults() const {
    calculate();
    return *fittingMethod_;
  }

  void PenalizedFittedBondDiscountCurve::update() {
    YieldTermStructure::update();
    LazyObject::update();
  }

  void PenalizedFittedBondDiscountCurve::setup() {
    for (auto& bondHelper : bondHelpers_)
      registerWith(bondHelper);
  }

  ql::DiscountFactor PenalizedFittedBondDiscountCurve::discountImpl(ql::Time t) const {
    calculate();
    return fittingMethod_->discount(fittingMethod_->solution_, t);
  }

  PenalizedFittedBondDiscountCurve::PenalizedFittedBondDiscountCurve(
      ql::Natural settlementDays,
      const ql::Calendar& calendar,
      vector<std::shared_ptr<ql::BondHelper> > bondHelpers,
      const ql::DayCounter& dayCounter,
      const ql::FittingMethod& fittingMethod,
      ql::Real accuracy,
      ql::Size maxEvaluations,
      ql::Array guess,
      ql::Real simplexLambda,
      ql::Size maxStationaryStateIterations)
  : YieldTermStructure(settlementDays, calendar, dayCounter), accuracy_(accuracy),
    maxEvaluations_(maxEvaluations), simplexLambda_(simplexLambda),
    maxStationaryStateIterations_(maxStationaryStateIterations), guessSolution_(std::move(guess)),
    bondHelpers_(std::move(bondHelpers)), fittingMethod_(fittingMethod) {
      fittingMethod_->curve_ = this;
      setup();
  }

  void PenalizedFittedBondDiscountCurve::performCalculations() const {

    QL_REQUIRE(!bondHelpers_.empty(), "no bondHelpers given");

    maxDate_ = Date::minDate();
    ql::Date refDate = referenceDate();

    // double check bond quotes still valid and/or instruments not expired
    for (Size i=0; i<bondHelpers_.size(); ++i) {
      std::shared_ptr<ql::Bond> bond = bondHelpers_[i]->bond();
      QL_REQUIRE(bondHelpers_[i]->quote()->isValid(),
                  io::ordinal(i+1) << " bond (maturity: " <<
                  bond->maturityDate() << ") has an invalid price quote");
      Date bondSettlement = bond->settlementDate();
      QL_REQUIRE(bondSettlement>=refDate,
                  io::ordinal(i+1) << " bond settlemente date (" <<
                  bondSettlement << ") before curve reference date (" <<
                  refDate << ")");
      QL_REQUIRE(BondFunctions::isTradable(*bond, bondSettlement),
                  io::ordinal(i+1) << " bond non tradable at " <<
                  bondSettlement << " settlement date (maturity"
                  " being " << bond->maturityDate() << ")");
      maxDate_ = std::max(maxDate_, bondHelpers_[i]->pillarDate());
      bondHelpers_[i]->setTermStructure(
                            const_cast<FittedBondDiscountCurve*>(this));
    }
    fittingMethod_->init();
    fittingMethod_->calculate();
  }
}