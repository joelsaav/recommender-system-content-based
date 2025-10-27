#ifndef TOOLS_H_
#define TOOLS_H_

#include <iostream>
#include <vector>
#include <set>
#include <map>

struct CommandLineArgs {
  std::vector<std::string> textFiles;
  std::string stopWordsFile;
  std::string lemmatizationFile;
};

void ErrorOutput();
void HelpOutput();
CommandLineArgs CheckArguments(int argc, char *argv[]);

// New utility functions
std::set<std::string> LoadStopWords(const std::string& filename);
std::map<std::string, std::string> LoadLemmatizationRules(const std::string& filename);

#endif