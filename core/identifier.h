#pragma once

#include <stdint.h>

template<class Tag, class IdType = size_t, IdType InvalidValue = 0>
class GenId {
    public:
      const static GenId invalid;

      explicit GenId() = default;
      explicit GenId(const IdType &id) : rawId(id) {}

      explicit operator size_t() const { return rawId; }
      GenId &operator=(const GenId &other) = default;
      bool operator==(const GenId &other) const { return rawId == other.rawId; }

      bool valid() const { return rawId != InvalidValue; }

      friend std::hash<GenId>;

      static constexpr const GenId next() { return GenId{++counter}; }

    private:
      IdType rawId = InvalidValue;
      static IdType counter;
};

template<class T>
struct is_serializable {
  static constexpr bool value = false;
};

template<class Tag, class IdType, IdType InvalidValue>
struct is_serializable<GenId<Tag, IdType, InvalidValue>> {
  static constexpr bool value = true;
};

template <class Tag, class IdType, IdType InvalidValue>
const GenId<Tag, IdType, InvalidValue> GenId<Tag, IdType, InvalidValue>::invalid = GenId{InvalidValue};

template <class Tag, class IdType, IdType InvalidValue>
IdType GenId<Tag, IdType, InvalidValue>::counter = InvalidValue;

namespace std {
template <class Tag, class IdType, IdType InvalidValue> struct hash<GenId<Tag, IdType, InvalidValue>> {
      size_t operator()(const GenId<Tag, IdType, InvalidValue> &id) const noexcept { return id.rawId; }
};
}   // namespace std
