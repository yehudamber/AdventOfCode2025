export module getInput;

import std;

using namespace std::literals;

export std::ifstream getInput(std::string_view dayNumber)
{
    const auto inputFilePath
        = std::filesystem::path(SOURCE_DIR) / std::format("day{}.txt", dayNumber);

    auto inputFile = std::ifstream(inputFilePath);
    if (!inputFile.is_open())
    {
        std::println(
            std::cerr,
            "Error: Could not open {}\n"
            "Please download the puzzle input from https://adventofcode.com/2025/day/{}/input into "
            "that location and make sure the program can open it.",
            inputFilePath.native(), dayNumber);
        std::exit(1);
    }

    return inputFile;
}
