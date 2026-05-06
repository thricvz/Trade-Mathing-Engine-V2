#ifndef RESSOURCE_MANAGER_H 
#define RESSOURCE_MANAGER_H 

#include <cstdint>
#include <cstdlib>

#include <iterator>
#include <mutex>
#include <string>
#include "json.hpp"

#include "OrderBook.hpp"
#include "FifoFactory.hpp"
#include "UserOrders.hpp"
#include "DataBase.hpp"

using json = nlohmann::json;


class RessourceManager {
  public:
    static RessourceManager* Instance();
    
    json register_user(const std::string& username,
                     const std::string& password);
   


    void delete_user(const std::string& username, const std::string& password); 

    json login_user(const std::string& username,
                       const std::string& password) const; 

    json user_balance(uint32_t) const; 

    json create_order(const OrderData& data);
    void delete_order(uint32_t orderId); 

   json retrieve_user_orders(uint32_t userId) const; 

    void reload_ressources(void(*db_src_modifications)()) {
      m_database.reload(db_src_modifications);
    };

  private:
    
    void format_user_orders_to_json(const OrdersDataList& , std::vector<json>& ) const;
    void format_orders_to_json(const std::vector<OrderData>& , std::vector<json>& ) const;
  

    bool order_prerequisites_met(const OrderData& order) const ;
    bool sufficient_stocks_for(const OrderData& order) const ;
    bool sufficient_balance_for(const OrderData& order) const ;

    json match_order(const OrderData& data);
       
    static const char* retrieve_file_path();

    RessourceManager(const char* database_path ) :
      m_database(database_path) {}

    FifoFactory m_orderbook_factory{};
    OrderBook m_orderbook{&m_orderbook_factory};
    DataBase  m_database;

    std::mutex m_orderbook_lock{};   
    std::mutex m_database_lock{};   

};



#endif
