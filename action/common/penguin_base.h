#pragma once

#include "object.h"

struct Point2 {
  float x = 0.f;
  float y = 0.f;
};

template<>
struct is_serializable<Point2> {
  static constexpr bool value = true;
};

class PenguinBase : public Object {
  public:
    inline static ObjTypeId type = ObjTypeId::next();

    enum Fields {
      Position = 1 << 0,
      All = Position
    };

    Point2 position;

    PenguinBase() = default;

    ObjTypeId getType() const override { return type; }
    ObjFields getFields() const override { return Fields::All; }

    ObjFields write(outstream &stream, const ObjFields fields) override {
      ObjFields writtenFields;

      if (fields & Fields::Position) {
        stream.write(true);
        stream.write(position);
        writtenFields |= Fields::Position;
      } else
        stream.write(true);

      return writtenFields;
    }

};
