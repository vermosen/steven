
#include <ctime>
#include <iomanip>
#include <limits>

#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <pybind11/stl.h>

#include <ql/handle.hpp>
#include <ql/settings.hpp>
#include <ql/exercise.hpp>
#include <ql/compounding.hpp>
#include <ql/time/schedule.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/instruments/bonds/fixedratebond.hpp>
#include <ql/instruments/bonds/zerocouponbond.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>

#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/processes/eulerdiscretization.hpp>

#include "yieldcurve.h"
#include "solver.h"
#include "handle.h"

namespace ql = QuantLib;

namespace py = pybind11;
using namespace pybind11::literals;
using clock_type = std::chrono::system_clock;

PYBIND11_MODULE(_steven, m) {

  m.doc() = "heston model bindings";

  py::enum_<ql::TimeUnit>(m, "timeunit")
    .value("days"         , ql::TimeUnit::Days      )
    .value("weeks"        , ql::TimeUnit::Weeks     )
    .value("months"       , ql::TimeUnit::Months    )
    .value("years"        , ql::TimeUnit::Years     )
    .export_values()
    ;

  py::enum_<ql::BusinessDayConvention>(m, "businessdayconvention")
    // ISDA only
    .value("following"        , ql::BusinessDayConvention::Following        )
    .value("modifiedfollowing", ql::BusinessDayConvention::ModifiedFollowing)
    .value("preceding"        , ql::BusinessDayConvention::Preceding        )
    .value("unadjusted"       , ql::BusinessDayConvention::Unadjusted       )
    .export_values()
    ;

  py::enum_<ql::Compounding>(m, "compounding")
    // ISDA only
    .value("simple"              , ql::Compounding::Simple              )
    .value("compounded"          , ql::Compounding::Compounded          )
    .value("continuous"          , ql::Compounding::Continuous          )
    .value("simplethencompounded", ql::Compounding::SimpleThenCompounded)
    .value("compoundedthensimple", ql::Compounding::CompoundedThenSimple)
    .export_values()
    ;

  py::enum_<ql::Frequency>(m, "frequency")
    .value("annual"    , ql::Frequency::Annual    )
    .value("semiannual", ql::Frequency::Semiannual)
    .value("quarterly" , ql::Frequency::Quarterly )
    .value("monthly"   , ql::Frequency::Monthly   )
    .export_values()
    ;

  py::enum_<ql::Option::Type>(m, "option_type")
    .value("put" , ql::Option::Type::Put )
    .value("call", ql::Option::Type::Call)
    .export_values()
    ;

  py::enum_<ql::DateGeneration::Rule>(m, "dategenerationrule")
    .value("backward"  , ql::DateGeneration::Rule::Backward)
    .value("forward"   , ql::DateGeneration::Rule::Forward )
    .value("zero"      , ql::DateGeneration::Rule::Zero    )
    .export_values()
    ;

  py::class_<ql::Date>(m, "date")
    /* .def(py::init<>()) */
    .def(py::init<>([](int year, int month, int day) {
          return ql::Date(day, static_cast<ql::Month>(month), year);
        }
      )
      , py::arg("year")  = 1970
      , py::arg("month") = 1
      , py::arg("day")   = 1
    )
    .def_property_readonly("year" , &ql::Date::year)
    .def_property_readonly("day"  , &ql::Date::dayOfMonth)
    .def_property_readonly("month", [](const ql::Date& dt) {
        return static_cast<int>(dt.month());
      }
    )
    .def("__eq__", [](const ql::Date& d1, const ql::Date& d2) { 
        return d1 == d2; 
      }
    )
    .def("__str__", [](const ql::Date& dt) {
        std::stringstream ss; ss
          << std::setfill('0') 
          << std::setw(4) << dt.year()  << "-"  
          << std::setw(2) << static_cast<int>(dt.month()) << "-" 
          << std::setw(2) << dt.dayOfMonth()
          ;

        return ss.str();
      }
    )
/*     .def("__repr__", [](const ql::Date& dt) {
        std::stringstream ss; ss
          << std::setfill('0') 
          << std::setw(4) << dt.year()  << "-"  
          << std::setw(2) << static_cast<int>(dt.month()) << "-" 
          << std::setw(2) << dt.dayOfMonth()
          ;

        return ss.str();
      }
    ) */
    ;

  py::class_<ql::DayCounter>(m, "daycounter")
    .def("yearfraction", [](
        ql::DayCounter& dc, const ql::Date& start, const ql::Date& end, const ql::Date& refstart, const ql::Date& refend) {
          return dc.yearFraction(start, end, refstart, refend);
        }
      , py::arg("start")
      , py::arg("end")
      , py::arg("refstart") = ql::Date() // warning ! when registering, create bugs with date __repr__ method
      , py::arg("refend") = ql::Date()
    )
    ;

  py::class_<ql::Actual360, ql::DayCounter>(m, "actual360")
    .def(py::init<>())
    ;

  py::class_<ql::Actual365Fixed, ql::DayCounter>(m, "actual365")
    .def(py::init<>())
    ;

  py::class_<ql::Calendar>(m, "calendar")
    .def("advance", [](
          const ql::Calendar& cal
        , const ql::Date& dt
        , int n
        , ql::TimeUnit unit
        , ql::BusinessDayConvention bdc
        , bool eom) {

        return cal.advance(dt, n, unit, bdc, eom);
      }
      , py::arg("date")
      , py::arg("amount")
      , py::arg("unit")
      , py::arg("bdc")
      , py::arg("eom") = false
    )
    ;

  py::class_<
      ql::UnitedStates
    , ql::Calendar
  >(m, "unitedstates")
    .def(py::init<>())
    ;

  // access evaluation_date settings
  m.def("set_evaluation_date", [](const ql::Date& dt) {
      ql::Settings::instance().evaluationDate() = dt;
    }
  )
  ;

  m.def("get_evaluation_date", []() {
      ql::Date dt = ql::Settings::instance().evaluationDate();
      return dt;
    }
  )
  ;

  py::class_<
      ql::PricingEngine
    , std::shared_ptr<ql::PricingEngine>
  >(m, "engine")
    ;

  py::class_<
      ql::Quote
    , std::shared_ptr<ql::Quote>
  >(m, "quote")
    ;

  py::class_<
      ql::SimpleQuote
    , std::shared_ptr<ql::SimpleQuote>
    , ql::Quote
  >(m, "simplequote")
  .def(py::init<double>()
    , py::arg("value")
    )
  .def_property("value", &ql::SimpleQuote::value, &ql::SimpleQuote::setValue)
    ;

  py::class_<
      ql::Period
    , std::shared_ptr<ql::Period>
  >(m, "period")
  .def(py::init<int, ql::TimeUnit>()
    , py::arg("n")
    , py::arg("unit")
    )
    ;

  py::class_<
      ql::Exercise
    , std::shared_ptr<ql::Exercise>
  >(m, "exercise")
    ;

  py::class_<
      ql::AmericanExercise
    , std::shared_ptr<ql::AmericanExercise>
    , ql::Exercise
  >(m, "americanexercise")
  .def(py::init<const ql::Date&, const ql::Date&>())
    ;

  py::class_<
      ql::EuropeanExercise
    , std::shared_ptr<ql::EuropeanExercise>
    , ql::Exercise
  >(m, "europeanexercise")
  .def(py::init<const ql::Date& /* expiry */>())
    ;

  py::class_<
      ql::StrikedTypePayoff
    , std::shared_ptr<ql::StrikedTypePayoff>
  >(m, "striketypepayoff")
    ;

  py::class_<
      ql::PlainVanillaPayoff
    , std::shared_ptr<ql::PlainVanillaPayoff>
    , ql::StrikedTypePayoff
  >(m, "plainvanillapayoff")
  .def(py::init<ql::Option::Type, double>()
    , py::arg("type")
    , py::arg("strike")
    )
    ;

  py::class_<
      ql::Instrument
    , std::shared_ptr<ql::Instrument>
  >(m, "instrument")
    .def("setpricingengine"
      , &ql::Instrument::setPricingEngine
      , py::arg("engine")
    )
    .def("npv", &ql::Instrument::NPV)
    .def("valuedate", &ql::Instrument::valuationDate)
    .def("error", &ql::Instrument::errorEstimate)
    ;

  py::class_<ql::FixedRateBond, std::shared_ptr<ql::FixedRateBond>, ql::Instrument>(m, "fixedratebond")
    .def(py::init<
          ql::Natural
        , ql::Real
        , const ql::Schedule&
        , const std::vector<ql::Rate>&
        , const ql::DayCounter&
        , ql::BusinessDayConvention
        , ql::Real
        , const ql::Date&
        , const ql::Calendar&
        , const ql::Period&
        , const ql::Calendar&
        , ql::BusinessDayConvention
        , bool 
        , const ql::DayCounter&
      >()
      , py::arg("settlementdays")
      , py::arg("faceamount")
      , py::arg("schedule")
      , py::arg("coupons")
      , py::arg("accrualdaycounter")
      , py::arg("paymentconvention") = ql::BusinessDayConvention::Following
      , py::arg("redemption") = 100.0
      , py::arg("issuedate") = ql::Date()
      , py::arg("paymentcalendar") = ql::Calendar()
      , py::arg("excouponperiod") = ql::Period()
      , py::arg("excouponcalendar") = ql::Calendar()
      , py::arg("excouponconvention") = ql::BusinessDayConvention::Unadjusted
      , py::arg("excouponeom") = false
      , py::arg("firstperioddaycounter") = ql::DayCounter()
    )
    ;

  py::class_<ql::ZeroCouponBond, std::shared_ptr<ql::ZeroCouponBond>, ql::Instrument>(m, "zerocouponbond")
    .def(py::init<
          ql::Natural
        , const ql::Calendar&
        , ql::Real
        , const ql::Date&
        , ql::BusinessDayConvention
        , ql::Real
        , const ql::Date&
      >()
      , py::arg("settlementdays")
      , py::arg("calendar")
      , py::arg("faceamount")
      , py::arg("maturity")
      , py::arg("paymentconvention") = ql::BusinessDayConvention::Following
      , py::arg("redemption") = 100.0
      , py::arg("issuedate") = ql::Date()
    )
    ;

  py::class_<ql::VanillaOption, std::shared_ptr<ql::VanillaOption>, ql::Instrument>(m, "vanillaoption")
    .def(py::init<
          std::shared_ptr<ql::StrikedTypePayoff>
        , std::shared_ptr<ql::Exercise>
      >()
      , py::arg("payoff")
      , py::arg("exercice")
    )
    .def_property_readonly("delta", [](const std::shared_ptr<ql::VanillaOption>& opt) {
        return opt->delta();
      }
    )
    .def_property_readonly("strike", [](const std::shared_ptr<ql::VanillaOption>& opt) {
        
        auto p = std::dynamic_pointer_cast<ql::StrikedTypePayoff>(opt->payoff());
        
        if (p) {
          return p->strike();
        } else {
          throw std::logic_error("wrong payoff type passed");
        }
        
      }
    )
    ;

  py::class_<
      ql::BlackVolTermStructure
    , std::shared_ptr<ql::BlackVolTermStructure>
  >(m, "blackvoltermstructure")
    .def("blackvol", [](const std::shared_ptr<ql::BlackVolTermStructure>& ts, const ql::Date& dt, double strike, bool extrapolate) {
        return ts->blackVol(dt, strike, extrapolate);
      }
      , py::arg("date")
      , py::arg("strike")
      , py::arg("extrapolate") = false
    )
    ;

  py::class_<
      ql::BlackConstantVol
    , std::shared_ptr<ql::BlackConstantVol>
    , ql::BlackVolTermStructure
  >(m, "blackconstantvol")
    .def(py::init<const ql::Date&, const ql::Calendar&, const ql::Handle<ql::Quote>&, const ql::DayCounter&>())
    ;

  py::class_<
      ql::StochasticProcess1D::discretization
    , std::shared_ptr<ql::StochasticProcess1D::discretization>
  >(m, "discretization1d")
    ;

  py::class_<
      ql::EulerDiscretization
    , std::shared_ptr<ql::EulerDiscretization>
    , ql::StochasticProcess1D::discretization
  >(m, "eulerdiscretization")
    .def(py::init<>())
    ;

  py::class_<
      ql::GeneralizedBlackScholesProcess
    , std::shared_ptr<ql::GeneralizedBlackScholesProcess>
  >(m, "generalizedblackscholesprocess")
    ;

  py::class_<
      ql::BlackScholesMertonProcess
    , std::shared_ptr<ql::BlackScholesMertonProcess>
    , ql::GeneralizedBlackScholesProcess
  >(m, "blackscholesmertonprocess")
    .def(py::init<
          const ql::Handle<ql::Quote>&
        , const ql::Handle<ql::YieldTermStructure>&
        , const ql::Handle<ql::YieldTermStructure>&
        , const ql::Handle<ql::BlackVolTermStructure>&
        , const std::shared_ptr<ql::StochasticProcess1D::discretization>&
        , bool
      >()
      , py::arg("udprice")
      , py::arg("dividendcurve")
      , py::arg("ratecurve")
      , py::arg("volatilitysurface")
      , py::arg("scheme") = std::shared_ptr<ql::StochasticProcess1D::discretization>(new ql::EulerDiscretization)
      , py::arg("force_discretization") = false
    )
    ;

  py::class_<
      ql::Schedule
  >(m, "schedule")
    .def(py::init<
          const ql::Date&
        , const ql::Date&
        , const ql::Period&
        , const ql::Calendar&
        , ql::BusinessDayConvention
        , ql::BusinessDayConvention
        , ql::DateGeneration::Rule
        , bool
        , const ql::Date&
        , const ql::Date&
      >()
      , py::arg("effectiveDate")
      , py::arg("terminationDate")
      , py::arg("tenor")
      , py::arg("calendar")
      , py::arg("convention")
      , py::arg("terminationDateConvention")
      , py::arg("rule")
      , py::arg("eom")
      , py::arg("firstDate") = ql::Date()
      , py::arg("nextToLastDate") = ql::Date()
    )
    .def("__str__", [](const ql::Schedule& s) {

          std::stringstream ss;

          for (auto& it : s) {
            ss 
              << it << '\n'
              ;
          }
          
          return ss.str();
      }
    )
    ;

  // engines submodule
  {
    auto sub = m.def_submodule("_engines");

    py::class_<
        ql::BinomialVanillaEngine<ql::CoxRossRubinstein>
      , std::shared_ptr<ql::BinomialVanillaEngine<ql::CoxRossRubinstein>>
      , ql::PricingEngine
    >(sub, "coxrossrubinstein")
      .def(py::init<std::shared_ptr<ql::GeneralizedBlackScholesProcess>, ql::Size>()
      , py::arg("process")
      , py::arg("step"))
      ;
  }

  init_submodule_solver(m);
  init_submodule_handle(m);
  init_submodule_yieldcurve(m);
}