#include <iostream>

#include "../include/file.h"
#include "../include/tools.h"

int main(const int argc, char* argv[]) {
  CommandLineArgs args = CheckArguments(argc, argv);

  const std::vector<std::string> textFiles = args.textFiles;
  std::string stopWordsFile = args.stopWordsFile;
  std::string lemmatizationFile = args.lemmatizationFile;

  // Load stop words and lemmatization rules
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "           LOADING CONFIGURATION FILES" << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  std::cout << "📄 Loading stop words from: " << stopWordsFile << std::endl;
  std::set<std::string> stopWords = LoadStopWords(stopWordsFile);
  std::cout << "✅ Loaded " << stopWords.size() << " stop words successfully." << std::endl;
  
  std::cout << "📄 Loading lemmatization rules from: " << lemmatizationFile << std::endl;
  std::map<std::string, std::string> lemmaRules = LoadLemmatizationRules(lemmatizationFile);
  std::cout << "✅ Loaded " << lemmaRules.size() << " lemmatization rules successfully." << std::endl;

  // Process each text file
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "            PROCESSING TEXT DOCUMENTS" << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  std::vector<File> files;
  for (const std::string& file : textFiles) {
    std::cout << "\n📖 Processing file: " << file << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    File tempFile(file); // Create File object for each text file
    
    // Process the file through all stages
    tempFile.RemoveStopWords(stopWords);
    tempFile.ApplyLemmatization(lemmaRules);
    tempFile.CalculateTF();
    files.push_back(tempFile);
    std::cout << "✅ File processed successfully." << std::endl;
  }
  
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "✅ Processing complete for " << files.size() << " files." << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  // Calculate IDF for the corpus
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "         CALCULATING TF-IDF METRICS" << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  std::cout << "📊 Calculating IDF values..." << std::endl;
  std::map<std::string, double> idfMap = CalculateIDF(files, files.size());
  std::cout << "✅ IDF calculation complete for " << idfMap.size() << " terms." << std::endl;
  
  // Calculate TF-IDF for each document
  std::cout << "📊 Calculating TF-IDF values..." << std::endl;
  for (auto& file : files) {
    file.CalculateTFIDF(idfMap);
  }
  std::cout << "✅ TF-IDF calculation complete for all documents." << std::endl;
  
  // Build vocabulary for indexing
  std::cout << "📚 Building vocabulary index..." << std::endl;
  std::map<std::string, int> vocabulary = BuildVocabulary(files);
  std::cout << "✅ Vocabulary built with " << vocabulary.size() << " unique terms." << std::endl;
  
  // Print results
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "              GENERATING RESULTS" << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  std::cout << "\n📋 GENERATING TF-IDF TABLES\n" << std::endl;
  for (size_t i = 0; i < files.size(); ++i) {
    std::cout << "📊 Generating table for document " << i << "..." << std::endl;
    files[i].PrintTFIDFTable(vocabulary, idfMap);
  }
  if (files.size() > 1) {
    std::cout << "\n📊 CALCULATING DOCUMENT SIMILARITIES\n" << std::endl;
    PrintSimilarityMatrix(files);
  } else {
    std::cout << "\n💡 Only one document provided - similarity analysis requires multiple documents." << std::endl;
  }

  return 0;
}