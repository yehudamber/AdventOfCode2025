import std;
import Utils;
import getInput;

using namespace std::literals;

using Point = std::array<std::uint64_t, 2>;

constexpr auto rectangleArea(const Point& p1, const Point& p2)
{
    auto length = [](const auto& pair)
    {
        const auto& [a, b] = pair;
        return a >= b ? a - b + 1 : b - a + 1;
    };
    return std::ranges::fold_left(std::views::zip(p1, p2) | std::views::transform(length), 1,
                                  std::multiplies<>{});
}

auto parsePoint(const std::string& line)
{
    auto coordinates = std::views::split(line, ',')
        | std::views::transform(
                           [](auto view)
                           {
                               Point::value_type c;
                               if (!std::from_chars(view.data(), view.data() + view.size(), c))
                               {
                                   throw std::runtime_error("Couldn't parse coordinate from input");
                               }
                               return c;
                           })
        | std::ranges::to<std::vector>();
    if (coordinates.size() != std::tuple_size_v<Point>)
    {
        throw std::runtime_error("Point has wrong number of coordinates");
    }
    return [&coordinates]<std::size_t... Is>(std::index_sequence<Is...>)
    { return Point{ coordinates[Is]... }; }(std::make_index_sequence<std::tuple_size_v<Point>>{});
}

auto generatePairs(auto begin, auto end)
{
    return std::views::iota(begin, end)
        | std::views::transform(
               [end](auto current)
               {
                   return std::views::iota(current + 1, end)
                       | std::views::transform([current](auto current2)
                                               { return std::pair(current, current2); });
               })
        | std::views::join;
}

int main()
{
    auto input = getInput("9"sv);

    auto points = Utils::readLines(input) | std::views::transform(parsePoint)
        | std::ranges::to<std::vector>();
    std::println(
        "{}",
        std::ranges::max(generatePairs(0uz, points.size())
                         | std::views::transform(
                             [&points](const auto& pair)
                             { return rectangleArea(points[pair.first], points[pair.second]); })));
}