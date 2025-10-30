#include "../include/tools.h"

#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>

/**
 * @brief This function prints an error message for incorrect arguments
 * and provides usage information before exiting the program
 */
void ErrorOutput() {
  std::cerr << "¡ERROR! WRONG ARGUMENTS" << std::endl;
  std::cerr << "\nUsage: ./recommender-system -d <document1> <document2> ... -s "
               "<stopWordsFile> -l <lemmatizationFile.json>"
            << std::endl;
  std::cerr << "Try './recommender-system [--help | -h]' for more information."
            << std::endl;
  exit(1);
}

/**
 * @brief This function prints detailed help information about the program
 * including its functionality, usage, and options, then exits the program
 */
void HelpOutput() {
  std::cout << "=============================================================="
            << std::endl;
  std::cout << "           CONTENT-BASED RECOMMENDER SYSTEM                "
            << std::endl;
  std::cout << "=============================================================="
            << std::endl;
  std::cout << "\nPROGRAM DESCRIPTION\n" << std::endl;
  std::cout << "  This program implements a content-based recommender system\n"
               "  that analyzes text documents to provide recommendations\n"
               "  based on content similarity.\n"
            << std::endl;
  std::cout << "USAGE\n" << std::endl;
  std::cout << "  ./recommender-system -d <document1> <document2> ... -s "
               "<stopWordsFile> -l <lemmatizationFile>\n"
            << std::endl;
  std::cout << "OPTIONS\n" << std::endl;
  std::cout << "  -d <documents>        One or more text documents to analyze\n";
  std::cout << "  -s <stopWordsFile>    Path to file containing stop words\n";
  std::cout << "  -l <lemmatizationFile> Path to JSON file containing lemmatization rules\n";
  std::cout << "\nEXAMPLES\n" << std::endl;
  std::cout << "  ./recommender-system -d doc1.txt doc2.txt doc3.txt -s stopwords.txt -l corpus-en.json\n";
  std::cout << "\nFor more information, use: ./recommender-system --help\n";
  std::cout << "=============================================================="
            << std::endl;
  exit(0);
}

/**
 * @brief This function checks the command line arguments and ensures they
 * are valid for the program's execution.
 * This function calls other helper functions to validate arguments and perform
 * the required operations.
 * @param argc - Number of command line arguments
 * @param argv - Array of command line arguments
 */
CommandLineArgs CheckArguments(int argc, char* argv[]) {
  CommandLineArgs args;
  
  if (argc == 1) {
    ErrorOutput();
  }
  
  // Check for help option
  if (argc == 2) {
    std::string firstArg = argv[1];
    if (firstArg == "--help" || firstArg == "-h") {
      HelpOutput();
    } else {
      ErrorOutput();
    }
  }
  
  // Minimum required arguments: program -d doc1 -s stopfile -l lemmafile = 7
  if (argc < 7) {
    ErrorOutput();
  }
  
  bool hasDocuments = false, hasStopWords = false, hasLemmatization = false;
  
  for (int i = 1; i < argc; i++) {
    std::string currentArg = argv[i];
    
    if (currentArg == "-d") {
      if (hasDocuments) {
        std::cerr << "Error: -d option specified multiple times" << std::endl;
        ErrorOutput();
      }
      hasDocuments = true;
      i++; // Move to next argument
      
      // Read document files until we hit another option or end of arguments
      while (i < argc && argv[i][0] != '-') {
        std::string filename = argv[i];
        std::ifstream file(filename);
        if (!file.is_open()) {
          std::cerr << "Error: Cannot open document file '" << filename << "'." << std::endl;
          exit(1);
        }
        args.textFiles.push_back(filename);
        file.close();
        i++;
      }
      i--; // Adjust for the for loop increment
      
      if (args.textFiles.empty()) {
        std::cerr << "Error: No document files specified after -d option" << std::endl;
        ErrorOutput();
      }
      
    } else if (currentArg == "-s") {
      if (hasStopWords) {
        std::cerr << "Error: -s option specified multiple times" << std::endl;
        ErrorOutput();
      }
      if (i + 1 >= argc) {
        std::cerr << "Error: -s option requires a filename" << std::endl;
        ErrorOutput();
      }
      hasStopWords = true;
      i++;
      std::string stopWordsFile = argv[i];
      std::ifstream file(stopWordsFile);
      if (!file.is_open()) {
        std::cerr << "Error: Cannot open stop words file '" << stopWordsFile << "'." << std::endl;
        exit(1);
      }
      args.stopWordsFile = stopWordsFile;
      file.close();
      
    } else if (currentArg == "-l") {
      if (hasLemmatization) {
        std::cerr << "Error: -l option specified multiple times" << std::endl;
        ErrorOutput();
      }
      if (i + 1 >= argc) {
        std::cerr << "Error: -l option requires a filename" << std::endl;
        ErrorOutput();
      }
      hasLemmatization = true;
      i++;
      std::string lemmatizationFile = argv[i];
      std::ifstream file(lemmatizationFile);
      if (!file.is_open()) {
        std::cerr << "Error: Cannot open lemmatization file '" << lemmatizationFile << "'." << std::endl;
        exit(1);
      }
      args.lemmatizationFile = lemmatizationFile;
      file.close();
      
    } else {
      std::cerr << "Error: Unknown option '" << currentArg << "'" << std::endl;
      ErrorOutput();
    }
  }
  
  // Check that all required options were provided
  if (!hasDocuments || !hasStopWords || !hasLemmatization) {
    std::cerr << "Error: Missing required options. All of -d, -s, and -l must be specified." << std::endl;
    ErrorOutput();
  }
  
  return args;
}

