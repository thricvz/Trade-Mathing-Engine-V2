#include <gtest/gtest.h>
#include <iostream>

#include "../../TestExtensions/TestExtensions.hpp"
#include "../../Include/ProRata/ProRataFactory.hpp"

TEST(ProRataAlgorithm,CompleteFill1) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 3874u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector<OrderData>{
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 3500}  ,
      OrderData{12u, 20u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 400}   ,
      OrderData{14u, 18u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 50}    ,
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{3874u, OrderFillState::FULL},
    std::vector<OrderMatch>{
      OrderMatch{11u, 23u, levelPrice, 3433, OrderFillState::PARTIAL} ,
      OrderMatch{12u, 20u, levelPrice, 392, OrderFillState::PARTIAL } ,
      OrderMatch{14u, 18u, levelPrice, 49,  OrderFillState::PARTIAL } ,
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 3); 
}


TEST(ProRataAlgorithm,CompleteFill2) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 2211u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector<OrderData>{
      OrderData{15u, 25u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 1074u } ,
      OrderData{16u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 3260u} ,
      OrderData{17u, 26u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 1671u} ,
      OrderData{18u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 670u} ,
      OrderData{18u, 22u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 400u} ,
      OrderData{15u, 21u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 797u} ,
      OrderData{17u, 20u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 571u} ,
      OrderData{16u, 6u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 248u} ,
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{2211u, OrderFillState::FULL},
    std::vector<OrderMatch>{
      OrderMatch{15u, 25u, levelPrice, 273u, OrderFillState::PARTIAL} ,
      OrderMatch{16u, 24u, levelPrice, 829u, OrderFillState::PARTIAL} ,
      OrderMatch{17u, 26u, levelPrice, 425u, OrderFillState::PARTIAL} ,
      OrderMatch{18u, 23u, levelPrice, 482u, OrderFillState::PARTIAL} ,
      OrderMatch{15u, 21u, levelPrice, 202u, OrderFillState::PARTIAL} ,
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 0u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 8);
}

TEST(ProRataAlgorithm, NoAssortiveOrderMatching) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 7u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 4999u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector<OrderData>{
      OrderData{10u, 22u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 50}   ,
      OrderData{10u, 25u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 50}   ,
      OrderData{10u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 50}   
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{0, OrderFillState::NO_FILL} ,
    std::vector<OrderMatch>{}
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 4999u);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 3);
}

TEST(ProRataAlgorithm, PartialFill1) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 1957u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 50}   ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 856}  ,
      OrderData{11u, 25u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 900}  ,
      OrderData{10u, 29u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 5665} ,
      OrderData{11u, 27u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 56}   ,
      OrderData{11u, 28u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 65}
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{1927u, OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 23u, levelPrice, 50 , OrderFillState::FULL} ,
      OrderMatch{11u, 24u, levelPrice, 856, OrderFillState::FULL} ,
      OrderMatch{11u, 25u, levelPrice, 900, OrderFillState::FULL} ,
      OrderMatch{11u, 27u, levelPrice, 56 , OrderFillState::FULL} ,
      OrderMatch{11u, 28u, levelPrice, 65 , OrderFillState::FULL}
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 30);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);

}

TEST(ProRataAlgorithm, PartialFill2) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 50u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 6}  ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 7}  ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 8}  ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 2}  ,
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{23u , OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 23u, levelPrice, 6, OrderFillState::FULL} ,
      OrderMatch{11u, 24u, levelPrice, 7, OrderFillState::FULL} ,
      OrderMatch{11u, 25u, levelPrice, 8, OrderFillState::FULL} ,
      OrderMatch{11u, 26u, levelPrice, 2, OrderFillState::FULL} 
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 27);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);
}


TEST(ProRataAlgorithm, PartialFill3) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 640u};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector{
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 240}  ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 360}  ,
      OrderData{11u, 24u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 10}  
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{610u, OrderFillState::PARTIAL} ,
    std::vector{
      OrderMatch{11u, 23u, levelPrice, 240, OrderFillState::FULL} ,
      OrderMatch{11u, 24u, levelPrice, 360, OrderFillState::FULL} ,
      OrderMatch{11u, 25u, levelPrice, 10, OrderFillState::FULL}  
    }
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 30);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);
}


TEST(ProRataAlgorithm, NoFill) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 0};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    {
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 240}      ,
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 3434240}  ,
      OrderData{13u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 334}      ,
      OrderData{13u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 8}        ,
      OrderData{11u, 23u, OrderSide::SELL , OrderType::LIMIT, levelPrice, 3}        
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
  EXPECT_EQ(askOrder->quantity, 0);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 5);
}

TEST(ProRataAlgorithm, NoFill2) {
  ProRataFactory factory;
  OrderData askOrderDetails{10u, 1u, OrderSide::BUY, OrderType::MARKET
                                   , Price()       , 3500};


  auto askOrder = factory.createOrder(askOrderDetails);
  const Price levelPrice {10,1};

  std::unique_ptr<PriceLevel> priceLevel = std::move(TestExtensions::PriceLevel::generatePriceLevel(
    &factory    ,
    levelPrice  , 
    std::vector<OrderData>{
    }
  )); 
  
  MatchData expectedResult = TestExtensions::PriceLevel::generateExpectedResult(
    OriginalOrderMatchData{0, OrderFillState::NO_FILL} ,
    std::vector<OrderMatch>{}
  );

  MatchData matchResult = TestExtensions::PriceLevel::generateResult(
    &factory,
    priceLevel.get(),
    askOrder.get()  
  );

  EXPECT_EQ(expectedResult, matchResult);
  EXPECT_EQ(askOrder->quantity, 3500);
  EXPECT_EQ(priceLevel->getNumberOfOrders(), 0);
}
