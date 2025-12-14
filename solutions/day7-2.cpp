import std;
import Utils;
import getInput;

using namespace std::literals;

enum class Cell : long long
{
    Empty = 0,
    Splitter = -1,
    Start = -2
};

auto isEmpty(Cell c) { return static_cast<long long>(c) >= 0; }
auto getBeam(Cell c) { return c == Cell::Start ? 1 : static_cast<long long>(c); }
auto addBeam(Cell& c, long long beam) { c = static_cast<Cell>(static_cast<long long>(c) + beam); }

constexpr auto readCell(char c)
{
    switch (c)
    {
    case '^':
        return Cell::Splitter;
    case 'S':
        return Cell::Start;
    default:
        return Cell::Empty;
    }
}

struct Diagram
{
    std::vector<std::vector<Cell>> m_data;

    explicit Diagram(std::ranges::input_range auto&& range)
        requires std::same_as<std::ranges::range_value_t<decltype(range)>, std::string>
        : m_data(range | std::views::transform(std::views::transform(readCell))
                 | std::ranges::to<decltype(m_data)>())
    {
    }

    auto sendBeam()
    {
        for (auto i : std::views::iota(0uz, m_data.size() - 1))
        {
            if (m_data[i].size() != m_data[i + 1].size())
            {
                throw std::runtime_error("Input lines have different length");
            }
            for (auto j : std::views::iota(0uz, m_data[i].size()))
            {
                if (auto beam = getBeam(m_data[i][j]); beam > 0)
                {
                    if (isEmpty(m_data[i + 1][j]))
                    {
                        addBeam(m_data[i + 1][j], beam);
                    }
                    else if (m_data[i + 1][j] == Cell::Splitter)
                    {
                        if (j >= 1 && isEmpty(m_data[i + 1][j - 1]))
                        {
                            addBeam(m_data[i + 1][j - 1], beam);
                        }
                        if (j + 1 < m_data[i + 1].size() && isEmpty(m_data[i + 1][j + 1]))
                        {
                            addBeam(m_data[i + 1][j + 1], beam);
                        }
                    }
                }
            }
        }
        return std::ranges::fold_left(m_data.back() | std::views::transform(getBeam), 0,
                                      std::plus<>{});
    }
};

int main()
{
    auto input = getInput("7"sv);

    auto diagram = Diagram(Utils::readLines(input));
    auto splitCount = diagram.sendBeam();
    std::println("{} timelines", splitCount);
}