#ifndef FIFO_H
#define FIFO_H

#include "../AbstractClasses/MatchingAlgorithm.hpp"


class Fifo : public MatchingAlgorithm {
  public:
    Fifo() = default;      

    MatchData match(Order*, PriceLevel*) const override; 
};

#endif
