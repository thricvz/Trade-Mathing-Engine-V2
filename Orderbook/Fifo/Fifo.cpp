#include <cstdint>
#include <algorithm>

#include "../Include/Fifo/Fifo.hpp"
#include "../Include/OrderParameters.hpp"
#include "../Include/Data.hpp"

MatchData Fifo::match(Order* incomingOrder, PriceLevel* priceLevel) const {

  MatchData matchResult{};
  OriginalOrderMatchData incomingOrderData{};  
  
  const std::int32_t nOrdersPriceLevel = priceLevel->getNumberOfOrders(); 

  if (incomingOrder->quantity == 0 || nOrdersPriceLevel == 0) {
    return std::move(matchResult);
  }

  std::vector<std::uint32_t> ordersToDelete{}; 

  for (auto orderIndex{0};
      (orderIndex < nOrdersPriceLevel) && (incomingOrder->quantity > 0);
      orderIndex ++) {

    Order* order = priceLevel->getAt(orderIndex);

    if (!incomingOrder->hasDifferentOwner(order)) 
      continue;
   
    const Quantity offeredQuantity = order->quantity;
    const Quantity askedQuantity = incomingOrder->quantity;

    const Quantity allocatedQuota = std::min(offeredQuantity, askedQuantity);
    const OrderFillState orderState = offeredQuantity > askedQuantity ? OrderFillState::PARTIAL : OrderFillState::FULL;

    // update order
    incomingOrderData.allocatedQuantity += allocatedQuota; 
    incomingOrder->quantity -= allocatedQuota; 
    // update the order
    order->quantity -= allocatedQuota;
    priceLevel->decrease_total_volume(allocatedQuota);

    OrderMatch match{
      order->ownerId , 
      order->id      ,
      order->price   ,
      allocatedQuota ,
      orderState     ,
    };

    matchResult.matches.push_back(std::move(match)); 

    //delete order in case
    if (orderState == OrderFillState::FULL)
      ordersToDelete.push_back(order->id);

    incomingOrderData.fillState = (incomingOrder->quantity == 0) ? OrderFillState::FULL : OrderFillState::PARTIAL;
  }

  //erases all orders that have been deleted
  for (auto orderToRemoveId : ordersToDelete) {
    priceLevel->removeOrder(orderToRemoveId);

  }

  matchResult.incomingOrderState = std::move(incomingOrderData);
  return std::move(matchResult);
};

