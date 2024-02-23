#pragma once

#include <stdint.h>
#include <memory>
#include <random>
#include <static_constructor.h>

class RandNumber {
public:
    inline static std::unique_ptr<RandNumber> instance;

    static void static_init() {
        static_constructor<&RandNumber::static_init>::c.run();
        instance = std::make_unique<RandNumber>();
        std::random_device device;
        instance->generator.seed(device());
    }

    void seed(uint32_t seed) { generator.seed(seed); }

    float getFloat() { return floatDistribution(generator); }
    template <class T, class TMin, class TMax>
    T get(const TMin& min, const TMax& max) {
        std::uniform_int_distribution distribution((T)min, (T)max);
        return distribution(generator);
    }

private:
    std::mt19937 generator;
    std::uniform_real_distribution<float> floatDistribution{0.f, 1.f};
};
