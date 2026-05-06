#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <vector>
#include <memory>
#include <cassert>
#include <cstdint>

#include "OrderParameters.hpp"
#include "Price.hpp"
#include "AbstractClasses/Order.hpp"



class PriceLevel {
  public:
    Price price{};

    PriceLevel(Price Price) : price{Price} {}

    Quantity volume_excluding_owner(UserId excludedOwner) {
      Quantity volume{};
      for (const auto& order : this->m_orderList) {
        if (order->ownerId != excludedOwner) 
          volume += order->quantity;
      }
      return volume;
    };


    virtual Order* getAt(OrderId index) const {
      if (index >= this->m_orderList.size()) {
        return nullptr ;
      }

      return this->m_orderList.at(index).get();
    }    

    std::uint32_t getNumberOfOrders() const {
      return this->m_orderList.size();
    }

    
    virtual void addOrder(std::unique_ptr<Order> order){
      increase_total_volume(order->quantity);
      auto position = m_orderList.begin();
      for ( ; position != m_orderList.end() ; position++) {
        if (order->hasPriorityOver( (*position).get() )) 
          break;
      } 

      m_orderList.insert(position, std::move(order) );
    }

    virtual bool removeOrder(OrderId orderId) {
      std::unique_ptr<Order> orderToDelete{};

      for (auto orderIndex{0}; orderIndex < m_orderList.size() ; orderIndex++) {
        if (m_orderList.at(orderIndex)->id == orderId) {
          orderToDelete = std::move(m_orderList.at(orderIndex));
          m_orderList.erase(m_orderList.begin() + orderIndex);
          decrease_total_volume(orderToDelete->quantity);
          return true;
        }
      } 
      
      return false;
    }


    void increase_total_volume(Quantity increase) {
      totalVolume += increase;
    }

    void decrease_total_volume(Quantity decrease) {
      assert(totalVolume >= decrease && "decrease cannot turn volume negative");
      totalVolume -= decrease;
    }

    Quantity totalVolume{};

  protected:
    std::vector<std::unique_ptr<Order>> m_orderList{};
};


#endif
