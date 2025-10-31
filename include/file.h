#ifndef FILE_H_
#define FILE_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>

class File {
 public:
  explicit File(const std::string& inputFile);
  
  // Methods to process text
  void RemoveStopWords(const std::set<std::string>& stopWords);
  void ApplyLemmatization(const std::map<std::string, std::string>& lemmaMap);
  
  // Getters for different text versions
  const std::vector<std::vector<std::string>>& GetOriginalText() const;
  const std::vector<std::vector<std::string>>& GetTextWithoutStopWords() const;
  const std::vector<std::vector<std::string>>& GetLemmatizedText() const;
  
  // Method to print different versions
  void PrintOriginalText() const;
  void PrintTextWithoutStopWords() const;
  void PrintLemmatizedText() const;
  
  // TF-IDF methods
  void CalculateTF();
  void CalculateTFIDF(const std::map<std::string, double>& idfMap);
  const std::map<std::string, int>& GetTF() const;
  const std::map<std::string, double>& GetTFIDF() const;
  std::string GetFileName() const;
  void PrintTFIDFTable(const std::map<std::string, int>& vocabulary, 
                       const std::map<std::string, double>& idfMap) const;
  
 private:
  std::string fileName_;
  std::vector<std::vector<std::string>> originalText_;
  std::vector<std::vector<std::string>> textWithoutStopWords_;
  std::vector<std::vector<std::string>> lemmatizedText_;
  
  std::map<std::string, int> tf_;
  std::map<std::string, double> tfidf_;  
  
  std::string ToLowerCase(const std::string& str) const;
  std::string CleanToken(const std::string& token) const;
};

#endif