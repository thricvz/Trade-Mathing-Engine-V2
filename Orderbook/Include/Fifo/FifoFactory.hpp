#ifndef FIFO_FACTORY_H
#define FIFO_FACTORY_H

#include "../AbstractClasses/AlgorithmFactory.hpp"
#include "Fifo.hpp"
#include "FifoOrder.hpp"
#include "../PriceLevel.hpp"


class FifoFactory : public AlgorithmFactory {
  public:
    FifoFactory() = default;

    std::unique_ptr<MatchingAlgorithm> createAlgorithm() override {
       return std::make_unique<Fifo>();
    }

    std::unique_ptr<Order> createOrder(OrderData orderDetails) override {
      return std::make_unique<FifoOrder>(std::move(orderDetails));
    } 

    std::unique_ptr<PriceLevel> createPriceLevel(const Price& price) override {
      return std::make_unique<PriceLevel>(price);
    } 
};

#endif
