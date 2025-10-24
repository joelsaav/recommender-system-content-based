#include "../include/tools.h"

#include <fstream>
#include <set>

/**
 * @brief This function prints an error message for incorrect arguments
 * and provides usage information before exiting the program
 */
void ErrorOutput() {
  std::cerr << "¡ERROR! WRONG ARGUMENTS" << std::endl;
  std::cerr << "\nUsage: ./recommender-system -mx <matrixFile> -mc "
               "<metric> -nb <numNeighbors> -pd <predictionType>"
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
  std::cout << "                  RECOMMENDER SYSTEM                       "
            << std::endl;
  std::cout << "=============================================================="
            << std::endl;
  std::cout << "\nPROGRAM DESCRIPTION\n" << std::endl;
  std::cout << "  This program implements a recommender system using\n"
               "  collaborative filtering methods. It provides item\n"
               "  recommendations based on user-item interaction data.\n"
            << std::endl;
  std::cout << "USAGE\n" << std::endl;
  std::cout << "  ./recommender-system -mx <matrixFile> -mc <metric> -nb "
               "<numNeighbors> -pd <predictionType>\n"
            << std::endl;
  std::cout << "OPTIONS\n" << std::endl;
  std::cout << "  -mx <matrixFile>      Path to file containing the user-item "
               "matrix\n";
  std::cout << "  -mc <metric>           Similarity metric (integer):\n"
               "                          1: Pearson Correlation\n"
               "                          2: Cosine Distance\n"
               "                          3: Euclidean Distance\n";
  std::cout << "  -nb <numNeighbors>    Number of neighbors to consider for "
               "predictions. Must be at least 3.\n";
  std::cout << "  -pd <predictionType>  Type of prediction (integer):\n"
               "                          1: Simple Prediction\n"
               "                          2: Mean Difference Prediction\n";
  std::cout << "\nEXAMPLES\n" << std::endl;
  std::cout << "  ./recommender-system -mx data.txt -mc 1 -nb 5 -pd 2\n";
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
  switch (argc) {
    case 1:
      ErrorOutput();
      break;
    case 9: {
      std::string firstArg = argv[1];
      if (firstArg == "--help" || firstArg == "-h") {
        HelpOutput();
      } else {
        std::set<std::string> optionsList = {"-mx", "-mc", "-nb", "-pd"};
        std::set<std::string> usedOptions;
        for (int i = 1; i < argc; i += 2) {
          std::string currentArg = argv[i];

          if (usedOptions.find(currentArg) != usedOptions.end()) {
            ErrorOutput();
          }

          if (optionsList.find(currentArg) == optionsList.end()) {
            ErrorOutput();
          } else {
            std::string optionValue = argv[i + 1];

            if (currentArg == "-mc") {
              int metric = std::stoi(optionValue);
              if (metric < 1 || metric > 3) ErrorOutput();
              args.metric = metric;
            } else if (currentArg == "-nb") {
              int numNeighbors = std::stoi(optionValue);
              if (numNeighbors < 3) ErrorOutput();
              args.numNeighbors = numNeighbors;
            } else if (currentArg == "-pd") {
              int predictionType = std::stoi(optionValue);
              if (predictionType < 1 || predictionType > 2) ErrorOutput();
              args.predictionType = predictionType;
            } else if (currentArg == "-mx") {
              std::ifstream matrixFile(optionValue);
              if (!matrixFile.is_open()) {
                std::cerr << "Error: Cannot open matrix file '" << optionValue
                          << "'." << std::endl;
                exit(1);
              }
              args.matrixFile = optionValue;
              matrixFile.close();
            }
            usedOptions.insert(currentArg);
            optionsList.erase(currentArg);
          }
        }

        if (optionsList.size() != 0) {
          ErrorOutput();
        }
      }
      break;
    }
    default: {
      std::string firstArg = argv[1];
      if (firstArg == "--help" || firstArg == "-h") {
        HelpOutput();
      } else {
        ErrorOutput();
      }
      break;
    }
  }
  return args;
}