#include "../include/file.h"
#include <algorithm>
#include <cctype>
#include <iomanip>

File::File(const std::string& inputFile) : fileName_(inputFile) {
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
  for (size_t i = 0; i < result.length(); i++) {
    result[i] = std::tolower(static_cast<unsigned char>(result[i]));
  }
  return result;
}

std::string File::CleanToken(const std::string& token) const {
  std::string cleaned = token;
  // Remove punctuation from beginning and end
  while (!cleaned.empty() && !std::isalnum(cleaned.front())) {
    cleaned.erase(cleaned.begin());
  }
  while (!cleaned.empty() && !std::isalnum(cleaned.back())) {
    cleaned.pop_back();
  }
  return ToLowerCase(cleaned);
}

void File::CalculateTF() {
  tf_.clear();
  
  // Count term frequencies from lemmatized text
  for (const auto& line : lemmatizedText_) {
    for (const auto& word : line) {
      if (word == "---" || word.empty()) continue;
      std::string cleanedWord = CleanToken(word);
      if (!cleanedWord.empty()) {
        tf_[cleanedWord]++;
      }
    }
  }
}

void File::CalculateTFIDF(const std::map<std::string, double>& idfMap) {
  tfidf_.clear();
  
  for (const auto& termFreq : tf_) {
    const std::string& term = termFreq.first;
    int tf = termFreq.second;
    auto idfIt = idfMap.find(term);
    if (idfIt != idfMap.end()) {
      double idf = idfIt->second;
      tfidf_[term] = tf * idf;
    }
  }
}

const std::map<std::string, int>& File::GetTF() const {
  return tf_;
}

const std::map<std::string, double>& File::GetTFIDF() const {
  return tfidf_;
}

std::string File::GetFileName() const {
  return fileName_;
}

void File::PrintTFIDFTable(const std::map<std::string, int>& vocabulary,
                           const std::map<std::string, double>& idfMap) const {

  std::cout << "\n" << std::string(90, '=') << std::endl;
  std::cout << "TF-IDF TABLE FOR: " << fileName_ << std::endl;
  std::cout << std::string(90, '=') << std::endl;
  std::cout << std::left << std::setw(8) << "Index" 
            << std::setw(20) << "Term" 
            << std::setw(10) << "TF"
            << std::setw(15) << "IDF"
            << std::setw(15) << "TF-IDF" << std::endl;
  std::cout << std::string(90, '-') << std::endl;
  
  for (const auto& vocabTerm : vocabulary) {
    const std::string& term = vocabTerm.first;
    int index = vocabTerm.second;
    
    auto tfIt = tf_.find(term);
    int tf = (tfIt != tf_.end()) ? tfIt->second : 0;
    
    auto idfIt = idfMap.find(term);
    double idf = (idfIt != idfMap.end()) ? idfIt->second : 0.0;
    
    auto tfidfIt = tfidf_.find(term);
    double tfidf = (tfidfIt != tfidf_.end()) ? tfidfIt->second : 0.0;
    
    if (tf > 0) {  // Only show terms that appear in this document
      std::cout << std::left << std::setw(8) << index
                << std::setw(20) << term
                << std::setw(10) << tf
                << std::setw(15) << std::fixed << std::setprecision(6) << idf
                << std::setw(15) << std::fixed << std::setprecision(6) << tfidf
                << std::endl;
    }
  }
  std::cout << std::string(90, '=') << std::endl;
}