#include <iostream>

#include "../include/file.h"
#include "../include/tools.h"

int main(const int argc, char* argv[]) {
  CommandLineArgs args = CheckArguments(argc, argv);

  const std::vector<std::string> textFiles = args.textFiles;
  std::string stopWordsFile = args.stopWordsFile;
  std::string lemmatizationFile = args.lemmatizationFile;

  // Load stop words and lemmatization rules
  std::cout << "Loading stop words from: " << stopWordsFile << std::endl;
  std::set<std::string> stopWords = LoadStopWords(stopWordsFile);
  std::cout << "Loaded " << stopWords.size() << " stop words." << std::endl;
  
  std::cout << "Loading lemmatization rules from: " << lemmatizationFile << std::endl;
  std::map<std::string, std::string> lemmaRules = LoadLemmatizationRules(lemmatizationFile);
  std::cout << "Loaded " << lemmaRules.size() << " lemmatization rules." << std::endl;

  // Process each text file
  std::vector<File> files;
  for (const std::string& file : textFiles) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Processing file: " << file << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    File tempFile(file); // Create File object for each text file
    
    // Process the file through all stages
    tempFile.RemoveStopWords(stopWords);
    tempFile.ApplyLemmatization(lemmaRules);
    
    // Calculate TF for this document
    tempFile.CalculateTF();
    
    files.push_back(tempFile);
  }
  
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "Processing complete for " << files.size() << " files." << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  
  // Calculate IDF for the corpus
  std::cout << "\nCalculating IDF values..." << std::endl;
  std::map<std::string, double> idfMap = CalculateIDF(files, files.size());
  std::cout << "IDF calculation complete for " << idfMap.size() << " terms." << std::endl;
  
  // Calculate TF-IDF for each document
  std::cout << "\nCalculating TF-IDF values..." << std::endl;
  for (auto& file : files) {
    file.CalculateTFIDF(idfMap);
  }
  
  // Build vocabulary for indexing
  std::map<std::string, int> vocabulary = BuildVocabulary(files);
  
  // Print TF-IDF tables for each document
  std::cout << "\n\nGENERATING TF-IDF TABLES\n" << std::endl;
  for (const auto& file : files) {
    file.PrintTFIDFTable(vocabulary, idfMap);
  }
  
  // Print similarity matrix
  if (files.size() > 1) {
    std::cout << "\n\nCALCULATING DOCUMENT SIMILARITIES\n" << std::endl;
    PrintSimilarityMatrix(files);
  }
    
  return 0;
}