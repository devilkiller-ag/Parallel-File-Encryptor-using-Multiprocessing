#include <ctime>
#include <iomanip>
#include <iostream>
#include <filesystem>

#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"


namespace fs = std::filesystem;


int main(int argc, char *argv[]) {
    std::string directory;
    std::string actionStr;

    std::cout<<"Enter the directory path: ";
    std::getline(std::cin, directory);

    std::cout<<"Enter the action (ENCRYPT/DECRYPT): ";
    std::getline(std::cin, actionStr);

    try {
        if(fs::exists(directory) && fs::is_directory(directory)) {
            ProcessManagement ProcessManagement;
            
            for(const auto &entry : fs::recursive_directory_iterator(directory)) {
                if(entry.is_regular_file()) {
                    std::string filePath = entry.path().string();
                    IO io(filePath);
                    std::fstream f_stream = std::move(io.getFileStream());
                    
                    if(f_stream.is_open()) {
                        Action action = (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
                        auto task = std::make_unique<Task>(std::move(f_stream), action, filePath);
                        
                        std::time_t t = std::time(nullptr);
                        std::tm* now = std::localtime(&t);
                        std::cout << "Starting the encryption/decryption at: " << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;
                        
                        if (!task) {
                            std::cerr << "Failed to create task for file: " << filePath << std::endl;
                            continue;
                        }

                        ProcessManagement.submitToQueue(std::move(task));
                    } else {
                        std::cout<<"Error opening file: " << filePath << std::endl;
                    }
                } else {
                    std::cout<<"Skipping directory: " << entry.path().string() << std::endl;
                }
            }
        } else {
            std::cout<<"Invalid directory path: " << directory << std::endl;
        }
    } catch(const fs::filesystem_error &e) {
        std::cout<<"Filesystem Error: " << e.what() << std::endl;
    }

    return 0;
}
