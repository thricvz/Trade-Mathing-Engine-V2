CREATE TABLE users(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR(30),
  password VARCHAR(30)
);

CREATE TABLE orders(
  id INTEGER PRIMARY KEY ,
  user_id INTEGER,
  dollars INTEGER,
  cents INTEGER,
  quantity INTEGER,
  fill_or_kill BOOLEAN,
  FOREIGN KEY (user_id) REFERENCES users(id)
);
