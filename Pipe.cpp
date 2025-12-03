#include "Pipe.h"
#include <cmath>
#include <limits>
#include <algorithm>

using namespace std;

// Статическая инициализация
const vector<int> Pipe::VALID_DIAMETERS = { 500, 700, 1000, 1400 };

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

bool Pipe::isValidDiameter(int diameter) {
    return find(VALID_DIAMETERS.begin(), VALID_DIAMETERS.end(), diameter) != VALID_DIAMETERS.end();
}

double Pipe::calculateCapacity() const {
    if (repair) {
        return 0.0;
    }

    // Упрощенная формула: capacity = k * sqrt(d^5 / l)
    // где d - диаметр в мм, l - длина в км
    // k - поправочный коэффициент для получения осмысленных значений

    const double k = 0.1; // Поправочный коэффициент
    double d_mm = static_cast<double>(diameter);
    double l_km = length / 1000.0; // Переводим в км

    if (l_km < 0.001) l_km = 0.001; // Минимальная длина для избежания деления на 0

    double capacity = k * sqrt(pow(d_mm, 5) / l_km);

    return capacity;
}

double Pipe::calculateWeight() const {
    if (repair) {
        return numeric_limits<double>::infinity();
    }

    // Вес = длина трубы (в км)
    return length / 1000.0;
}

ostream& operator<<(ostream& os, const Pipe& pipe) {
    os << "ID: " << pipe.id << " | Name: " << pipe.name
        << " | Length: " << pipe.length << " | Diameter: " << pipe.diameter
        << " | Condition: " << pipe.condition;
    return os;
}