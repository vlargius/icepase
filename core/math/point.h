#pragma once

struct Point2 {
  float x = 0.f;
  float y = 0.f;
};

template<>
struct is_serializable<Point2> {
  static constexpr bool value = true;
};