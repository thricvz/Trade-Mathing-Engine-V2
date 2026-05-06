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

    static inline Price DEFAULT_INITIAL_BALANCE = dollars(200);

    json create_user(const std::string& username, const std::string& password) const; 
    json retrieve_user(const std::string& username, const std::string& password) const; 
    void delete_user(const std::string& username, const std::string& password) const; 

    void update_user_balance(uint32_t user_id, const Price& new_balance) const; 
    Price retrieve_user_balance(uint32_t userId) const; 
    
    uint64_t retrieve_user_stock_quantity(uint32_t user_id) const;

    json register_order(const OrderData& ) const; 
    void register_order_as_complete(OrderId) const;

    json retrieve_orders() const; 
    void delete_order(OrderId) const; 


    void reload(void(*modify_database_file)()) ;

  private: 
    using sqlite3_callback = int(void*,int, char**, char**);
    
    void setup_user(const std::string& username, const std::string& password) const;
    void setup_user_balance(uint32_t user_id, const Price& user_initial_balance) const;

    uint32_t next_user_id() const;

    bool user_exists(const std::string& username, const std::string& password) const;
    bool order_exists(OrderId) const ;

    void execute_request(const std::string& , sqlite3_callback , void* callback_argument, char** error_msg) const;

        
    std::string m_db_file_path{};
    sqlite3* m_database{nullptr};
};

#endif 
