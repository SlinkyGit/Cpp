/**
* Nikhil Solanki
* Please refer to code comments and citations as necessary.
*/
#ifndef FILE_SEARCH_H
#define FILE_SEARCH_H

#include <vector>
#include <string>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class FileSearch {
public:
    FileSearch(const std::string& target, const std::filesystem::path& dir);
    void run();

private:
    void producer();
    void worker();
    std::vector<std::filesystem::path> locateFiles(const std::filesystem::path& dir);

    std::mutex filesMutex;
    std::string target;
    std::filesystem::path dir;
    std::vector<std::filesystem::path> files;
    std::queue<std::filesystem::path> fileQueue;
    std::vector<std::thread> workers;
    std::mutex queTex;
    std::condition_variable cvQueue;
    bool allFilesListed = false;
};

#endif // FILE_SEARCH_H
