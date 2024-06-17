/**
 * Nikhil Solanki
 * CS 361
 * HW 7
 * Please refer to code comments and citations as necessary.
*/
#include <iostream>
#include <filesystem>
#include <string>
#include "FileSearch.h"
#include <thread>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <target string> <directory>\n";
        return 1;
    }

    std::string target = argv[1];

    // ref -> https://en.cppreference.com/w/cpp/filesystem/path
    // ref -> https://en.cppreference.com/w/cpp/filesystem/current_path
    std::filesystem::path dir;
    if (argc > 2) {
        dir = std::filesystem::path(argv[2]);  // Use the directory provided as a command-line argument

    } else {
        dir = std::filesystem::current_path(); // Use the current directory if no argument is provided
        
    }


    std::cout << "!---- Search Started ----!\n";
    std::cout << "Target Folder: " << dir << '\n';
    std::cout << "Target Text: " << "'" << target << "'" << '\n';

    unsigned int numThreads = std::thread::hardware_concurrency();

    if (numThreads < 2) {
        numThreads = 2; // Need minimum 2 threads
    } 

    std::cout << "Using a Pool of " << numThreads << " threads to search.\n";

    // Create FileSearch object and run it
    FileSearch fileSearch(target, dir);
    fileSearch.run();

    std::cout << "!---- Search Complete ----!\n";
    return 0;
}
