#include "../include/document.h"

/**
 * @brief Constructor for Document class
 * @param inputDocument Path to the input document file
 */
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

/**
 * @brief Remove stop words from the document
 * @param stopWords Set of stop words to remove
 */
void Document::RemoveStopWords(const std::set<std::string> &stopWords) {
  std::vector<std::vector<std::string>> filtered;

  for (const auto &line : simplifiedText_) {
    std::vector<std::string> processedLine;
    for (const auto &word : line) {
      if (stopWords.find(word) == stopWords.end()) {
        processedLine.push_back(word);
      } else {
        processedLine.push_back("");
      }
    }
    filtered.push_back(processedLine);
  }
  simplifiedText_ = filtered;
}

/**
 * @brief Clean tokens in the document by removing non-alphanumeric characters
 *        and converting to lowercase
 */
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

/**
 * @brief Clean a single token by removing non-alphanumeric characters
 *        and converting to lowercase
 * @param token The input token to clean
 * @return The cleaned token
 */
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

/**
 * @brief Perform lemmatization on the document using the provided lemma map
 * @param lemmaMap Map of words to their lemmas
 */
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

/**
 * @brief Getter for original text
 * @return Reference to the original text vector
 */
const std::vector<std::vector<std::string>> &Document::originalText() const {
  return originalText_;
}

/**
 * @brief Getter for simplified text
 * @return Reference to the simplified text vector
 */
const std::vector<std::vector<std::string>> &Document::simplifiedText() const {
  return simplifiedText_;
}

/**
 * @brief Getter for Term Frequency Normalized (TFNormalized) map
 * @return Map of terms to their normalized TF values
 */
const std::map<std::string, double> &Document::TFNormalized() const {
  return TFNormalized_;
}

/**
 * @brief Setter for all words in corpus
 * @param allWordsInCorpus Set of all unique words in the corpus
 */
void Document::setAllWordsInCorpus(
    const std::set<std::string> &allWordsInCorpus) {
  allWordsInCorpus_ = allWordsInCorpus;
}

/**
 * @brief Setter for lemmatization map
 * @param lemmatizationMap Map of words to their lemmas
 */
void Document::setLemmatizationMap(
    const std::map<std::string, std::string> &lemmatizationMap) {
  lemmatizationMap_ = lemmatizationMap;
}

/**
 * @brief Convert a string to lowercase
 * @param str Input string
 * @return Lowercase version of the input string
 */
std::string Document::ToLowerCase(const std::string &str) const {
  std::string result = str;
  for (size_t i = 0; i < result.length(); i++) {
    result[i] = std::tolower(static_cast<unsigned char>(result[i]));
  }
  return result;
}

/**
 * @brief Calculate Term Frequency (TF) for all terms in the document
 */
void Document::CalculateTF() {
  TF_.clear();
  for (const std::string &word : allWordsInCorpus_) {
    TF_[word] = 0.0;
  }
  for (const std::vector<std::string> &line : simplifiedText_) {
    for (const std::string &word : line) {
      if (!word.empty()) {
        TF_[word] += 1.0;
      }
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

/**
 * @brief Calculate the index of the first occurrence of each term in the simplified text
 */
void Document::CalculateTermIndices() {
  termIndices_.clear();
  
  for (const std::string &word : allWordsInCorpus_) {
    termIndices_[word] = std::make_pair(-1, -1);
  }
  
  for (size_t row = 0; row < simplifiedText_.size(); ++row) {
    for (size_t col = 0; col < simplifiedText_[row].size(); ++col) {
      const std::string &term = simplifiedText_[row][col];
      if (term.empty()) {
        continue;
      }
      if (allWordsInCorpus_.find(term) != allWordsInCorpus_.end() && 
          termIndices_[term].first == -1) {
        termIndices_[term] = std::make_pair(static_cast<int>(row), static_cast<int>(col));
      }
    }
  }
}

/**
 * @brief Calculate the length of the TF vector
 */
void Document::CalculateVectorLength() {
  double sumSquares = 0.0;
  for (const auto &termFreq : TF_) {
    sumSquares += termFreq.second * termFreq.second;
  }
  vectorLength_ = std::sqrt(sumSquares);
}

/**
 * @brief Calculate Normalized Term Frequency (TFNormalized) for all terms
 */
void Document::CalculateTFNormalized() {
  TFNormalized_.clear();
  for (const auto &termFreq : TF_) {
    TFNormalized_[termFreq.first] = termFreq.second / vectorLength_;
  }
}

/**
 * @brief Overloaded output operator for Document
 * @param os Output stream
 * @param doc Document object
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &os, const Document &doc) {
  os << "Vector Length: " << doc.vectorLength() << std::endl;

  return os;
}