#include "Include/RessourceManager.hpp"
#include "OrderParameters.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <cstring>
#include <iostream>
#include <string>

#define RESSOURCE_MANAGER_TESTING

using namespace std::literals;


void remove_file(const std::string& file_path);
void create_file(const std::string& file_path);
void init_db(const std::string& database_path);

void reset_database() {
  const char* db_path = std::getenv("TM_DATABASE_PATH");
  assert(db_path != NULL && "TM_DATABASE_PATH must be set");

  remove_file(db_path);
  create_file(db_path);
  init_db(db_path);
};

void remove_file(const std::string& file_path) {
  const std::string& remove_file = "rm " + file_path;  
  system(remove_file.c_str());
};

void create_file(const std::string& file_path) {
  const std::string& create_file  = "touch " + file_path;  
  system(create_file.c_str());
};

void init_db(const std::string& database_path) {
  const char* init_file = std::getenv("TM_DATABASE_INIT_FILE");
  assert(init_file != NULL && "TM_DATABASE_INIT_FILE must be set");

  const std::string initiate_database = "sqlite3 " + database_path + " '.read " +  init_file + "'";  
  system(initiate_database.c_str());
};

TEST(UserAPI, create_already_existing_user) {
  RessourceManager* ressources = RessourceManager::Instance();
  ressources->reload_ressources(reset_database);

  ressources->register_user("duo", "exists");
  const auto& response = ressources->register_user("duo", "exists");
  
  EXPECT_EQ(response["message"], "user already exists"s);
}

TEST(UserAPI, register_user) {
  RessourceManager* ressources = RessourceManager::Instance();
  ressources->reload_ressources(reset_database);

  ressources->register_user("jason", "secret password");
  const auto& user = ressources->login_user("jason", "secret password");

  // check that entry for account has been created

  EXPECT_EQ(user.at("username"), "jason");
  EXPECT_EQ(user.at("id"), std::to_string(1));
}


TEST(UserAPI, user_deletion) {
  RessourceManager* ressources = RessourceManager::Instance();
  ressources->reload_ressources(reset_database);

  ressources->register_user("hackertommy99", " lol");
  ressources->delete_user("hackertommy99", " lol");
  
  const auto& response = ressources->login_user("hackertommy99", " lol");

  EXPECT_EQ(response["message"], "User is non existent"s);
}

TEST(UserAPI, inexistent_user_deletion) {

  RessourceManager* ressources = RessourceManager::Instance();
  ressources->reload_ressources(reset_database);

  ressources->delete_user("average_human", "yadayadayada");
  const auto& response = ressources->login_user("average_human", "yadayadayada");

  EXPECT_EQ(response["message"], "User is non existent"s);
}

TEST(OrderAPI, create_orders_and_retrieve) {
  RessourceManager* ressources = RessourceManager::Instance();
  ressources->reload_ressources(reset_database);
 
  const uint32_t user_id {19};
  const uint32_t order1_id {143};

  ressources->create_order({
    .ownerId = user_id,
    .id = 143,
    .side = OrderSide::BUY,
    .type = OrderType::LIMIT ,
    .price = dollars(23),
    .quantity = 23,
    .fillOrKill = false 
  }) ;
  
  std::cout << "should display the tests\n";
  auto user_orders = ressources->retrieve_user_orders(user_id);
  std::cout << user_orders;
}
