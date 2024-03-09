#pragma once

#include <memory>

namespace delivery {

class Notification;

class TransmissionData {
  public:
    using ptr = std::shared_ptr<TransmissionData>;
    virtual void fail(Notification *notification) const = 0;
    virtual void success() const = 0;
};
}