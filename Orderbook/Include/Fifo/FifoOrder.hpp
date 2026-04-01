#ifndef FIFO_ORDER_H
#define FIFO_ORDER_H

#include "../AbstractClasses/Order.hpp"

struct FifoOrder : public Order {
  FifoOrder(OrderData orderDetails) : Order(orderDetails) {};

  bool hasPriorityOver(const Order*) const override {
    // has to be deduced with the timestamp
    return false;
  }

};

#endif
