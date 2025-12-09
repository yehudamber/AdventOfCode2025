import std;
import Utils;
import getInput;

using namespace std::literals;

struct Bank
{
    std::vector<std::uint8_t> m_batteries;
    std::array<std::size_t, 12> m_selected;

    void selectBest()
    {
        if (m_batteries.size() < m_selected.size())
        {
            throw std::runtime_error("Not enough batteries");
        }
        for (auto [nextBattery, i] = std::pair(m_batteries.cbegin(), 0zu); i < m_selected.size();
             ++i)
        {
            auto max = std::ranges::max_element(
                nextBattery,
                m_batteries.cend() - static_cast<std::ptrdiff_t>(m_selected.size() - i - 1));
            m_selected[i] = static_cast<std::size_t>(max - m_batteries.cbegin());
            nextBattery = max + 1;
        }
    }

    auto joltage() const
    {
        return std::ranges::fold_left(
            m_selected | std::views::transform([this](auto i) { return m_batteries[i]; }),
            static_cast<std::uint64_t>(0), [](auto x, auto y) { return 10 * x + y; });
    }

    void print() const
    {
        static constexpr auto toChar = [](auto d) { return static_cast<char>(d + '0'); };
        auto print = [this](auto begin, auto end)
        {
            std::print("{:s}",
                       std::ranges::subrange(m_batteries.data() + begin, m_batteries.data() + end)
                           | std::views::transform(toChar));
        };
        auto printSelected
            = [this](auto index) { std::print("\033[1;34m{}\033[0m", toChar(m_batteries[index])); };
        print(0, m_selected.front());
        for (auto i : std::views::iota(0zu, m_selected.size()))
        {
            printSelected(m_selected[i]);
            print(m_selected[i] + 1,
                  i + 1 != m_selected.size() ? m_selected[i + 1] : m_batteries.size());
        }
        std::println("\t-> {}", joltage());
    }

    static auto fromString(const std::string& str)
    {
        return Bank{ .m_batteries{
                         std::from_range,
                         std::views::transform(str, [](char c)
                                               { return static_cast<std::uint8_t>(c - '0'); }) },
                     .m_selected = {} };
    }
};

int main()
{
    auto input = getInput("3"sv);

    std::uint64_t totalJoltage = 0;
    for (auto bank : Utils::readLines(input) | std::views::transform(Bank::fromString))
    {
        bank.selectBest();
        bank.print();
        totalJoltage += bank.joltage();
    }
    std::println("Total: {}", totalJoltage);
}