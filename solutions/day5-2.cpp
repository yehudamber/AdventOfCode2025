import std;
import Utils;
import getInput;

using namespace std::literals;

auto parseRange(const std::string& str)
{
    auto separator = str.find('-');
    std::array<std::uint64_t, 2> range;
    if (separator == str.npos || !std::from_chars(str.data(), str.data() + separator, range.front())
        || !std::from_chars(str.data() + separator + 1, str.data() + str.size(), range.back()))
    {
        throw std::runtime_error("Invalid ID range");
    }
    ++range.back(); // input is inclusive ranges, we use exclusive
    return range;
}

template <std::integral T> struct Segments
{
    std::vector<T> m_points;
    std::vector<std::uint8_t> m_state; // should have been vector<bool>

    template <std::ranges::input_range Range>
        requires std::convertible_to<std::ranges::range_reference_t<Range>, T>
    explicit Segments(Range&& points) : m_points(std::from_range, std::forward<Range>(points))
    {
        std::ranges::sort(m_points);
        auto [toRemove, end] = std::ranges::unique(m_points);
        m_points.erase(toRemove, end);
        m_state.assign(m_points.size() - 1, false);
    }

    void addRange(T begin, T end)
    {
        auto beginIndex = static_cast<std::size_t>(std::ranges::lower_bound(m_points, begin)
                                                   - m_points.begin());
        auto endIndex
            = static_cast<std::size_t>(std::ranges::lower_bound(m_points, end) - m_points.begin());
        if (beginIndex == m_points.size() || endIndex == m_points.size())
        {
            throw std::invalid_argument("Unrecognized range");
        }
        for (auto i : std::views::iota(beginIndex, endIndex))
        {
            m_state[i] = true;
        }
    }

    auto totalSize() const
    {
        auto res = static_cast<T>(0);
        for (auto i : std::views::iota(0uz, m_state.size()))
        {
            if (m_state[i])
            {
                res += m_points[i + 1] - m_points[i];
            }
        }
        return res;
    }
};

template <std::ranges::input_range Range>
Segments(Range&&) -> Segments<std::ranges::range_value_t<Range>>;

int main()
{
    auto input = getInput("5"sv);

    auto idRanges = Utils::readLines(input)
        | std::views::take_while([](const auto& str) { return !str.empty(); })
        | std::views::transform(parseRange) | std::ranges::to<std::vector>();
    auto segments = Segments(idRanges | std::views::join);
    for (const auto& [begin, end] : idRanges)
    {
        segments.addRange(begin, end);
    }
    std::println("Total size: {}", segments.totalSize());
}