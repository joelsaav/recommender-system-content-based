#include "../include/documentManager.h"
#include "../include/tools.h"

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 */
int main(const int argc, char* argv[]) {
  CommandLineArgs args = CheckArguments(argc, argv);

  std::vector<std::string> documentFiles = args.textFiles;
  std::string stopWordsFile = args.stopWordsFile;
  std::string lemmatizationFile = args.lemmatizationFile;

  std::cout << "=============================== INPUT ARGUMENTS "
               "================================\n"
            << std::endl;
  std::cout << "•Documents:\n";
  for (const std::string& file : documentFiles) {
    std::cout << "  - " << file << std::endl;
  }
  std::cout << std::endl;
  std::cout << "•Stop Words File: " << stopWordsFile << std::endl;
  std::cout << "•Lemmatization File: " << lemmatizationFile << std::endl;

  DocumentManager dm(documentFiles, stopWordsFile, lemmatizationFile);
  dm.Recommend();
  std::cout << dm << std::endl;
  return 0;
}