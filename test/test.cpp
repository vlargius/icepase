
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "instream.h"
#include "outstream.h"

const char *BLACK = "\033[30m";
const char *RED = "\033[31m";
const char *GREEN = "\033[32m";
struct Color {
    std::string color;
    template <class T> std::string operator()(const T &data) {
        std::stringstream ss;
        ss << color << data << BLACK;
        return ss.str();
    }
} black{BLACK}, red{RED}, green{GREEN};

#include "stream_test.h"
#include "object_test.h"

#include "linker.h"
#include "client/penguin.h"

void linker_test() {
    Object::ptr obj1 = Object::Factory::create<client::Penguin>();
    Object::ptr obj2 = Object::Factory::create<client::Penguin>();
    Object::ptr obj3 = Object::Factory::create<client::Penguin>();

    Linker linker;

    NetId id1 = linker.get(obj1, true);
    assert(id1 == linker.get(obj1));

    linker.add(obj2, NetId::next());
    linker.add(obj3, NetId::next());
    NetId id2 = linker.get(obj2, true);
    NetId id3 = linker.get(obj3, true);
    assert(id2 == linker.get(obj2));
    assert(id3 == linker.get(obj3));

    linker.remove(obj1);
    linker.remove(obj2);
    assert(id3 == linker.get(obj3));
}



#include "replication_in.h"
#include "replication_out.h"

struct Print {
    static RpcId type;

    std::string str;
    int k;

    static void init() {
        static_constructor<&Print::init>::c.run();
        type = RpcId::next();
        RpcRegistry::add<Print>();
    }

    template <class Stream> Print &serialize(Stream &stream) {
        stream.serialize(str);
        stream.serialize(k);
        return *this;
    }

    static void create(instream &stream) { Print().serialize(stream).function(); }

    void function() const { std::cout << str << " " << k << std::endl; }
};
RpcId Print::type;

void replication_test() {
    using namespace client;

    NetId netId;
    Penguin::ptr penguin = Object::Factory::create<Penguin>();
    Point2 position = Point2{666, -111};
    penguin->position = position;
    Object::ptr object = penguin;
    {
        outstream out;
        Linker serverLinker;
        {
            netId = serverLinker.get(object, true);
            replication::Output replication;
            replication.create(netId, object->getFields());
            replication.process(out, serverLinker);
        }
        Linker clientLinker;
        {
            instream in(out.data(), out.size());
            replication::Input replication;
            replication.process(in, clientLinker);
            assert(clientLinker.get(netId));
            Penguin::ptr clientPenguin = std::dynamic_pointer_cast<Penguin>(clientLinker.get(netId));
            assert(position.x == clientPenguin->position.x && position.y == clientPenguin->position.y);
            out.reset();
        }
        {
            position = Point2{77, 13};
            penguin->position = position;
            replication::Output replication;
            replication.markField(netId, Penguin::All);
            replication.process(out, serverLinker);
        }
        {
            instream in(out.data(), out.size());
            replication::Input replication;
            replication.process(in, clientLinker);
            Penguin::ptr clientPenguin = std::dynamic_pointer_cast<Penguin>(clientLinker.get(netId));
            assert(position.x == clientPenguin->position.x && position.y == clientPenguin->position.y);
            out.reset();
        }
        {
            Linker serverLinker;
            replication::Output replication;
            replication.destroy(netId);
            replication.process(out, serverLinker);
        }
        {
            instream in(out.data(), out.size());
            replication::Input replication;
            replication.process(in, clientLinker);
            assert(clientLinker.get(netId) == nullptr);
        }
    }
    {
        outstream out;
        {
            replication::Output replication;
            replication.call<Print>(out, "hello, iceworld!", 666);
        }
        {
            instream in(out.data(), out.size());
            Linker linker;
            replication::Input replication;
            replication.process(in, linker);
        }
    }
}

#include "netutils.h"

#include "client/network.h"
#include "server/network.h"

void network_test() {
    net::address::ptr serverAddress = net::make_address("localhost:6666");
    server::Network::get().init(*serverAddress);

    assert(client::Network::get().is<client::Network::State::Uninitialized>());
    std::string clientName = "test_client";
    client::Network::get().init(*serverAddress, clientName);
    assert(client::Network::get().is<client::Network::State::Joining>());

    while (!client::Network::get().is<client::Network::State::Joined>()) {
        client::Network::get().processOutput();
        server::Network::get().processInput();
        client::Network::get().processInput();
    }

    MoveList &clientMoves = Input::get().moveList;
    clientMoves.add(InputState{}, timing::current());

    client::Network::get().processOutput();
    server::Network::get().processInput();

    MoveList &serverMoves = server::Network::get().getClient(client::Network::get().getUserId())->unprocessedMoves;
    assert(serverMoves.size() == 1);
}

int main() {
    stream_test();
    linker_test();
    creation_registry_test();
    replication_test();
    network_test();

    std::cout << green("success!") << std::endl;
    return 0;
}