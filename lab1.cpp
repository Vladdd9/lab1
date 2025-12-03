#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include "Pipe.h"
#include "KS.h"
#include "Logger.h"
#include "Network.h"

using namespace std;

unordered_map<int, Pipe> pipes;
unordered_map<int, KS> stations;
int nextPipeId = 1;
int nextKSId = 1;
Logger logger;
GasNetwork gasNetwork;

// Прототипы функций
void saveToFile(const string& filename);
void loadFromFile(const string& filename);
void displayAllPipes();
void displayAllStations();
vector<int> findPipesByName(const string& name);
vector<int> findPipesByRepairStatus(bool inRepair);
vector<int> findStationsByName(const string& name);
vector<int> findStationsByPercentNotWorking(double minPercent, double maxPercent = 100.0);
void batchEditPipes();
void searchObjects();
void connectObjects();
void topologicalSortNetwork();
void calculateMaxFlow();
void findShortestPath();
void Menu();

void saveToFile(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "Pipes:" << endl;
        file << pipes.size() << endl;
        for (const auto& pair : pipes) {
            const Pipe& t = pair.second;
            file << t.getId() << endl;
            file << t.getName() << endl;
            file << t.getLength() << endl;
            file << t.getDiameter() << endl;
            file << t.isInRepair() << endl;
            file << t.getCondition() << endl;
        }

        file << "KS:" << endl;
        file << stations.size() << endl;
        for (const auto& pair : stations) {
            const KS& k = pair.second;
            file << k.getId() << endl;
            file << k.getName() << endl;
            file << k.getWorkshopCount() << endl;
            file << k.getWorkshopWorking() << endl;
            file << k.getType() << endl;
        }

        // Сохраняем сеть
        file << "Network:" << endl;
        const auto& adjList = gasNetwork.getAdjacencyList();
        file << adjList.size() << endl;
        for (const auto& pair : adjList) {
            file << pair.first << endl; // start KS
            file << pair.second.size() << endl; // number of edges
            for (const auto& edge : pair.second) {
                file << edge.pipeId << endl;
                file << edge.endKS << endl;
                file << edge.diameter << endl;
            }
        }

        file.close();
        string msg = "Data saved to " + filename + " successfully!";
        logger.log(msg);
    }
    else {
        logger.log("Error: Unable to open file for writing!");
    }
}

void loadFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        pipes.clear();
        stations.clear();
        gasNetwork.clear();
        string line;
        int count;

        getline(file, line);
        file >> count;
        file.ignore();

        for (int i = 0; i < count; i++) {
            int id;
            string name;
            float length;
            int diameter;
            bool repair;
            string condition;

            file >> id;
            file.ignore();
            getline(file, name);
            file >> length;
            file >> diameter;
            file >> repair;
            file.ignore();
            getline(file, condition);

            Pipe t(id, name, length, diameter);
            if (repair) {
                t.setRepair(true);
            }
            pipes[id] = t;

            if (id >= nextPipeId) {
                nextPipeId = id + 1;
            }
        }

        getline(file, line);
        file >> count;
        file.ignore();

        for (int i = 0; i < count; i++) {
            int id;
            string name;
            int workshop_count;
            int workshop_working;
            string type;

            file >> id;
            file.ignore();
            getline(file, name);
            file >> workshop_count;
            file >> workshop_working;
            file.ignore();
            getline(file, type);

            stations[id] = KS(id, name, type, workshop_count, workshop_working);

            if (id >= nextKSId) {
                nextKSId = id + 1;
            }
        }

        // Загружаем сеть
        getline(file, line);
        if (line == "Network:") {
            file >> count;
            file.ignore();

            for (int i = 0; i < count; i++) {
                int startKS;
                file >> startKS;
                file.ignore();

                int edgeCount;
                file >> edgeCount;
                file.ignore();

                for (int j = 0; j < edgeCount; j++) {
                    int pipeId, endKS, diameter;
                    file >> pipeId;
                    file.ignore();
                    file >> endKS;
                    file.ignore();
                    file >> diameter;
                    file.ignore();

                    gasNetwork.addConnection(pipeId, startKS, endKS, diameter);
                }
            }
        }

        file.close();
        string msg = "Data loaded from " + filename + " successfully!";
        logger.log(msg);
    }
    else {
        logger.log("Error: Unable to open file for reading!");
    }
}

