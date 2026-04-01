#ifndef ORIGINAL_ORDER_MATCH_DATA_H
#define ORIGINAL_ORDER_MATCH_DATA_H

#include "../OrderParameters.hpp"

struct OriginalOrderMatchData {
  Quantity allocatedQuantity {0};
  OrderFillState fillState {OrderFillState::NO_FILL};

  bool operator==(const OriginalOrderMatchData& rhs) const {
    return allocatedQuantity == rhs.allocatedQuantity 
        && fillState == rhs.fillState; 
  }
};


#endif
