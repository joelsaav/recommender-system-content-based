#include "../include/file.h"
#include <algorithm>
#include <cctype>

File::File(const std::string& inputFile) {
  std::ifstream file{inputFile, std::ios::in};
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << inputFile << std::endl;
    exit(1);
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::vector<std::string> row;
    std::string value;
    std::istringstream ss(line);
    while (ss >> value) {
      row.push_back(value);
    }
    originalText_.push_back(row);
  }
  file.close();
  
  // Initialize other versions as copies of original
  textWithoutStopWords_ = originalText_;
  lemmatizedText_ = originalText_;
}

void File::RemoveStopWords(const std::set<std::string>& stopWords) {
  textWithoutStopWords_.clear();
  
  for (const auto& line : originalText_) {
    std::vector<std::string> processedLine;
    for (const auto& word : line) {
      std::string lowerWord = ToLowerCase(word);
      if (stopWords.find(lowerWord) != stopWords.end()) {
        // Replace stop word with a placeholder to maintain position
        processedLine.push_back("---");
      } else {
        processedLine.push_back(word);
      }
    }
    textWithoutStopWords_.push_back(processedLine);
  }
}

void File::ApplyLemmatization(const std::map<std::string, std::string>& lemmaMap) {
  lemmatizedText_.clear();
  
  for (const auto& line : textWithoutStopWords_) {
    std::vector<std::string> lemmatizedLine;
    for (const auto& word : line) {
      if (word == "---") {
        // Keep placeholder for stop words
        lemmatizedLine.push_back("---");
      } else {
        std::string lowerWord = ToLowerCase(word);
        auto it = lemmaMap.find(lowerWord);
        if (it != lemmaMap.end()) {
          // Apply lemmatization
          lemmatizedLine.push_back(it->second);
        } else {
          // Keep original word if no lemmatization rule found
          lemmatizedLine.push_back(word);
        }
      }
    }
    lemmatizedText_.push_back(lemmatizedLine);
  }
}

const std::vector<std::vector<std::string>>& File::GetOriginalText() const {
  return originalText_;
}

const std::vector<std::vector<std::string>>& File::GetTextWithoutStopWords() const {
  return textWithoutStopWords_;
}

const std::vector<std::vector<std::string>>& File::GetLemmatizedText() const {
  return lemmatizedText_;
}

void File::PrintOriginalText() const {
  std::cout << "\n=== ORIGINAL TEXT ===" << std::endl;
  for (size_t i = 0; i < originalText_.size(); ++i) {
    std::cout << "Line " << i + 1 << ": ";
    for (const auto& word : originalText_[i]) {
      std::cout << word << " ";
    }
    std::cout << std::endl;
  }
}

void File::PrintTextWithoutStopWords() const {
  std::cout << "\n=== TEXT WITHOUT STOP WORDS ===" << std::endl;
  for (size_t i = 0; i < textWithoutStopWords_.size(); ++i) {
    std::cout << "Line " << i + 1 << ": ";
    for (const auto& word : textWithoutStopWords_[i]) {
      std::cout << word << " ";
    }
    std::cout << std::endl;
  }
}

void File::PrintLemmatizedText() const {
  std::cout << "\n=== LEMMATIZED TEXT ===" << std::endl;
  for (size_t i = 0; i < lemmatizedText_.size(); ++i) {
    std::cout << "Line " << i + 1 << ": ";
    for (const auto& word : lemmatizedText_[i]) {
      std::cout << word << " ";
    }
    std::cout << std::endl;
  }
}

std::string File::ToLowerCase(const std::string& str) const {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), 
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}