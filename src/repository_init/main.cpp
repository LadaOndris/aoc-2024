
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace {

struct InputArgs {
    std::string argv0;
    int startDay;
    int endDay;
    bool areInvalid;
};

InputArgs parseArguments(int argc, char* argv[]) {
    if (argc != 3) {
        return {
            .argv0 = argv[0], .startDay = -1, .endDay = -1, .areInvalid = true};
    }
    InputArgs args{};
    try {
        args.argv0 = argv[0];
        args.startDay = std::stoi(argv[1]);
        args.endDay = std::stoi(argv[2]);
        args.areInvalid = false;
    } catch (...) {
        args.areInvalid = true;
    }
    return args;
}

class TemplateHandler {
   public:
    TemplateHandler(std::string_view argv0)
        : executablePath(getExecutablePath(argv0)) {}

    fs::path getTemplatePath() const {
        return executablePath / "main_template.cpp";
    }

   private:
    fs::path executablePath;

    static fs::path getExecutablePath(std::string_view argv0) {
        fs::path exePath(argv0);
        if (exePath.is_relative()) {
            exePath = fs::current_path() / exePath;
        }
        return exePath.parent_path();
    }
};

class FileCreator {
   public:
    explicit FileCreator(const fs::path& root) : rootPath(root) {}

    void createEmptyInputFiles(int firstDay, int lastDay) const {
        auto folderPath = rootPath / "input_files";
        constexpr auto fileNameFormat = "day{:02}.txt";

        for (int i = firstDay; i <= lastDay; ++i) {
            std::string fileName = std::format(fileNameFormat, i);
            createEmptyFile(folderPath, fileName);
        }
    }

    void createSourceFilesFromTemplate(
        int firstDay, int lastDay,
        const TemplateHandler& templateHandler) const {
        auto templatePath = templateHandler.getTemplatePath();
        if (!fs::exists(templatePath)) {
            std::cerr << "Template file does not exist: " << templatePath
                      << std::endl;
            return;
        }

        for (int i = firstDay; i <= lastDay; ++i) {
            createSourceFileFromTemplate(i, templatePath);
        }
    }

   private:
    fs::path rootPath;

    static void createEmptyFile(const fs::path& folderPath,
                                const std::string& fileName) {
        if (!fs::exists(folderPath)) {
            fs::create_directory(folderPath);
            std::cout << "Created folder: " << folderPath << '\n';
        }
        fs::path filePath = folderPath / fileName;
        std::ofstream file(filePath);
        if (file) {
            std::cout << "Created file: " << filePath << '\n';
        } else {
            std::cerr << "Failed to create file: " << filePath << '\n';
        }
    }

    void createSourceFileFromTemplate(int dayNumber,
                                      const fs::path& templatePath) const {
        auto targetDirPath = rootPath / std::format("src/day{:02}", dayNumber);
        auto targetFilePath = targetDirPath / "main.cpp";

        try {
            fs::create_directories(targetDirPath);
            fs::copy_file(templatePath, targetFilePath,
                          fs::copy_options::skip_existing);
            std::cout << "Template copied to: " << targetFilePath << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

std::optional<fs::path> navigateToParentDirectory(
    const std::string& targetDirName) {
    auto path = fs::current_path();

    while (true) {
        if (path.filename() == targetDirName) {
            std::cout << "Found target directory: " << path << std::endl;
            return path;
        }
        if (path == path.root_path()) {
            break;
        }
        path = path.parent_path();
    }

    std::cout << "Target directory not found." << std::endl;
    return std::nullopt;
}

}  // namespace

int main(int argc, char* argv[]) {
    InputArgs args{parseArguments(argc, argv)};
    if (args.areInvalid) {
        std::cout << "Input arguments are invalid." << std::endl;
        std::cout << "Usage: repository_init <start_day> <end_day>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    auto rootDirectory = navigateToParentDirectory("aoc-2024");
    if (!rootDirectory.has_value()) {
        std::cout << "Could not navigate to the root folder of the repository."
                  << std::endl;
        return EXIT_FAILURE;
    }

    TemplateHandler templateHandler(args.argv0);
    FileCreator fileCreator(rootDirectory.value());

    fileCreator.createEmptyInputFiles(args.startDay, args.endDay);
    fileCreator.createSourceFilesFromTemplate(args.startDay, args.endDay,
                                              templateHandler);

    return EXIT_SUCCESS;
}
