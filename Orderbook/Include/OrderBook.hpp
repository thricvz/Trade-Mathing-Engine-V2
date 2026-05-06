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
#include "json.hpp"

using json = nlohmann::json;

class OrderBook {

  public:
    using PriceLevelList = std::vector<std::unique_ptr<PriceLevel>>;
    

    OrderBook(AlgorithmFactory* factory) 
      : m_algorithm{std::move(factory->createAlgorithm())}
      , m_factory{factory}
    {
      assert((factory != nullptr) && "factory must be a valid instance, cannot be nullptr" );

    }

    virtual ~OrderBook() = default;    


    MatchData match(std::unique_ptr<Order> order); 
   
    Price bestBid() const;
    Price bestAsk() const;         
 
    OrdersDataList getUserOrders(UserId userId) const ;
    OrdersDataList getAllOrders() const ;
  
    void deleteOrder(OrderId id);


  private:
    virtual MatchData matchMarket(Order* order); 
    virtual MatchData matchLimit(Order* order); 
    void store(std::unique_ptr<Order> order);

    std::unique_ptr<MatchingAlgorithm> m_algorithm{}; 
    AlgorithmFactory* m_factory{};
   
    PriceLevelList m_sellOrders{};
    PriceLevelList m_buyOrders{};

    #ifdef ORDERBOOK_TESTS
      friend void TestExtensions::OrderBook::loadOrders(AlgorithmFactory*, OrderBook*, std::vector<OrderData>&&);
    #endif
};

#endif
