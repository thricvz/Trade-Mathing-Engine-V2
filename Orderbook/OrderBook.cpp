#include <algorithm>
#include <cstdint>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>

#include "Include/OrderBook.hpp" 
#include "Include/Data/MatchData.hpp"
#include "Include/AbstractClasses/Order.hpp"
#include "Include/OrderData.hpp"
#include "Include/OrderParameters.hpp"
#include "Include/Price.hpp"
#include "Include/PriceLevel.hpp"

using orderListType = std::vector<std::unique_ptr<PriceLevel>>;
// compare functions for std::sort
namespace {
  bool best_ask_price_order(
      const std::unique_ptr<PriceLevel>& lhs, 
      const std::unique_ptr<PriceLevel>& rhs) {

      return lhs->price < rhs->price;
  };

  bool best_bid_price_order(
      const std::unique_ptr<PriceLevel>& lhs,
      const std::unique_ptr<PriceLevel>& rhs) {
      return rhs->price < lhs->price;
  };

  bool meets_sell_limit_criteria(
      const Order* incomingOrder,
      const PriceLevel* priceLevel) {
      return  incomingOrder->price <= priceLevel->price;

    };

  bool meets_buy_limit_criteria(
      const Order* incomingOrder,
      const PriceLevel* priceLevel) {
      return  priceLevel->price <= incomingOrder->price;

    };

}

MatchData OrderBook::match(std::unique_ptr<Order> order) {

  MatchData result {};
  switch (order->type) {
    case OrderType::MARKET:
      result = std::move( matchMarket(order.get()) );
      break;

    case OrderType::LIMIT:
      result = std::move( matchLimit(order.get()) );
      if (order->quantity > 0 )
        store(std::move(order));
  }

  return result;
}; 



MatchData OrderBook::matchMarket(Order *order) {
  auto& orderList = order->side == OrderSide::BUY ? 
    m_sellOrders : m_buyOrders;

  // handle fillOrKill order
  if (order->fillOrKill) {
    Quantity offeredQuantity{};
    for (const auto& pricelevel : orderList) {
        offeredQuantity += pricelevel->volume_excluding_owner(order->ownerId);
    }

    if (offeredQuantity < order->quantity) return MatchData{};
  }

  MatchData matchResult{};
  auto currentPriceLevel{orderList.begin()};

  while(currentPriceLevel != orderList.end()) {
    if (order->quantity == 0)
      break;

    matchResult.extend( m_algorithm->match(order, currentPriceLevel->get()) );

    if (currentPriceLevel->get()->totalVolume == 0) {
      currentPriceLevel = orderList.erase(currentPriceLevel);

    } else {
      currentPriceLevel++ ;
    }
  } 

  return std::move(matchResult);
}; 

MatchData OrderBook::matchLimit(Order *order) {
  // order and define rules for priority according to sell or buy side
  auto& orderList = order->side == OrderSide::BUY ? 
    m_sellOrders : m_buyOrders;

  const auto& meets_criteria = order->side == OrderSide::BUY ?
    meets_buy_limit_criteria  : meets_sell_limit_criteria  ;

  // handle fillOrKill order
  if (order->fillOrKill) {
    Quantity offeredQuantity{};
    for (const auto& pricelevel : orderList) {
      if ( meets_criteria(order, pricelevel.get()) )
        offeredQuantity += pricelevel->volume_excluding_owner(order->ownerId);
    }

    if (offeredQuantity < order->quantity) return MatchData{};
  }

  MatchData matchResult{};
  auto currentPriceLevel {orderList.begin()};

  while(currentPriceLevel != orderList.end()) {
    if ((order->quantity == 0) || !meets_criteria(order, currentPriceLevel->get()) )
      break;

    matchResult.extend(m_algorithm->match(
      order,
      currentPriceLevel->get()
    ));

    // delete empty orderList
    if (currentPriceLevel->get()->totalVolume == 0) {
      currentPriceLevel = orderList.erase(currentPriceLevel);

    } else {
      currentPriceLevel++ ;
    }
  } 

  return std::move(matchResult);
}; 

