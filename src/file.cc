#include "../include/file.h"

File::File(const std::string inputFile) {
  std::ifstream file{inputFile, std::ios::in};
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << inputFile << std::endl;
    exit(1);
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::vector<std::string> row;
    std::string value;
    std::istringstream ss(line);
    while (ss >> value) {
      row.push_back(value);
    }
    text_.push_back(row);
  }
  file.close();
}