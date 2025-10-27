#ifndef FILE_H_
#define FILE_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <map>

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
  
 private:
  std::vector<std::vector<std::string>> originalText_;
  std::vector<std::vector<std::string>> textWithoutStopWords_;
  std::vector<std::vector<std::string>> lemmatizedText_;
  
  // Helper method to convert string to lowercase
  std::string ToLowerCase(const std::string& str) const;
};

#endif