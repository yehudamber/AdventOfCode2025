import std;
import Utils;
import getInput;

using namespace std::literals;

auto splitColumns()
{
    return std::views::transform(
        [](auto&& line)
        {
            return std::views::split(line, ' ')
                | std::views::filter([](auto&& subrange) { return !std::ranges::empty(subrange); })
                | std::ranges::to<std::vector<std::string>>();
        });
}

auto calculate(auto&& column)
{
    auto impl = [&column](auto f, auto init)
    {
        return std::ranges::fold_left(
            column | std::views::take(std::ranges::size(column) - 1)
                | std::views::transform([](const auto& str) { return std::stoi(str); }),
            init, f);
    };
    return column.back() == "*"sv ? impl(std::multiplies<>{}, 1ll) : impl(std::plus<>{}, 0ll);
}

int main()
{
    auto input = getInput("6"sv);

    auto lines = std::ranges::to<std::vector>(Utils::readLines(input) | splitColumns());
    std::println(
        "{}",
        std::ranges::fold_left(
            std::views::iota(0uz, lines.front().size())
                | std::views::transform(
                    [&lines = std::as_const(lines)](auto columnIndex)
                    {
                        return calculate(
                            lines
                            | std::views::transform(
                                [columnIndex](auto&& line)
                                { return std::forward<decltype(line)>(line)[columnIndex]; }));
                    }),
            0ll, std::plus<>{}));
}