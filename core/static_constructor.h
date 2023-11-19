#pragma once

template<void(*ctr_function)()>
struct static_constructor {
    struct constructor {
        constructor() { ctr_function(); }
        constexpr static void run(){};
    };
    static constructor c;
};

template<void(*ctr_function)()>
typename static_constructor<ctr_function>::constructor static_constructor<ctr_function>::c;
