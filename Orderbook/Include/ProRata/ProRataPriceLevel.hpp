#ifndef PRO_RATA_PRICE_LEVEL_H
#define PRO_RATA_PRICE_LEVEL_H

#include <cstdint>
#include <algorithm>

#include "../PriceLevel.hpp"
#include "../ProRata/ProRataOrder.hpp"




struct ProRataPriceLevel : public PriceLevel {
  ProRataPriceLevel(Price price) : PriceLevel(price) {}
 

  ProRataOrder* getAt(std::int32_t index) const override {
    Order* orderPtr = PriceLevel::getAt(index);
    if (orderPtr) {
      return static_cast<ProRataOrder*>(orderPtr);
    }
    return nullptr;
  }

  void sort(std::int32_t start, std::int32_t end) {
    auto orderCompare = [](const std::unique_ptr<Order>& rhs,
                          const std::unique_ptr<Order>& lhs) {
      return rhs->hasPriorityOver(lhs.get());
    };

    std::sort(this->m_orderList.begin(), this->m_orderList.end(), orderCompare);
  }
};

#endif
