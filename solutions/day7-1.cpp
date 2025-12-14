import std;
import Utils;
import getInput;

using namespace std::literals;

enum class Cell
{
    Empty,
    Splitter,
    HitSplitter,
    Start,
    Beam
};

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

constexpr auto toString(Cell c)
{
    switch (c)
    {
    case Cell::Splitter:
        return "\033[1m^\033[0m"sv;
    case Cell::HitSplitter:
        return "\033[1;32m^\033[0m"sv;
    case Cell::Start:
        return "\033[1;32mS\033[0m"sv;
    case Cell::Beam:
        return "\033[34m|\033[0m"sv;
    default:
        return "."sv;
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

    void print() const
    {
        for (const auto& line : m_data)
        {
            for (auto cell : line)
            {
                std::print("{}", toString(cell));
            }
            std::println();
        }
    }

    auto sendBeam()
    {
        auto splitCount = 0;
        for (auto i : std::views::iota(0uz, m_data.size() - 1))
        {
            if (m_data[i].size() != m_data[i + 1].size())
            {
                throw std::runtime_error("Input lines have different length");
            }
            for (auto j : std::views::iota(0uz, m_data[i].size()))
            {
                if (m_data[i][j] == Cell::Start || m_data[i][j] == Cell::Beam)
                {
                    if (m_data[i + 1][j] == Cell::Empty)
                    {
                        m_data[i + 1][j] = Cell::Beam;
                    }
                    else if (m_data[i + 1][j] == Cell::Splitter)
                    {
                        m_data[i + 1][j] = Cell::HitSplitter;
                        if (j >= 1 && m_data[i + 1][j - 1] == Cell::Empty)
                        {
                            m_data[i + 1][j - 1] = Cell::Beam;
                        }
                        if (j + 1 < m_data[i + 1].size() && m_data[i + 1][j + 1] == Cell::Empty)
                        {
                            m_data[i + 1][j + 1] = Cell::Beam;
                        }
                        ++splitCount;
                    }
                }
            }
        }
        return splitCount;
    }
};

int main()
{
    auto input = getInput("7"sv);

    auto diagram = Diagram(Utils::readLines(input));
    auto splitCount = diagram.sendBeam();
    diagram.print();
    std::println("Splitted {} times", splitCount);
}