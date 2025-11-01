#include "../include/documentManager.h"

#include <iomanip>

DocumentManager::DocumentManager(const std::vector<std::string>& documents,
                                 const std::string& stopWordsFile,
                                 const std::string& lemmatizationFile) {
  std::ifstream stopWordsStream{stopWordsFile};
  if (!stopWordsStream.is_open()) {
    std::cerr << "Error opening stop words file: " << stopWordsFile
              << std::endl;
    exit(1);
  }
  std::string word;
  while (stopWordsStream >> word) {
    stopWords_.insert(word);
  }
  stopWordsStream.close();
  lemmatizationMap_ = LoadLemmatizationRules(lemmatizationFile);

  for (const auto& document : documents) {
    Document doc{document};
    doc.CleanTokens();
    doc.Lemmatization(lemmatizationMap_);
    doc.RemoveStopWords(stopWords_);
    documents_.push_back(doc);
    for (const auto& line : doc.simplifiedText()) {
      for (const auto& word : line) {
        allWordsInCorpus_.insert(word);
      }
    }
  }

  for (Document& doc : documents_) {
    doc.setAllWordsInCorpus(allWordsInCorpus_);
  }

  CountDocumentsOccurrences();
}

std::map<std::string, int> DocumentManager::documentsOccurrences() const {
  return documentsOccurrences_;
}

std::map<std::string, std::string> DocumentManager::lemmatizationMap() const {
  return lemmatizationMap_;
}

std::map<std::string, std::string> DocumentManager::LoadLemmatizationRules(
    const std::string& lemmatizationFile) {
  std::map<std::string, std::string> lemmaMap;
  std::ifstream file(lemmatizationFile);

  if (!file.is_open()) {
    std::cerr << "Error: Cannot open lemmatization file '" << lemmatizationFile
              << "'." << std::endl;
    exit(1);
  }

  std::string content;
  std::string line;

  while (std::getline(file, line)) {
    content += line;
  }
  file.close();
  size_t pos = 0;
  bool inObject = false;
  while (pos < content.length()) {
    char c = content[pos];
    if (c == '{') {
      inObject = true;
      pos++;
      continue;
    }
    if (c == '}') {
      break;
    }
    if (inObject && c == '"') {
      size_t keyStart = pos + 1;
      size_t keyEnd = content.find('"', keyStart);
      if (keyEnd == std::string::npos) {
        std::cerr << "Error: Malformed JSON in lemmatization file" << std::endl;
        exit(1);
      }
      std::string key = content.substr(keyStart, keyEnd - keyStart);
      pos = content.find(':', keyEnd);
      if (pos == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing colon" << std::endl;
        exit(1);
      }
      pos = content.find('"', pos);
      if (pos == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing value quote" << std::endl;
        exit(1);
      }
      size_t valueStart = pos + 1;
      size_t valueEnd = content.find('"', valueStart);
      if (valueEnd == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing closing value quote"
                  << std::endl;
        exit(1);
      }
      std::string value = content.substr(valueStart, valueEnd - valueStart);
      std::transform(key.begin(), key.end(), key.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      std::transform(value.begin(), value.end(), value.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      lemmaMap[key] = value;
      pos = valueEnd + 1;
    } else {
      pos++;
    }
  }

  return lemmaMap;
}

void DocumentManager::CountDocumentsOccurrences() {
  documentsOccurrences_.clear();

  for (const Document& doc : documents_) {
    std::set<std::string> uniqueTerms;

    const std::vector<std::vector<std::string>>& simplifiedText =
        doc.simplifiedText();

    for (const std::vector<std::string>& line : simplifiedText) {
      for (const std::string& word : line) {
        uniqueTerms.insert(word);
      }
    }

    for (const std::string& term : uniqueTerms) {
      ++documentsOccurrences_[term];
    }
  }
}

void DocumentManager::CalculateIDF() {
  for (const std::string& word : allWordsInCorpus_) {
    int docCount = documentsOccurrences_[word];
    if (docCount > 0) {
      IDF_[word] = log10(static_cast<double>(TotalDocuments()) /
                         static_cast<double>(docCount));
    } else {
      IDF_[word] = 0.0;
    }
  }
}

void DocumentManager::Recommend() {
  for (Document& doc : documents_) {
    doc.CalculateTF();
    doc.CalculateVectorLength();
    doc.CalculateTFNormalized();
  }
  CalculateIDF();
  CalculateCosineSimilarity();
}

void DocumentManager::CalculateCosineSimilarity() {
  int n = documents_.size();
  similarityMatrix_.clear();
  similarityMatrix_.resize(n, std::vector<double>(n, 0.0));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (i == j) {
        similarityMatrix_[i][j] = 1.0;
      } else {
        double dotProduct = 0.0;
        const std::map<std::string, double>& tfNorm_i =
            documents_[i].TFNormalized();
        const std::map<std::string, double>& tfNorm_j =
            documents_[j].TFNormalized();

        for (const std::string& word : allWordsInCorpus_) {
          double tf_i = 0.0;
          double tf_j = 0.0;

          auto it_i = tfNorm_i.find(word);
          if (it_i != tfNorm_i.end()) {
            tf_i = it_i->second;
          }

          auto it_j = tfNorm_j.find(word);
          if (it_j != tfNorm_j.end()) {
            tf_j = it_j->second;
          }

          dotProduct += tf_i * tf_j;
        }

        similarityMatrix_[i][j] = dotProduct;
      }
    }
  }
}

void DocumentManager::PrintSimilarityMatrix() const {
  int n = documents_.size();

  std::cout << "\n" << std::string(80, '=') << std::endl;
  std::cout << "COSINE SIMILARITY MATRIX" << std::endl;
  std::cout << std::string(80, '=') << std::endl;

  std::cout << std::setw(12) << " ";
  for (int i = 0; i < n; ++i) {
    std::cout << std::setw(12) << "Doc " << i + 1;
  }
  std::cout << std::endl;
  std::cout << std::string(80, '-') << std::endl;

  for (int i = 0; i < n; ++i) {
    std::cout << std::setw(10) << "Doc " << i + 1 << ": ";
    for (int j = 0; j < n; ++j) {
      std::cout << std::setw(12) << std::fixed << std::setprecision(6)
                << similarityMatrix_[i][j];
    }
    std::cout << std::endl;
  }
  std::cout << std::string(80, '=') << std::endl;
}

std::ostream& operator<<(std::ostream& os, const DocumentManager& dm) {
  os << std::endl << std::endl;
  os << "=== DOCUMENTS SUMMARY ===" << std::endl;
  os << "Total Documents: " << dm.TotalDocuments() << std::endl;
  for (const Document& doc : dm.documents()) {
    os << "Document 1\n" << doc.documentName() << std::endl;
  }
  return os;
}