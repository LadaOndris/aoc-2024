
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

namespace fs = std::filesystem;

namespace {
    struct InputArgs {
        int startDay;
        int endDay;
        bool areInvalid;
    };

    InputArgs parseArguments(int argc, char *argv[]) {
        if (argc != 3) {
            return {.areInvalid = true};
        }
        InputArgs args{};
        try {
            args.startDay = std::stoi(argv[1]);
            args.endDay = std::stoi(argv[2]);
            args.areInvalid = false;
        }
        catch (...) {
            args.areInvalid = true;
        }
        return args;
    }

    std::optional<fs::path> navigateToParentDirectory(const std::string &targetDirName) {
        auto path = fs::current_path();

        while (true) {
            // Check if the current directory matches the target directory name
            if (path.filename() == targetDirName) {
                std::cout << "Found target directory: " << path << std::endl;
                return path; // Return the path to the target directory
            }

            // Move up one directory
            if (path == path.root_path()) {
                // We've reached the root directory; stop searching
                break;
            }
            path = path.parent_path();
        }

        std::cout << "Target directory not found." << std::endl;
        return std::nullopt; // Return no path
    }

    void createEmptyFile(const std::string &folderPath, const std::string &fileName) {
        if (!fs::exists(folderPath)) {
            fs::create_directory(folderPath);
            std::cout << "Created folder: " << folderPath << '\n';
        }
        std::string filePath = fs::path(folderPath) / fileName;
        std::ofstream file(filePath);
        if (file) {
            std::cout << "Created file: " << filePath << '\n';
        } else {
            std::cerr << "Failed to create file: " << filePath << '\n';
        }
    }

    void createEmptyInputFiles(int firstDay, int lastDay, const fs::path &rootPath) {
        auto folderPath = rootPath / "input_files";
        constexpr auto fileNameFormat = "day{:02}.txt";

        for (int i = firstDay; i <= lastDay; i++) {
            auto fileName = std::format(fileNameFormat, i);
            createEmptyFile(folderPath, fileName);
        }
    }

    void createSourceFileFromTemplate(int dayNumber, const fs::path &rootPath) {
        std::string templatePath = "main_template.cpp"; // This should be located next to the binary

        if (!std::filesystem::exists(templatePath)) {
            std::cerr << "Template file does not exist: " << templatePath << std::endl;
            return;
        }
        auto targetDirPath = rootPath / std::format("src/day{:02}", dayNumber);
        auto targetFilePath = targetDirPath / "main.cpp";

        try {
            // Create the directory if it doesn't exist
            std::filesystem::create_directories(targetDirPath);

            // Copy the template to the target file
            std::filesystem::copy_file(templatePath, targetFilePath, std::filesystem::copy_options::skip_existing);

            std::cout << "Template copied to: " << targetFilePath << std::endl;
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void createSourceFilesFromTemplate(int firstDay, int lastDay, const fs::path &rootPath) {
        for (int i = firstDay; i <= lastDay; i++) {
            createSourceFileFromTemplate(i, rootPath);
        }
    }
}


int main(int argc, char *argv[]) {
    InputArgs args{parseArguments(argc, argv)};
    if (args.areInvalid) {
        std::cout << "Input arguments are invalid." << std::endl;
        return 1;
    }

    auto rootDirectory = navigateToParentDirectory("aoc-2024");
    if (!rootDirectory.has_value()) {
        std::cout << "Could not navigate to the root folder of the repository." << std::endl;
        return 1;
    }

    createEmptyInputFiles(args.startDay, args.endDay, rootDirectory.value());
    createSourceFilesFromTemplate(args.startDay, args.endDay, rootDirectory.value());
}