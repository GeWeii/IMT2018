
#include "binomialengine.hpp"
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/methods/lattices/binomialtree.hpp>

#include <iostream>
#include <ql/quantlib.hpp>
#include <boost/timer.hpp>
#include <iomanip>

using namespace QuantLib;

#if defined(QL_ENABLE_SESSIONS)
namespace QuantLib {

    Integer sessionId() { return 0; }

}
#endif

int main() {

    try {

        // add your code here

        boost::timer timer;
        std::cout << std::endl;
        // set up dates
        Calendar calendar = TARGET();
        Date todaysDate(15, May, 1998);
        Date settlementDate(17, May, 1998);
        Settings::instance().evaluationDate() = todaysDate;
        // our options
        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.06;
        Volatility volatility = 0.20;
        Date maturity(17, May, 1999);
        //bool mustSmoothen = true;
        DayCounter dayCounter = Actual365Fixed();
        std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "        << maturity << std::endl;
        std::cout << "Underlying price = "        << underlying << std::endl;
        std::cout << "Strike = "                  << strike << std::endl;
        std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate)
                  << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield)
                  << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility)
                  << std::endl;
        std::cout << std::endl;
        std::string method;
        std::cout << std::endl ;
        // write column headings

        std::vector<Date> exerciseDates;
        for (Integer i=1; i<=4; i++)
            exerciseDates.push_back(settlementDate + 3*i*Months);

        boost::shared_ptr<Exercise> bermudanExercise(
                                         new BermudanExercise(exerciseDates));
        boost::shared_ptr<Exercise> americanExercise(
                                         new AmericanExercise(settlementDate,
                                                              maturity));
        Handle<Quote> underlyingH(
            boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
        // bootstrap the yield/dividend/vol curves
        Handle<YieldTermStructure> flatTermStructure(
            boost::shared_ptr<YieldTermStructure>(
                new FlatForward(settlementDate, riskFreeRate, dayCounter)));
        Handle<YieldTermStructure> flatDividendTS(
            boost::shared_ptr<YieldTermStructure>(
                new FlatForward(settlementDate, dividendYield, dayCounter)));
        Handle<BlackVolTermStructure> flatVolTS(
            boost::shared_ptr<BlackVolTermStructure>(
                new BlackConstantVol(settlementDate, calendar, volatility,
                                     dayCounter)));
        boost::shared_ptr<StrikedTypePayoff> payoff(
                                        new PlainVanillaPayoff(type, strike));
        boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
                 new BlackScholesMertonProcess(underlyingH, flatDividendTS,
                                               flatTermStructure, flatVolTS));
        // options

        VanillaOption americanOption(payoff, americanExercise);
        VanillaOption americanOption2(payoff, americanExercise);
    
 

        method = "Binomial Cox-Ross-Rubinstein";
        
        std::vector<Size> timeSteps;
        Size timeStep0 = 360;
        for (int i = 0; i < 10; i++ ){
            timeSteps.push_back(timeStep0 + i);
        }

        std::cout << "B&S smooth for american option" << std::endl;
        for (int i = 0; i < timeSteps.size(); i++){
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(
                      new BinomialVanillaEngine_2<CoxRossRubinstein>(bsmProcess,
                                                                   timeSteps[i], true)));

        std::cout << timeSteps[i] << "---" << americanOption.NPV() << std::endl;

        }

        std::cout << "original method for american option" << std::endl;

        for (int i = 0; i < timeSteps.size(); i++){

        americanOption2.setPricingEngine(boost::shared_ptr<PricingEngine>(
                      new BinomialVanillaEngine_2<CoxRossRubinstein>(bsmProcess,
                                                                   timeSteps[i], false)));
        std::cout << timeSteps[i] << "---" << americanOption2.NPV() << std::endl;

        }
        // End test
        double seconds = timer.elapsed();
        Integer hours = int(seconds/3600);
        seconds -= hours * 3600;
        Integer minutes = int(seconds/60);
        seconds -= minutes * 60;
        std::cout << " \nRun completed in ";
        if (hours > 0)
            std::cout << hours << " h ";
        if (hours > 0 || minutes > 0)
            std::cout << minutes << " m ";
        std::cout << std::fixed << std::setprecision(0)
                  << seconds << " s\n" << std::endl;
        return 0;



        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

