import std;
import Utils;
import getInput;

using namespace std::literals;

auto parseLine(const std::string& line)
{
    if (line.length() < 2)
    {
        throw std::runtime_error("Line too short");
    }

    auto direction = line[0] == 'R' ? 1 : -1;
    auto distance = 0;
    if (!std::from_chars(line.data() + 1, line.data() + line.size(), distance))
    {
        throw std::runtime_error("Failed to parse distance");
    }
    return direction * distance;
}

int main()
{
    auto input = getInput("1"sv);

    static constexpr auto N = 100;
    auto result = 0;
    for (auto pos = 50; auto move : Utils::readLines(input) | std::views::transform(parseLine))
    {
        auto nextPos = pos + move;
        result += (move >= 0 ? nextPos : pos == 0 ? -nextPos : N - nextPos) / N;
        nextPos %= N;
        pos = nextPos >= 0 ? nextPos : nextPos + N;
    }

    std::println("{}", result);
}