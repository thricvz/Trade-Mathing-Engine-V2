#ifndef USER_ORDERS_H
#define USER_ORDERS_H
#include "OrderData.hpp"
#include <vector>

struct OrdersDataList {
  std::vector<OrderData> buyOrders{};
  std::vector<OrderData> sellOrders{};
};

#endif
