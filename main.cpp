#include "RessourceManager/Include/RessourceManager.hpp"
#include <exception>
#include <iostream>

RessourceManager* ressources;

int main() {
  try {
    ressources = RessourceManager::Instance();

    ressources->create_user("dark maul", "f obi wan kenobi");
    auto result = ressources->retrieve_user("dark maul", "f obi wan kenobi");
    std::cout << result;

  } catch(std::exception& error) {
    std::cout << error.what();
  }
};
