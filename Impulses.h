//
//  Impulses.h
//  GuitarAmp
//
//  Created by Andreas Pohl on 07/10/16.
//
//

#ifndef Impulses_h
#define Impulses_h

#include <vector>

struct Impulse {
    Impulse() : Name(nullptr), Vendor(nullptr), Mic(nullptr), SampleRate(0) {}
    Impulse(const char* n, const char* v, const char* m, std::vector<double>& f, int r)
        : Name(n), Vendor(v), Mic(m), Frames(f), SampleRate(r) {}
    const char* Name;
    const char* Vendor;
    const char* Mic;
    std::vector<double> Frames;
    int SampleRate;
};

std::vector<Impulse> gImpulses;

#include "cabinets/all.h"

#endif /* Impulses_h */