void displayAllPipes() {
    if (pipes.empty()) {
        logger.log("No pipes available.");
        return;
    }

    cout << "Pipes:\n";
    for (const auto& pair : pipes) {
        cout << pair.second << "\n";
        // Показываем также производительность и вес
        cout << "  Capacity: " << pair.second.calculateCapacity()
            << " units, Weight: " << pair.second.calculateWeight() << " km\n";
    }
}

void displayAllStations() {
    if (stations.empty()) {
        logger.log("No compressor stations available.");
        return;
    }

    cout << "Compressor Stations:\n";
    for (const auto& pair : stations) {
        cout << pair.second << "\n";
    }
}

vector<int> findPipesByName(const string& name) {
    vector<int> result;
    for (const auto& pair : pipes) {
        if (pair.second.getName().find(name) != string::npos) {
            result.push_back(pair.first);
        }
    }
    return result;
}

vector<int> findPipesByRepairStatus(bool inRepair) {
    vector<int> result;
    for (const auto& pair : pipes) {
        if (pair.second.isInRepair() == inRepair) {
            result.push_back(pair.first);
        }
    }
    return result;
}

vector<int> findStationsByName(const string& name) {
    vector<int> result;
    for (const auto& pair : stations) {
        if (pair.second.getName().find(name) != string::npos) {
            result.push_back(pair.first);
        }
    }
    return result;
}

vector<int> findStationsByPercentNotWorking(double minPercent, double maxPercent) {
    vector<int> result;
    for (const auto& pair : stations) {
        double percent = pair.second.getPercentNotWorking();
        if (percent >= minPercent && percent <= maxPercent) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void batchEditPipes() {
    if (pipes.empty()) {
        logger.log("No pipes to edit.");
        return;
    }

    cout << "Search pipes for batch editing:\n";
    cout << "1 - By name\n";
    cout << "2 - By repair status\n";
    cout << "3 - All pipes\n";

    int searchOption;
    while (!(cin >> searchOption) || searchOption < 1 || searchOption > 3) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid option. Please enter 1-3: ";
    }

    vector<int> pipeIds;

    if (searchOption == 1) {
        cout << "Enter pipe name to search: ";
        string name;
        cin >> name;
        pipeIds = findPipesByName(name);
        logger.log("Search pipes by name: " + name);
    }
    else if (searchOption == 2) {
        cout << "Search pipes in repair? (y/n): ";
        char ans;
        cin >> ans;
        bool inRepair = (ans == 'y' || ans == 'Y');
        pipeIds = findPipesByRepairStatus(inRepair);
        logger.log("Search pipes by repair status: " + string(inRepair ? "in repair" : "working"));
    }
    else {
        for (const auto& pair : pipes) {
            pipeIds.push_back(pair.first);
        }
        logger.log("Selected all pipes for batch editing");
    }

    if (pipeIds.empty()) {
        logger.log("No pipes found matching the criteria.");
        return;
    }

    cout << "Found " << pipeIds.size() << " pipes:\n";
    for (int id : pipeIds) {
        cout << pipes[id] << "\n";
    }

    cout << "Edit all found pipes? (y/n): ";
    char editAll;
    cin >> editAll;

    vector<int> pipesToEdit;
    if (editAll == 'y' || editAll == 'Y') {
        pipesToEdit = pipeIds;
        logger.log("User selected to edit all found pipes");
    }
    else {
        cout << "Enter pipe IDs to edit (one per line, 0 to finish):\n";
        int id;
        while (cin >> id && id != 0) {
            if (find(pipeIds.begin(), pipeIds.end(), id) != pipeIds.end()) {
                pipesToEdit.push_back(id);
            }
            else {
                cout << "ID " << id << " not in found pipes. Try again.\n";
            }
        }
        logger.log("User selected specific pipes for editing");
    }

    if (pipesToEdit.empty()) {
        logger.log("No pipes selected for editing.");
        return;
    }

    cout << "Choose editing option:\n";
    cout << "1 - Change repair status\n";
    cout << "2 - Change name\n";

    int editOption;
    while (!(cin >> editOption) || editOption < 1 || editOption > 2) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid option. Please enter 1-2: ";
    }

    if (editOption == 1) {
        cout << "Set repair status (y - in repair, n - working): ";
        char ans;
        cin >> ans;
        bool inRepair = (ans == 'y' || ans == 'Y');

        for (int id : pipesToEdit) {
            pipes[id].setRepair(inRepair);
        }

        string status = inRepair ? "repair" : "working";
        logger.log("Batch updated " + to_string(pipesToEdit.size()) + " pipes to status: " + status);
    }
    else {
        cout << "Enter new name: ";
        string newName;
        cin >> newName;

        for (int id : pipesToEdit) {
            pipes[id].setName(newName);
        }

        logger.log("Batch updated " + to_string(pipesToEdit.size()) + " pipes with new name: " + newName);
    }

    cout << "Successfully updated " << pipesToEdit.size() << " pipes.\n";
}

