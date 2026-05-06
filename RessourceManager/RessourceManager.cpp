#include "Include/RessourceManager.hpp"
#include "OrderParameters.hpp"

#include <cstdint>
#include <mutex>
#include <cstring>
#include <stdexcept>

RessourceManager* RessourceManager::Instance() {
  static RessourceManager single_instance(retrieve_file_path());
  return &single_instance;
}


bool empty_path(const char* file_path) {
  return strcmp(file_path, "") == 0;
}

const char* RessourceManager::retrieve_file_path() {
  const char* file_path = std::getenv("TM_DATABASE_PATH");
  if (empty_path(file_path)) 
    throw std::runtime_error("Path not found");
  
  return file_path;
}

json RessourceManager::register_user(const std::string& username,
                     const std::string& password) {

  std::scoped_lock(m_database_lock);
  auto json_response  = m_database.create_user(username, password);

  return json_response;
};
    
void RessourceManager::delete_user(const std::string& username,
                     const std::string& password) {

  std::scoped_lock(m_database_lock);
  m_database.delete_user(username, password);
}

json RessourceManager::login_user(const std::string& username,
                       const std::string& password) const {

  std::scoped_lock(m_database_lock);
  auto user_data = m_database.retrieve_user(username, password);

  return user_data;
}; 

json RessourceManager::user_balance(uint32_t user_id) const {
  std::scoped_lock(m_database_lock);
  const Price& user_balance = m_database.retrieve_user_balance(user_id);

  return json{
    {"user_id", user_id},
    {"balance", user_balance.totalValue()}
  };
}; 

json RessourceManager::create_order(const OrderData& order) {
  std::scoped_lock(m_orderbook_lock);
  json match_result_json = {"message","nothing to see here gng"};

  if (m_database_lock.try_lock()) {
    if ( order_prerequisites_met(order) )  {
      match_result_json = match_order(order);
    }

    m_database_lock.unlock();
  }

  return match_result_json;
};

bool RessourceManager::order_prerequisites_met(const OrderData& order) const {
  return order.side == OrderSide::BUY 
    : sufficient_balance_for(order) 
    ? sufficient_stocks_for(order);
} 

bool RessourceManager::sufficient_stocks_for(const OrderData& order) const {
  const auto& user_stock_quantity = retrieve_user_stock_quantity(order.ownerId);

  return user_stock_quantity >= order.quantity;
}

bool RessourceManager::sufficient_balance_for(const OrderData& order) const {
  if (order.type == OrderType::LIMIT) {
    const Price& total_order_price = order.quantity * order.price.totalValue();
    const Price& user_balance = m_database->retrieve_user_balance(order.ownerId);

    return user_balance >= total_order_price;

  } else {
    // remaining logic  
  }
}

json RessourceManager::match_order(const OrderData& data) {
    
};


void RessourceManager::delete_order(uint32_t orderId) {
  std::scoped_lock(m_orderbook_lock);

  if (m_database_lock.try_lock()) {
    m_orderbook.deleteOrder(orderId);
    m_database.delete_order(orderId);   
    m_database_lock.unlock();
  }

}; 

uint32_t number_of_orders(const OrdersDataList& user_orders) {
  return  user_orders.buyOrders.size() + user_orders.sellOrders.size();
};

json RessourceManager::retrieve_user_orders(uint32_t userId) const {
  std::scoped_lock(m_orderbook_lock);

  const OrdersDataList& user_orders = m_orderbook.getUserOrders(userId);
  std::vector<json> user_orders_in_json;

  user_orders_in_json.reserve(number_of_orders(user_orders));

  format_user_orders_to_json(user_orders, user_orders_in_json);

  return json{ "orders", user_orders_in_json };
   
}; 


void RessourceManager::format_user_orders_to_json(const OrdersDataList& user_orders, std::vector<json>& json_storage) const {
    format_orders_to_json(user_orders.buyOrders, json_storage);
    format_orders_to_json(user_orders.sellOrders, json_storage);
}

void RessourceManager::format_orders_to_json(const std::vector<OrderData>& user_orders, std::vector<json>& json_storage) const {
  for (const auto& order : user_orders) {
     json_storage.push_back({
            {"id", order.id},
            {"ownerId", order.ownerId},
            {"side", OrderParameters::to_string(order.side)},
            {"type", OrderParameters::to_string(order.type)},
            {"price", order.price.totalValue()},
            {"quantity", order.quantity},
            {"fillOrKill", order.fillOrKill}
      });
  }

}
