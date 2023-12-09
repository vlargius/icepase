#pragma once

#include "object.h"
#include "math/point.h"

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
