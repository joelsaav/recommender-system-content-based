#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

class Document {
 public:
  Document(const std::string &inputDocument);

  /**
   * @brief Getter for document name
   * @return Document name as a string
   */
  std::string documentName() const { return documentName_; }
  const std::vector<std::vector<std::string>> &originalText() const;
  const std::vector<std::vector<std::string>> &simplifiedText() const;
  /**
   * @brief Getter for Term Frequency (TF) map
   * @return Map of terms to their TF values
   */
  const std::map<std::string, double> &TF() const { return TF_; }
  const std::map<std::string, double> &TFNormalized() const;
  /**
   * @brief Getter for vector length
   * @return Length of the TF vector
   */
  double vectorLength() const { return vectorLength_; }
  /**
   * @brief Setter for all words in corpus
   * @param allWordsInCorpus Set of all unique words in the corpus
   */
  void setAllWordsInCorpus(const std::set<std::string> &allWordsInCorpus);

  void CleanTokens();
  void RemoveStopWords(const std::set<std::string> &stopWords);
  void Lemmatization(
      const std::map<std::string, std::string> &lemmatizationMap);
  void CalculateTF();
  void CalculateVectorLength();
  void CalculateTFNormalized();

 private:
  std::string documentName_;
  std::vector<std::vector<std::string>> originalText_;
  std::vector<std::vector<std::string>> simplifiedText_;
  std::map<std::string, double> TF_;
  std::map<std::string, double> TFNormalized_;
  std::set<std::string> allWordsInCorpus_;
  double vectorLength_;

  std::string ToLowerCase(const std::string &str) const;
  std::string CleanToken(const std::string &token) const;
};

std::ostream &operator<<(std::ostream &os, const Document &doc);

#endif