void OrderBook::store(std::unique_ptr<Order> order) {
  auto found_price_position = [](OrderSide side, const Price& lhs, const Price& rhs) {
    return side == OrderSide::SELL 
      ? lhs < rhs 
      : rhs < lhs;
  };
  
  auto create_and_store_pricelevel = 
    [this](std::unique_ptr<Order> orderToStore,
      orderListType* orderList,
      const orderListType::iterator& position) {
      
      auto newLevel = std::move( this->m_factory->createPriceLevel(orderToStore->price) );
      newLevel->addOrder(std::move(orderToStore));
      orderList->insert(position, std::move(newLevel));

    };

  std::vector<std::unique_ptr<PriceLevel>> *storeList = order->side == OrderSide::BUY ?
    &m_buyOrders : &m_sellOrders;
  
  auto* const storeListOrder = order->side == OrderSide::BUY ?
    best_bid_price_order : best_ask_price_order;


  std::sort(storeList->begin(), storeList->end(), storeListOrder);

  for (auto i{0}; i < storeList->size(); i++) {
    const auto& priceLevel = storeList->at(i);
    if (priceLevel->price == order->price) {
      priceLevel->addOrder(std::move(order));
      return;
    }   
    
    if ( found_price_position(order->side, order->price, priceLevel->price)) {
      create_and_store_pricelevel(std::move(order), storeList, storeList->begin() + i);
      return;
    }
  }

  // no position before the end was found
  uint32_t offset = std::max<int32_t>(0, storeList->size());
  create_and_store_pricelevel(std::move(order), storeList, storeList->begin() + offset);
}

Price OrderBook::bestBid() const {
  return this->m_buyOrders.size() > 0 
    ? this->m_buyOrders.begin()->get()->price 
    : Price{};
};

Price OrderBook::bestAsk() const{
  return this->m_sellOrders.size() > 0 
    ? this->m_sellOrders.begin()->get()->price 
    : Price{};
};



void register_user_order(const Order* order, std::vector<OrderData>& data_list) {
        data_list.push_back({
             order->ownerId,
             order->id,
             order->side,
             order->type,
             order->price,
             order->quantity,
             order->fillOrKill
       });
}

void register_user_orders_from_pricelevel(uint32_t userId, const PriceLevel* pricelevel, std::vector<OrderData>& registry_list) {
    const uint32_t order_count{pricelevel->getNumberOfOrders()};

    for (auto i{0}; i < order_count; i++ ) {
      const Order* order = pricelevel->getAt(i);
      if (order->ownerId == userId)
        register_user_order(order, registry_list);
    }

}
void register_all_orders_pricelevel(
    const PriceLevel* pricelevel,
    std::vector<OrderData>& registry_list) {

    const uint32_t order_count{pricelevel->getNumberOfOrders()};
    for (auto i{0}; i < order_count; i++ ) {
      const Order* order = pricelevel->getAt(i);
      register_user_order(order, registry_list);
    }
}

// beautiful_code has goood tips : short precise functions
OrdersDataList OrderBook::getUserOrders(uint32_t userId) const {
  OrdersDataList userOrders{};
  userOrders.sellOrders.reserve(10);
  userOrders.buyOrders.reserve(10);

  for (const auto& priceLevel : this->m_sellOrders ) {
    register_user_orders_from_pricelevel(userId, priceLevel.get(), userOrders.sellOrders);
  }

  for (const auto& priceLevel : this->m_buyOrders ) {
    register_user_orders_from_pricelevel(userId, priceLevel.get(), userOrders.buyOrders);
  }

  return userOrders;
};

OrdersDataList OrderBook::getAllOrders() const {
  OrdersDataList allOrders;
  allOrders.sellOrders.reserve(10);
  allOrders.buyOrders.reserve(10);

  for (const auto& priceLevel : this->m_sellOrders ) {
    register_all_orders_pricelevel(priceLevel.get(), allOrders.sellOrders );
  }

  for (const auto& priceLevel : this->m_buyOrders ) {
    register_all_orders_pricelevel(priceLevel.get(), allOrders.buyOrders );
  }

  return allOrders;
};

