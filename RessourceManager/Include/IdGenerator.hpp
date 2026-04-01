#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <cstdint>


class IdGenerator {
  public:
    IdGenerator() = default;

    uint32_t get_new_id() {
      m_id ++;
      return m_id;
    }

  private:  
    uint32_t m_id{};
};

#endif
