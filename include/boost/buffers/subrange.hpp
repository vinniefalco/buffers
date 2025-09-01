//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SUBRANGE_HPP
#define BOOST_BUFFERS_SUBRANGE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/range.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/assert.hpp>
#include <iterator>

namespace boost {
namespace buffers {

enum class how
{
    prefix,
    suffix,
    sans_prefix,
    sans_suffix
};

/** subrange tag for tag_invoke.
*/
struct subrange_tag {};

/** Alias for the type of a subrange of a buffer sequence
*/
template<class T>
using subrange_type = decltype(
    tag_invoke(
        std::declval<subrange_tag const&>(),
        std::declval<T const&>(),
        std::declval<how>(),
        std::declval<std::size_t>()));

/** Return the subrange of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    constexpr auto
    operator()(
        BufferSequence const& bs,
        how h,
        std::size_t n) const ->
            subrange_type<BufferSequence>
    {
        return tag_invoke(
            subrange_tag{}, bs, h, n);
    }
} const subrange_of{};

//------------------------------------------------

/** A wrapper enabling a buffer sequence to be consumed
*/
template<class BufferSequence>
class subrange
{
    //using iter_type = decltype(
        //std::declval<BufferSequence&>().begin());
    using iter_type = typename
        BufferSequence::const_iterator;

    BufferSequence bs_;
    iter_type begin_;
    iter_type end_;
    std::size_t len_ = 0;       // length of bs_
    std::size_t size_ = 0;      // total bytes
    std::size_t prefix_ = 0;    // used prefix bytes
    std::size_t suffix_ = 0;    // used suffix bytes

    // If you get a compile error here it
    // means that your type does not meet
    // the requirements.
    static_assert(
        is_const_buffer_sequence<BufferSequence>::value,
        "Type requirements not met.");

public:
    /** The type of values returned by iterators
    */
    using value_type = typename std::conditional<
        is_mutable_buffer_sequence<BufferSequence>::value,
        mutable_buffer, const_buffer>::type;

    /** The type of returned iterators
    */
    class const_iterator;

    /** Constructor
    */
    subrange() = default;

    /** Constructor
    */
    subrange(
        BufferSequence const& bs)
        : bs_(bs)
        , begin_(buffers::begin(bs_))
        , end_(buffers::end(bs_))
    {
        auto it = begin_;
        while(it != end_)
        {
            value_type b(*it);
            size_ += b.size();
            ++len_;
            ++it;
        }
    }

    /** Return an iterator to the beginning of the sequence
    */
    const_iterator
    begin() const noexcept;

    /** Return an iterator to the end of the sequence
    */
    const_iterator
    end() const noexcept;

    /** Remove a prefix of bytes from the sequence
    */
    void
    consume(
        std::size_t n)
    {
        // nice hack to simplify the loop (M. Nejati)
        n += prefix_;
        size_ += prefix_;
        prefix_ = 0;

        while(n > 0 && len_ > 1)
        {
            auto n1 = (*begin_).size();
            if(n < n1)
            {
                prefix_ = n;
                size_ -= n;
                return;
            }
            size_ -= n1;
            ++begin_;
            --len_;
            n -= n1;
        }
        if(n > 0 && len_ == 1)
        {
            auto n1 = (*begin_).size();
            n1 -= suffix_;
            if(n < n1)
            {
                prefix_ = n;
                size_ -= n;
                return;
            }
            begin_ = end_;
            len_ = 0;
            size_ = 0;
        }
    }

#if 0
    friend
    std::size_t
    tag_invoke(
        size_tag const&,
        subrange<BufferSequence> const& bs) noexcept
    {
        return bs.size_ - (bs.prefix_ + bs.suffix_);
    }
#endif

    friend
    subrange<BufferSequence>
    tag_invoke(
        subrange_tag const&,
        subrange<BufferSequence> const& bs,
        how h, std::size_t n)
    {
        auto bs_ = bs;
        return bs.subrange_impl(h, n);
    }

private:
    void
    consume_front(
        std::size_t n)
    {
        // nice hack to simplify the loop (M. Nejati)
        n += prefix_;
        size_ += prefix_;
        prefix_ = 0;

        auto it = begin_;
        while(n > 0 && it != end_)
        {
            value_type b = *it;
            if(n < b.size())
            {
                prefix_ = n;
                size_ -= n;
                break;
            }
            n -= b.size();
            size_ -= b.size();
            ++it;
            --len_;
        }
        begin_ = it;
    }

    void
    keep_front(
        std::size_t n)
    {
    }

    void
    consume_back(
        std::size_t n)
    {
    }

    void
    keep_back(
        std::size_t n)
    {
    }

    subrange
    subrange_impl(
        how h,
        std::size_t n) const
    {
        switch(h)
        {
        case how::prefix:
        {
            break;
        }
        case how::suffix:
        {
            break;
        }
        case how::sans_prefix:
        {
            subrange result = *this;
            result.consume(n);
            return result;
        }
        default:
            break;
        }
        return *this;
    }
};

//------------------------------------------------

template<class BufferSequence>
class subrange<BufferSequence>::
    const_iterator
{
    using iter_type = typename
        subrange::iter_type;

    iter_type it_;
    // VFALCO we could just point back to
    // the original sequence to save size
    std::size_t prefix_ = 0;
    std::size_t suffix_ = 0;
    std::size_t i_ = 0;
    std::size_t n_ = 0;

    friend class subrange<BufferSequence>;

    const_iterator(
        iter_type it,
        std::size_t prefix__,
        std::size_t suffix__,
        std::size_t i,
        std::size_t n) noexcept
        : it_(it)
        , prefix_(prefix__)
        , suffix_(suffix__)
        , i_(i)
        , n_(n)
    {
        // n_ is the index of the end iterator
    }

public:
    using value_type = typename
        subrange::value_type;
    using reference = value_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    const_iterator() = default;

    bool
    operator==(
        const_iterator const& other) const noexcept
    {
        return
            it_     == other.it_ &&
            prefix_ == other.prefix_ &&
            suffix_ == other.suffix_ &&
            i_      == other.i_ &&
            n_      == other.n_;
    }

    bool
    operator!=(
        const_iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    reference
    operator*() const noexcept
    {
        value_type v = *it_;
        using P = typename std::conditional<
            is_mutable_buffer_sequence<BufferSequence>::value,
            char*, char const*>::type;
        auto p = reinterpret_cast<P>(v.data());
        auto n = v.size();
        if(i_ == 0)
        {
            p += prefix_;
            n -= prefix_;
        }
        if(i_ == n_ - 1)
            n -= suffix_;
        return value_type(p, n);
    }

    const_iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(i_ < n_);
        ++it_;
        ++i_;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    const_iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(i_ > 0);
        --it_;
        --i_;
        return *this;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto temp = *this;
        --(*this);
        return temp;
    }
};

//------------------------------------------------

template<class BufferSequence>
auto
subrange<BufferSequence>::
begin() const noexcept ->
    const_iterator
{
    return const_iterator(
        this->begin_, prefix_, suffix_, 0, len_);
}

template<class BufferSequence>
auto
subrange<BufferSequence>::
end() const noexcept ->
    const_iterator
{
    return const_iterator(
        this->end_, prefix_, suffix_, len_, len_);
}

} // buffers
} // boost

#endif
