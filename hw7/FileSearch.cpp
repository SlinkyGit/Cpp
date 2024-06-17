/**
 * Nikhil Solanki
 * CS 361
 * HW 7
 * Please refer to code comments and citations as necessary.
 * Inspired by Professor Boady's searchTask and threadPool classes.
*/

#include "FileSearch.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream> // Granted by Professor Boady -> https://cplusplus.com/reference/fstream/fstream/

/**
* This class initializes a thread pool that concurrently processes files to search for
* a specified text string within them. It uses one producer thread to identify and enqueue
* eligible files based on their extension.
*/

/**
 * Initializes a FileSearch object with a target search string and a directory path.
* @param target The text string to search within the files.
* @param dir The directory path where the files will be searched.
*/
FileSearch::FileSearch(const std::string& target, const std::filesystem::path& dir) : target(target), dir(dir) {

}

/**
* run()
* Starts the file searching process by initializing the respective producer and worker threads.
* This method launches one producer thread to queue files and several worker threads
* to process these files by searching for the target string. It ensures all threads
* finish execution before returning.
*/
void FileSearch::run() {
    std::thread producerThread(&FileSearch::producer, this); // Call producer (T_0) to list out all files in directory

    unsigned int numThreads = std::thread::hardware_concurrency();
    workers.reserve(numThreads);  // Reserve to prevent reallocation affecting threads
    for (unsigned int i = 0; i < numThreads; ++i) {
        workers.emplace_back(&FileSearch::worker, this);
    }

    producerThread.join();

    for (auto& worker : workers) {
        worker.join();
    }
}

/**
* worker() 
* This method that processes files from the queue to search for the given target string.
* Each worker thread runs this method concurrently. They remove files from the queue and search
* each file's content for the target string, logging results as specified.
*/
void FileSearch::worker() {
    while (true) {
        std::filesystem::path filePath;
        {
            std::unique_lock<std::mutex> lock(queTex);

            // Derived from Prof Boady's taskQueue.h:
            cvQueue.wait(lock, [this]{ return !fileQueue.empty() || allFilesListed; });
            if (fileQueue.empty()) {
                break;
            }
            
            filePath = fileQueue.front();
            fileQueue.pop();
            std::cout << "Thread " << std::this_thread::get_id() << " is searching in " << filePath << ".\n";
        }
        
        //ref -> https://stackoverflow.com/questions/7880/how-do-you-open-a-file-in-c
        //ref -> https://stackoverflow.com/questions/46719183/c-using-ifstream-to-read-file
        //ref -> https://cplusplus.com/reference/fstream/ifstream/
        std::ifstream fileStream(filePath);
        if (!fileStream.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            continue;
        }

        std::string line;
        unsigned int lineNum = 0;

        while (getline(fileStream, line)) {
            lineNum++;
            // ref -> https://stackoverflow.com/questions/55437983/question-about-get-line-and-string-find-function
            // ref -> https://stackoverflow.com/questions/21654609/how-does-stringnpos-know-which-string-i-am-referring-to
            // ref -> https://www.geeksforgeeks.org/stringnpos-in-c-with-examples/
            if (line.find(target) != std::string::npos) {
                {
                    std::lock_guard<std::mutex> printLock(filesMutex);
                    std::cout << "\n";
                    std::cout << "Thread " << std::this_thread::get_id() << " found a match in " << filePath << " at line " << lineNum << ".\n";
                    std::cout << "\n";
                }
            }
        }
    }
}

/**
* producer()
* This method scans the given directory and all subdirectories for files with specific
* extensions, adding them to a shared queue for processing by worker threads.
*/
void FileSearch::producer(){
    files = locateFiles(dir);
    // Loop over the files in directory
    std::cout << "Searching in " << dir << '\n';
    for (const auto& file : files) {
        std::cout << "File in Directory: " << file << '\n';
        std::unique_lock<std::mutex> lock(queTex);

        // Push files to queue
        fileQueue.push(file);
    }

    // Notify after adding all files
    allFilesListed = true;
    cvQueue.notify_all();
}

/**
* locateFiles()
* This method iterates over all files in the specified directory and its subdirectories,
* checking if they have the specified extensions, and returns a list of these files.
* @param dir the directory to search within
* @return std::vector<std::filesystem::path> a vector containing the paths of all eligible files
*/
std::vector<std::filesystem::path> FileSearch::locateFiles(const std::filesystem::path& dir) {
    std::vector<std::filesystem::path> localFiles;

    // File extensions: cc, c, cpp, h, hpp, pl, sh, py, txt
    std::string extensions[] = {".cc", ".c", ".cpp", ".h", ".hpp", ".pl", ".sh", ".py", ".txt"};

    // ref -> https://stackoverflow.com/questions/67273/how-do-you-iterate-through-every-file-directory-recursively-in-standard-c
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        
        // ref -> https://en.cppreference.com/w/cpp/filesystem/is_regular_file
        // ref -> https://stackoverflow.com/questions/73227898/stdfilesystemis-regular-file-vs-exists-what-counts-as-a-regular-file
        if (std::filesystem::is_regular_file(entry.status()) &&
            // ref -> https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
            std::find(std::begin(extensions), std::end(extensions), entry.path().extension()) != std::end(extensions)) {
            // localFiles.push_back(entry.path());
            localFiles.emplace_back(entry.path());
        }
    }
    return localFiles;
}
