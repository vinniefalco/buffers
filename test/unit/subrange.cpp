//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/subrange.hpp>

#include <boost/buffers/const_buffer_pair.hpp>
#include <boost/buffers/copy.hpp>
#include <boost/buffers/make_buffer.hpp>
#include <boost/buffers/size.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/static_assert.hpp>

#include <array>

#include "test_helpers.hpp"
#include "test_suite.hpp"

namespace boost {
namespace buffers {

template<
    std::size_t I,
    std::size_t N>
void
set(
    std::string&,
    std::array<const_buffer, N>&)
{
}

template<
    std::size_t I,
    std::size_t N,
    class... Args>
void
set(
    std::string& s,
    std::array<const_buffer, N>& v,
    char const* p,
    Args const&... args)
{
    core::string_view sv(p);
    v[I] = const_buffer(sv.data(), sv.size());
    s.append(sv.data(), sv.size());
    set<I+1>(s, v, args...);
}

auto
make_buffers(
    std::string&) ->
    std::array<const_buffer, 0>
{
    return {};
}

template<
    class... Args>
auto
make_buffers(
    std::string& s,
    char const* arg0,
    Args const&... args) ->
    std::array<const_buffer, 1 + sizeof...(Args)>
{
    s = {};
    std::array<const_buffer, 1 + sizeof...(Args)> v;
    set<0>(s, v, arg0, args...);
    return v;
}

struct consuming_buffer_test
{
    static
    void
    checkStatic()
    {
        using T = subrange<const_buffer_pair>;

        BOOST_STATIC_ASSERT(std::is_default_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_copy_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_move_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_copy_assignable<T>::value);
        BOOST_STATIC_ASSERT(std::is_move_assignable<T>::value);

        using U = T::const_iterator;

        BOOST_STATIC_ASSERT(std::is_default_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_copy_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_move_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_copy_assignable<U>::value);
        BOOST_STATIC_ASSERT(std::is_move_assignable<U>::value);
    }

    template<class B>
    static
    void
    check(
        B const& b,
        core::string_view s)
    {
        auto constexpr M = 1024;
        char buf[M];
        if(! BOOST_TEST_LE(size(b), M))
            return;
        if(! BOOST_TEST_EQ(size(b), s.size()))
            return;
        auto const n = copy(
            mutable_buffer(buf, M), b);
        if(! BOOST_TEST_EQ(n, s.size()))
            return;
        if(! BOOST_TEST_EQ(core::string_view(buf, n), s))
            return;
    }

    void
    run()
    {
        std::string s;
        auto bs =  make_buffers(s, "boost.", "buffers.", "subrange");
        using BS = std::array<const_buffer, 3>;
        check(bs, s);
        for(std::size_t n = 0; n < s.size(); ++n)
        {
            subrange<BS> cb(bs);
            //cb.consume(n);
            //auto cb1 = subrange_of(cb, how::sans_prefix, n);
            cb = subrange_of(cb, how::sans_prefix, n);
            check(cb, s.substr(n));
        }
    }
};

TEST_SUITE(
    consuming_buffer_test,
    "boost.buffers.subrange");

} // buffers
} // boost
