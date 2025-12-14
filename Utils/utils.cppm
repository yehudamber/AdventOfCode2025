export module Utils;

import std;

namespace Utils
{

// PartialSumsView
template <typename F, typename T, typename I, typename U>
concept PartialSumsOperatorImpl = std::movable<U> && std::convertible_to<const T&, U>
    && std::convertible_to<std::invoke_result_t<F&, const T&, std::iter_reference_t<I>>, U>
    && std::invocable<F&, U, std::iter_reference_t<I>>
    && std::assignable_from<U&, std::invoke_result_t<F&, U, std::iter_reference_t<I>>>;

template <typename F, typename T, typename I>
concept PartialSumsOperator
    = std::movable<F> && std::invocable<F&, const T&, std::iter_reference_t<I>>
    && PartialSumsOperatorImpl<
          F, T, I, std::decay_t<std::invoke_result_t<F&, const T&, std::iter_reference_t<I>>>>;

export template <std::ranges::input_range Underlying, std::movable T,
                 PartialSumsOperator<T, std::ranges::iterator_t<Underlying>> Add = std::plus<>>
    requires std::ranges::view<Underlying>
class PartialSumsView : public std::ranges::view_interface<PartialSumsView<Underlying, T, Add>>
{
    using UnderlyingIter = std::ranges::iterator_t<Underlying>;
    using UnderlyingSentinel = std::ranges::sentinel_t<Underlying>;
    using ValueType
        = std::decay_t<std::invoke_result_t<Add&, const T&, std::iter_reference_t<UnderlyingIter>>>;

    Underlying m_underlying;
    UnderlyingSentinel m_end;
    T m_init;
    Add m_add;

    class iterator
    {
        static constexpr auto isForwardIter
            = std::forward_iterator<UnderlyingIter> && std::semiregular<ValueType>;

    public:
        using value_type = ValueType;
        using difference_type = std::iter_difference_t<UnderlyingIter>;
        using iterator_concept
            = std::conditional_t<isForwardIter, std::forward_iterator_tag, std::input_iterator_tag>;

        constexpr iterator()
            requires std::default_initializable<UnderlyingIter>
            && std::default_initializable<value_type>
        = default;

        constexpr const value_type& operator*() const { return m_sum; }

        constexpr iterator& operator++()
        {
            if (m_underlying != m_parent->m_end)
            {
                m_sum = std::invoke(m_parent->m_add, std::move(m_sum), *m_underlying);
                ++m_underlying;
            }
            else
            {
                m_parent = nullptr;
            }
            return *this;
        }

        constexpr void operator++(int) { ++*this; }

        constexpr iterator operator++(int)
            requires isForwardIter
        {
            auto temp = *this;
            ++*this;
            return temp;
        }

        constexpr difference_type operator-(std::default_sentinel_t) const
            requires std::sized_sentinel_for<UnderlyingSentinel, UnderlyingIter>
        {
            return m_underlying - m_parent->m_end - 1;
        }

        friend constexpr difference_type operator-(std::default_sentinel_t, const iterator& it)
            requires std::sized_sentinel_for<UnderlyingSentinel, UnderlyingIter>
        {
            return -(it - std::default_sentinel);
        }

        friend constexpr bool operator==(const iterator& lhs, const iterator& rhs)
            requires isForwardIter
        {
            return lhs.m_underlying == rhs.m_underlying && lhs.m_parent == rhs.m_parent;
        }

        friend constexpr bool operator==(const iterator& it, std::default_sentinel_t)
        {
            return it.m_parent == nullptr;
        }

    private:
        UnderlyingIter m_underlying;
        value_type m_sum;
        PartialSumsView* m_parent = nullptr;

        constexpr iterator(UnderlyingIter underlying, const T& init, PartialSumsView* parent)
            : m_underlying(std::move(underlying)), m_sum(init), m_parent(parent)
        {
        }

        friend PartialSumsView;
    };

public:
    explicit constexpr PartialSumsView(Underlying underlying, T init, Add add = Add{})
        : m_underlying(std::move(underlying)), m_end(std::ranges::end(m_underlying)),
          m_init(std::move(init)), m_add(std::move(add))
    {
    }

