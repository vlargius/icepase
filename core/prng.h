#pragma once

#include <stdint.h>
#include <random>

class Prng {
  public:
    static Prng &get() {
        static Prng instance;
        std::random_device rd;
        instance.generator.seed(rd());
        return instance;
    }

    void seed(size_t seed) { generator.seed(seed); }

    std::mt19937 &getGenerator() { return generator; }

    float getFloat() {
        std::uniform_real_distribution<uint32_t> distr(0.f, 1.f);
        return distr(generator);
    }

    uint32_t getUInt(uint32_t min_value, uint32_t max_value) {
        std::uniform_int_distribution<uint32_t> distr(min_value, max_value);
        return distr(generator);
    }
    int32_t getInt(int32_t min_value, int32_t max_value) {
        std::uniform_int_distribution<int32_t> distr(min_value, max_value);
        return distr(generator);
    }
    // Vector3 getVector(const Vector3 &inMin, const Vector3 &inMax);

  private:
    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;

    Prng() = default;
};