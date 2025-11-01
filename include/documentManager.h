#ifndef DOCUMENT_MANAGER_H_
#define DOCUMENT_MANAGER_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "document.h"

class DocumentManager {
 public:
  DocumentManager(const std::vector<std::string>& documents,
                  const std::string& stopWordsFile,
                  const std::string& lemmatizationFile);

  std::vector<Document> documents() const { return documents_; }
  std::map<std::string, int> documentsOccurrences() const;
  std::set<std::string> stopWords() const { return stopWords_; }
  std::map<std::string, std::string> lemmatizationMap() const;
  std::set<std::string> allWordsInCorpus() const { return allWordsInCorpus_; }
  int TotalDocuments() const { return documents_.size(); }

  void CalculateIDF();
  void Recommend();
  void CalculateCosineSimilarity();
  void PrintSimilarityMatrix() const;

 private:
  std::vector<Document> documents_;
  std::set<std::string> stopWords_;
  std::map<std::string, std::string> lemmatizationMap_;
  std::map<std::string, int> documentsOccurrences_;
  std::set<std::string> allWordsInCorpus_;
  std::map<std::string, double> IDF_;
  std::vector<std::vector<double>> similarityMatrix_;

  std::map<std::string, std::string> LoadLemmatizationRules(
      const std::string& lemmatizationFile);
  void CountDocumentsOccurrences();
};

std::ostream& operator<<(std::ostream& os, const DocumentManager& dm);

#endif