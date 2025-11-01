#include "../include/tools.h"

/**
 * @brief This function prints an error message for incorrect arguments
 * and provides usage information before exiting the program
 */
void ErrorOutput() {
  std::cerr << "¡ERROR! WRONG ARGUMENTS" << std::endl;
  std::cerr
      << "\nUsage: ./recommender-system -d <document1> <document2> ... -s "
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
  std::cout
      << "  -d <documents>        One or more text documents to analyze\n";
  std::cout << "  -s <stopWordsFile>    Path to file containing stop words\n";
  std::cout << "  -l <lemmatizationFile> Path to JSON file containing "
               "lemmatization rules\n";
  std::cout << "\nEXAMPLES\n" << std::endl;
  std::cout << "  ./recommender-system -d doc1.txt doc2.txt doc3.txt -s "
               "stopwords.txt -l corpus-en.json\n";
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

  if (argc == 2) {
    std::string firstArg = argv[1];
    if (firstArg == "--help" || firstArg == "-h") {
      HelpOutput();
    } else {
      ErrorOutput();
    }
  }

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
      i++;
      while (i < argc && argv[i][0] != '-') {
        std::string filename = argv[i];
        std::ifstream file(filename);
        if (!file.is_open()) {
          std::cerr << "Error: Cannot open document file '" << filename << "'."
                    << std::endl;
          exit(1);
        }
        args.textFiles.push_back(filename);
        file.close();
        i++;
      }
      i--;
      if (args.textFiles.empty()) {
        std::cerr << "Error: No document files specified after -d option"
                  << std::endl;
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
        std::cerr << "Error: Cannot open stop words file '" << stopWordsFile
                  << "'." << std::endl;
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
        std::cerr << "Error: Cannot open lemmatization file '"
                  << lemmatizationFile << "'." << std::endl;
        exit(1);
      }
      args.lemmatizationFile = lemmatizationFile;
      file.close();
    } else {
      std::cerr << "Error: Unknown option '" << currentArg << "'" << std::endl;
      ErrorOutput();
    }
  }

  if (!hasDocuments || !hasStopWords || !hasLemmatization) {
    std::cerr << "Error: Missing required options. All of -d, -s, and -l must "
                 "be specified."
              << std::endl;
    ErrorOutput();
  }

  return args;
}