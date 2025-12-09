import std;
import Utils;
import getInput;

using namespace std::literals;

enum class Cell
{
    Empty,
    PaperRoll,
    AccessiblePaperRoll
};

auto toString(Cell c)
{
    switch (c)
    {
    case Cell::PaperRoll:
        return "@"sv;
    case Cell::AccessiblePaperRoll:
        return "\033[1;32m@\033[0m"sv;
    default:
        return "."sv;
    }
}

struct Diagram
{
    std::vector<std::vector<Cell>> m_data;

    explicit Diagram(std::istream& input)
        : m_data(std::from_range,
                 Utils::readLines(input)
                     | std::views::transform(
                         std::views::transform([](char c)
                                               { return c == '@' ? Cell::PaperRoll : Cell::Empty; })
                         | std::ranges::to<std::vector>()))
    {
        if (m_data.empty())
        {
            throw std::runtime_error("Empty diagram");
        }
        if (!std::ranges::all_of(m_data, [this](const auto& row)
                                 { return row.size() == m_data.front().size(); }))
        {
            throw std::runtime_error("Diagram is not a matrix");
        }
    }

    auto findAccessible()
    {
        auto adjacents = [this](auto row, auto column)
        {
            std::vector<std::pair<std::size_t, std::size_t>> res;
            if (column > 0)
            {
                res.emplace_back(row, column - 1);
            }
            if (column + 1 < m_data.front().size())
            {
                res.emplace_back(row, column + 1);
            }
            if (row > 0)
            {
                res.emplace_back(row - 1, column);
                if (column > 0)
                {
                    res.emplace_back(row - 1, column - 1);
                }
                if (column + 1 < m_data.front().size())
                {
                    res.emplace_back(row - 1, column + 1);
                }
            }
            if (row + 1 < m_data.size())
            {
                res.emplace_back(row + 1, column);
                if (column > 0)
                {
                    res.emplace_back(row + 1, column - 1);
                }
                if (column + 1 < m_data.front().size())
                {
                    res.emplace_back(row + 1, column + 1);
                }
            }
            return res;
        };

        auto count = 0;
        for (auto row : std::views::iota(0uz, m_data.size()))
        {
            for (auto column : std::views::iota(0uz, m_data.front().size()))
            {
                if (m_data[row][column] == Cell::PaperRoll)
                {
                    if (std::ranges::count_if(
                            adjacents(row, column), [this](auto pos)
                            { return m_data[pos.first][pos.second] != Cell::Empty; })
                        < 4)
                    {
                        m_data[row][column] = Cell::AccessiblePaperRoll;
                        ++count;
                    }
                }
            }
        }
        return count;
    }

    void print() const
    {
        for (const auto& row : m_data)
        {
            for (auto cell : row)
            {
                std::print("{}", toString(cell));
            }
            std::println();
        }
    }
};

int main()
{
    auto input = getInput("4"sv);

    Diagram d(input);
    auto count = d.findAccessible();
    d.print();
    std::println("Found: {}", count);
}
