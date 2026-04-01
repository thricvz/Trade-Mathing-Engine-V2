#ifndef ORDERDATA_H
#define ORDERDATA_H

#include <cstdint>

#include "Price.hpp"
#include "OrderParameters.hpp"

struct OrderData {
  std::uint32_t ownerId{ };
  std::uint32_t id{ };
  OrderSide     side{ };
  OrderType     type{ };
  Price         price{ };
  Quantity      quantity{ };
  bool          fillOrKill{false};
};

#endif
