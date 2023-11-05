#pragma once

#include <stdint.h>
#include <iostream>


template<class Tag, class IdType = size_t, IdType InvalidValue = 0>
class GenId {
    public:
      inline const static GenId invalid = GenId{InvalidValue};

      explicit GenId() = default;
      explicit GenId(const IdType &id) : rawId(id) {}

      explicit operator size_t() const { return rawId; }
      GenId &operator=(const GenId &other) = default;
      bool operator==(const GenId &other) const { return rawId == other.rawId; }
      operator bool() const { return valid(); }

      bool valid() const { return rawId != InvalidValue; }

      friend std::hash<GenId>;

      static GenId next() {
        static IdType counter = InvalidValue;
        return GenId{++counter};
      }

    private:
      IdType rawId = InvalidValue;
};

template<class T>
struct is_serializable {
  static constexpr bool value = false;
};

template<class Tag, class IdType, IdType InvalidValue>
struct is_serializable<GenId<Tag, IdType, InvalidValue>> {
  static constexpr bool value = true;
};

namespace std {
template <class Tag, class IdType, IdType InvalidValue> struct hash<GenId<Tag, IdType, InvalidValue>> {
      size_t operator()(const GenId<Tag, IdType, InvalidValue> &id) const noexcept { return id.rawId; }
};
}   // namespace std

template <class Tag, class IdType, IdType InvalidValue>
std::ostream& operator<<(std::ostream &os, const GenId<Tag, IdType, InvalidValue> &id) {
  os << std::hash<GenId<Tag, IdType, InvalidValue>>{}(id);
  return os;
}
