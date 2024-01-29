/**
 * This file defines an easy-to-use terminal-based interface and makes use of
 * the RegularExpression class.
 *
 * @file
 * @author Jort van Leenen
 * @copyright GNU General Public License v3.0
 */

#include "RegularExpression.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

/**
 * Execute a given operation on the given RegularExpression. If an invalid
 * operation is given, we mention this to the user.
 *
 * @param operation the operation we want to execute on given RegularExpression
 * @param expression the RegularExpression we want to execute an operation on
 * @return true if the program should continue, false if it should stop
 */
bool execute(std::string_view operation, RegularExpression& expression) {
  if (auto carriage_return_index{operation.find('\r')};
      carriage_return_index != std::string::npos) {
    operation = operation.substr(0, carriage_return_index);
  }
  std::istringstream inputStream{operation.data()};
  std::string token{};
  inputStream >> token;
  if (token == "exp") {
    if (inputStream >> token) {
      token = operation.substr(4);
    } else {
      std::cout << "Please enter a regular expression:";
      std::getline(std::cin, token);
    }
    expression = RegularExpression{token};
  } else if (token == "dot") {
    if (!(inputStream >> token)) {
      std::cout << "Please enter a filepath to write the output to:";
      std::getline(std::cin, token);
    }
    try {
      std::ofstream file{token};
      file << expression.dot();
    } catch (const std::ofstream::failure& e) {
      std::cout << "Error while exporting .dot: " << e.what() << '\n';
    }
  } else if (token == "mat") {
    if (inputStream >> token) {
      token = operation.substr(4);
    } else {
      std::cout << "Please enter a string to check:";
      std::getline(std::cin, token);
    }
    std::cout << (expression.mat(token) ? "match" : "no match") << '\n';
  } else if (token == "end") {
    return false;
  } else {
    std::cout << "Unknown command: " << (token.empty() ? "(none)" : token)
              << '\n';
  }
  return true;
}

/**
 * Entry point of the interactive program.
 *
 * @param argc the number of arguments
 * @param argv the arguments
 * @return EXIT_SUCCESS if the program ran successfully, EXIT_FAILURE otherwise
 */
int main(int argc, char* argv[]) {
  if (argc <= 2) {
    const bool DEBUG = argc == 2 && *argv[1] == 'd';

    if (!DEBUG) {
      std::cout << "Regular expression parsing by Jort van Leenen\n";
    }

    RegularExpression expression{};
    std::string operation{};
    while (true) {
      if (!DEBUG) {
        std::cout << "Available operations:\n"
                     " - exp <expression>\tRead in regular expression\n"
                     " - dot <filename>\tExport regular expression to "
                     "dot-notation\n"
                     " - mat <string>\t\tCheck whether a string is accepted by "
                     "automaton\n"
                     " - end\t\t\tClose the program\n"
                     "Please enter an operation. If applicable, you can "
                     "immediately provide\nan argument for the operation:";
      }

      if (std::getline(std::cin, operation)) {
        if (!execute(operation, expression)) {
          return EXIT_SUCCESS;
        }
      } else if (DEBUG) {
        return EXIT_SUCCESS;
      }
    }
  } else {
    std::cout << "Usage: " << argv[0] << " [d]\n";
    return EXIT_FAILURE;
  }
}
