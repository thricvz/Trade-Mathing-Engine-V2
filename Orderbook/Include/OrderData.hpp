#ifndef ORDERDATA_H
#define ORDERDATA_H

#include <cstdint>

#include "Price.hpp"
#include "OrderParameters.hpp"

struct OrderData {
  UserId ownerId{ };
  OrderId id{ };
  OrderSide     side{ };
  OrderType     type{ };
  Price         price{ };
  Quantity      quantity{ };
  bool          fillOrKill{false};
};

#endif
