//
//  Sequence.cpp
//  ProgramminngProject
//
//  Created by Yahui Liang on 11/30/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#include "Sequence.hpp"
#include <iostream>
#include <fstream>
#include <set>

using namespace std;

// Complementary the gene
static char complement(char gene);

Sequence::Sequence() {
    sequence = new vector<char>();
    name = "";
}

Sequence::Sequence(const Sequence &src) {
    name = src.name;
    sequence = new vector<char>();
    *sequence = *src.sequence; // Deep copy the contect of the vector
}

Sequence::Sequence(Sequence &&src) {
    name = src.name;
    if (sequence != nullptr) {
        delete sequence;
    }
    sequence = src.sequence; // Just move the ownership of the reference to increase the performance
    // Clear the src resource
    src.name = "";
    src.sequence = nullptr;
}

Sequence::~Sequence() {
    if (sequence != nullptr) {
        delete sequence;
    }
}

bool Sequence::operator==(const Sequence &other) const {
    bool rval = true;
    if (size() != other.size()) {
        return false;
    }
    // Compare two sequences without considering lower or captital cases
    for (int i = 0; i < size(); ++i) {
        char otherG = other[i];
        char otherCG = toupper(otherG);
        char thisG = (*this)[i];
        char thisCG = toupper(thisG);
        if (thisCG != otherCG) {
            rval = false;
            break;
        }
    }
    return rval;
}

Sequence &Sequence::operator=(Sequence &&src) {
    name = src.name;
    if (sequence != nullptr) {
        delete sequence;
    }
    sequence = src.sequence; // Just move the ownership of the reference to increase the performance
    // Clear the src resource
    src.name = "";
    src.sequence = nullptr;
    return *this;
}

Sequence &Sequence::operator=(const Sequence &src) {
    name = src.name;
    sequence = new vector<char>();
    *sequence = *src.sequence; // Deep copy the contect of the vector
    return *this;
}

char &Sequence::operator[](unsigned int index) {
    return get(index);
}

const char &Sequence::operator[](unsigned int index) const {
    return get(index);
}

Sequence Sequence::operator()(int start, int end) const {
    Sequence rval;
    
    if (start >= 0 && end >= 0) {
        // Keep adding values to the return sequence in the original order
        if (start >= end) {
            for (int i = start; i < size(); ++i) {
                rval.sequence->push_back((*this)[i]);
            }
            for (int i = 0; i < end; ++i) {
                rval.sequence->push_back((*this)[i]);
            }
        } else {
            for (int i = start; i < end; ++i) {
                rval.sequence->push_back((*this)[i]);
            }
        }
    } else if (start < 0 && end < 0) {
        // Keep adding values to the return sequence in the reverse order
        if (end >= start) {
            for (int i = -start - 1; i < size(); ++i) {
                rval.sequence->push_back((*this)[i]);
            }
            for (int i = 0; i < -end - 1; ++i) {
                rval.sequence->push_back((*this)[i]);
            }
        } else {
            for (int i = -start - 1; i < -end - 1; ++i) {
                rval.sequence->push_back((*this)[i]);
            }
        }
    } else {
        throw invalid_argument("The start index and the end index should both have the same sign");
    }
    
    return rval;
}

Sequence &Sequence::operator+(char gene) {
    sequence->push_back(gene);
    return *this;
}

#warning double check with Dr. Lee where the starting index is for the sequence
Sequence Sequence::operator-() const {
    Sequence rval;
    // Keep adding genes in the reverse order
    for (int i = (int) sequence->size() - 1; i > -1; --i) {
        char gene = (*this)[i];
        gene = complement(gene);
        rval.sequence->push_back(gene);
    }
    rval.name = name + " reversed complementary";
    return rval;
}

static char complement(char gene) {
    // Reverse the pair for the capital letter
    switch (gene) {
        case 'A':
            gene = 'T';
            break;
        case 'T':
            gene = 'A';
            break;
        case 'C':
            gene = 'G';
            break;
        case 'G':
            gene = 'C';
            break;
        default:
            // Simply do nothing here
            break;
    }
    // Reverse the pair for the little letter
    switch (gene) {
        case 'a':
            gene = 't';
            break;
        case 't':
            gene = 'a';
            break;
        case 'c':
            gene = 'g';
            break;
        case 'g':
            gene = 'c';
            break;
        default:
            // Simply do nothing here
            break;
    }
    return gene;
}

int Sequence::size() const {
    return (int) sequence->size();
}

std::string Sequence::getName() const {
    return name;
}

void Sequence::readSequence(const string &filename) {
    ifstream inFile(filename);
    // Determine if the file stream is opened successfully
    if (inFile.is_open()) {
        char c;
        // Skip the first information line
        getline(inFile, name);
        name.erase(0, 1); // Remove '>' symbol
        // Keep reading sequences
        while (inFile.get(c)) {
            if (c != '\n') {
                sequence->push_back(c);
            }
        }
        inFile.close();
    }
}

string Sequence::toString() const {
    string rval = "";
    int counter = 0;
    for (char gene : *sequence) {
        rval += toupper(gene);
        counter++;
        if (counter % 60 == 0) {
            rval += "\n";
        }
    }
    return rval;
}

