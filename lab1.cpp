

#include <iostream>
#include <string>
using namespace std;


struct Pipe {
    string name;
    float length;
    int diametr;
    bool repair;
};
struct KS {
    string name;
    int workshop_count;
    int workshop_working;
    string type;
};

void Menu(Pipe t, KS k) {
    int option;
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
        cin >> option;
        cout << "\n";
        switch (option) {
        case 1:
            cout << "Insert pipe name: ";
            cin >> t.name;
            cout << "Insert length: ";
            cin >> t.length;
            cout << "Insert diametr: ";
            cin >> t.diametr;
            cout << "Is pipe working: ";
            //cin >> t.repair;
            cout << "\n";
            break;
        case 2:
            cout << "Insert compressor station name: ";
            cin >> k.name;
            cout << "Insert station type: ";
            cin >> k.type;
            cout << "Insert number of workshops: ";
            cin >> k.workshop_count;
            cout << "Insert number of workshops which actually work: ";
            cin >> k.workshop_working;
            cout << "\n";

            break;
        case 3:
            cout << "Pipe: \nname: " << t.name << " length: " << t.length << " diametr: " << t.diametr << " condition: " << t.repair << "\n";
            cout << "Compressor station: \nname: " << k.name << " type: " << k.type << " workshops at work: " << k.workshop_working << "\n \n";

        }

    }
}

int main()
{
    Pipe truba;
    KS ks;
    Menu(truba, ks);

}

