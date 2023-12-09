#pragma once

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