void searchObjects() {
    cout << "Search objects:\n";
    cout << "1 - Search pipes\n";
    cout << "2 - Search compressor stations\n";

    int option;
    while (!(cin >> option) || option < 1 || option > 2) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid option. Please enter 1-2: ";
    }

    if (option == 1) {
        cout << "Search pipes by:\n";
        cout << "1 - Name\n";
        cout << "2 - Repair status\n";

        int searchType;
        while (!(cin >> searchType) || searchType < 1 || searchType > 2) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option. Please enter 1-2: ";
        }

        vector<int> foundPipes;

        if (searchType == 1) {
            cout << "Enter pipe name to search: ";
            string name;
            cin >> name;
            foundPipes = findPipesByName(name);
            logger.log("Search pipes by name: " + name);
        }
        else {
            cout << "Search pipes in repair? (y/n): ";
            char ans;
            cin >> ans;
            bool inRepair = (ans == 'y' || ans == 'Y');
            foundPipes = findPipesByRepairStatus(inRepair);
            logger.log("Search pipes by repair status: " + string(inRepair ? "in repair" : "working"));
        }

        if (foundPipes.empty()) {
            cout << "No pipes found.\n";
        }
        else {
            cout << "Found " << foundPipes.size() << " pipes:\n";
            for (int id : foundPipes) {
                cout << pipes[id] << "\n";
            }
        }
    }
    else {
        cout << "Search compressor stations by:\n";
        cout << "1 - Name\n";
        cout << "2 - Percent of not working workshops\n";

        int searchType;
        while (!(cin >> searchType) || searchType < 1 || searchType > 2) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option. Please enter 1-2: ";
        }

        vector<int> foundStations;

        if (searchType == 1) {
            cout << "Enter station name to search: ";
            string name;
            cin >> name;
            foundStations = findStationsByName(name);
            logger.log("Search stations by name: " + name);
        }
        else {
            cout << "Enter minimum percent of not working workshops: ";
            double minPercent;
            while (!(cin >> minPercent) || minPercent < 0 || minPercent > 100) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 0 and 100: ";
            }
            foundStations = findStationsByPercentNotWorking(minPercent);
            logger.log("Search stations by percent not working: " + to_string(minPercent) + "%");
        }

        if (foundStations.empty()) {
            cout << "No compressor stations found.\n";
        }
        else {
            cout << "Found " << foundStations.size() << " stations:\n";
            for (int id : foundStations) {
                cout << stations[id] << "\n";
            }
        }
    }
}

