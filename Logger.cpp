#include "Logger.h"
#include <iostream>

using namespace std;

Logger::Logger() {
    logFile.open("user_actions.log", ios::app);
    if (!logFile.is_open()) {
        cerr << "Error: Unable to open log file!" << endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << endl;
    }
    cout << message << endl;
}