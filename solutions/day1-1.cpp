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

template <int N> auto plusModN()
{
    return [](auto a, auto b) { return (a + b + N) % N; };
}

int main()
{
    auto input = getInput("1"sv);
    std::println("{}",
                 std::ranges::count(Utils::readLines(input) | std::views::transform(parseLine)
                                        | Utils::partialSums(50, plusModN<100>()),
                                    0));
}