import std;
import getInput;

using namespace std::literals;

auto readCommaSeperated(std::istream& input)
{
    return std::views::istream<char>(input) | std::views::lazy_split(',');
}

constexpr auto parseIdRanges()
{
    return std::views::transform(
        [](std::ranges::input_range auto&& chars)
            requires std::same_as<std::ranges::range_value_t<decltype(chars)>, char>
        {
            const auto end = std::ranges::end(chars);
            auto it = std::ranges::begin(chars);
            auto start = 0ll;
            for (; it != end && *it != '-'; ++it)
            {
                start = start * 10 + (*it - '0');
            }
            if (it == end || ++it == end)
            {
                throw std::runtime_error("Invalid range: missing '-' or end");
            }
            auto finish = 0ll;
            for (; it != end; ++it)
            {
                finish = finish * 10 + (*it - '0');
            }
            return std::views::iota(start, finish + 1);
        });
}

auto filterInvalidIds()
{
    return std::views::filter(
        [](auto id)
        {
            auto digits = std::to_string(id);
            if (digits.length() % 2 != 0)
            {
                return false;
            }
            return std::ranges::all_of(
                std::views::zip(std::string_view(digits).substr(0, digits.length() / 2),
                                std::string_view(digits).substr(digits.length() / 2)),
                [](auto pair) { return std::get<0>(pair) == std::get<1>(pair); });
        });
}

int main()
{
    auto input = getInput("2"sv);

    // For some reason, applying std::views::join directly removes the last range
    auto idRanges = readCommaSeperated(input) | parseIdRanges() | std::ranges::to<std::vector>();
    std::println(
        "{}",
        std::ranges::fold_left(idRanges | std::views::join | filterInvalidIds(), 0ll, std::plus{}));
}