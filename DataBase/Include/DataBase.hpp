#ifndef DATABASE_H
#define DATABASE_H

#include <cstdint>
#include <sqlite3.h>
#include <string>

#include "OrderData.hpp"
#include "json.hpp"

using json = nlohmann::json;

class DataBase {
  public:
    DataBase(const char* file_path); 
    ~DataBase(); 


    json create_user(const std::string& username, const std::string& password) const; 
    json retrieve_user(const std::string& username, const std::string& password) const; 
    void delete_user(const std::string& username, const std::string& password) const; 

    json register_order(const OrderData& ) const; 
    void register_order_completion(OrderId) const;

    json retrieve_orders() const; 
    void delete_order(const OrderData&) const; 


  private: 
    using sqlite3_callback = int(void*,int, char**, char**);
    bool user_exists(const std::string& username, const std::string& password) const;
    void execute_request(const std::string& , sqlite3_callback , void* callback_argument, char** error_msg) const;


    sqlite3* m_database{nullptr};
};

#endif 
