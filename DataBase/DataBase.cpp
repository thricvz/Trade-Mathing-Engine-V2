#include "Include/DataBase.hpp"
#include <cstdint>
#include <iostream>
#include <exception>
#include <sys/types.h>
#include <type_traits>

using namespace std::literals;

#define REQUEST_PLACEHOLDER '?'
#define REQUEST_PLACEHOLDER_LENGTH 1

int default_callback(void*, int args, char** columns, char **rows) { return 0;}   ;

json json_message(const std::string& message) {
  return {{"message", message}};
}


template<typename... Types>
constexpr bool all_basic_string_v = (std::is_same_v<std::string, Types> && ...) ;

template<typename... Attributes ,
  typename = std::enable_if_t<all_basic_string_v<Attributes...>>>

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
    int(callback)(void*,int, char**, char**) = nullptr,
    void* callback_argument = nullptr, char** error_msg = nullptr) const {
  
  sqlite3_exec(m_database, sql_request.c_str(), callback, callback_argument, nullptr);
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
      "INSERT INTO users(username, password) VALUES('?','?');",
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


}; 

void DataBase::delete_order(const OrderData& order) const {

}; 

json DataBase::retrieve_orders() const {

}; 
