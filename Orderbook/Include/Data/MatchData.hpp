#ifndef MATCH_DATA_H
#define MATCH_DATA_H

#include <cstdint>
#include <vector>
#include <algorithm>

#include "../OrderParameters.hpp"
#include "OriginalOrderMatchData.hpp"
#include "OrderMatch.hpp"

static bool per_ascending_order_id(const OrderMatch& lhs, const OrderMatch& rhs) {
    return lhs.orderId < rhs.orderId;
}

struct MatchData {
  OriginalOrderMatchData incomingOrderState{};
  std::vector<OrderMatch> matches{};


  void extend(MatchData&& _additionalData) { 

      OriginalOrderMatchData newOrderState{
        _additionalData.incomingOrderState.allocatedQuantity + incomingOrderState.allocatedQuantity,
        _additionalData.incomingOrderState.fillState
      };
      
      // extends the matches list
      auto& additionalMatches = _additionalData.matches;

      std::sort(matches.begin(), matches.end(), per_ascending_order_id);
      std::sort(additionalMatches.begin(), additionalMatches.end(), per_ascending_order_id);
     
      // index for the i 
      const std::int32_t nMatches = matches.size();
      const std::int32_t nAdditionalMatches = additionalMatches.size();
      
      std::int32_t iMatch {};
      std::int32_t iAdditionalMatch{};

      while ((iMatch < nMatches) && (iAdditionalMatch < nAdditionalMatches)) {
        auto &match = matches.at(iMatch);
        auto &additionalMatch = additionalMatches.at(iAdditionalMatch);

        if (match.orderId == additionalMatch.orderId) {
          match.fillState = additionalMatch.fillState;
          match.allocatedQuantity += additionalMatch.allocatedQuantity;
          iMatch ++;
          iAdditionalMatch ++;

        } else if (match.orderId < additionalMatch.orderId) {
          iMatch ++;
        } else {
          iAdditionalMatch ++;
          matches.push_back(additionalMatch);
        }

      }
      
      for (; iAdditionalMatch < nAdditionalMatches; iAdditionalMatch ++) {
        matches.push_back(additionalMatches.at(iAdditionalMatch));
      }

      this->incomingOrderState = std::move(newOrderState); 
  }


  bool operator==(const MatchData& rhs) const{
    if ( !(incomingOrderState == rhs.incomingOrderState)  
         || (matches.size() != rhs.matches.size()))
      return false;

    if (matches.size() != rhs.matches.size()) 
      return false; 
   
    for (const auto& match : rhs.matches) {
        auto value = std::find(matches.begin(), matches.end(), match);
        if (value == matches.end()) {
          return false;
        }
    }

    return true;
  }

};

#endif
