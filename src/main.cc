#include <iostream>

#include "../include/file.h"
#include "../include/tools.h"

int main(int argc, char* argv[]) {
  CommandLineArgs args = CheckArguments(argc, argv);

  std::vector<std::string> textFiles = args.textFiles;
  std::string stopWordsFile = args.stopWordsFile;
  std::string lemmatizationFile = args.lemmatizationFile;

  std::vector<File> files;
  for (const std::string file : textFiles) {
    File temFile(file);
    files.push_back(temFile);
  }
    
  return 0;
}