    constexpr iterator begin() { return { std::ranges::begin(m_underlying), m_init, this }; }
    constexpr std::default_sentinel_t end() const { return {}; }

    constexpr auto size()
        requires std::ranges::sized_range<Underlying>
    {
        return std::ranges::size(m_underlying) + 1;
    }
};

template <typename T, typename Add>
class PartialSumsViewAdaptorClosure
    : public std::ranges::range_adaptor_closure<PartialSumsViewAdaptorClosure<T, Add>>
{
    T m_init;
    Add m_add;

public:
    constexpr PartialSumsViewAdaptorClosure(T init, Add add)
        : m_init(std::move(init)), m_add(std::move(add))
    {
    }
    constexpr auto operator()(std::ranges::viewable_range auto&& underlying)
        requires requires {
            PartialSumsView{ std::views::all(std::forward<decltype(underlying)>(underlying)),
                             std::move(m_init), std::move(m_add) };
        }
    {
        return PartialSumsView{ std::views::all(std::forward<decltype(underlying)>(underlying)),
                                std::move(m_init), std::move(m_add) };
    };
};

struct PartialSumsViewAdaptor
{
    template <std::ranges::input_range R, std::movable T,
              PartialSumsOperator<T, std::ranges::iterator_t<std::views::all_t<R>>> Add
              = std::plus<>>
        requires std::ranges::viewable_range<R>
    static constexpr auto operator()(R&& underlying, T init, Add add = Add{})
    {
        return PartialSumsView{ std::views::all(std::forward<R>(underlying)), std::move(init),
                                std::move(add) };
    }

    template <std::movable T, std::movable Add = std::plus<>>
    static constexpr auto operator()(T init, Add add = Add{})
    {

        return PartialSumsViewAdaptorClosure{ std::move(init), std::move(add) };
    }
};

export constexpr PartialSumsViewAdaptor partialSums;

// rotate
template <std::regular Size>
    requires requires { typename std::ranges::iota_view<Size, Size>; }
class Rotate : public std::ranges::range_adaptor_closure<Rotate<Size>>
{
    Size m_width;

public:
    constexpr explicit Rotate(Size width) : m_width(width) { }

    template <std::ranges::viewable_range R>
        requires std::ranges::forward_range<std::views::all_t<R>>
        && std::ranges::random_access_range<std::ranges::range_reference_t<std::views::all_t<R>>>
        && std::convertible_to<Size,
                               std::ranges::range_difference_t<
                                   std::ranges::range_reference_t<std::views::all_t<R>>>>
    constexpr auto operator()(R&& range) const
    {
        return std::views::iota(Size(), m_width)
            | std::views::transform(
                   [view = std::views::all(std::forward<R>(range))](Size index)
                   {
                       return std::ranges::subrange(view)
                           | std::views::transform(
                                  [index](auto&& row)
                                  {
                                      return *(std::ranges::begin(row)
                                               + static_cast<
                                                   std::ranges::range_difference_t<decltype(row)>>(
                                                   index));
                                  });
                   });
    }
};

struct RotateAdaptor
{
    template <typename R, typename Size>
        requires requires { typename Rotate<Size>; } && requires(Rotate<Size> rotate, R&& range) {
            { rotate(std::forward<R>(range)) };
        }
    static constexpr auto operator()(R&& range, Size width)
    {
        return Rotate<Size>(width)(std::forward<R>(range));
    }

    template <typename Size>
        requires requires { typename Rotate<Size>; }
    static constexpr auto operator()(Size width)
    {
        return Rotate<Size>(width);
    }
};

export constexpr RotateAdaptor rotate;

// readLines
template <typename Char, typename Traits> struct ReadLine
{
    std::basic_string<Char, Traits> m_line;

    friend auto& operator>>(std::basic_istream<Char, Traits>& istr, ReadLine& rl)
    {
        return std::getline(istr, rl.m_line);
    }
};

export template <typename Char, typename Traits>
auto readLines(std::basic_istream<Char, Traits>& istr)
{
    return std::ranges::istream_view<ReadLine<Char, Traits>>(istr)
        | std::views::transform(&ReadLine<Char, Traits>::m_line);
}

} // namespace Utils