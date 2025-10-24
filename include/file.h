#ifndef FILE_H_
#define FILE_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class File {
 public:
  explicit File(const std::string& inputFile);
 private:
  std::vector<std::vector<std::string>> text_;
};

#endif