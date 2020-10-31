
#include <gtest/gtest.h>

#include <ql/instruments/dividendbarrieroption.hpp>
#include <ql/instruments/dividendvanillaoption.hpp>
#include <ql/processes/hestonprocess.hpp>
#include <ql/math/randomnumbers/rngtraits.hpp>
#include <ql/math/integrals/gausslobattointegral.hpp>
#include <ql/models/equity/hestonmodel.hpp>
#include <ql/models/equity/hestonmodelhelper.hpp>
#include <ql/models/equity/piecewisetimedependenthestonmodel.hpp>
#include <ql/pricingengines/vanilla/analyticdividendeuropeanengine.hpp>
#include <ql/pricingengines/vanilla/analytichestonengine.hpp>
#include <ql/pricingengines/vanilla/hestonexpansionengine.hpp>
#include <ql/pricingengines/vanilla/coshestonengine.hpp>
#include <ql/pricingengines/vanilla/analyticptdhestonengine.hpp>
#include <ql/pricingengines/vanilla/exponentialfittinghestonengine.hpp>
#include <ql/pricingengines/barrier/fdhestonbarrierengine.hpp>
#include <ql/pricingengines/barrier/fdblackscholesbarrierengine.hpp>
#include <ql/pricingengines/vanilla/fdblackscholesvanillaengine.hpp>
#include <ql/pricingengines/vanilla/fdhestonvanillaengine.hpp>
#include <ql/pricingengines/vanilla/mceuropeanhestonengine.hpp>
#include <ql/pricingengines/blackformula.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/methods/montecarlo/pathgenerator.hpp>
#include <ql/math/optimization/levenbergmarquardt.hpp>
#include <ql/math/optimization/differentialevolution.hpp>
#include <ql/time/period.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/experimental/exoticoptions/analyticpdfhestonengine.hpp>
#include <ql/methods/finitedifferences/operators/numericaldifferentiation.hpp>

namespace ql = QuantLib;

struct CalibrationMarketData {
  ql::Handle<ql::Quote> s0;
  ql::Handle<ql::YieldTermStructure> riskFreeTS, dividendYield;
  std::vector<ql::ext::shared_ptr<ql::CalibrationHelper> > options;
};

ql::ext::shared_ptr<ql::YieldTermStructure>
flatRate(const ql::Date& today,
  const ql::ext::shared_ptr<ql::Quote>& forward,
  const ql::DayCounter& dc) {
  return ql::ext::shared_ptr<ql::YieldTermStructure>(
    new ql::FlatForward(today, ql::Handle<ql::Quote>(forward), dc));
}

ql::ext::shared_ptr<ql::YieldTermStructure>
flatRate(const ql::Date& today, ql::Rate forward, const ql::DayCounter& dc) {
  return flatRate(
    today, ql::ext::shared_ptr<ql::Quote>(new ql::SimpleQuote(forward)), dc);
}

CalibrationMarketData getDAXCalibrationMarketData() {
  
  /* 
   * this example is taken from A. Sepp
   * Pricing European-Style Options under Jump Diffusion Processes
   * with Stochastic Volatility: Applications of Fourier Transform
   * http://math.ut.ee/~spartak/papers/stochjumpvols.pdf
   */
  ql::Date settlementDate(ql::Settings::instance().evaluationDate());

  ql::DayCounter dayCounter = ql::Actual365Fixed();
  ql::Calendar calendar = ql::TARGET();

  ql::Integer t[] = { 13, 41, 75, 165, 256, 345, 524, 703 };
  ql::Rate r[] = { 0.0357,0.0349,0.0341,0.0355,0.0359,0.0368,0.0386,0.0401 };

  std::vector<ql::Date> dates;
  std::vector<ql::Rate> rates;
  dates.push_back(settlementDate);
  rates.push_back(0.0357);

  for (ql::Size i = 0; i < 8; ++i) {
    dates.push_back(settlementDate + t[i]);
    rates.push_back(r[i]);
  }
  // FLOATING_POINT_EXCEPTION
  ql::Handle<ql::YieldTermStructure> riskFreeTS(
    ql::ext::make_shared<ql::ZeroCurve>(dates, rates, dayCounter));

  ql::Handle<ql::YieldTermStructure> dividendYield(
    ql::ext::shared_ptr<ql::YieldTermStructure>(
      new ql::FlatForward(settlementDate, ql::Handle<ql::Quote>(
        ql::ext::shared_ptr<ql::Quote>(new ql::SimpleQuote(0.0))), dayCounter)));

  ql::Volatility v[] =
  { 0.6625,0.4875,0.4204,0.3667,0.3431,0.3267,0.3121,0.3121,
    0.6007,0.4543,0.3967,0.3511,0.3279,0.3154,0.2984,0.2921,
    0.5084,0.4221,0.3718,0.3327,0.3155,0.3027,0.2919,0.2889,
    0.4541,0.3869,0.3492,0.3149,0.2963,0.2926,0.2819,0.2800,
    0.4060,0.3607,0.3330,0.2999,0.2887,0.2811,0.2751,0.2775,
    0.3726,0.3396,0.3108,0.2781,0.2788,0.2722,0.2661,0.2686,
    0.3550,0.3277,0.3012,0.2781,0.2781,0.2661,0.2661,0.2681,
    0.3428,0.3209,0.2958,0.2740,0.2688,0.2627,0.2580,0.2620,
    0.3302,0.3062,0.2799,0.2631,0.2573,0.2533,0.2504,0.2544,
    0.3343,0.2959,0.2705,0.2540,0.2504,0.2464,0.2448,0.2462,
    0.3460,0.2845,0.2624,0.2463,0.2425,0.2385,0.2373,0.2422,
    0.3857,0.2860,0.2578,0.2399,0.2357,0.2327,0.2312,0.2351,
    0.3976,0.2860,0.2607,0.2356,0.2297,0.2268,0.2241,0.2320 };

  ql::Handle<ql::Quote> s0(ql::ext::make_shared<ql::SimpleQuote>(4468.17));
  ql::Real strike[] = { 3400,3600,3800,4000,4200,4400,
                    4500,4600,4800,5000,5200,5400,5600 };

  std::vector<ql::ext::shared_ptr<ql::CalibrationHelper> > options;

  for (ql::Size s = 0; s < 13; ++s) {
    for (ql::Size m = 0; m < 8; ++m) {
      ql::Handle<ql::Quote> vol(ql::ext::make_shared<ql::SimpleQuote>(v[s * 8 + m]));

      ql::Period maturity((int)((t[m] + 3) / 7.), ql::Weeks); // round to weeks
      options.push_back(ql::ext::make_shared<ql::HestonModelHelper>(
        maturity, calendar,
        s0, strike[s], vol,
        riskFreeTS, dividendYield,
        ql::BlackCalibrationHelper::ImpliedVolError));
    }
  }

  CalibrationMarketData marketData = { s0, riskFreeTS, dividendYield, options };

  return marketData;
}

TEST(unittest, create_dax_data) {
  auto res = getDAXCalibrationMarketData();
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
