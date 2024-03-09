#include "replication_out.h"

#include "replication_out.h"
#include "network.h"
#include "notification.h"

void replication::TransmissionData::fail(delivery::Notification *notification) const {
    for (const Transmission &transmission : transmissions) {
        switch (transmission.action) {
        case replication::Action::Create: {
            if (Object::ptr object = server::Network::get().linker.get(transmission.netId)) {
                out->create(transmission.netId, object->getFields());
            }
            break;
        }
        case replication::Action::Update: {
            ObjFields state = transmission.state;
            if (Object::ptr object = server::Network::get().linker.get(transmission.netId)) {
                for (const auto& packet : notification->getPackets()) {
                    const auto& data = std::static_pointer_cast<replication::TransmissionData>(packet.data);
                    for (const auto& dataRt : data->transmissions) {
                        state &= ~dataRt.state;
                    }
                }
                if (state)
                    out->markField(transmission.netId, state);
            }
            break;
        }
        case replication::Action::Destroy: {
            out->destroy(transmission.netId);
            break;
        }
        default:
            break;
        }
    }
}

void replication::TransmissionData::success() const {
    for (const Transmission &transmission : transmissions) {
        switch (transmission.action) {
        case replication::Action::Create: {
            out->createAck(transmission.netId);
            break;
        }
        case replication::Action::Destroy: {
            out->remove(transmission.netId);
            break;
        }
        default:
            break;
        }
    }
}