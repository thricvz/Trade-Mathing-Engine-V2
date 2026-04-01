#include <gtest/gtest.h>

#include "../../TestExtensions/TestExtensionsOrderBook.hpp"
#include "../../Include/Fifo/FifoFactory.hpp"

FifoFactory factory;

TEST(Sell_fill_or_kill_tests, sufficient_number_of_buy_orders ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::MARKET, Price{}, 3008u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{30, 3};
  Price pricelevel2_price{10, 34};
  Price pricelevel3_price{3,0};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u,  101u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 300u },
      OrderData{1u,  102u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 38u },
      OrderData{2u,  103u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 2700u },
      OrderData{2u,  104u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 17u },
      OrderData{2u,  105u , OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 30u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {3008u, OrderFillState::FULL},
    {
      OrderMatch{9u,  101u, pricelevel1_price, 300u  , OrderFillState::FULL},
      OrderMatch{2u,  103u, pricelevel2_price, 2700u, OrderFillState::FULL},
      OrderMatch{2u,  104u, pricelevel2_price, 8u   , OrderFillState::PARTIAL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel2_price);
}
TEST(Sell_fill_or_kill_tests, insufficient_number_of_buy_orders ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::MARKET, Price{}, 10'000u, true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{10, 34};
  Price pricelevel2_price{11, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u, 101u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 20u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{10u, 500u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 36u },
      OrderData{9u, 101u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 96u },
      OrderData{10u, 20u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{10u, 500u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 50u },

    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel2_price);
}


TEST(Sell_fill_or_kill_tests, empty_fill_or_kill_order ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 0u, OrderSide::SELL, OrderType::MARKET, Price{}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{10, 34};
  Price pricelevel2_price{11, 3};
  Price pricelevel3_price{100, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u, 101u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 20u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{10u, 500u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 36u },
      OrderData{9u, 101u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 96u },
      OrderData{10u, 20u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{10u, 500u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 50u },
      OrderData{10u, 500u, OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 148u },
      OrderData{10u, 504u, OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 5058u },
      OrderData{10u, 510u, OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 40u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel3_price);
}


TEST(Sell_fill_or_kill_tests, insufficient_number_of_buy_orders_from_different_owner ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::MARKET, Price{}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{34, 0};
  Price pricelevel2_price{200, 34};
  Price pricelevel3_price{3, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u,  101u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 104u , OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 1007u },
      OrderData{11u, 102u , OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 2008u },
      OrderData{11u, 103u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 30u },
      OrderData{11u, 107u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 33u },
      OrderData{1u,  109u , OrderSide::BUY, OrderType::LIMIT, pricelevel3_price, 100'000'000u },
      OrderData{1u,  110u , OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 500'000u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel2_price);
}


// buy incoming order
TEST(Buy_fill_or_kill_tests, insufficient_number_of_sell_orders ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::MARKET, Price{}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{10, 34};
  Price pricelevel2_price{11, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u, 101u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 20u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{10u, 500u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 36u },
      OrderData{9u, 101u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 96u },
      OrderData{10u, 20u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{10u, 500u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 50u },

    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel1_price);
}


TEST(Buy_fill_or_kill_tests, empty_fill_or_kill_order ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 0u, OrderSide::BUY, OrderType::MARKET, Price{}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{10, 34};
  Price pricelevel2_price{11, 3};
  Price pricelevel3_price{100, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u, 101u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 20u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{10u, 500u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 36u },
      OrderData{9u, 101u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 96u },
      OrderData{10u, 20u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{10u, 500u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 50u },
      OrderData{10u, 500u, OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 148u },
      OrderData{10u, 504u, OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 5058u },
      OrderData{10u, 510u, OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 40u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel1_price);
}


TEST(Buy_fill_or_kill_tests, insufficient_number_of_sell_orders_from_different_owner ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::MARKET, Price{}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{34, 0};
  Price pricelevel2_price{200, 34};
  Price pricelevel3_price{3, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u,  101u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{10u, 104u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 1007u },
      OrderData{11u, 102u , OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 2008u },
      OrderData{11u, 103u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 30u },
      OrderData{11u, 107u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 33u },
      OrderData{1u,  109u , OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 100'000'000u },
      OrderData{1u,  110u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 500'000u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {0u, OrderFillState::NO_FILL},
    {}
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel3_price);
}
TEST(Buy_fill_or_kill_tests, sufficient_number_of_sell_orders ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::MARKET, Price{}, 3008u , true}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{3,0};
  Price pricelevel2_price{10, 34};
  Price pricelevel3_price{30, 3};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{9u,  101u , OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 300u },
      OrderData{1u,  102u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 38u },
      OrderData{2u,  103u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 2700u },
      OrderData{2u,  104u , OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 17u },
      OrderData{2u,  105u , OrderSide::SELL, OrderType::LIMIT, pricelevel3_price, 30u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {3008u, OrderFillState::FULL},
    {
      OrderMatch{9u,  101u, pricelevel1_price, 300u  , OrderFillState::FULL},
      OrderMatch{2u,  103u, pricelevel2_price, 2700u, OrderFillState::FULL},
      OrderMatch{2u,  104u, pricelevel2_price, 8u   , OrderFillState::PARTIAL},
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel2_price);
}
