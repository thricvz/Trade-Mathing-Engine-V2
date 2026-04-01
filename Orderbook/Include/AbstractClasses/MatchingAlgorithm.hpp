#ifndef MATCHING_ALGORITHM_H
#define MATCHING_ALGORITHM_H

#include <utility>

#include "Order.hpp"
#include "../PriceLevel.hpp"
#include "../Data/MatchData.hpp"

class MatchingAlgorithm {
  public:
    MatchingAlgorithm() = default;      
    virtual ~MatchingAlgorithm() = default;

    virtual MatchData match(Order*, PriceLevel*) const = 0; 
};

#endif
