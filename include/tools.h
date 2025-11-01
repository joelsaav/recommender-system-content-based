#ifndef TOOLS_H_
#define TOOLS_H_

#include <iostream>
#include <fstream>
#include <vector>

struct CommandLineArgs {
  std::vector<std::string> textFiles;
  std::string stopWordsFile;
  std::string lemmatizationFile;
};

void ErrorOutput();
void HelpOutput();
CommandLineArgs CheckArguments(int argc, char *argv[]);

#endif