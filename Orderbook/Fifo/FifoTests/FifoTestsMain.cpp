#include <gtest/gtest.h>

#include "../../Include/Fifo/FifoFactory.hpp"
#include "../../Include/Fifo/Fifo.hpp"
#include "../../TestExtensions/TestExtensions.hpp"


TEST(FifoAlgorithm,CompleteFill1) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 50u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{
        11u, 8u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 100u
      }
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{50u, OrderFillState::FULL},
    {
      OrderMatch{
        11u, 8u, levelPrice, 50u, OrderFillState::PARTIAL
      }
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  ASSERT_EQ(priceLevel->getNumberOfOrders(), 1); 

  // check that the elements have the correct quantity 
  EXPECT_EQ(priceLevel->getAt(0)->quantity, 50u); 
}

TEST(FifoAlgorithm,CompleteFill2) {
  FifoFactory factory;
  OrderData askOrderDetails{30u, 99u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 5370u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 8u , OrderSide::SELL, OrderType::LIMIT, levelPrice, 998u },
      OrderData{30u, 9u , OrderSide::SELL, OrderType::LIMIT, levelPrice, 1000u},
      OrderData{34u, 10u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 223u },
      OrderData{22u, 11u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 334u },
      OrderData{33u, 12u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 3342u},
      OrderData{33u, 7u , OrderSide::SELL, OrderType::LIMIT, levelPrice, 22u  },
      OrderData{11u, 5u , OrderSide::SELL, OrderType::LIMIT, levelPrice, 112u },
      OrderData{11u, 6u , OrderSide::SELL, OrderType::LIMIT, levelPrice, 339u }
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{5370u, OrderFillState::FULL},
    {
      OrderMatch{11u, 8u , levelPrice, 998u , OrderFillState::FULL},
      OrderMatch{34 , 10u, levelPrice, 223u , OrderFillState::FULL},
      OrderMatch{22 , 11u, levelPrice, 334u , OrderFillState::FULL},
      OrderMatch{33 , 12u, levelPrice, 3342u, OrderFillState::FULL},
      OrderMatch{33 , 7u , levelPrice, 22u  , OrderFillState::FULL},
      OrderMatch{11u, 5u , levelPrice, 112u , OrderFillState::FULL},
      OrderMatch{11u, 6u , levelPrice, 339u , OrderFillState::FULL} 
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 1);

  EXPECT_EQ(priceLevel->getAt(0)->quantity, 1000u); 
}

TEST(FifoAlgorithm,CompleteFill3) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 225u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 8u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 223u},
      OrderData{11u, 9u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 4   },
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(

    OriginalOrderMatchData{225u, OrderFillState::FULL},
    {
      OrderMatch{11u, 8u, levelPrice, 223u, OrderFillState::FULL   },
      OrderMatch{11u, 9u, levelPrice, 2   , OrderFillState::PARTIAL},
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 1);

  EXPECT_EQ(priceLevel->getAt(0)->quantity, 2u);
}

TEST(FifoAlgorithm, NoAssortiveOrderMatching) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 4999u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{10u, 33u, OrderSide::BUY, OrderType::LIMIT, levelPrice, 500u},
      OrderData{10u, 34u, OrderSide::BUY, OrderType::LIMIT, levelPrice, 300u},
      OrderData{10u, 35u, OrderSide::BUY, OrderType::LIMIT, levelPrice, 250u}
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{0, OrderFillState::NO_FILL} ,
    {}
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 4999u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 3);

  EXPECT_EQ(priceLevel->getAt(0)->quantity, 500u);
  EXPECT_EQ(priceLevel->getAt(1)->quantity, 300u);
  EXPECT_EQ(priceLevel->getAt(2)->quantity, 250u);
}

TEST(FifoAlgorithm, PartialFill1) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 345};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 33u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 300u} ,
      OrderData{12u, 34u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 41u}
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{341u, OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 33u, levelPrice, 300u, OrderFillState::FULL } ,
      OrderMatch{12u, 34u, levelPrice, 41u, OrderFillState::FULL }
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 4);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);

}

TEST(FifoAlgorithm, PartialFill2) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 10'002};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 34u, OrderSide::SELL, OrderType::LIMIT, levelPrice , 300u} ,
      OrderData{12u, 36u, OrderSide::SELL, OrderType::LIMIT, levelPrice , 200u} ,
      OrderData{12u, 35u, OrderSide::SELL, OrderType::LIMIT, levelPrice , 400u}
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{900u, OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 34u, levelPrice, 300u, OrderFillState::FULL} ,
      OrderMatch{12u, 36u, levelPrice, 200u, OrderFillState::FULL} ,
      OrderMatch{12u, 35u, levelPrice, 400u, OrderFillState::FULL}
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 9102);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);
}

TEST(FifoAlgorithm, PartialFill3) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 5'000'000u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 31u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 1'000'000u} ,
      OrderData{13u, 32u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 1'000'000u} ,
      OrderData{12u, 33u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 1'000'000u} ,
      OrderData{13u, 34u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 1'000'000u} ,
      OrderData{16u, 35u, OrderSide::SELL, OrderType::LIMIT, levelPrice, 999'995u}   
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{4'999'995u, OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 31u, levelPrice, 1'000'000u, OrderFillState::FULL} ,
      OrderMatch{13u, 32u, levelPrice, 1'000'000u, OrderFillState::FULL} ,
      OrderMatch{12u, 33u, levelPrice, 1'000'000u, OrderFillState::FULL} ,
      OrderMatch{13u, 34u, levelPrice, 1'000'000u, OrderFillState::FULL} ,
      OrderMatch{16u, 35u, levelPrice, 999'995u  , OrderFillState::FULL}   
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 5u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);

}

TEST(FifoAlgorithm, NoFill2) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 0u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    {
      OrderData{11u, 2u, OrderSide::SELL, OrderType::MARKET, levelPrice, 10u}
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{0, OrderFillState::NO_FILL} ,
    {}
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 1);

  EXPECT_EQ(priceLevel->getAt(0)->quantity, 10u);
}

TEST(FifoAlgorithm, NoFill1) {
  FifoFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 5'000'000u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    {}
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{0, OrderFillState::NO_FILL} ,
    {}
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 5'000'000u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);
}
