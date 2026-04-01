#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include "../OrderParameters.hpp"
#include "../OrderData.hpp"

struct Order {
  Order(OrderData data) 
    : ownerId {std::move(data.ownerId)} 
    , id {data.id} 
    , side {data.side}
    , type {data.type}
    , price {data.price} 
    , quantity {data.quantity}
    , fillOrKill{ data.fillOrKill}
  {};
 
  virtual ~Order() = default;

  void setFillOrKill() {
    this->fillOrKill = true;
  } 

  virtual bool hasDifferentOwner(Order* order) const {
    return this->ownerId != order->ownerId; 
  }

  virtual bool hasPriorityOver(const Order*) const = 0;

  std::uint32_t ownerId{ };
  std::uint32_t id{ };
  OrderSide     side{ };
  OrderType     type{ };
  Price         price{ };
  Quantity      quantity{ };
  bool          fillOrKill{false};
};

#endif
