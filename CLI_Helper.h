#ifndef CLI_HELPER_H
#define CLI_HELPER_H

#include <iostream>
#include <string>
#include <limits>
#include <algorithm>

namespace CLI {
    const std::string RESET   = "\033[0m";
    const std::string BLACK   = "\033[30m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    
    const std::string BOLD_RED     = "\033[1;31m";
    const std::string BOLD_GREEN   = "\033[1;32m";
    const std::string BOLD_YELLOW  = "\033[1;33m";
    const std::string BOLD_BLUE    = "\033[1;34m";
    const std::string BOLD_CYAN    = "\033[1;36m";
    const std::string BOLD_WHITE   = "\033[1;37m";

    inline void clearScreen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    inline void printLine(char c = '-', int width = 70, const std::string& color = WHITE) {
        std::cout << color;
        for (int i = 0; i < width; ++i) std::cout << c;
        std::cout << RESET << "\n";
    }

    inline void printHeader(const std::string& title, const std::string& color = BOLD_CYAN) {
        int width = 70;
        int titleLen = title.length();
        int padding = (width - titleLen - 4) / 2;
        
        printLine('=', width, color);
        std::cout << color << "||" << std::string(padding, ' ') << title 
                  << std::string(width - padding - titleLen - 4, ' ') << "||" << RESET << "\n";
        printLine('=', width, color);
    }

    inline int readInt(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max()) {
        int val;
        while (true) {
            std::cout << prompt;
            if (std::cin >> val) {
                if (val >= minVal && val <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return val;
                }
                std::cout << RED << "Error: Value must be between " << minVal << " and " << maxVal << "." << RESET << "\n";
            } else {
                std::cout << RED << "Error: Invalid numeric input." << RESET << "\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    inline double readDouble(const std::string& prompt, double minVal = 0.0, double maxVal = std::numeric_limits<double>::max()) {
        double val;
        while (true) {
            std::cout << prompt;
            if (std::cin >> val) {
                if (val >= minVal && val <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return val;
                }
                std::cout << RED << "Error: Value must be between " << minVal << " and " << maxVal << "." << RESET << "\n";
            } else {
                std::cout << RED << "Error: Invalid numeric input." << RESET << "\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    inline std::string readString(const std::string& prompt, bool allowEmpty = false) {
        std::string val;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, val);
            if (!allowEmpty && val.empty()) {
                std::cout << RED << "Error: Input cannot be empty." << RESET << "\n";
            } else {
                return val;
            }
        }
    }

    inline void pressAnyKey() {
        std::cout << "\n" << YELLOW << "Press Enter to continue..." << RESET;
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

#endif