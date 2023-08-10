#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <iostream>
#include <string>

using namespace std;

class AudioSystem
{
    public:
        AudioSystem();

        void play(const string&) const;
};

#endif // AUDIOSYSTEM_H
