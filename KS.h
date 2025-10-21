#ifndef KS_H
#define KS_H

#include <string>
#include <iostream>

class KS {
private:
    int id;
    std::string name;
    int workshop_count;
    int workshop_working;
    std::string type;

public:
    KS();
    KS(int id, const std::string& name, const std::string& type, int workshop_count, int workshop_working);

    int getId() const;
    std::string getName() const;
    std::string getType() const;
    int getWorkshopCount() const;
    int getWorkshopWorking() const;
    int getWorkshopNotWorking() const;
    double getPercentNotWorking() const;

    void setName(const std::string& newName);
    void setType(const std::string& newType);
    void setWorkshopCount(int count);
    void setWorkshopWorking(int working);

    friend std::ostream& operator<<(std::ostream& os, const KS& station);
};

#endif