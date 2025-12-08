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
            auto length = static_cast<int>(std::floor(std::log10(id))) + 1;
            for (auto div = 1; div * 2 <= length; ++div)
            {
                if (length % div == 0
                    && id % static_cast<int>((std::pow(10, length) - 1) / (std::pow(10, div) - 1))
                        == 0)
                {
                    return true;
                }
            }
            return false;
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