#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <cassert>

#include "identifier.h"
#include "enum_helper.h"


class instream {
  public:
    instream(const void *data, size_t count)
        : bitSize(count),
          buffer(static_cast<const uint8_t *>(data), static_cast<const uint8_t *>(data) + (count + 7) / 8) {}

    ~instream() {
        // assert(empty());
    }

    size_t size() const { return bitSize; }
    bool empty() const { return size() == head; }
    size_t left() const { return bitSize - head; }
    const void* data() const { return buffer.data(); }
    void reset(size_t size_) {
        bitSize = size_;
        head = 0;
    }

    void read(void *data, size_t count) {
        uint8_t* srcByte = static_cast<uint8_t*>(data);
        while (count >= 8) {
            read((uint8_t &)*srcByte, 8);
            ++srcByte;
            count -= 8;
        }
        if (count > 0)
            read((uint8_t &)*srcByte, count);
    }

    template <class T>
    typename std::enable_if<!std::is_enum<T>::value>::type
    read(T &instance) {
        static_assert(is_serializable<T>::value || std::is_arithmetic<T>::value,
                      "read only handles primitive and trivially serializable data types");
        read(&instance, sizeof(T) * 8);
    }

    template<>
    void read<bool>(bool &flag) {
        read((uint8_t&)flag, 1);
    }

    template<>
    void read<std::string>(std::string& str) {
        size_t count;
        read(count);
        str.resize(count);
        read(str.data(), (sizeof(char) * count + 1) * 8);
    }

    template<int Max, class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    read(T &enumiration) {
        read(&enumiration, utils::required_bits<Max>::value);
    }

    template<class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    read(T &enumiration, size_t max_value) {
        read(&enumiration, log2(max_value) - 1);
    }

    template<int Max, class T>
    typename std::enable_if<!std::is_enum<T>::value>::type
    read(T &enumiration) {
        read(&enumiration, Max);
    }

    template <class T> void read(std::vector<T> &data) {
        size_t count;
        read(count);
        data.resize(count);
        for (auto &item : data) {
            read(item);
        }
    }

    template<class Key, class Value>
    void read(std::unordered_map<Key, Value>& data) {
        size_t count;
        read(count);
        for (size_t i = 0; i < count; ++i) {
            Key key;
            read(key);
            read(data[key]);
        }
    }

    template <class T> void serialize(T &data) { read(data); }
    template <int Max, class T> void serialize(T &data) { read<Max, T>(data); }
    template <class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    serialize(T &data, size_t max_value) {
        read<T>(data, max_value);
    }

  private:
    size_t head = 0;
    size_t bitSize = 0;
    std::vector<uint8_t> buffer;

    void read(uint8_t &data, size_t count) {
        const size_t byteOffset = head / 8;
        const size_t bitOffset = head % 8;

        data = buffer[byteOffset] >> bitOffset;

        const size_t unusedBits = 8 - bitOffset;
        if (unusedBits < count) {
            data |= buffer[byteOffset + 1] << unusedBits;
        }
        data &= ~(0x00ff << count);

        head += count;
    }
};