#include <gtest/gtest.h>

#include "../../TestExtensions/TestExtensionsOrderBook.hpp"
#include "../../Include/Fifo/FifoFactory.hpp"
#include "../../Include/OrderBook.hpp"

FifoFactory factory;

TEST(Sell_full_fill_test, no_different_owners ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::LIMIT, Price{5, 0}, 10'000u }
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{5, 34};
  Price pricelevel2_price{5, 1};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{1u, 1u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{1u, 2u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{1u, 3u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 3'000u },
      OrderData{1u, 4u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 5'000u },
      OrderData{1u, 5u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{1u, 6u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 10'000u },

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
  EXPECT_EQ(orderbook.bestBid(), pricelevel1_price);
}

TEST(Sell_full_fill_test, insufficient_orders_within_criteria) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::LIMIT, Price{15, 0}, 10'000u , true}
  ));

  OrderBook orderbook(&factory); 

  Price inaccessible_price1{0, 3};
  Price inaccessible_price2{1, 20};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::BUY, OrderType::LIMIT, inaccessible_price1, 20u},
      OrderData{4u, 2u, OrderSide::BUY, OrderType::LIMIT, inaccessible_price1, 2'000u },
      OrderData{6u, 2u, OrderSide::BUY, OrderType::LIMIT, inaccessible_price2, 1'500'000u },
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
  EXPECT_EQ(orderbook.bestBid(), inaccessible_price2);

}

TEST(Sell_full_fill_test, across_mulitple_pricelevels ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::LIMIT, Price{15, 0}, 4'500'000u }
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{30,0};
  Price pricelevel2_price{30,3,1};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 500'000u },
      OrderData{4u, 2u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 1'000'000u },
      OrderData{6u, 3u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 1'500'000u },
      OrderData{6u, 4u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 1'500'000u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {4'500'000u, OrderFillState::FULL},
    {
      OrderMatch{3u, 1u, pricelevel1_price, 500'000u  , OrderFillState::FULL },
      OrderMatch{4u, 2u, pricelevel1_price, 1'000'000u, OrderFillState::FULL },
      OrderMatch{6u, 3u, pricelevel1_price, 1'500'000u, OrderFillState::FULL },
      OrderMatch{6u, 4u, pricelevel2_price, 1'500'000u, OrderFillState::FULL },
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), Price{});
}

TEST(Sell_full_fill_test, one_pricelevel ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::SELL, OrderType::LIMIT, Price{15, 0}, 300u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{30,3,1};
  Price pricelevel2_price{30,0};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::BUY, OrderType::LIMIT, pricelevel1_price, 300u },
      OrderData{4u, 3u, OrderSide::BUY, OrderType::LIMIT, pricelevel2_price, 5898u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {300u, OrderFillState::FULL},
    {
      OrderMatch{3u, 1u, pricelevel1_price, 300u, OrderFillState::FULL },
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestBid(), pricelevel2_price);

}


// incoming buy order 
TEST(Buy_full_fill_test, no_different_owners ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::LIMIT, Price{6, 0}, 10'000u }
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{5, 34};
  Price pricelevel2_price{5, 1};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{1u, 1u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 50u },
      OrderData{1u, 2u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 150u },
      OrderData{1u, 3u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 3'000u },
      OrderData{1u, 4u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 5'000u },
      OrderData{1u, 5u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 237u },
      OrderData{1u, 6u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 10'000u },

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
  EXPECT_EQ(orderbook.bestAsk(), pricelevel2_price);
}

TEST(Buy_full_fill_test, insufficient_orders_within_criteria) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::LIMIT, Price{0, 2}, 3u }
  ));

  OrderBook orderbook(&factory); 

  Price inaccessible_price1{0, 3};
  Price inaccessible_price2{1, 20};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::SELL, OrderType::LIMIT, inaccessible_price1, 20u},
      OrderData{4u, 2u, OrderSide::SELL, OrderType::LIMIT, inaccessible_price1, 2'000u },
      OrderData{6u, 2u, OrderSide::SELL, OrderType::LIMIT, inaccessible_price2, 1'500'000u },
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
  EXPECT_EQ(orderbook.bestAsk(), inaccessible_price1);

}

TEST(Buy_full_fill_test, across_mulitple_pricelevels ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::LIMIT, Price{31, 0}, 4'500'000u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{30,0};
  Price pricelevel2_price{30,3,1};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 500'000u },
      OrderData{4u, 2u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 1'000'000u },
      OrderData{6u, 3u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 1'500'000u },
      OrderData{6u, 4u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 1'500'001u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {4'500'000u, OrderFillState::FULL},
    {
      OrderMatch{3u, 1u, pricelevel1_price, 500'000u  , OrderFillState::FULL },
      OrderMatch{4u, 2u, pricelevel1_price, 1'000'000u, OrderFillState::FULL },
      OrderMatch{6u, 3u, pricelevel1_price, 1'500'000u, OrderFillState::FULL },
      OrderMatch{6u, 4u, pricelevel2_price, 1'500'000u, OrderFillState::PARTIAL },
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel2_price);
}

TEST(Buy_full_fill_test, one_pricelevel ) {
  auto incomingOrder = std::move(factory.createOrder(
      OrderData{1u, 999u, OrderSide::BUY, OrderType::LIMIT, Price{1508, 0, 2}, 300u}
  ));

  OrderBook orderbook(&factory); 

  Price pricelevel1_price{999,3,1};
  Price pricelevel2_price{999,0};
 
  TestExtensions::OrderBook::loadOrders(&factory, &orderbook, 
    std::vector{
      OrderData{3u, 1u, OrderSide::SELL, OrderType::LIMIT, pricelevel1_price, 300u },
      OrderData{4u, 3u, OrderSide::SELL, OrderType::LIMIT, pricelevel2_price, 5898u },
    }    
  );
  
  MatchData expectedResult = std::move(TestExtensions::OrderBook::generateExpectedResult(
    {300u, OrderFillState::FULL},
    {
      OrderMatch{4u, 3u, pricelevel2_price, 300u, OrderFillState::PARTIAL },
    }
  ));

  MatchData matchResult = std::move(TestExtensions::OrderBook::generateResult(
    &orderbook, std::move(incomingOrder)
  ));

  EXPECT_EQ(matchResult, expectedResult);
  EXPECT_EQ(orderbook.bestAsk(), pricelevel2_price);

}
