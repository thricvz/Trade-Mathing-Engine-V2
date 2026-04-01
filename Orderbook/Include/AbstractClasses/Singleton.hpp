#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton {
  public:
    Singleton() = default;

    static T* getInstance() {
      static T singleInstance;      
      return &singleInstance;
    }
};

#endif
