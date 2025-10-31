#ifndef TOOLS_H_
#define TOOLS_H_

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include "file.h"

struct CommandLineArgs {
  std::vector<std::string> textFiles;
  std::string stopWordsFile;
  std::string lemmatizationFile;
};

void ErrorOutput();
void HelpOutput();
CommandLineArgs CheckArguments(int argc, char *argv[]);

// Text treatment
std::set<std::string> LoadStopWords(const std::string& filename);
std::map<std::string, std::string> LoadLemmatizationRules(const std::string& filename);

// TF-IDF calculation functions
std::map<std::string, int> BuildVocabulary(const std::vector<File>& files);
std::map<std::string, double> CalculateIDF(const std::vector<File>& files, int totalDocs);
double CalculateCosineSimilarity(const std::map<std::string, double>& tfidf1, 
                                  const std::map<std::string, double>& tfidf2);
void PrintSimilarityMatrix(const std::vector<File>& files);

#endif