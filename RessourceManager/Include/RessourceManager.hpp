#ifndef RESSOURCE_MANAGER_H 
#define RESSOURCE_MANAGER_H 

#include <cstdint>
#include <mutex>
#include <string>
#include "json.hpp"

#include "IdGenerator.hpp"

using json = nlohmann::json;
using OrderData = int;

class RessourceManager {
  public:
    static RessourceManager* Instance() {
      static RessourceManager single_instance;
      return &single_instance;
    }
    
    // user related
    json create_user(const std::string& username,
                     const std::string& password);
    
    json delete_user(uint32_t userId); 
    json retrieve_user(const std::string& username,
                       const std::string& password) const; 

    // order related
    json create_order(const OrderData& data);
    json delete_order(uint32_t orderId); 
    json retrieve_user_orders(uint32_t userId) const; 
      
  private:
    RessourceManager() = default; 


    std::mutex m_orderbook_lock{};   
    std::mutex m_database_lock{};   

    IdGenerator m_orderbook_id_generator{};
};



#endif
