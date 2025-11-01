#include "../include/document.h"

#include <algorithm>
#include <cctype>
#include <iomanip>

Document::Document(const std::string &inputDocument)
    : documentName_(inputDocument) {
  std::ifstream file{inputDocument};
  if (!file.is_open()) {
    std::cerr << "Error opening document: " << inputDocument << std::endl;
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

  simplifiedText_ = originalText_;
}

void Document::RemoveStopWords(const std::set<std::string> &stopWords) {
  std::vector<std::vector<std::string>> filtered;

  for (const auto &line : simplifiedText_) {
    std::vector<std::string> processedLine;
    for (const auto &word : line) {
      if (stopWords.find(word) == stopWords.end()) {
        processedLine.push_back(word);
      }
    }
    if (!processedLine.empty()) {
      filtered.push_back(processedLine);
    }
  }
  simplifiedText_ = filtered;
}

void Document::CleanTokens() {
  std::vector<std::vector<std::string>> cleanedText;
  cleanedText.reserve(simplifiedText_.size());
  for (const auto &line : simplifiedText_) {
    std::vector<std::string> cleanedLine;
    cleanedLine.reserve(line.size());
    for (const auto &word : line) {
      std::string cleaned = CleanToken(word);
      if (!cleaned.empty()) cleanedLine.push_back(cleaned);
    }
    cleanedText.push_back(std::move(cleanedLine));
  }
  simplifiedText_ = std::move(cleanedText);
}

std::string Document::CleanToken(const std::string &token) const {
  std::string result;
  result.reserve(token.size());
  for (unsigned char c : token) {
    if (std::isalnum(c)) {
      result.push_back(static_cast<char>(std::tolower(c)));
    }
  }
  return result;
}

void Document::Lemmatization(
    const std::map<std::string, std::string> &lemmaMap) {
  std::vector<std::vector<std::string>> lemmatized;
  for (const auto &line : simplifiedText_) {
    std::vector<std::string> lemmatizedLine;
    for (const auto &word : line) {
      auto it = lemmaMap.find(word);
      if (it != lemmaMap.end()) {
        lemmatizedLine.push_back(it->second);
      } else {
        lemmatizedLine.push_back(word);
      }
    }
    lemmatized.push_back(lemmatizedLine);
  }
  simplifiedText_ = lemmatized;
}

const std::vector<std::vector<std::string>> &Document::originalText() const {
  return originalText_;
}

const std::vector<std::vector<std::string>> &Document::simplifiedText() const {
  return simplifiedText_;
}

const std::map<std::string, double> &Document::TFNormalized() const {
  return TFNormalized_;
}

void Document::setAllWordsInCorpus(
    const std::set<std::string> &allWordsInCorpus) {
  allWordsInCorpus_ = allWordsInCorpus;
}

std::string Document::ToLowerCase(const std::string &str) const {
  std::string result = str;
  for (size_t i = 0; i < result.length(); i++) {
    result[i] = std::tolower(static_cast<unsigned char>(result[i]));
  }
  return result;
}

void Document::CalculateTF() {
  TF_.clear();
  for (const std::string &word : allWordsInCorpus_) {
    TF_[word] = 0.0;
  }
  for (const std::vector<std::string> &line : simplifiedText_) {
    for (const std::string &word : line) {
      TF_[word] += 1.0;
    }
  }
  for (auto &termFreq : TF_) {
    if (termFreq.second > 0) {
      termFreq.second = 1 + log10(termFreq.second);
    } else {
      termFreq.second = 0.0;
    }
  }
}

void Document::CalculateVectorLength() {
  double sumSquares = 0.0;
  for (const auto &termFreq : TF_) {
    sumSquares += termFreq.second * termFreq.second;
  }
  vectorLength_ = std::sqrt(sumSquares);
}

void Document::CalculateTFNormalized() {
  TFNormalized_.clear();
  for (const auto &termFreq : TF_) {
    TFNormalized_[termFreq.first] = termFreq.second / vectorLength_;
  }
}

std::ostream &operator<<(std::ostream &os, const Document &doc) {
  os << "Vector Length: " << doc.vectorLength() << std::endl;

  return os;
}