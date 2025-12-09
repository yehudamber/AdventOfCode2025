import std;
import Utils;
import getInput;

using namespace std::literals;

struct Bank
{
    std::vector<std::uint8_t> m_batteries;
    std::size_t m_selected1;
    std::size_t m_selected2;

    void selectBest()
    {
        if (m_batteries.size() < 2)
        {
            throw std::runtime_error("Not enough batteries");
        }
        auto maxIt = std::ranges::max_element(m_batteries.begin(), m_batteries.end() - 1);
        m_selected1 = static_cast<std::size_t>(maxIt - m_batteries.begin());
        maxIt = std::ranges::max_element(maxIt + 1, m_batteries.end());
        m_selected2 = static_cast<std::size_t>(maxIt - m_batteries.begin());
    }

    auto joltage() const { return 10 * m_batteries[m_selected1] + m_batteries[m_selected2]; }

    void print() const
    {
        static constexpr auto toChar = [](auto d) { return static_cast<char>(d + '0'); };
        auto subStr = [this](auto begin, auto end)
        {
            return std::ranges::subrange(m_batteries.data() + begin, m_batteries.data() + end)
                | std::views::transform(toChar);
        };
        std::println("{:s}\033[1;34m{}\033[0m{:s}\033[1;34m{}\033[0m{:s} -> {}",
                     subStr(0, m_selected1), toChar(m_batteries[m_selected1]),
                     subStr(m_selected1 + 1, m_selected2), toChar(m_batteries[m_selected2]),
                     subStr(m_selected2 + 1, m_batteries.size()), joltage());
    }

    static auto fromString(const std::string& str)
    {
        return Bank{ .m_batteries{
                         std::from_range,
                         std::views::transform(str, [](char c)
                                               { return static_cast<std::uint8_t>(c - '0'); }) },
                     .m_selected1 = 0,
                     .m_selected2 = 0 };
    }
};

int main()
{
    auto input = getInput("3"sv);

    auto totalJoltage = 0;
    for (auto bank : Utils::readLines(input) | std::views::transform(Bank::fromString))
    {
        bank.selectBest();
        bank.print();
        totalJoltage += bank.joltage();
    }
    std::println("Total: {}", totalJoltage);
}