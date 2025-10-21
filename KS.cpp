#include "KS.h"

using namespace std;

KS::KS() : id(0), name(""), workshop_count(0), workshop_working(0), type("") {}

KS::KS(int id, const std::string& name, const std::string& type, int workshop_count, int workshop_working)
    : id(id), name(name), type(type), workshop_count(workshop_count), workshop_working(workshop_working) {
}

int KS::getId() const { return id; }
string KS::getName() const { return name; }
string KS::getType() const { return type; }
int KS::getWorkshopCount() const { return workshop_count; }
int KS::getWorkshopWorking() const { return workshop_working; }
int KS::getWorkshopNotWorking() const { return workshop_count - workshop_working; }
double KS::getPercentNotWorking() const {
    return workshop_count > 0 ? (100.0 * getWorkshopNotWorking()) / workshop_count : 0.0;
}

void KS::setName(const std::string& newName) { name = newName; }
void KS::setType(const std::string& newType) { type = newType; }

void KS::setWorkshopCount(int count) {
    workshop_count = count;
    if (workshop_working > workshop_count) {
        workshop_working = workshop_count;
    }
}

void KS::setWorkshopWorking(int working) {
    if (working >= 0 && working <= workshop_count) {
        workshop_working = working;
    }
}

ostream& operator<<(ostream& os, const KS& station) {
    os << "ID: " << station.id << " | Name: " << station.name
        << " | Type: " << station.type << " | Total workshops: " << station.workshop_count
        << " | Workshops working: " << station.workshop_working
        << " | Percent not working: " << station.getPercentNotWorking() << "%";
    return os;
}
