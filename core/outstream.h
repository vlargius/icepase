#pragma once

#include <vector>
#include <unordered_map>
#include <cmath>

#include "enum_helper.h"


class outstream {
  public:
    outstream(size_t size_ = 256) { resize(size_); }

    size_t size() const { return head; }
    size_t byte_size() const { return (head + 7) / 8; }
    const void *data() const { return buffer.data(); }

    void reset() { head = 0; }

    void write(const void *data, size_t count) {
        const uint8_t *srcByte = static_cast<const uint8_t *>(data);
        while (count >= 8) {
            write(*srcByte, 8);
            ++srcByte;
            count -= 8;
        }
        if (count > 0) {
            write(*srcByte, count);
        }
    }

    template <class T>
    typename std::enable_if<!std::is_enum<T>::value>::type
    write(const T &instance) {
        static_assert(is_serializable<T>::value || std::is_arithmetic<T>::value,
                      "write only handles primitive and trivially serializable data types");
        write(&instance, sizeof(T) * 8);
    }

    template<>
    void write<bool>(const bool &flag) {
        write(flag, 1);
    }

    template<>
    void write<std::string>(const std::string& str) {
        write(str.size());
        write(str.data(), (sizeof(char) * str.size() + 1) * 8);
    }

    template<int Max, class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    write(const T &enumiration) {
        write((const void*)&enumiration, utils::required_bits<Max>::value);
    }

    template<int Max, class T>
    typename std::enable_if<!std::is_enum<T>::value>::type
    write(const T &enumiration) {
        write((const void*)&enumiration, Max);
    }

    template<class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    write(const T &enumiration, const size_t max_value) {
        write((const void*)&enumiration, log2(max_value) - 1);
    }

    template<class T>
    void write(const std::vector<T>& data) {
        const size_t count = data.size();
        write(count);
        for (const auto& item : data) {
            write(item);
        }
    }

    template<class Key, class Value>
    void write(const std::unordered_map<Key, Value> data) {
        const size_t count = data.size();
        write(count);
        for (const auto& record : data) {
            write(record.first);
            write(record.second);
        }
    }

    template <class T> void serialize(const T &data) { write(data); }
    template<int Max, class T> void serialize(const T &data) { write<Max, T>(data); }
    template <class T>
    typename std::enable_if<std::is_enum<T>::value>::type
    serialize(const T &data, const size_t max_value) { write<T>(data, max_value); }

  private:
    size_t head = 0;
    std::vector<uint8_t> buffer;

    void resize(size_t new_size) {
        std::vector<uint8_t> newBuffer;
        newBuffer.resize(new_size);
        std::copy(buffer.begin(), buffer.end(), newBuffer.begin());
        buffer.swap(newBuffer);
    }

    void write(uint8_t data, size_t count) {
        const size_t byteOffset = head / 8;
        const size_t bitOffset = head % 8;

        head += count;

        if (head > size() * 8) {
            resize(std::max((buffer.size() + 7) * 2, (head + 7) / 8));
        }

        const uint8_t mask = ~(0xff << bitOffset);
        buffer[byteOffset] = (buffer[byteOffset] & mask) | (data << bitOffset);

        const size_t unusedBits = 8 - bitOffset;
        if (unusedBits < count) {
            buffer[byteOffset + 1] = data >> unusedBits;
        }
    }
};