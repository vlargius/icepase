#pragma once

#include <stdint.h>
#include <iostream>


template<class Tag, class IdType = size_t, IdType InvalidValue = 0>
class GenId {
    public:
      inline const static GenId invalid = GenId{InvalidValue};

      explicit GenId() = default;
      explicit GenId(const IdType &id) : rawId(id) {}

      inline explicit operator size_t() const { return rawId; }
      inline GenId &operator=(const GenId &other) = default;
      inline bool operator==(const GenId &other) const { return rawId == other.rawId; }
      inline bool operator<(const GenId &other) const { return rawId < other.rawId; }
      inline operator bool() const { return valid(); }

      inline bool valid() const { return rawId != InvalidValue; }

      friend std::hash<GenId>;

      inline static GenId next() {
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
      inline size_t operator()(const GenId<Tag, IdType, InvalidValue> &id) const noexcept { return id.rawId; }
};
}   // namespace std

template <class Tag, class IdType, IdType InvalidValue>
inline std::ostream& operator<<(std::ostream &os, const GenId<Tag, IdType, InvalidValue> &id) {
  os << std::hash<GenId<Tag, IdType, InvalidValue>>{}(id);
  return os;
}
