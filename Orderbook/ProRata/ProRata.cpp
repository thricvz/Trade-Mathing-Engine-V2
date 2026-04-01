#include <algorithm>
#include <cstdint>

#include "../Include/ProRata/ProRata.hpp"
#include "../Include/PriceLevel.hpp"
#include "../Include/ProRata/ProRataPriceLevel.hpp"
#include "../Include/ProRata/ProRataOrder.hpp"
#include "../Include/Data.hpp"

MatchData ProRata::match(Order *askOrder, PriceLevel *priceLevel) const
{
  // sort the priceLevel per decreasing quantity per order
  ProRataPriceLevel *_priceLevel = static_cast<ProRataPriceLevel*>(priceLevel);
  _priceLevel->sort(0, _priceLevel->getNumberOfOrders() - 1);

  MatchData matchData {}; 
  OriginalOrderMatchData askOrderData{}; 
  std::vector<std::uint32_t> ordersToDelete {};

  const std::uint32_t nTotalOrders{_priceLevel->getNumberOfOrders()};
  const Quantity askQuantity = askOrder->quantity; 

  for (auto orderIndex{0}; orderIndex < nTotalOrders; orderIndex++) {
    ProRataOrder* order = _priceLevel->getAt(orderIndex);
    Quantity allocatedQuantity = askQuantity * order->calculateProRataRatio(_priceLevel->totalVolume);

    allocatedQuantity =  std::min(order->quantity, allocatedQuantity);      

    if (askOrder->quantity == 0)
      break;

    if (!order->hasDifferentOwner(askOrder) || allocatedQuantity <= 0)
      continue;

    OrderFillState offeringOrderFillState{};

    if (allocatedQuantity == askOrder->quantity) {
      offeringOrderFillState = OrderFillState::FULL;
      ordersToDelete.push_back(order->id);

    } else {
      offeringOrderFillState = OrderFillState::PARTIAL;
    }

  
    askOrder->quantity -= allocatedQuantity;
    askOrderData.allocatedQuantity += allocatedQuantity;
    order->quantity -= allocatedQuantity;

    OrderMatch matchEntry{
      order->ownerId         ,
      order->id              ,
      order->price           ,
      allocatedQuantity      ,
      offeringOrderFillState ,
    };

    matchData.matches.push_back( std::move(matchEntry) );
  }

  // set the relative data
  matchData.incomingOrderState = std::move(askOrderData);
  
  for (auto orderId : ordersToDelete) {
    _priceLevel->removeOrder(orderId);
  }

  if (askOrder->quantity > 0) {
    MatchData remainingMatchData = matchRemaining(askOrder, priceLevel); 
    matchData.extend(std::move(remainingMatchData));
  }

  return std::move(matchData);
}

MatchData ProRata::matchRemaining(Order* order, PriceLevel* priceLevel) const {
  return std::move(fifoAlgorithm.match(order, priceLevel));
}; 