void connectObjects() {
    if (stations.size() < 2) {
        logger.log("Need at least 2 compressor stations to create connection");
        cout << "Need at least 2 compressor stations to create connection.\n";
        return;
    }

    cout << "Available compressor stations:\n";
    displayAllStations();

    int startKS, endKS;
    cout << "Enter start KS ID: ";
    while (!(cin >> startKS) || stations.find(startKS) == stations.end()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID: ";
    }

    cout << "Enter end KS ID: ";
    while (!(cin >> endKS) || stations.find(endKS) == stations.end() || endKS == startKS) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID different from start: ";
    }

    if (gasNetwork.hasConnection(startKS, endKS)) {
        logger.log("Connection already exists between KS " + to_string(startKS) + " and KS " + to_string(endKS));
        cout << "Connection already exists between these stations.\n";
        return;
    }

    // Доступные диаметры из Pipe::VALID_DIAMETERS
    cout << "Available diameters: ";
    for (size_t i = 0; i < Pipe::VALID_DIAMETERS.size(); ++i) {
        cout << Pipe::VALID_DIAMETERS[i];
        if (i < Pipe::VALID_DIAMETERS.size() - 1) cout << ", ";
    }
    cout << "\nEnter pipe diameter: ";

    int diameter;
    while (!(cin >> diameter) || !Pipe::isValidDiameter(diameter)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid diameter. Please choose from available diameters: ";
        for (size_t i = 0; i < Pipe::VALID_DIAMETERS.size(); ++i) {
            cout << Pipe::VALID_DIAMETERS[i];
            if (i < Pipe::VALID_DIAMETERS.size() - 1) cout << ", ";
        }
        cout << ": ";
    }

    // Поиск свободной трубы с нужным диаметром
    int selectedPipeId = -1;
    for (const auto& pair : pipes) {
        const Pipe& pipe = pair.second;
        if (pipe.getDiameter() == diameter && !gasNetwork.isPipeUsed(pipe.getId())) {
            selectedPipeId = pipe.getId();
            break;
        }
    }

    if (selectedPipeId == -1) {
        cout << "No available pipe with diameter " << diameter << "mm found.\n";
        cout << "Would you like to create a new pipe? (y/n): ";
        char answer;
        cin >> answer;

        if (answer == 'y' || answer == 'Y') {
            string name;
            float length;

            cout << "Enter pipe name: ";
            cin >> name;

            cout << "Enter pipe length (in meters, positive number): ";
            while (!(cin >> length) || length <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive number: ";
            }

            Pipe newPipe(nextPipeId++, name, length, diameter);
            pipes[newPipe.getId()] = newPipe;
            selectedPipeId = newPipe.getId();

            string msg = "New pipe created with ID: " + to_string(newPipe.getId());
            logger.log(msg);
            cout << msg << "\n";
        }
        else {
            logger.log("Connection creation cancelled - no available pipe");
            cout << "Connection creation cancelled.\n";
            return;
        }
    }

    // Создаем соединение
    gasNetwork.addConnection(selectedPipeId, startKS, endKS, diameter);

    string msg = "Connected KS " + to_string(startKS) + " to KS " + to_string(endKS) +
        " using pipe " + to_string(selectedPipeId) + " (Diameter: " + to_string(diameter) + "mm)";
    logger.log(msg);
    cout << "Connection created successfully!\n";
}

void topologicalSortNetwork() {
    vector<int> sorted = gasNetwork.topologicalSort();

    if (sorted.empty()) {
        logger.log("Topological sort failed - network contains cycles");
        cout << "The gas network contains cycles. Topological sort is not possible.\n";
    }
    else {
        cout << "Topological order of compressor stations:\n";
        for (size_t i = 0; i < sorted.size(); ++i) {
            cout << (i + 1) << ". KS " << sorted[i];
            auto it = stations.find(sorted[i]);
            if (it != stations.end()) {
                cout << " (" << it->second.getName() << ")";
            }
            cout << "\n";
        }
        logger.log("Topological sort completed successfully");
    }
}

