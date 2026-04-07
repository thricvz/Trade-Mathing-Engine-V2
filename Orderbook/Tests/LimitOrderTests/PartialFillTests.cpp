#include <gtest/gtest.h>

#include "../../TestExtensions/TestExtensionsOrderBook.hpp"
#include "../../Include/Fifo/FifoFactory.hpp"
#include "../../Include/OrderBook.hpp"

FifoFactory factory;

TEST(sell_partial_fill_tests, partial_match_due_to_matches_of_same_owner) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::SELL, OrderType::LIMIT, Price{5, 0}, 56u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{234, 0, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{6u, 12u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 30u},
      OrderData{7u, 13, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 234u},
      OrderData{7u, 14u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 898u},
      OrderData{7u, 15u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 3u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {30u, OrderFillState::PARTIAL},
    {
      OrderMatch{6u, 12u,pricelevel1_price, 30u , OrderFillState::FULL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel1_price);

}

TEST(sell_partial_fill_tests, partial_match_because_of_inaccessible_pricelevels) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::SELL, OrderType::LIMIT, dollars(33), 38u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{234, 0, 3};
  Price inacessible_pricelevel_price{32, 99};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{6u, 12u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 30u},
      OrderData{5u, 13u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 6u},
      OrderData{8u, 14u, OrderSide::BUY, OrderType::LIMIT, inacessible_pricelevel_price, 898u},
      OrderData{8u, 15u, OrderSide::BUY, OrderType::LIMIT, inacessible_pricelevel_price, 3u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {36u, OrderFillState::PARTIAL},
    {
      OrderMatch{6u, 12u, pricelevel1_price, 30u , OrderFillState::FULL},
      OrderMatch{5u, 13u, pricelevel1_price, 6u, OrderFillState::FULL}
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), inacessible_pricelevel_price);


}

TEST(sell_partial_fill_tests, partial_match_due_to_insufficient_number_of_orders) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::SELL, OrderType::LIMIT, Price{5, 0}, 56u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{234, 0, 3};
  Price pricelevel2_price{13, 33, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 23u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 30u},
      OrderData{5u, 12u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 20u},
      OrderData{4u, 104u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 4u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {54u, OrderFillState::PARTIAL},
    {
      OrderMatch{3u, 23u, pricelevel1_price, 30u, OrderFillState::FULL},
      OrderMatch{5u, 12u, pricelevel1_price, 20u, OrderFillState::FULL},
      OrderMatch{4u, 104u, pricelevel2_price, 4u, OrderFillState::FULL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), Price());


}


TEST(buy_partial_fill_tests, partial_match_due_to_matches_of_same_owner) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::BUY, OrderType::LIMIT, Price{234, 5} , 508u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{234, 0, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{6u, 12u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 50u},
      OrderData{7u, 13, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 234u},
      OrderData{7u, 14u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 898u},
      OrderData{7u, 15u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 3u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {50u, OrderFillState::PARTIAL},
    {
      OrderMatch{6u, 12u,pricelevel1_price, 50u , OrderFillState::FULL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel1_price);

}

TEST(buy_partial_fill_tests, partial_match_because_of_inaccessible_pricelevels) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::BUY, OrderType::LIMIT, dollars(33), 38u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{32, 99};
  Price inacessible_pricelevel_price{234, 0, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{6u, 12u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 29u},
      OrderData{5u, 13, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 7u},
      OrderData{8u, 14u, OrderSide::SELL, OrderType::LIMIT, inacessible_pricelevel_price, 898u},
      OrderData{8u, 15u, OrderSide::SELL, OrderType::LIMIT, inacessible_pricelevel_price, 3u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {36u, OrderFillState::PARTIAL},
    {
      OrderMatch{6u, 12u, pricelevel1_price, 29u , OrderFillState::FULL},
      OrderMatch{5u, 13, pricelevel1_price, 7u, OrderFillState::FULL}
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), inacessible_pricelevel_price);

}

TEST(buy_partial_fill_tests, partial_match_due_to_insufficient_number_of_orders) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{7u, 11u, OrderSide::BUY, OrderType::LIMIT, dollars(500), 700u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{234, 0, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 23u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 35u},
      OrderData{5u, 12u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 16u},
      OrderData{4u, 104u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 3u},
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {54u, OrderFillState::PARTIAL},
    {
      OrderMatch{3u, 23u, pricelevel1_price, 35u, OrderFillState::FULL},
      OrderMatch{5u, 12u, pricelevel1_price, 16u, OrderFillState::FULL},
      OrderMatch{4u, 104u, pricelevel1_price, 3u, OrderFillState::FULL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), Price());


}
