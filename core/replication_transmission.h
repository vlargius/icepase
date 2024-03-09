#pragma once

#include <vector>
#include <transmission_data.h>
#include <linker.h>
#include <replication_header.h>

namespace replication {

class Output;
class Notification;

class TransmissionData final : public delivery::TransmissionData {
  public:
    struct Transmission {
        NetId netId;
        Action action;
        ObjFields state;
    };

    TransmissionData() {};

    inline void addTransmission(NetId net_id, Action action_, ObjFields state_) {
        transmissions.emplace_back(Transmission{net_id, action_, state_});
    }

    void fail(delivery::Notification* notification) const override;
    void success() const override;

  private:
    Output *out = nullptr;
    std::vector<Transmission> transmissions;
};
}