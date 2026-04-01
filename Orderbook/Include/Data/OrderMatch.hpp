#ifndef ORDER_MATCH_H
#define ORDER_MATCH_H

#include <cstdint>
#include "../OrderParameters.hpp"
#include "../Price.hpp"

struct OrderMatch {
  std::uint32_t ownerId{};
  std::uint32_t orderId{};
  Price price {};
  Quantity allocatedQuantity{};
  OrderFillState fillState{};

  bool operator==(const OrderMatch& rhs) const {
    return ownerId == rhs.ownerId 
        && orderId == rhs.orderId 
        && allocatedQuantity == rhs.allocatedQuantity 
        && price == rhs.price 
        && fillState == rhs.fillState; 
  }


};

#endif
