CREATE TABLE users(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR(30),
  password VARCHAR(30)
);

CREATE TABLE orders(
  id INTEGER PRIMARY KEY ,
  user_id INTEGER,

  order_side VARCHAR(10), 
  order_type VARCHAR(10), 

  price UNSIGNED BIGINT,
  quantity INTEGER,
  fill_or_kill BOOLEAN,

  active BOOLEAN,


  FOREIGN KEY (user_id) REFERENCES users(id)
);
