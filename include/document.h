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

  std::string documentName() const { return documentName_; }
  const std::vector<std::vector<std::string>> &originalText() const;
  const std::vector<std::vector<std::string>> &simplifiedText() const;
  const std::map<std::string, double> &TF() const { return TF_; }
  double vectorLength() const { return vectorLength_; }
  const std::map<std::string, double> &TFNormalized() const;

  void setAllWordsInCorpus(const std::set<std::string> &allWordsInCorpus);

  void CleanTokens();
  std::string CleanToken(const std::string &token) const;
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
};

std::ostream &operator<<(std::ostream &os, const Document &doc);

#endif