#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <cstdint>
#ifdef ORDERBOOK_TESTS
class OrderBook;
namespace TestExtensions::OrderBook {
  void loadOrders(AlgorithmFactory*, ::OrderBook*, std::vector<OrderData>&&);
};
#endif

#include <memory>
#include <vector>
#include <cassert>

#include "AbstractClasses/AlgorithmFactory.hpp"
#include "AbstractClasses/MatchingAlgorithm.hpp"
#include "UserOrders.hpp"


#include "Data.hpp"
#include "Data/MatchData.hpp"
#include "OrderData.hpp"
#include "PriceLevel.hpp"

class OrderBook {

  public:
    OrderBook(AlgorithmFactory* factory) 
      : m_algorithm{std::move(factory->createAlgorithm())}
      , m_factory{factory}
    {
      assert((factory != nullptr) && "factory must be a valid instance, cannot be nullptr" );

    }
    
    // returns the matched orders
    MatchData match(std::unique_ptr<Order> order); 
   
    Price bestBid(); // should be const
    Price bestAsk(); // should be const         
 
    OrdersDataList getUserOrders(uint32_t userId) const ;
    OrdersDataList getAllOrders() const ;
    
  private:
    virtual MatchData matchMarket(Order* order); 
    virtual MatchData matchLimit(Order* order); 

    void store(std::unique_ptr<Order> order);

    // data members
    std::unique_ptr<MatchingAlgorithm> m_algorithm{}; 
    AlgorithmFactory* m_factory{};
   
    std::vector<std::unique_ptr<PriceLevel>> m_sellOrders{};
    std::vector<std::unique_ptr<PriceLevel>> m_buyOrders{};

    #ifdef ORDERBOOK_TESTS
      friend void TestExtensions::OrderBook::loadOrders(AlgorithmFactory*, OrderBook*, std::vector<OrderData>&&);
    #endif
};

#endif
