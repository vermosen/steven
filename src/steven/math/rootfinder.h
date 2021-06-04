#include <memory>

namespace QuantLib {
  class Instrument;
  class SimpleQuote;
}

namespace steven {

  class rootfinder {
  public:  
    rootfinder(
        const std::shared_ptr<QuantLib::Instrument>&
      , const std::shared_ptr<QuantLib::SimpleQuote>&);

    double solve(
        double guess
      , double target, double acc
      , double xmax, double xmin);

  private:
    std::shared_ptr<QuantLib::Instrument>  m_in;
    std::shared_ptr<QuantLib::SimpleQuote> m_qt;
  };

}