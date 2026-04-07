#ifndef ORDER_PARAMETERS_H
#define ORDER_PARAMETERS_H 

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



#endif