void calculateMaxFlow() {
    if (stations.size() < 2) {
        logger.log("Need at least 2 compressor stations to calculate flow");
        cout << "Need at least 2 compressor stations to calculate flow.\n";
        return;
    }

    if (pipes.empty()) {
        logger.log("Need pipes to calculate flow");
        cout << "Need pipes to calculate flow.\n";
        return;
    }

    displayAllStations();

    int sourceKS, sinkKS;
    cout << "Enter source KS ID: ";
    while (!(cin >> sourceKS) || stations.find(sourceKS) == stations.end()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID: ";
    }

    cout << "Enter sink KS ID: ";
    while (!(cin >> sinkKS) || stations.find(sinkKS) == stations.end() || sinkKS == sourceKS) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID different from source: ";
    }

    double maxFlow = gasNetwork.calculateMaxFlow(sourceKS, sinkKS, pipes);

    string msg = "Maximum flow from KS " + to_string(sourceKS) +
        " to KS " + to_string(sinkKS) + ": " + to_string(maxFlow) + " units";
    logger.log(msg);
    cout << msg << "\n";
}

void findShortestPath() {
    if (stations.size() < 2) {
        logger.log("Need at least 2 compressor stations to find path");
        cout << "Need at least 2 compressor stations to find path.\n";
        return;
    }

    if (pipes.empty()) {
        logger.log("Need pipes to find path");
        cout << "Need pipes to find path.\n";
        return;
    }

    displayAllStations();

    int startKS, endKS;
    cout << "Enter start KS ID: ";
    while (!(cin >> startKS) || stations.find(startKS) == stations.end()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID: ";
    }

    cout << "Enter end KS ID: ";
    while (!(cin >> endKS) || stations.find(endKS) == stations.end() || endKS == startKS) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID. Please enter a valid KS ID different from start: ";
    }

    auto result = gasNetwork.findShortestPath(startKS, endKS, pipes);
    double distance = result.first;
    const vector<int>& path = result.second;

    if (distance == numeric_limits<double>::infinity()) {
        string msg = "No path found from KS " + to_string(startKS) +
            " to KS " + to_string(endKS);
        logger.log(msg);
        cout << msg << "\n";
        cout << "Possible reasons:\n";
        cout << "1. No connection exists between these stations\n";
        cout << "2. All pipes on the path are in repair\n";
    }
    else {
        cout << "Shortest path distance: " << distance << " km\n";
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << "KS " << path[i];
            auto it = stations.find(path[i]);
            if (it != stations.end()) {
                cout << " (" << it->second.getName() << ")";
            }
            if (i < path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << "\n";

        // Показываем детали пути (трубы)
        cout << "Path details:\n";
        for (size_t i = 0; i < path.size() - 1; ++i) {
            int from = path[i];
            int to = path[i + 1];

            // Находим трубу между этими станциями
            const auto& adjList = gasNetwork.getAdjacencyList();
            auto it_from = adjList.find(from);
            if (it_from != adjList.end()) {
                for (const auto& edge : it_from->second) {
                    if (edge.endKS == to) {
                        auto pipeIt = pipes.find(edge.pipeId);
                        if (pipeIt != pipes.end()) {
                            cout << "  Pipe " << edge.pipeId << " from KS " << from
                                << " to KS " << to << ": " << pipeIt->second.getLength()
                                << "m, Diameter: " << edge.diameter << "mm, "
                                << "Weight: " << pipeIt->second.calculateWeight() << "km\n";
                        }
                        break;
                    }
                }
            }
        }

        string msg = "Found path from KS " + to_string(startKS) +
            " to KS " + to_string(endKS) + " with distance " +
            to_string(distance) + " km";
        logger.log(msg);
    }
}

