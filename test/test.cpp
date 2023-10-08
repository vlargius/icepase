
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

void stream_test() {

    outstream out;

    bool b_out = true;
    out.write(b_out);

    int i_out = 66666666;
    out.write(i_out);

    bool b1_out = false;
    out.write(b1_out);

    float f_out = -123456789.12345;
    out.write(f_out);

    enum class TestEnum { A, B, C, f, fgdfh, gfhfg, wetw, Count } t_out = TestEnum::B;
    out.write<int(TestEnum::Count)>(t_out);

    bool b2_out = false;
    out.write(b2_out);

    bool b3_out = true;
    out.write(b3_out);

    std::vector<int> v_out{1, 2, 3, 3, 2, 1, 0, -666};
    out.write(v_out);

    std::unordered_map<int, int> m_out;
    m_out[123] = 321;
    m_out[666] = 777;
    m_out[555] = 333;
    out.write(m_out);

    instream in(out.data(), out.size());

    bool b_in = false;
    in.read(b_in);

    int i_in = 0;
    in.read(i_in);

    bool b1_in = false;
    in.read(b1_in);

    float f_in = 0;
    in.read(f_in);

    TestEnum t_in = TestEnum::C;
    in.read<(int) TestEnum::Count>(t_in);

    bool b2_in = false;
    in.read(b2_in);

    bool b3_in = false;
    in.read(b3_in);

    std::vector<int> v_in;
    in.read(v_in);

    std::unordered_map<int, int> m_in;
    in.read(m_in);

    assert(b_in == b_out);
    assert(b1_in == b1_out);
    assert(b2_in == b2_out);
    assert(b3_in == b3_out);
    assert(f_in == f_out);
    assert(t_in == t_out);
    assert(i_in == i_out);

    assert(v_in.size() == v_out.size());
    for (size_t i = 0; i < v_in.size(); ++i) {
        assert(v_in[i] == v_out[i]);
    }

    assert(m_in.size() == m_out.size());
    for (const auto &record : m_in) {
        auto m_it = m_out.find(record.first);
        assert(m_it != m_out.end());
        assert(record.first == m_it->first);
        assert(record.second == m_it->second);
    }
}

#include "linker.h"

void linker_test() {
    Object::ptr obj1 = std::make_shared<Object>(ObjId::next());
    Object::ptr obj2 = std::make_shared<Object>(ObjId::next());
    Object::ptr obj3 = std::make_shared<Object>(ObjId::next());

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

void creation_registry_test() {
    Object::ptr obj = Object::Factory::create(Object::type);

    assert(obj->getType() == Object::type);
}

#include "replication_in.h"
#include "replication_out.h"

struct Print {
    static RpcId type;

    std::string str;
    int k;

    static void init() {
        static_constructor<&Print::init>::c();
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
    NetId netId;
    ObjId testId = ObjId::next();
    {
        outstream out;
        Linker serverLinker;
        {
            Object::ptr object = std::make_shared<Object>(testId);
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
            assert(clientLinker.get(netId)->getId() == testId);
            out.reset();
        }
        {
            replication::Output replication;
            replication.markField(netId, 1);
            replication.process(out, serverLinker);
        }
        {
            instream in(out.data(), out.size());
            replication::Input replication;
            replication.process(in, clientLinker);
            assert(clientLinker.get(netId)->getId() == testId);
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

    Input::get().moveList.add(InputState{}, timing::current());

    client::Network::get().processOutput();
    server::Network::get().processInput();

    const auto &moves = server::Network::get().getClient(client::Network::get().getPlayerId())->unprocessedMoves;
    assert(moves.size() == 1);
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