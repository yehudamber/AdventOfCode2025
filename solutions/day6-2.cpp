import std;
import Utils;
import getInput;

using namespace std::literals;

template <typename Value> auto isAllEqual(Value&& value, auto&& range)
{
    return std::ranges::all_of(range, [&value](auto&& element) { return element == value; });
}

template <std::regular Value> auto splitByAllEqual(Value value)
{
    return std::views::chunk_by(
        [value]<std::ranges::forward_range R>
            requires std::equality_comparable_with<std::ranges::range_reference_t<R>, Value>
        (R&& lhs, auto&&) { return !isAllEqual(value, lhs); })
        | std::views::transform(
            std::views::filter([value](auto&& range) { return !isAllEqual(value, range); }));
}

auto findOperation(std::ranges::input_range auto&& lastRow)
    requires std::same_as<std::ranges::range_value_t<decltype(lastRow)>, char>
{
    auto it = std::ranges::find_if(lastRow, [](char c) { return c != ' '; });
    if (it == std::ranges::end(lastRow))
    {
        throw std::runtime_error("No operation in problems last row");
    }
    return *it;
}

auto toNumber(std::input_iterator auto begin, std::sentinel_for<decltype(begin)> auto end)
    requires std::same_as<std::iter_value_t<decltype(begin)>, char>
{
    auto res = 0;
    for (; begin != end; ++begin)
    {
        if (std::isdigit(*begin))
        {
            res *= 10;
            res += *begin - '0';
        }
    }
    return res;
}

auto calculate()
{
    return []<std::ranges::forward_range R>
        requires std::ranges::bidirectional_range<std::ranges::range_reference_t<R>>
        && std::ranges::common_range<std::ranges::range_reference_t<R>>
        && std::same_as<std::ranges::range_value_t<std::ranges::range_reference_t<R>>, char>
    (R&& columns)
    {
        auto operation
            = findOperation(std::views::transform(columns, [](auto&& c) { return c.back(); }));
        auto numbers = std::views::transform(
            columns, [](auto&& c)
            { return toNumber(std::ranges::begin(c), std::ranges::prev(std::ranges::end(c))); });
        return operation == '*'
            ? std::ranges::fold_left(std::move(numbers), 1ll, std::multiplies<>{})
            : std::ranges::fold_left(std::move(numbers), 0ll, std::plus<>{});
    };
}

int main()
{
    auto input = getInput("6"sv);

    const auto lines = Utils::readLines(input) | std::ranges::to<std::vector>();
    if (lines.size() < 2)
    {
        std::println(std::cerr, "Input must have at least 2 lines");
        return 1;
    }

    const auto width = lines.front().size();
    if (std::ranges::any_of(lines, [width](auto n) { return n != width; }, std::ranges::size))
    {
        std::println(std::cerr, "All input lines must have the same length");
        return 1;
    }

    std::println("Grand total: {}",
                 std::ranges::fold_left(lines | std::views::take(lines.size())
                                            | Utils::rotate(width) | splitByAllEqual(' ')
                                            | std::views::transform(calculate()),
                                        0ll, std::plus<>{}));
}