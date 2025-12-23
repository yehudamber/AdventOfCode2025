import std;
import Utils;
import getInput;

using namespace std::literals;

class DisjointSetUnion
{
public:
    explicit DisjointSetUnion(std::size_t size)
        : m_parent(std::from_range, std::views::iota(0uz, size)), m_size(size, 1)
    {
    }

    auto findSet(std::size_t v)
    {
        if (v == m_parent[v])
            return v;

        return m_parent[v] = findSet(m_parent[v]);
    }

    void unionSets(std::size_t a, std::size_t b)
    {
        a = findSet(a);
        b = findSet(b);
        if (a != b)
        {
            if (m_size[a] < m_size[b])
            {
                std::swap(a, b);
            }
            m_parent[b] = a;
            m_size[a] += m_size[b];
        }
    }

    auto setSizes()
    {
        auto res = std::vector(m_parent.size(), 0uz);
        for (auto i : std::views::iota(0uz, m_parent.size()))
        {
            ++res[findSet(i)];
        }
        std::erase(res, 0);
        std::ranges::sort(res, std::ranges::greater{});
        return res;
    }

private:
    std::vector<std::size_t> m_parent;
    std::vector<std::size_t> m_size;
};

using Point = std::array<std::uint64_t, 3>;

constexpr auto distance(const Point& p1, const Point& p2)
{
    auto squaredDifference = [](const auto& pair)
    {
        const auto& [a, b] = pair;
        return (a - b) * (a - b);
    };
    return std::sqrt(std::ranges::fold_left(
        std::views::zip(p1, p2) | std::views::transform(squaredDifference), 0, std::plus<>{}));
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
    auto input = getInput("8"sv);

    const auto points = Utils::readLines(input) | std::views::transform(parsePoint)
        | std::ranges::to<std::vector>();
    auto distances = generatePairs(0uz, points.size())
        | std::views::transform(
                         [&points](const auto& pair)
                         {
                             return std::tuple(pair.first, pair.second,
                                               distance(points[pair.first], points[pair.second]));
                         })
        | std::ranges::to<std::vector>();
    std::ranges::sort(distances, std::ranges::less{},
                      [](const auto& tuple) { return std::get<2>(tuple); });

    auto dsu = DisjointSetUnion(points.size());
    for (const auto& [a, b, _] : distances | std::views::take(1000))
    {
        dsu.unionSets(a, b);
    }

    std::println(
        "{}",
        std::ranges::fold_left(dsu.setSizes() | std::views::take(3), 1ull, std::multiplies<>{}));
}