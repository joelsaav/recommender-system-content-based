#include "../include/documentManager.h"
#include "../include/tools.h"

int main(const int argc, char* argv[]) {
  CommandLineArgs args = CheckArguments(argc, argv);

  std::vector<std::string> documentFiles = args.textFiles;
  std::string stopWordsFile = args.stopWordsFile;
  std::string lemmatizationFile = args.lemmatizationFile;
  std::cout << "Stop Words File: " << stopWordsFile << std::endl;
  std::cout << "Lemmatization File: " << lemmatizationFile << std::endl;

  for (const std::string& file : documentFiles) {
    std::cout << "Document File: " << file << std::endl;
  }

  DocumentManager dm(documentFiles, stopWordsFile, lemmatizationFile);
  dm.Recommend();
  dm.PrintSimilarityMatrix();
  std::cout << dm << std::endl;
  return 0;
}