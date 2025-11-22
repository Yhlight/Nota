#pragma once

#include <string>
#include <fstream>
#include <filesystem>

// Helper function to create a test file
inline void createTestFile(const std::string& path, const std::string& content) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    file << content;
}
