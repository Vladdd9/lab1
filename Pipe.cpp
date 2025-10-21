#include "Pipe.h"

using namespace std;

Pipe::Pipe() : id(0), name(""), length(0.0f), diameter(0), repair(false), condition("unknown") {}

Pipe::Pipe(int id, const std::string& name, float length, int diameter)
    : id(id), name(name), length(length), diameter(diameter), repair(false), condition("working") {
}

int Pipe::getId() const { return id; }
string Pipe::getName() const { return name; }
float Pipe::getLength() const { return length; }
int Pipe::getDiameter() const { return diameter; }
bool Pipe::isInRepair() const { return repair; }
string Pipe::getCondition() const { return condition; }

void Pipe::setName(const std::string& newName) { name = newName; }
void Pipe::setLength(float newLength) { length = newLength; }
void Pipe::setDiameter(int newDiameter) { diameter = newDiameter; }

void Pipe::setRepair(bool inRepair) {
    repair = inRepair;
    condition = inRepair ? "repair" : "working";
}

void Pipe::setCondition(const std::string& newCondition) { condition = newCondition; }

ostream& operator<<(ostream& os, const Pipe& pipe) {
    os << "ID: " << pipe.id << " | Name: " << pipe.name
        << " | Length: " << pipe.length << " | Diameter: " << pipe.diameter
        << " | Condition: " << pipe.condition;
    return os;
}
