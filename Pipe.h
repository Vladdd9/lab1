#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <iostream>

class Pipe {
private:
    int id;
    std::string name;
    float length;
    int diameter;
    bool repair;
    std::string condition;

public:
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

    friend std::ostream& operator<<(std::ostream& os, const Pipe& pipe);
};

#endif