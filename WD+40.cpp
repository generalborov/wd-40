#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

std::string RUSTC_MESSAGE = "/rustc/";
std::string RUSTC_PANIC_MESSAGE = "rust_panic";
std::string RUST_BACKTRACE_MESSAGE = "`RUST_BACKTRACE=1`";

bool getYesNoInput(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt << " (y/n): ";
        std::getline(std::cin, input);

        if (input == "y" || input == "Y") {
            return true;  // Пользователь выбрал "да"
        }
        else if (input == "n" || input == "N") {
            return false; // Пользователь выбрал "нет"
        }
        else {
            std::cout << "Пожалуйста, введите 'y' или 'n'." << std::endl;
        }
    }
}

bool containsString(const std::string& filePath, const std::string& searchString) {
	std::ifstream file(filePath, std::ios::binary);
    if (fs::is_directory(filePath)) {
        return false;
    }
    if (!file.is_open()) {
		std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
		return false;
	}

	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return fileContent.find(searchString) != std::string::npos;
}

std::vector<std::string> files;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    if (argc < 2) {
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    if (fs::is_directory(filename)) {
        std::cout << filename << " является директорией." << std::endl;

        try {
            if (fs::exists(filename) && fs::is_directory(filename)) {
                for (const auto& entry : fs::recursive_directory_iterator(filename)) {
                    filename = entry.path().string();
                    bool foundRUSTC_MESSAGE = containsString(filename, RUSTC_MESSAGE);
                    bool foundRUSTC_PANIC_MESSAGE = containsString(filename, RUSTC_PANIC_MESSAGE);
                    bool foundRUST_BACKTRACE_MESSAGE = containsString(filename, RUST_BACKTRACE_MESSAGE);

                    if (foundRUSTC_MESSAGE && foundRUSTC_PANIC_MESSAGE && foundRUST_BACKTRACE_MESSAGE)
                    {
                        std::cout << "Файл '" << filename << "' 100% растобинарь." << std::endl;
                        files.push_back(filename);
                    }
                    
                }
            }
            else {
                std::cerr << "Указанный путь не существует или не является директорией." << std::endl;
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }

    }
    else if (fs::is_regular_file(argv[1])) {
        std::cout << argv[1] << " является файлом." << std::endl;

        bool foundRUSTC_MESSAGE = containsString(filename, RUSTC_MESSAGE);
        bool foundRUSTC_PANIC_MESSAGE = containsString(filename, RUSTC_PANIC_MESSAGE);
        bool foundRUST_BACKTRACE_MESSAGE = containsString(filename, RUST_BACKTRACE_MESSAGE);

        if (foundRUSTC_MESSAGE && foundRUSTC_PANIC_MESSAGE && foundRUST_BACKTRACE_MESSAGE)
        {
            std::cout << "Файл '" << filename <<"' 100% растобинарь." << std::endl;

            files.push_back(filename);
        }

        

    }
    else {
        std::cout << argv[1] << " не является ни файлом, ни директорией." << std::endl;
        return EXIT_FAILURE;
    }

    if (getYesNoInput("Хотите удалить?")) {
        for (auto& file : files ) {
            try {
                if (fs::exists(file)) {
                    fs::remove(file);
                    std::cout << "Файл " << file << " успешно удален." << std::endl;
                }
                else {
                    std::cerr << "Файл не существует." << std::endl;
                }
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Ошибка: " << e.what() << std::endl;
            }
        }
    }
}
