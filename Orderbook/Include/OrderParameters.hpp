#ifndef ORDER_PARAMETERS_H
#define ORDER_PARAMETERS_H 
#include <string>

using Quantity = unsigned int;
using OrderId = std::uint32_t;
using UserId = std::uint32_t;

enum class OrderSide {
  BUY  ,
  SELL ,
};

enum class OrderType {
  MARKET     ,
  LIMIT      ,
  STOP_LIMIT ,
  STOP_LOSS  ,
};

enum class OrderFillState {
  FULL    ,
  PARTIAL ,
  NO_FILL ,
};

namespace OrderParameters {
  inline std::string to_string(OrderSide orderSide) {
      return (orderSide == OrderSide::BUY) ? "BUY" : "SELL";
  }

  inline std::string to_string(OrderType orderType) {
    switch(orderType) {
      case OrderType::MARKET     : return "MARKET"; 
      case OrderType::LIMIT      : return "LIMIT"; 
      case OrderType::STOP_LIMIT : return "STOP_LIMIT"; 
      case OrderType::STOP_LOSS  : return "STOP_LOSS"; 
      default : return "";
          
    }
  }
} 


#endif
