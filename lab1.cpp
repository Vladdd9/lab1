

#include <iostream>
#include <string>
using namespace std;


void Menu(Pipe t, KS k) {
    int option;
    while (true) {
        cout << "Выберите действие:";
        cout << "1 - добавить трубу";
        cout << "2 - добавить компрессорную станцию \n";
        cin >> option;
        switch (option) {
        case 1:
            cout << "Insert pipe name: ";
            cin >> t.name;
            break;

        };
    };
};

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

int main()
{
    Pipe truba;
    KS ks;
    Menu(truba);

}