/**
 * @brief Loads stop words from a file into a set
 * @param filename Path to the stop words file
 * @return Set containing all stop words in lowercase
 */
std::set<std::string> LoadStopWords(const std::string& filename) {
  std::set<std::string> stopWords;
  std::ifstream file(filename);
  
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open stop words file '" << filename << "'." << std::endl;
    exit(1);
  }
  
  std::string word;
  while (file >> word) {
    // Convert to lowercase for case-insensitive matching
    std::transform(word.begin(), word.end(), word.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    stopWords.insert(word);
  }
  
  file.close();
  return stopWords;
}

/**
 * @brief Loads lemmatization rules from a JSON file into a map
 * Expected format: JSON object with key-value pairs {"original_word": "lemmatized_word"}
 * @param filename Path to the JSON lemmatization file
 * @return Map with original words as keys and lemmatized words as values
 */
std::map<std::string, std::string> LoadLemmatizationRules(const std::string& filename) {
  std::map<std::string, std::string> lemmaMap;
  std::ifstream file(filename);
  
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open lemmatization file '" << filename << "'." << std::endl;
    exit(1);
  }
  
  std::string content;
  std::string line;
  
  // Read entire file content
  while (std::getline(file, line)) {
    content += line;
  }
  file.close();
  
  // Simple JSON parser for key-value pairs
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
      // Found start of key
      size_t keyStart = pos + 1;
      size_t keyEnd = content.find('"', keyStart);
      
      if (keyEnd == std::string::npos) {
        std::cerr << "Error: Malformed JSON in lemmatization file" << std::endl;
        exit(1);
      }
      
      std::string key = content.substr(keyStart, keyEnd - keyStart);
      
      // Find the colon
      pos = content.find(':', keyEnd);
      if (pos == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing colon" << std::endl;
        exit(1);
      }
      
      // Find start of value
      pos = content.find('"', pos);
      if (pos == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing value quote" << std::endl;
        exit(1);
      }
      
      size_t valueStart = pos + 1;
      size_t valueEnd = content.find('"', valueStart);
      
      if (valueEnd == std::string::npos) {
        std::cerr << "Error: Malformed JSON - missing closing value quote" << std::endl;
        exit(1);
      }
      
      std::string value = content.substr(valueStart, valueEnd - valueStart);
      
      // Convert both to lowercase for consistent matching
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

/**
 * @brief Builds a vocabulary (term -> index mapping) from all documents
 * @param files Vector of File objects
 * @return Map with term as key and index as value
 */
