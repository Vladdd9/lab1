#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <iostream>
#include <vector>

class Pipe {
private:
    int id;
    std::string name;
    float length;
    int diameter;
    bool repair;
    std::string condition;

public:
    static const std::vector<int> VALID_DIAMETERS;

    Pipe();
    Pipe(int id, const std::string& name, float length, int diameter);

    int getId() const;
    std::string getName() const;
    float getLength() const;
    int getDiameter() const;
    bool isInRepair() const;
    std::string getCondition() const;

    void setName(const std::string& newName);
    void setLength(float newLength);
    void setDiameter(int newDiameter);
    void setRepair(bool inRepair);
    void setCondition(const std::string& newCondition);

    // Новые методы для расчета производительности и веса
    double calculateCapacity() const;
    double calculateWeight() const;
    static bool isValidDiameter(int diameter);

    friend std::ostream& operator<<(std::ostream& os, const Pipe& pipe);
};

#endif