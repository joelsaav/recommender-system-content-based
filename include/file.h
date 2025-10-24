#ifndef FILE_H_
#define FILE_H_

#include <iostream>
#include <vector>

class File {
 public:
  File(const std::string inputFile);
 private:
  std::vector<std::string> text_;
};

#endif