#ifndef ORDER_PARAMETERS_H
#define ORDER_PARAMETERS_H 

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

using Quantity = unsigned int;


#endif