std::map<std::string, int> BuildVocabulary(const std::vector<File>& files) {
  std::set<std::string> uniqueTerms;
  
  // Collect all unique terms from all documents
  for (const auto& file : files) {
    const auto& tf = file.GetTF();
    for (const auto& termFreq : tf) {
      uniqueTerms.insert(termFreq.first);
    }
  }
  
  // Create indexed vocabulary
  std::map<std::string, int> vocabulary;
  int index = 0;
  for (const auto& term : uniqueTerms) {
    vocabulary[term] = index++;
  }
  
  return vocabulary;
}

/**
 * @brief Calculates IDF (Inverse Document Frequency) for all terms
 * @param files Vector of File objects
 * @param totalDocs Total number of documents
 * @return Map with term as key and IDF value
 */
std::map<std::string, double> CalculateIDF(const std::vector<File>& files, int totalDocs) {
  std::map<std::string, int> documentFrequency;
  
  // Count in how many documents each term appears
  for (const auto& file : files) {
    const auto& tf = file.GetTF();
    std::set<std::string> termsInDoc;
    
    for (const auto& termFreq : tf) {
      termsInDoc.insert(termFreq.first);
    }
    
    for (const auto& term : termsInDoc) {
      documentFrequency[term]++;
    }
  }
  
  // Calculate IDF for each term
  std::map<std::string, double> idfMap;
  for (const auto& dfPair : documentFrequency) {
    const std::string& term = dfPair.first;
    int df = dfPair.second;
    // Using smoothed IDF: log((N + 1) / (df + 1))
    double idf = std::log((double)(totalDocs + 1) / (df + 1));
    idfMap[term] = idf;
  }
  
  return idfMap;
}

/**
 * @brief Calculates cosine similarity between two TF-IDF vectors
 * @param tfidf1 TF-IDF map for document 1
 * @param tfidf2 TF-IDF map for document 2
 * @return Cosine similarity value (0 to 1)
 */
double CalculateCosineSimilarity(const std::map<std::string, double>& tfidf1, 
                                  const std::map<std::string, double>& tfidf2) {
  double dotProduct = 0.0;
  double norm1 = 0.0;
  double norm2 = 0.0;
  
  // Calculate dot product and norm for tfidf1
  for (const auto& term1 : tfidf1) {
    const std::string& term = term1.first;
    double value1 = term1.second;
    
    norm1 += value1 * value1;
    
    auto it2 = tfidf2.find(term);
    if (it2 != tfidf2.end()) {
      dotProduct += value1 * it2->second;
    }
  }
  
  // Calculate norm for tfidf2
  for (const auto& term2 : tfidf2) {
    double value2 = term2.second;
    norm2 += value2 * value2;
  }
  
  // Avoid division by zero
  if (norm1 == 0.0 || norm2 == 0.0) {
    return 0.0;
  }
  
  return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

/**
 * @brief Prints a similarity matrix showing cosine similarity between all document pairs
 * @param files Vector of File objects
 */
void PrintSimilarityMatrix(const std::vector<File>& files) {
  int n = files.size();
  
  std::cout << "\n" << std::string(80, '=') << std::endl;
  std::cout << "COSINE SIMILARITY MATRIX" << std::endl;
  std::cout << std::string(80, '=') << std::endl;
  
  // Print header
  std::cout << std::setw(20) << " ";
  for (int j = 0; j < n; j++) {
    std::cout << std::setw(15) << "Doc " + std::to_string(j);
  }
  std::cout << std::endl;
  std::cout << std::string(80, '-') << std::endl;
  
  // Print matrix
  for (int i = 0; i < n; i++) {
    std::cout << std::setw(20) << ("Document " + std::to_string(i));
    
    for (int j = 0; j < n; j++) {
      if (i == j) {
        std::cout << std::setw(15) << std::fixed << std::setprecision(6) << 1.0;
      } else {
        double similarity = CalculateCosineSimilarity(
          files[i].GetTFIDF(), 
          files[j].GetTFIDF()
        );
        std::cout << std::setw(15) << std::fixed << std::setprecision(6) << similarity;
      }
    }
    std::cout << std::endl;
  }
  
  std::cout << std::string(80, '=') << std::endl;
}