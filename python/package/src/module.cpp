
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
#include <ql/quotes/simplequote.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/processes/eulerdiscretization.hpp>

#include "rootfinder.h"

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
    .def("__repr__", [](const ql::Date& dt) {
        std::stringstream ss; ss
          << std::setfill('0') 
          << std::setw(4) << dt.year()  << "-"  
          << std::setw(2) << static_cast<int>(dt.month()) << "-" 
          << std::setw(2) << dt.dayOfMonth()
          ;

        return ss.str();
      }
    )
    ;

  py::class_<ql::DayCounter>(m, "daycounter")
    .def("yearfraction", [](
        ql::DayCounter& dc, const ql::Date& start, const ql::Date& end) {
          return dc.yearFraction(start, end, ql::Date(), ql::Date());
        }
      , py::arg("start")
      , py::arg("end")
    /*, py::arg("refstart") = ql::Date() // warning ! when registering, create bugs with date __repr__ method
      , py::arg("refend") = ql::Date() */
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
      ql::YieldTermStructure
    , std::shared_ptr<ql::YieldTermStructure>
  >(m, "yieldtermstructure")
    .def("zerorate", [](
          std::shared_ptr<ql::YieldTermStructure> ts
        , const ql::Date& dt
        , const ql::DayCounter& dc
        , ql::Compounding c
        , ql::Frequency freq
        , bool extrapolate) -> double {
        return ts->zeroRate(dt, dc, c, freq, extrapolate).rate();
      }
      , py::arg("date")
      , py::arg("daycounter")
      , py::arg("compounding")
      , py::arg("frequency")
      , py::arg("extrapolate") = false
    )
    ;

  py::class_<
      ql::FlatForward
    , std::shared_ptr<ql::FlatForward>
    , ql::YieldTermStructure
  >(m, "flatforward")
    .def(py::init<const ql::Date&, ql::Rate, const ql::DayCounter&, ql::Compounding>())
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

  // handles submodule
  {
    auto sub = m.def_submodule("_handles");

    py::class_<ql::Handle<ql::Quote>>(sub, "_quote")
      .def(py::init<std::shared_ptr<ql::Quote>>())
      .def("link", &ql::Handle<ql::Quote>::currentLink)
      .def("__str__", [](const ql::Handle<ql::Quote>& q) {
          std::stringstream ss; ss 
            << q->value()
            ;
          
          return ss.str();
        }
      )
      .def("__repr__", [](const ql::Handle<ql::Quote>& q) {
          std::stringstream ss; ss
            << q->value()
            ;

          return ss.str();
        }
      )
      ;

    py::class_<ql::RelinkableHandle<ql::Quote>, ql::Handle<ql::Quote>>(sub, "quote")
      .def(py::init<std::shared_ptr<ql::Quote>>())
      .def("linkto", [](ql::RelinkableHandle<ql::Quote>& hdl, const std::shared_ptr<ql::Quote>& qt, bool reg) {
          hdl.linkTo(qt, reg);
        }
        , py::arg("quote")
        , py::arg("register") = true
      )
      ;

    py::class_<ql::Handle<ql::YieldTermStructure>>(sub, "yieldtermstructure")
      .def(py::init<std::shared_ptr<ql::YieldTermStructure>>())
      .def("link", &ql::Handle<ql::YieldTermStructure>::currentLink)
      ;

    py::class_<ql::Handle<ql::BlackVolTermStructure>>(sub, "blackvoltermstructure")
      .def(py::init<std::shared_ptr<ql::BlackVolTermStructure>>())
      .def("link", &ql::Handle<ql::BlackVolTermStructure>::currentLink)
      ;
  }

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

  // solver module
  {
    auto sub = m.def_submodule("_solvers");

    py::class_<rootfinder>(sub, "rootfinder")
      .def(py::init<const std::shared_ptr<ql::Instrument>&, const std::shared_ptr<ql::SimpleQuote>&>()
        , py::arg("instrument")
        , py::arg("quote")
      )
      .def("solve", [](rootfinder& rf
            , double guess, double npv
            , double acc, double xmax
            , double xmin, bool verbose) {

          try {
            return rf.solve(guess, npv, acc, xmax, xmin);
          } catch(const std::exception& ex) {
              
            if (verbose) {
              py::print(ex.what());
            }
            throw ex;
          }
        }
        , py::arg("guess")
        , py::arg("npv")
        , py::arg("accuracy") = 1e-12
        , py::arg("xmax")     = 10
        , py::arg("xmin")     = 1e-2
        , py::arg("verbose")  = false
      )
      ;
  }
}