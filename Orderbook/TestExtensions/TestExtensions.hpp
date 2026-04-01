#ifndef  TEST_EXTENSIONS_H 
#define  TEST_EXTENSIONS_H 

#include <vector>
#include <memory>

#include "../Include/AbstractClasses/AlgorithmFactory.hpp"
#include "../Include/OrderData.hpp"
#include "../Include/Data.hpp"
#include "../Include/PriceLevel.hpp"


namespace TestExtensions::PriceLevel{
  std::unique_ptr<::PriceLevel> generatePriceLevel(AlgorithmFactory* factory,
      Price price,
      std::vector<OrderData>&& orderDetails) {

    auto newPriceLevel = factory->createPriceLevel(price); 

    for (auto orderDetail : orderDetails) {
      newPriceLevel->addOrder(factory->createOrder( std::move(orderDetail) ));
    }

    return std::move(newPriceLevel);
  }

  // create the expected matches
  MatchData generateExpectedResult(const OriginalOrderMatchData& orderStateAfterMatch,
      std::vector<OrderMatch>&& expectedMatches) {
    
    MatchData expectedResult{std::move(orderStateAfterMatch)};

    for (auto expectedMatch : expectedMatches) {
      expectedResult.matches.push_back(std::move(expectedMatch));
    }

    return std::move(expectedResult);
  }

  MatchData generateResult(AlgorithmFactory* factory, ::PriceLevel* priceLevel, Order* askOrder) {
    auto algorithm = factory->createAlgorithm();
    MatchData matchResult = std::move(algorithm->match(askOrder, priceLevel));

    return std::move(matchResult);
  }
}

#endif
