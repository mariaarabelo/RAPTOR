#include <iostream>
#include "Application.h"

int main(int argc, char *argv[]) {

  std::string inputDirectory;

  if (argc >= 2) {
    inputDirectory = argv[1];
  } else {
    std::cout << "GTFS Input Directory: ";
    std::getline(std::cin, inputDirectory);
  }

  Application application(inputDirectory);
  application.run();

  // query 5777 5776 22:00:00

  return 0;
}
