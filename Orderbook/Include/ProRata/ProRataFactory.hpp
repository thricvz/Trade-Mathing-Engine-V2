#ifndef PRO_RATA_FACTORY_H
#define PRO_RATA_FACTORY_H

#include "../AbstractClasses/AlgorithmFactory.hpp"
#include "ProRata.hpp"
#include "ProRataOrder.hpp"
#include "ProRataPriceLevel.hpp"



class ProRataFactory : public AlgorithmFactory {
  public:
    ProRataFactory() = default;

    std::unique_ptr<MatchingAlgorithm> createAlgorithm() override {
       return std::make_unique<ProRata>();
    }

    std::unique_ptr<Order> createOrder(OrderData orderDetails) override {
      return std::make_unique<ProRataOrder>(orderDetails);
    } 

    std::unique_ptr<PriceLevel> createPriceLevel(const Price& price) override {
      return std::make_unique<ProRataPriceLevel>(price);
    } 
};

#endif
