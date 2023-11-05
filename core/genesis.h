#pragma once

#include <unordered_map>


template<class IdType, class CreatorType>
class Genesis {
public:
    static Genesis& get() {
        static Genesis instance;
        return instance;
    }

    template<class T>
    static void add() {
        assert(T::type.valid());
        assert(get().type_creator.find(T::type) == get().type_creator.end());
        get().type_creator[T::type] = T::create;
    }

    template<class Base, class T>
    static void add() {
        assert(T::type.valid());
        assert(get().type_creator.find(T::type) == get().type_creator.end());
        get().type_creator[T::type] = T::base_create;
    }

    template<class ...Args>
    static auto create(IdType class_id, Args&... args) {
        auto funcIt = get().type_creator.find(class_id);
        assert(funcIt != get().type_creator.end());
        return funcIt->second(args...);
    }

    template<class T>
    static typename T::ptr create() {
        return T::create();
    }
private:
    Genesis() = default;

    std::unordered_map<IdType, CreatorType> type_creator;
};
