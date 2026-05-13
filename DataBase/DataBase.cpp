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

std::string retrieve_from_row(char** row, int8_t data_index) {
  return row[data_index];
}

int value_exists_callback(void* return_value,
    int number_results, char** columns, char** rows) {

    bool* value_exists = (bool*) return_value;
    *value_exists = number_results > 0;
    
    return 0;
};

int default_callback(void*, int args, char** columns, char **rows) { 
  return 0;
}   


int retrieve_user_stock_quantity_callback(void* return_object, int args, char **row , char **columns) {
    uint64_t *return_value = static_cast<uint64_t*>(return_object);

    const std::string& str_result = retrieve_from_row(row, 0);
    *return_value = std::stoull(str_result);
    return 0;
};



void DataBase::execute_request(const std::string& sql_request,
    sqlite3_callback callback = nullptr,
    void* callback_argument = nullptr, char** error_msg = nullptr) const {
  
  sqlite3_exec(m_database, sql_request.c_str(), callback, callback_argument, error_msg);
}

DataBase::DataBase(const char* file_name) : m_db_file_path(file_name) {
  sqlite3_open(m_db_file_path.c_str(), &m_database);

  if (!m_database) {
    std::cerr << "Failed to open database with file:  " << file_name << "\n";
    std::terminate();
  }
}

DataBase::~DataBase(){
  sqlite3_close(m_database);
}; 

void DataBase::reload(void(*modify_database_file)()){ 
  sqlite3_close(m_database);
  modify_database_file();
  sqlite3_open(m_db_file_path.c_str(), &m_database);
}; 

json DataBase::create_user(const std::string& username, const std::string& password) const {
  if (user_exists(username, password)) {
    return json_message("user already exists");
  } 
  
  const int32_t user_id = next_user_id(); 
  setup_user(username, password);
  setup_user_balance(user_id, DEFAULT_INITIAL_BALANCE);

  return json_message("user created successfully");
}; 

int calculate_new_id(void* return_out, int args, char** row_value, char **) { 
  uint32_t* next_id = static_cast<uint32_t*>(return_out);
  if (*row_value)
    *next_id = std::stoi(*row_value) + 1;

  return 0;
}   

uint32_t DataBase::next_user_id() const {
  const uint32_t FIRST_USER_ID{ 1 };
  uint32_t next_id{ FIRST_USER_ID }; 
  
  const std::string& retrieve_latest_assigned_id = create_sql_request(
      "select max(id) from users;");

  execute_request(retrieve_latest_assigned_id, calculate_new_id, &next_id);
  
  return next_id;
}

void DataBase::setup_user(const std::string& username, const std::string& password) const {
  const std::string& user_creation  = create_sql_request(
      "insert into users(username, password) values('?','?');",
      username, 
      password
  );

  execute_request(user_creation);
};

void DataBase::setup_user_balance(uint32_t user_id, const Price& intial_balance) const {
  const std::string& balance_creation  = create_sql_request(
      "insert into balance(uid, amount) values('?','?');",
      to_string(user_id), 
      to_string(intial_balance.totalValue())
  );

  execute_request(balance_creation);
};

bool DataBase::user_exists(const std::string& username, const std::string& password) const {
  const std::string& find_user = create_sql_request(
      "select * from users where username = '?' and password = '?';",
       username, password);

  bool user_exists = false;
  execute_request(find_user, value_exists_callback , &user_exists);

  return user_exists;
};



int create_user_json_data(void* user_return_data, int column_count, char** row, char** columns) {
  const int8_t ID{0};
  const int8_t USERNAME{1};
  json* user_data{ static_cast<json*>(user_return_data) };
  

  (*user_data)["id"] =  retrieve_from_row(row, ID); 
  (*user_data)["username"] =  retrieve_from_row(row, USERNAME); 

  return 0;
};

json DataBase::retrieve_user(const std::string& username, const std::string& password) const {
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

    const auto& user_id = retrieve_user(username, password);
    const std::string& user_deletion  = create_sql_request(
        "delete from users where username='?' and password='?';",
        username, 
        password
    );
    
    const std::string& balance_deletion = create_sql_request(
      "delete from balance where uid='?';",
      user_id["id"]
    );

    execute_request(user_deletion);
    execute_request(balance_deletion);
  }
}; 

bool valid_price(const Price& price) {
  return price.totalValue() >= 0;
}

void DataBase::update_user_balance(uint32_t user_id,const Price& new_account_balance) const {
  if (valid_price(user_id)) {
    
    const std::string& balance_update = create_sql_request(
        "update balance set amount='?' where uid='?';",
        to_string(new_account_balance.totalValue()),
        to_string(user_id)
    );
  
    execute_request(balance_update);
  }
};  

int retrieve_user_balance_callback (void* return_object, int args, char **row , char **columns) {
    Price* const balance = static_cast<Price*>(return_object);

    const std::string& balance_str = retrieve_from_row(row, 0);
    *balance = Price(std::stol(balance_str));
    return 0;
};

Price DataBase::retrieve_user_balance(uint32_t user_id) const {
  Price user_balance; 
  
  execute_request(
      create_sql_request("select amount from balance where uid='?';", to_string(user_id)),
      retrieve_user_balance_callback,
      &user_balance
  );

  return user_balance;
}; 

uint64_t DataBase::retrieve_user_stock_quantity(uint32_t user_id) const {
  uint64_t user_stock_balance{0};

  execute_request(
      create_sql_request("select sum(stock_amount) from balance where uid='?';", to_string(user_id)),
      retrieve_user_stock_quantity_callback,
      &user_stock_balance
  );
  return user_stock_balance;
};

json DataBase::register_order(const OrderData& order) const {
  if (order_exists(order.id)) {
    return json_message("Order does already exist");
  }
  const std::string& order_registration =  create_sql_request(
    "insert into orders values('?', '?', '?', '?', '?', '?', '?', TRUE);",
    to_string(order.id),
    to_string(order.ownerId),
    OrderParameters::to_string(order.side),
    OrderParameters::to_string(order.type),
    to_string(order.price.totalValue()),
    to_string(order.quantity),
    to_string(order.fillOrKill)
  );
  
  execute_request(order_registration, default_callback);
  return json_message("Order registered successfully");
}


bool DataBase::order_exists(OrderId orderId) const {
  const std::string& find_order = create_sql_request(
      "select * from orders where id = '?';",
       to_string(orderId)
  );

  bool order_existant = false;
  execute_request(find_order, value_exists_callback, &order_existant);

  return order_existant;
};

void DataBase::register_order_as_complete(OrderId orderId) const {
    const std::string& order_completion_registration = create_sql_request(
        "update orders set active = FALSE where id = '?';",
        to_string(orderId)
    );

    execute_request(order_completion_registration);
};

void DataBase::delete_order(OrderId orderId) const {
    if ( order_exists(orderId) ) {
      const std::string& order_deletion  = create_sql_request(
          "delete from orders where id = '?';",
          std::to_string(orderId)
      );
      
      execute_request(order_deletion);
    }
}; 

std::string format_price(const std::string& price_in_cents_fractions) {
  Price price( atol(price_in_cents_fractions.c_str()) );

  return to_string(price.dollars) + "." + to_string(price.cents) + to_string(price.fractionsOfPenny);
}

int create_json_order_list(void* return_data, int column_count, char** row, char** columns) {
  std::vector<json>* orders_list = static_cast<std::vector<json>*>(return_data);
  
  // use an unscoped enum instead
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
