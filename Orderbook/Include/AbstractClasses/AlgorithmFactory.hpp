#ifndef ALGORITHM_FACTORY_H
#define ALGORITHM_FACTORY_H

#include <memory>
#include "Order.hpp"
#include "MatchingAlgorithm.hpp"
#include "../OrderData.hpp"
#include "../Price.hpp"


class AlgorithmFactory {
  public:
    AlgorithmFactory() = default; 

    virtual std::unique_ptr<MatchingAlgorithm> createAlgorithm() = 0;
    virtual std::unique_ptr<Order> createOrder(OrderData) = 0;
    virtual std::unique_ptr<PriceLevel> createPriceLevel(const Price&) = 0;

};

#endif
