#ifndef PRO_RATA_H
#define PRO_RATA_H

#include "../AbstractClasses/MatchingAlgorithm.hpp"
#include "../Fifo/Fifo.hpp"

class ProRata : public MatchingAlgorithm {
  public:
    ProRata() = default;      

    MatchData match(Order* order, PriceLevel* priceLevel) const override; 
  
  private:
    MatchData matchRemaining(Order* order, PriceLevel* priceLevel) const; 
    static inline Fifo fifoAlgorithm{}; 
};

#endif
