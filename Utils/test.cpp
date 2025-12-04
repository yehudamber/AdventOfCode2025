import std;
import Utils;

// Compile-time tests for PartialSumsView
static_assert(std::ranges::view<Utils::PartialSumsView<std::span<int>, int>>);
static_assert(std::ranges::forward_range<Utils::PartialSumsView<std::span<int>, int>>);
static_assert(
    std::ranges::view<Utils::PartialSumsView<std::ranges::basic_istream_view<double, char>, int>>);
static_assert(std::ranges::input_range<
              Utils::PartialSumsView<std::ranges::basic_istream_view<double, char>, int>>);
static_assert(!std::ranges::forward_range<
              Utils::PartialSumsView<std::ranges::basic_istream_view<double, char>, int>>);
static_assert(
    std::same_as<
        std::iter_value_t<std::ranges::iterator_t<Utils::PartialSumsView<std::span<double>, int>>>,
        double>);

int main()
{
    std::println("Running Utils tests...");

    // Test PartialSumsView
    {
        std::vector<int> data = { 1, 2, 3, 4, 5 };
        auto view = data | Utils::partialSums(0);

        std::vector<int> expected = { 0, 1, 3, 6, 10, 15 };
        if (std::ranges::size(view) != std::ranges::size(expected))
        {
            std::println(std::cerr, "Test failed: expected size {}, got {}",
                         std::ranges::size(expected), std::ranges::ssize(view));
            return 1;
        }

        auto it = view.begin();
        auto i = 0uz;
        for (; i < expected.size() && it != view.end(); ++i, ++it)
        {
            if (*it != expected[i])
            {
                std::println(std::cerr, "Test failed at index {}: expected {}, got {}", i,
                             expected[i], *it);
                return 1;
            }
        }
        if (it != view.end())
        {
            std::println(std::cerr, "Test failed: iterator did not reach the end");
            return 1;
        }
        if (i != expected.size())
        {
            std::println(std::cerr, "Test failed: iterator stopped early at index {}", i);
            return 1;
        }
    }

    {
        std::istringstream iss("1 2 3 4 5");
        auto view = Utils::partialSums(std::views::istream<int>(iss), 1, std::multiplies<>());

        std::vector<int> expected = { 1, 1, 2, 6, 24, 120 };
        auto it = view.begin();
        auto i = 0uz;
        for (; i < expected.size() && it != view.end(); ++i, ++it)
        {
            if (*it != expected[i])
            {
                std::println(std::cerr, "Test failed at index {}: expected {}, got {}", i,
                             expected[i], *it);
                return 1;
            }
        }
        if (it != view.end())
        {
            std::println(std::cerr, "Test failed: iterator did not reach the end");
            return 1;
        }
        if (i != expected.size())
        {
            std::println(std::cerr, "Test failed: iterator stopped early at index {}", i);
            return 1;
        }
    }

    std::println("All tests passed!");
}