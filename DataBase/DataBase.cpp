#include <cstdint>
#include <iostream>
#include <exception>
#include <string>
#include <sys/types.h>
#include <type_traits>

#include "Include/DataBase.hpp"
#include "Price.hpp"

using namespace std::literals;
using std::to_string;

#define REQUEST_PLACEHOLDER '?'
#define REQUEST_PLACEHOLDER_LENGTH 1

int default_callback(void*, int args, char** columns, char **rows) { return 0;}   ;

json json_message(const std::string& message) {
  return {{"message", message}};
}


template<typename... Types>
constexpr bool all_convertible_to_string = (std::is_convertible_v<Types, std::string> && ...) ;

template<typename... Attributes ,
  typename = std::enable_if_t<all_convertible_to_string<Attributes...> >>

std::string create_sql_request(std::string request, Attributes ...attributes)  {
  if (sizeof...(attributes) == 0) 
    return request;
  
  auto replace_placeholder = [](std::string& request, const std::string& attribute) {
    auto placeholder_position = request.find(REQUEST_PLACEHOLDER);
    
    if (placeholder_position != std::string::npos) {
      request.replace(placeholder_position, REQUEST_PLACEHOLDER_LENGTH, attribute);
    } 

  };

  (replace_placeholder(request, attributes) , ...);

  return request;
}


void DataBase::execute_request(const std::string& sql_request,
    sqlite3_callback callback = nullptr,
    void* callback_argument = nullptr, char** error_msg = nullptr) const {
  
  sqlite3_exec(m_database, sql_request.c_str(), callback, callback_argument, nullptr);
  // handle errors
}

DataBase::DataBase(const char* file_name) {
  sqlite3_open(file_name, &m_database);
  if (!m_database) {
    std::cerr << "Failed to open database with file:  " << file_name << "\n";
    std::terminate();
  }
}

DataBase::~DataBase(){
  sqlite3_close(m_database);
}; 

json DataBase::create_user(const std::string& username, const std::string& password) const {
  if (user_exists(username, password)) {
    return json_message("user already exists");
  } 
  
  const std::string& user_creation  = create_sql_request(
      "insert into users(username, password) values('?','?');",
      username, 
      password
  );

  execute_request(user_creation);
  return json_message("user created successfully");
}; 


bool DataBase::user_exists(const std::string& username, const std::string& password) const {
  auto callback = [](void* return_value, int number_users, char** columns, char** rows) {
    bool* user_is_existent = (bool*) return_value;
    *user_is_existent = number_users > 0;
    
    return 0;
  };

  bool user_exists = false;
  const std::string& find_user = create_sql_request(
      "select * from users where username = '?' and password = '?';",
       username, password);

  execute_request(find_user, callback, &user_exists);

  return user_exists;
};


std::string retrieve_from_row(char** row, int8_t data_index) {
  return row[data_index];
}

int create_user_json_data(void* user_return_data, int column_count, char** row, char** columns) {
  const int8_t ID{0};
  const int8_t USERNAME{1};
  json* user_data{ static_cast<json*>(user_return_data) };
  

  (*user_data)["id"] =  retrieve_from_row(row, ID); 
  (*user_data)["username"] =  retrieve_from_row(row, USERNAME); 

  return 0;
};

json DataBase::retrieve_user(const std::string& username, const std::string& password) const {
 // return json with username id 
  if (!user_exists(username, password)) 
    return json_message("User is non existent");

  json user_json_data;

  const std::string& user_retrieval = create_sql_request(
      "select id, username from users where username = '?' and password = '?';",
      username,
      password
  );
  
  execute_request(user_retrieval, create_user_json_data, &user_json_data);

  return user_json_data;
}; 

void DataBase::delete_user(const std::string& username, const std::string& password) const {
  if (user_exists(username, password)) {
    const std::string& user_deletion  = create_sql_request(
        "delete from users where username='?' and password='?';",
        username, 
        password
    );
    
    execute_request(user_deletion);
  }
}; 

json DataBase::register_order(const OrderData& order) const {
  /*
  const std::string& order_registration =  create_sql_request(
    "insert into orders values('?', '?', '?', '?', '?', '?', '?', TRUE)",
    order.id, order.ownerId, order.side,
    order.type, order.price.totalValue(),
    order.quantity, order.fillOrKill
  );
   
  execute_request(order_registration);
  */
}; 


void DataBase::register_order_completion(OrderId orderId) const {
    /*
    const std::string& order_completion_registration = create_sql_request(
        "update orders set active = FALSE where id = '?';",
        to_string(orderId)
    );

    execute_request(order_completion_registration);
    */
};

void DataBase::delete_order(const OrderData& order) const {
    /*
    const std::string& order_deletion  = create_sql_request(
        "delete from orders where id = '?';",
        std::to_string(order.id),
    );
    
    execute_request(order_deletion);
    */
}; 

std::string format_price(const std::string& price_in_cents_fractions) {
  Price price( atol(price_in_cents_fractions.c_str()) );

  return to_string(price.dollars) + "." + to_string(price.cents) + to_string(price.fractionsOfPenny);
}

int create_json_order_list(void* return_data, int column_count, char** row, char** columns) {
  std::vector<json>* orders_list = static_cast<std::vector<json>*>(return_data);

  const int8_t ID = 0,
               OWNER_ID = 1,
               SIDE = 2,
               TYPE = 3,
               PRICE = 4,
               QUANTITY = 5,
               FILL_OR_KILL = 6;

  orders_list->push_back({
     {"id",           retrieve_from_row(row, ID)},
     {"owner_id",     retrieve_from_row(row, OWNER_ID)},
     {"side",         retrieve_from_row(row, SIDE)},
     {"type",         retrieve_from_row(row, TYPE)},
     {"price",        format_price(retrieve_from_row(row, PRICE))},
     {"quantity",     retrieve_from_row(row, QUANTITY)},
     {"fill_or_kill", retrieve_from_row(row, FILL_OR_KILL)},
  });

  return 0;
}

json DataBase::retrieve_orders() const {
    const std::string& order_retrieval  = create_sql_request("select * from orders where active = TRUE;");
    std::vector<json> orders_list;
    execute_request(order_retrieval, create_json_order_list, &orders_list);

    json orders_json{{"orders", orders_list}};

    return orders_json;
}; 
