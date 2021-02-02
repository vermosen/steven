
#include <gtest/gtest.h>

#include <ql/handle.hpp>
#include <ql/exercise.hpp>
#include <ql/time/date.hpp>
#include <ql/compounding.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/processes/blackscholesprocess.hpp>

namespace ql = QuantLib;

TEST(unittest, rate_vol_fit) {

  // general pricing data
  auto pricing_date = ql::Date(8, ql::Jan, 2021);
  auto calendar = ql::UnitedStates();
  auto tol = 1e-5;

  // option settings
  auto expiry = ql::Date(29, ql::Jan, 2021);
  
  // risk free rate
  auto rt_settle = calendar.advance(pricing_date, 1, ql::Days);     // settlement (11/01)
  auto dc   = ql::Actual360();
  auto comp = ql::Compounding::Simple;
  auto rf   = ql::Rate(0.001264);                                       // 1M rate from http://www.markit.com/news/InterestRates_USD_20210108.zip

  auto rt_ts = ql::Handle<ql::YieldTermStructure>(
    ql::ext::shared_ptr<ql::YieldTermStructure>(new ql::FlatForward(rt_settle, rf, dc, comp)));

  // implied vol ts
  auto vol_settle = calendar.advance(pricing_date, 1, ql::Days);
  double vol = 1e-5;
  ql::DayCounter vol_dc = ql::Actual365Fixed();

  auto vol_qt = std::shared_ptr<ql::SimpleQuote>(new ql::SimpleQuote(vol)); // can be updated
  ql::Handle<ql::Quote> vol_hdl(vol_qt);
  
  ql::Handle<ql::BlackVolTermStructure> vol_ts(
    ql::ext::shared_ptr<ql::BlackVolTermStructure>(new ql::BlackConstantVol(vol_settle, calendar, vol_hdl, vol_dc)));

  // setup
  ql::Settings::instance().evaluationDate() = pricing_date;

  // test rate/vol precision
  EXPECT_NEAR(rt_ts->zeroRate(expiry, dc, comp).rate(), rf, tol);
  EXPECT_NEAR(vol_ts->blackVol(expiry, 1.000), vol, tol);             // in bps

  vol_qt->setValue(2.0 * vol);                                        // shock vol level
  EXPECT_NEAR(vol_ts->blackVol(expiry, 1.000), 2.0 * vol , tol);
}

TEST(unittest, option_pricing) {

  // general pricing data
  auto pricing_date = ql::Date(8, ql::Jan, 2021);
  auto calendar = ql::UnitedStates();
  auto tol = 1e-5;

  // option settings
  auto expiry = ql::Date(29, ql::Jan, 2021);
  auto type = ql::Option::Call;
  auto strike = 90.0;
  auto ex = ql::ext::shared_ptr<ql::Exercise>(new ql::AmericanExercise(pricing_date, expiry));
  auto payoff = ql::ext::shared_ptr<ql::StrikedTypePayoff>(new ql::PlainVanillaPayoff(type, strike));
  auto opt = ql::VanillaOption(payoff, ex);

  // risk free ts
  auto rt_settle = calendar.advance(pricing_date, 2, ql::Days);     // settlement (11/01)
  auto dc   = ql::Actual360();
  auto comp = ql::Compounding::Simple;
  auto rf   = ql::Rate(0.0015);                                       // 1M rate from http://www.markit.com/news/InterestRates_USD_20210108.zip

  auto rt_ts = ql::Handle<ql::YieldTermStructure>(
    ql::ext::shared_ptr<ql::YieldTermStructure>(new ql::FlatForward(rt_settle, rf, dc, comp)));

  // dividend ts
  auto div_dc = ql::Actual360();
  auto div_comp = ql::Compounding::Continuous;

  auto div_ts = ql::Handle<ql::YieldTermStructure>(
    ql::ext::shared_ptr<ql::YieldTermStructure>(new ql::FlatForward(rt_settle, 0.0, div_dc, div_comp)));

  // implied vol ts
  auto vol_settle = calendar.advance(pricing_date, 1, ql::Days);
  double vol = 5.0;
  ql::DayCounter vol_dc = ql::Actual365Fixed();

  auto vol_qt = ql::ext::shared_ptr<ql::SimpleQuote>(new ql::SimpleQuote(vol)); // can be updated
  ql::Handle<ql::Quote> vol_hdl(vol_qt);
  
  ql::Handle<ql::BlackVolTermStructure> vol_ts(
    ql::ext::shared_ptr<ql::BlackVolTermStructure>(new ql::BlackConstantVol(vol_settle, calendar, vol_hdl, vol_dc)));

  auto ud_quote = std::shared_ptr<ql::Quote>(new ql::SimpleQuote(128.665));
  auto ud_hdl = ql::Handle<ql::Quote>(ud_quote);
  
  // BSM process
  auto bsm = std::shared_ptr<ql::BlackScholesMertonProcess>(new ql::BlackScholesMertonProcess(ud_hdl, div_ts, rt_ts, vol_ts));

  // pricing engine
	opt.setPricingEngine(
		ql::ext::shared_ptr<ql::PricingEngine>(
	  	new ql::BinomialVanillaEngine<ql::CoxRossRubinstein>(bsm, 1000)));

  // setup
  ql::Settings::instance().evaluationDate() = pricing_date;

  ASSERT_NEAR(opt.delta(), 0.8084319065304163, tol);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