void Menu() {
    int option;
    string filename;

    while (true) {
        cout << "\n========================================\n";
        cout << "Gas Transport Network Management System\n";
        cout << "========================================\n";
        cout << "Choose an option: \n";
        cout << "1 - Add pipe \n";
        cout << "2 - Add a compressor station \n";
        cout << "3 - View all objects \n";
        cout << "4 - Edit pipe \n";
        cout << "5 - Edit compressor station \n";
        cout << "6 - Search objects \n";
        cout << "7 - Batch edit pipes \n";
        cout << "8 - Save to file \n";
        cout << "9 - Load from file \n";
        cout << "10 - Connect objects (create network) \n";
        cout << "11 - Display network \n";
        cout << "12 - Topological sort \n";
        cout << "13 - Calculate maximum flow \n";
        cout << "14 - Find shortest path \n";
        cout << "0 - Exit \n";
        cout << "========================================\n";
        cout << "Your choice: ";

        while (!(cin >> option)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
        }
        cout << "\n";

        logger.log("User selected option: " + to_string(option));

        switch (option) {
        case 0:
            logger.log("Exiting program. Goodbye!");
            cout << "Exiting program. Goodbye!" << endl;
            return;

        case 1: {
            string name;
            float length;
            int diameter;

            cout << "Insert pipe name: ";
            cin >> name;

            cout << "Insert length (in meters): ";
            while (!(cin >> length) || length <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive number: ";
            }

            cout << "Available diameters: ";
            for (size_t i = 0; i < Pipe::VALID_DIAMETERS.size(); ++i) {
                cout << Pipe::VALID_DIAMETERS[i] << "mm";
                if (i < Pipe::VALID_DIAMETERS.size() - 1) cout << ", ";
            }
            cout << "\nInsert diameter: ";
            while (!(cin >> diameter) || !Pipe::isValidDiameter(diameter)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid diameter. Please choose from: ";
                for (size_t i = 0; i < Pipe::VALID_DIAMETERS.size(); ++i) {
                    cout << Pipe::VALID_DIAMETERS[i];
                    if (i < Pipe::VALID_DIAMETERS.size() - 1) cout << ", ";
                }
                cout << ": ";
            }

            Pipe t(nextPipeId++, name, length, diameter);
            pipes[t.getId()] = t;

            string msg = "Pipe added with ID: " + to_string(t.getId()) +
                ", Capacity: " + to_string(t.calculateCapacity()) + " units";
            logger.log(msg);
            cout << msg << "\n\n";
            break;
        }

        case 2: {
            string name, type;
            int workshop_count, workshop_working;

            cout << "Insert compressor station name: ";
            cin >> name;

            cout << "Insert station type: ";
            cin >> type;

            cout << "Insert number of workshops: ";
            while (!(cin >> workshop_count) || workshop_count <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive integer: ";
            }

            cout << "Insert number of workshops which actually work: ";
            while (!(cin >> workshop_working) || workshop_working < 0 || workshop_working > workshop_count) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 0 and " << workshop_count << ": ";
            }

            KS k(nextKSId++, name, type, workshop_count, workshop_working);
            stations[k.getId()] = k;

            string msg = "Compressor station added with ID: " + to_string(k.getId());
            logger.log(msg);
            cout << msg << "\n\n";
            break;
        }

        case 3:
            if (pipes.empty() && stations.empty()) {
                logger.log("No objects to display");
                cout << "No objects to display. Please add pipes or compressor stations first.\n\n";
            }
            else {
                displayAllPipes();
                cout << "\n";
                displayAllStations();
                cout << "\n";
            }
            break;

        case 4:
            if (pipes.empty()) {
                logger.log("No pipes to edit");
                cout << "No pipes to edit. Please add a pipe first.\n\n";
                break;
            }

            displayAllPipes();
            cout << "Enter pipe ID to edit: ";
            int pipeId;
            while (!(cin >> pipeId) || pipes.find(pipeId) == pipes.end()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid ID. Please enter a valid pipe ID: ";
            }

            cout << "Is pipe in repair? (y/n): ";
            char ans;
            cin >> ans;
            while (ans != 'y' && ans != 'n' && ans != 'Y' && ans != 'N') {
                cout << "Type y or n: ";
                cin >> ans;
            }

            if (ans == 'y' || ans == 'Y') {
                pipes[pipeId].setRepair(true);
                logger.log("Pipe " + to_string(pipeId) + " marked as in repair");
                cout << "Pipe marked as in repair.\n";
                cout << "New capacity: " << pipes[pipeId].calculateCapacity() << " units\n";
            }
            else {
                pipes[pipeId].setRepair(false);
                logger.log("Pipe " + to_string(pipeId) + " marked as working");
                cout << "Pipe marked as working.\n";
                cout << "New capacity: " << pipes[pipeId].calculateCapacity() << " units\n";
            }
            cout << "\n";
            break;

        case 5:
            if (stations.empty()) {
                logger.log("No compressor stations to edit");
                cout << "No compressor stations to edit. Please add a compressor station first.\n\n";
                break;
            }

            displayAllStations();
            cout << "Enter compressor station ID to edit: ";
            int ksId;
            while (!(cin >> ksId) || stations.find(ksId) == stations.end()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid ID. Please enter a valid station ID: ";
            }

            cout << "Choose action (1 - put workshops into operation, 2 - put workshops out of operation): ";
            int action;
            while (!(cin >> action) || (action != 1 && action != 2)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Wrong action. Please enter 1 or 2: ";
            }

            if (action == 1) {
                cout << "Set a number of workshops to put into operation: ";
                int num;
                while (!(cin >> num) || num <= 0 || num + stations[ksId].getWorkshopWorking() > stations[ksId].getWorkshopCount()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Unacceptable number. Please enter a number between 1 and "
                        << stations[ksId].getWorkshopCount() - stations[ksId].getWorkshopWorking() << ": ";
                }
                stations[ksId].setWorkshopWorking(stations[ksId].getWorkshopWorking() + num);
                logger.log("Station " + to_string(ksId) + ": " + to_string(num) + " workshops put into operation");
                cout << num << " workshops put into operation. Total working: " << stations[ksId].getWorkshopWorking() << "\n";
            }
            else {
                cout << "Set a number of workshops to put out of operation: ";
                int num;
                while (!(cin >> num) || num <= 0 || stations[ksId].getWorkshopWorking() - num < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Unacceptable number. Please enter a number between 1 and "
                        << stations[ksId].getWorkshopWorking() << ": ";
                }
                stations[ksId].setWorkshopWorking(stations[ksId].getWorkshopWorking() - num);
                logger.log("Station " + to_string(ksId) + ": " + to_string(num) + " workshops put out of operation");
                cout << num << " workshops put out of operation. Total working: " << stations[ksId].getWorkshopWorking() << "\n";
            }
            cout << "\n";
            break;

        case 6:
            searchObjects();
            cout << "\n";
            break;

        case 7:
            batchEditPipes();
            cout << "\n";
            break;

        case 8:
            if (pipes.empty() && stations.empty()) {
                logger.log("No data to save");
                cout << "No data to save. Please add pipes or compressor stations first.\n\n";
                break;
            }

            cout << "Enter filename to save: ";
            cin >> filename;
            saveToFile(filename);
            cout << "\n";
            break;

        case 9:
            cout << "Enter filename to load: ";
            cin >> filename;
            loadFromFile(filename);
            cout << "\n";
            break;

        case 10:
            connectObjects();
            cout << "\n";
            break;

        case 11:
            gasNetwork.displayNetwork();
            cout << "\n";
            break;

        case 12:
            topologicalSortNetwork();
            cout << "\n";
            break;

        case 13:
            calculateMaxFlow();
            cout << "\n";
            break;

        case 14:
            findShortestPath();
            cout << "\n";
            break;

        default:
            logger.log("Invalid option selected: " + to_string(option));
            cout << "Invalid option. Please try again.\n\n";
            break;
        }
    }
}

int main() {
    logger.log("Program started");
    Menu();
    logger.log("Program ended");
    return 0;
}