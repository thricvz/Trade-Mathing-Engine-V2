#ifndef PRO_RATA_ORDER_H
#define PRO_RATA_ORDER_H

#include "../AbstractClasses/Order.hpp"

struct ProRataOrder : public Order {
  ProRataOrder(OrderData orderDetails) : Order(orderDetails) {};

  bool hasPriorityOver(const Order* order) const override {
    return  quantity > order->quantity;

  };


  double calculateProRataRatio(unsigned int priceLevelVolume) {
      if ( quantity == 0 ) {
         return 0. ;
      }
      return  static_cast<double> (quantity) / priceLevelVolume;
  }

};


#endif
