

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

struct Pipe {
    string name;
    float length;
    int diametr;
    bool repair;
    string condition;
};

struct KS {
    string name;
    int workshop_count;
    int workshop_working;
    string type;
};

void saveToFile(const Pipe& t, const KS& k, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "Pipe:" << endl;
        file << t.name << endl;
        file << t.length << endl;
        file << t.diametr << endl;
        file << t.repair << endl;
        file << t.condition << endl;

        file << "KS:" << endl;
        file << k.name << endl;
        file << k.workshop_count << endl;
        file << k.workshop_working << endl;
        file << k.type << endl;

        file.close();
        cout << "Data saved to " << filename << " successfully!" << endl;
    }
    else {
        cout << "Error: Unable to open file for writing!" << endl;
    }
}

void loadFromFile(Pipe& t, KS& k, const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;

        getline(file, line);

        getline(file, t.name);
        file >> t.length;
        file >> t.diametr;
        file >> t.repair;
        file.ignore();
        getline(file, t.condition);

        getline(file, line);

        getline(file, k.name);
        file >> k.workshop_count;
        file >> k.workshop_working;
        file.ignore();
        getline(file, k.type);

        file.close();
        cout << "Data loaded from " << filename << " successfully!" << endl;
    }
    else {
        cout << "Error: Unable to open file for reading!" << endl;
    }
}

void Menu() {
    Pipe t = { "", 0.0f, 0, false, "unknown" };
    KS k = { "", 0, 0, "" };
    int option;
    string filename;

    while (true) {
        cout << "Choose an option: \n";
        cout << "1 - add pipe \n";
        cout << "2 - add a compressor station \n";
        cout << "3 - view all objects \n";
        cout << "4 - edit pipe \n";
        cout << "5 - edit compressor station \n";
        cout << "6 - save \n";
        cout << "7 - load \n";
        cout << "0 - exit \n";

        while (!(cin >> option)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
        }
        cout << "\n";

        switch (option) {
        case 0:
            cout << "Exiting program. Goodbye!" << endl;
            return;

        case 1:
            cout << "Insert pipe name: ";
            cin >> t.name;

            cout << "Insert length: ";
            while (!(cin >> t.length) || t.length <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive number: ";
            }

            cout << "Insert diametr: ";
            while (!(cin >> t.diametr) || t.diametr <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive integer: ";
            }

            t.condition = "working";
            t.repair = false;
            cout << "\n";
            break;

        case 2:
            cout << "Insert compressor station name: ";
            cin >> k.name;

            cout << "Insert station type: ";
            cin >> k.type;

            cout << "Insert number of workshops: ";
            while (!(cin >> k.workshop_count) || k.workshop_count <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a positive integer: ";
            }

            cout << "Insert number of workshops which actually work: ";
            while (!(cin >> k.workshop_working) || k.workshop_working < 0 || k.workshop_working > k.workshop_count) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 0 and " << k.workshop_count << ": ";
            }
            cout << "\n";
            break;

        case 3:
            if (t.name.empty() && k.name.empty()) {
                cout << "No objects to display. Please add pipes or compressor stations first.\n\n";
            }
            else {
                if (!t.name.empty()) {
                    cout << "Pipe: \nname: " << t.name << " length: " << t.length
                        << " diametr: " << t.diametr << " condition: " << t.condition << "\n";
                }
                if (!k.name.empty()) {
                    cout << "Compressor station: \nname: " << k.name << " type: " << k.type
                        << " total workshops: " << k.workshop_count
                        << " workshops at work: " << k.workshop_working << "\n";
                }
                cout << "\n";
            }
            break;

        case 4:
            if (t.name.empty()) {
                cout << "No pipe to edit. Please add a pipe first.\n\n";
                break;
            }

            cout << "Is pipe in repair? (y/n): ";
            char ans;
            cin >> ans;
            while (ans != 'y' && ans != 'n') {
                cout << "Type y or n: ";
                cin >> ans;
            }
            if (ans == 'y') {
                t.repair = true;
                t.condition = "repair";
                cout << "Pipe marked as in repair.\n";
            }
            else {
                t.repair = false;
                t.condition = "working";
                cout << "Pipe marked as working.\n";
            }
            cout << "\n";
            break;

        case 5:
            if (k.name.empty()) {
                cout << "No compressor station to edit. Please add a compressor station first.\n\n";
                break;
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
                while (!(cin >> num) || num <= 0 || num + k.workshop_working > k.workshop_count) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Unacceptable number. Please enter a number between 1 and "
                        << k.workshop_count - k.workshop_working << ": ";
                }
                k.workshop_working += num;
                cout << num << " workshops put into operation. Total working: " << k.workshop_working << "\n";
            }
            else {
                cout << "Set a number of workshops to put out of operation: ";
                int num;
                while (!(cin >> num) || num <= 0 || k.workshop_working - num < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Unacceptable number. Please enter a number between 1 and "
                        << k.workshop_working << ": ";
                }
                k.workshop_working -= num;
                cout << num << " workshops put out of operation. Total working: " << k.workshop_working << "\n";
            }
            cout << "\n";
            break;

        case 6:
            if (t.name.empty() && k.name.empty()) {
                cout << "No data to save. Please add pipes or compressor stations first.\n\n";
                break;
            }

            cout << "Enter filename to save: ";
            cin >> filename;
            saveToFile(t, k, filename);
            cout << "\n";
            break;

        case 7:
            cout << "Enter filename to load: ";
            cin >> filename;
            loadFromFile(t, k, filename);
            cout << "\n";
            break;

        default:
            cout << "Invalid option. Please try again.\n\n";
            break;
        }
    }
}

int main() {
    Menu();
    return 0;
}