#ifndef DOCUMENT_MANAGER_H_
#define DOCUMENT_MANAGER_H_

#include <algorithm>
#include <iomanip>

#include "document.h"

class DocumentManager {
 public:
  DocumentManager(const std::vector<std::string>& documents,
                  const std::string& stopWordsFile,
                  const std::string& lemmatizationFile);

  /**
   * @brief Getter for all documents in corpus
   * @return Vector of Document objects
   */
  std::vector<Document> documents() const { return documents_; }
  /**
   * @brief Getter for stop words set
   * @return Set of stop words
   */
  std::set<std::string> stopWords() const { return stopWords_; }
  /**
   * @brief Getter for all words in corpus
   * @return Set of all unique words in the corpus
   */
  std::set<std::string> allWordsInCorpus() const { return allWordsInCorpus_; }
  /**
   * @brief Getter for IDF map
   * @return Map of terms to their IDF values
   */
  std::map<std::string, double> IDF() const { return IDF_; }
  std::map<std::string, int> documentsOccurrences() const;
  std::map<std::string, std::string> lemmatizationMap() const;

  void Recommend();
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
  void CalculateIDF();
  void CalculateCosineSimilarity();
};

std::ostream& operator<<(std::ostream& os, const DocumentManager& dm);

#endif