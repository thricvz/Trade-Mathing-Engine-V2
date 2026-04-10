#include <iostream>
#include "Include/DataBase.hpp"
#include "OrderData.hpp"

int main() {
  DataBase db("test.db");
  const auto& message =  db.create_user("eric", "secret");

  unsigned int orderId {}; 
  std::cout << "enter order id : ";
  std::cin >> orderId;

  OrderData orderDetails { // making use of designated initialization
    .ownerId = 2,
    .id = orderId, // designator 
    .side = OrderSide::BUY,
    .type = OrderType::LIMIT,
    .price = dollars(200),
    .quantity = 200,
    .fillOrKill = false
  };

  
  return 0;
}
