#ifndef PRICE_H
#define PRICE_H

#include <cstdint>
#include <algorithm>



struct Price {
  std::uint32_t dollars{};
  std::uint8_t cents{};
  std::uint8_t fractionsOfPenny{}; // up to 2 digits of precision


  Price () = default;
  Price(std::uint32_t dollars, std::uint32_t cents) {
      this->cents = cents % 100;
      this->dollars = dollars + (cents/100);
  };

  Price(std::uint32_t dollars, std::uint8_t cents, std::uint8_t fractionsOfPenny) {
      this->fractionsOfPenny = fractionsOfPenny % 100;
      cents = cents + (fractionsOfPenny / 100);
      this->cents = cents % 100;
      this->dollars = dollars + (cents/100);

  }

  Price(std::uint64_t fractionsOfPenny) {
      uint64_t totalCents = fractionsOfPenny / 100;
      this->fractionsOfPenny = fractionsOfPenny % 100;
      this->cents = totalCents % 100;
      this->dollars = totalCents / 100;
  }
    

  bool operator==(const Price& rhs) const {
    return  (dollars == rhs.dollars)
         && (cents == rhs.cents) 
         && (fractionsOfPenny == rhs.fractionsOfPenny);
  }

  bool operator<(const Price& rhs) const {
    return this->totalValue() < rhs.totalValue();
  }

  bool operator<=(const Price& rhs) const {
    return (*this == rhs) || (*this < rhs);
  }


  std::uint64_t totalValue() const {
    return dollars * 10'000 + cents * 100 + fractionsOfPenny;
  }
};

static Price dollars(int amount_in_dollars) {
  return Price(amount_in_dollars, 0, 0);
}

#endif
