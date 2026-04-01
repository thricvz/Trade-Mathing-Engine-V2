#ifndef  TEST_EXTENSIONS_ORDERBOOK_H 
#define  TEST_EXTENSIONS_ORDERBOOK_H 
#define ORDERBOOK_TESTS

#include <vector>
#include <memory>

#include "../Include/AbstractClasses/AlgorithmFactory.hpp"
#include "../Include/OrderData.hpp"
#include "../Include/Data.hpp"
#include "../Include/PriceLevel.hpp"
#define ORDERBOOK_TESTS
#include "../Include/OrderBook.hpp"

namespace TestExtensions::OrderBook {
  void loadOrders(AlgorithmFactory* factory, ::OrderBook* orderbook, std::vector<OrderData>&& orders) {

    for (auto orderDetail : orders) {
      orderbook->store(factory->createOrder(orderDetail));
    }
  };


  MatchData generateResult(::OrderBook *orderbook, std::unique_ptr<Order> incomingOrder) {
    MatchData matchResult = std::move( orderbook->match(std::move(incomingOrder)) );
    return std::move(matchResult);
  }

  MatchData generateExpectedResult(const OriginalOrderMatchData& orderStateAfterMatch,
      std::vector<OrderMatch>&& expectedMatches) {
    
    MatchData expectedResult{std::move(orderStateAfterMatch)};

    for (auto expectedMatch : expectedMatches) {
      expectedResult.matches.push_back(std::move(expectedMatch));
    }

    return std::move(expectedResult);
  }


}

#endif
