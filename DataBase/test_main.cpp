#include <iostream>
#include "Include/DataBase.hpp"


int main() {
  DataBase db("test.db");
  const auto& message =  db.create_user("eric", "secret");
  std::cout << db.retrieve_orders();
  
  return 0;
}
