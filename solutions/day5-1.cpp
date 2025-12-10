import std;
import Utils;
import getInput;

using namespace std::literals;

auto parseRange(const std::string& str)
{
    auto separator = str.find('-');
    std::uint64_t begin;
    std::uint64_t end;
    if (separator == str.npos || !std::from_chars(str.data(), str.data() + separator, begin)
        || !std::from_chars(str.data() + separator + 1, str.data() + str.size(), end))
    {
        throw std::runtime_error("Invalid ID range");
    }
    return [begin, end](auto id) { return id >= begin && id <= end; };
}

int main()
{
    auto input = getInput("5"sv);

    auto idRanges = Utils::readLines(input)
        | std::views::take_while([](const auto& str) { return !str.empty(); })
        | std::views::transform(parseRange) | std::ranges::to<std::vector>();
    std::println("Found {} IDs",
                 std::ranges::count_if(std::views::istream<std::uint64_t>(input),
                                       [&idRanges](auto id)
                                       {
                                           return std::ranges::any_of(idRanges,
                                                                      [id](const auto& range)
                                                                      { return range(id); });
                                       }));
}