string Sequence::toHighlightenedString(std::vector<tuple<int, int>> locations) const {
    string rval = "";
    set<int> highlightIndexes;
    for (tuple<int, int> &location : locations) {
        // Convert negative indexes to positive
        if (std::get<0>(location) < 0 && std::get<1>(location) < 0) {
            location = make_tuple(-std::get<0>(location) - 1, -std::get<1>(location) - 1);
        }
        if (std::get<0>(location) < std::get<1>(location)) {
            for (int i = std::get<0>(location); i < std::get<1>(location); i++) {
                highlightIndexes.insert(i % size());
            }
        } else {
            for (int i = std::get<0>(location); i < size(); i++) {
                highlightIndexes.insert(i);
            }
            for (int i = 0; i < std::get<1>(location); ++i) {
                highlightIndexes.insert(i);
            }
        }
    }
    int i = 0;
    for (char gene : *sequence) {
        if (highlightIndexes.find(i) != highlightIndexes.end()) {
            rval += toupper(gene);
        } else {
            rval += tolower(gene);
        }
        i++;
        if (i % 60 == 0) {
            rval += "\n";
        }
    }
    return rval;
}

Sequence::Distribution Sequence::computeDistribution() const {
    double AOccurence = 0, COccurence = 0, GOccurence = 0, TOccurence = 0;
    for (char gene : *sequence) {
        switch (gene) {
            case 'A':
                AOccurence++;
                break;
            case 'C':
                COccurence++;
                break;
            case 'G':
                GOccurence++;
                break;
            case 'T':
                TOccurence++;
                break;
            default:
                // Do nothing here
                break;
        }
    }
    
    // Set the distribution structure
    Distribution distribution;
    // Compute %A
    distribution.APercent = AOccurence / (AOccurence + COccurence + GOccurence + TOccurence) * 100;
    // Compute %C
    distribution.CPercent = COccurence / (AOccurence + COccurence + GOccurence + TOccurence) * 100;
    // Compute %G
    distribution.GPercent = GOccurence / (AOccurence + COccurence + GOccurence + TOccurence) * 100;
    // Compute %T
    distribution.TPercent = TOccurence / (AOccurence + COccurence + GOccurence + TOccurence) * 100;
    // Compute %GC
    distribution.GCPercent = (GOccurence + COccurence) / (AOccurence + COccurence + GOccurence + TOccurence) * 100;
    
    return distribution;
}

char &Sequence::get(unsigned int index) const {
    if (startIndex < 0) {
        return (*sequence)[(-index + startIndex) % size()];
    } else {
        return (*sequence)[(index + startIndex) % size()];
    }
}

void Sequence::indexof(vector<tuple<int, int>> &rval, const Sequence &other) const {
    // By using the frame size, scanning through the sequence to see if the matching sequence has been found
    int frameSize = other.size();
    for (int i = 0; i < size(); ++i) {
        Sequence frame = (*this)(i % size(), (i + frameSize) % size());
        if (frame == other) {
            // Here are values for recording the location range for the sub-sequence
            rval.push_back(make_tuple(i % size(), (i + frameSize) % size()));
        }
    }
}

tuple<int, int> Sequence::firstOccurence(const Sequence &other) const {
    // By using the frame size, scanning through the sequence to see if the matching sequence has been found
    int frameSize = other.size();
    for (int i = 0; i < size(); ++i) {
        Sequence frame = (*this)(i % size(), (i + frameSize) % size());
        if (frame == other) {
            // Here are values for recording the location range for the sub-sequence
            return make_tuple(i % size(), (i + frameSize) % size());
        }
    }
    return make_tuple(0, 0);
}

vector<tuple<int, int>> Sequence::indexof(const Sequence &other) const {
    vector<tuple<int, int>> rval;
    indexof(rval, other);
    vector<tuple<int, int>> rrval;
    indexof(rrval, -other);
    for (tuple<int, int> &location : rrval) {
        location = make_tuple(-std::get<0>(location) - 1, -std::get<1>(location) - 1);
        rval.push_back(move(location));
    }
    return rval;
}

tuple<int, int> Sequence::locate(const Sequence &other) const {
    cout << ">locatinng " << other.name << "..." << endl;
    tuple<int, int> rval;
    rval = firstOccurence(other);
    if (rval == make_tuple(0, 0)) {
        rval = firstOccurence(-other);
        rval = make_tuple(-std::get<0>(rval) - 1, -std::get<1>(rval) - 1);
    }
    return rval;
}

int Sequence::syncStart(const Sequence &oriC) {
    tuple<int, int> startLocatoin = locate(oriC);
    if (std::get<0>(startLocatoin) != 0 && std::get<1>(startLocatoin) != 0) {
        startIndex = std::get<0>(startLocatoin);
    }
    return startIndex;
}

string Sequence::Distribution::toString() {
    string AP = "A%=" + to_string(APercent) + "%";
    string CP = "C%=" + to_string(CPercent) + "%";
    string GP = "G%=" + to_string(GPercent) + "%";
    string TP = "T%=" + to_string(TPercent) + "%";
    string GCP = "GC%=" + to_string(GCPercent) + "%";
    return AP + string("\n") + CP + string("\n") + GP + string("\n") + TP + string("\n") + GCP;
}
