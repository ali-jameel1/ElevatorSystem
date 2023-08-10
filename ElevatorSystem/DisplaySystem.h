#ifndef DISPLAYSYSTEM_H
#define DISPLAYSYSTEM_H

#include <iostream>
#include <string>

using namespace std;

class DisplaySystem
{
    public:
        DisplaySystem();

        void display(const string&) const;
};

#endif // DISPLAYSYSTEM_H
