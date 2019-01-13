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
    name = "";
}

Sequence::Sequence(const Sequence & src) {
    name = src.name;
    startIndex = src.startIndex;
    startIndexR = src.startIndexR;
    sequence = src.sequence; // Deep copy the contect of the vector
    rsequence = src.rsequence;
}

Sequence::Sequence(Sequence && src) {
    name = move(src.name);
    
    startIndex = src.startIndex;
    startIndexR = src.startIndexR;
    src.startIndex = 0;
    src.startIndexR = 0;
    
    sequence = move(src.sequence); // Just move the ownership of the reference to increase the performance
    rsequence = move(src.rsequence);
}

Sequence::Sequence(const std::string & genes) {
    name = "";
    for (int i = 0; i < genes.length(); ++i) {
        char gene = genes.at(i);
        sequence.push_back(gene);
        rsequence.push_back(complement(gene));
    }
    name = "unknown name";
}

Sequence::~Sequence() {
    // No memory should be released
}

// The method is the absolute comparison between two sequences
bool Sequence::operator==(const Sequence & other) const {
    if (size() != other.size()) {
        return false;
    }
    bool isEqual = true;
    // Compare two sequences without considering lower or captital cases
    for (int i = 0; isEqual && i < size(); ++i) {
        char otherG = other[i];
        char otherCG = toupper(otherG);
        char thisG = (*this)[i];
        char thisCG = toupper(thisG);
        if (thisCG != otherCG) {
            isEqual = false;
        }
    }
    if (!isEqual) {
        isEqual = true;
        for (int i = -1; isEqual && i > -(size() + 1); --i) {
            char otherG = other[-i - 1];
            char otherCG = toupper(otherG);
            char thisG = (*this)[i];
            char thisCG = toupper(thisG);
            if (thisCG != otherCG) {
                isEqual = false;
            }
        }
    }
    return isEqual;
}

Sequence & Sequence::operator=(Sequence && src) {
    name = move(src.name);
    
    startIndex = src.startIndex;
    startIndexR = src.startIndexR;
    src.startIndex = 0;
    src.startIndexR = 0;
    
    sequence = move(src.sequence); // Just move the ownership of the reference to increase the performance
    rsequence = move(src.rsequence);
    
    return *this;
}

Sequence & Sequence::operator=(const Sequence & src) {
    name = src.name;
    startIndex = src.startIndex;
    startIndexR = src.startIndexR;
    sequence = src.sequence; // Deep copy the contect of the vector
    rsequence = src.rsequence;
    return *this;
}

// The operator automatically read the reverse complmentary in order with the base index
const char &Sequence::operator[](int index) const {
    if ((index > -1 && index >= size()) || (index < 0 && index < -size())) {
        throw invalid_argument("the index is out of range");
    }
    if (index > -1) {
        index = (index + startIndex) % size();
        return sequence[index];
    } else {
        index = (-index + startIndexR - 1) % size();
        return rsequence[rsequence.size() - index - 1];
    }
}

Sequence &Sequence::operator+(char gene) {
    sequence.push_back(gene);
    rsequence.push_back(complement(gene));
    return *this;
}

Sequence Sequence::operator()(int start, int end) const {
    if ((start < 0 && -start > size()) || start >= size()) {
        throw invalid_argument("The start index is out of range");
    }
    if ((end < 0 && -end > size()) || end >= size()) {
        throw invalid_argument("The end index is out of range");
    }
    Sequence rval;
    if (start >= 0 && end >= 0) {
        // Keep adding values to the return sequence in the original order
        if (start < end) {
            for (int i = start; i < end; ++i) {
                rval + (*this)[i];
            }
        } else {
            for (int i = start; i < size(); ++i) {
                rval + (*this)[i];
            }
            for (int i = 0; i < end; ++i) {
                rval + (*this)[i];
            }
        }
    } else if (start < 0 && end < 0) {
        // Keep adding values to the return sequence in the reverse order
        if (start > end) {
            for (int i = start; i > end; --i) {
                rval + (*this)[i];
            }
        } else {
            for (int i = start; i >= -size(); --i) {
                rval + (*this)[i];
            }
            for (int i = -1; i > end; --i) {
                rval + (*this)[i];
            }
        }
    } else {
        throw invalid_argument("The start index and the end index should both have the same sign");
    }
    return rval;
}

Sequence Sequence::operator-() const {
    Sequence rval;
    // Keep adding genes in the reverse order
    for (int i = 0; i < size(); ++i) {
        char gene = (*this)[-(i + 1)];
        rval + gene;
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
    return (int) sequence.size();
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
                sequence.push_back(c);
                rsequence.push_back(complement(c));
            }
        }
        inFile.close();
    }
}

string Sequence::toString() const {
    string rval = "";
    int counter = 0;
    for (char gene : sequence) {
        rval += toupper(gene);
        counter++;
        if (counter % 60 == 0) {
            rval += "\n";
        }
    }
    return rval;
}

string Sequence::toHighlightenedString(vector<tuple<tuple<int, int>, tuple<int, int>>> locations) const {
    string rval = "";
    set<int> highlightIndexes;
    for (tuple<tuple<int, int>, tuple<int, int>> &location : locations) {
        // Find the one corresponds to the ori sequence
        tuple<int, int> tmp = get<0>(location);
        if (get<0>(tmp) < 0) {
            tmp = get<1>(location);
        }
        // Convert negative indexes to positive
        int location1 = std::get<0>(tmp);
        int location2 = std::get<1>(tmp);
        if (location1 < location2) {
            for (int i = location1; i < location2; i++) {
                highlightIndexes.insert(i);
            }
        } else {
            for (int i = location1; i < size(); i++) {
                highlightIndexes.insert(i);
            }
            for (int i = 0; i < location2; ++i) {
                highlightIndexes.insert(i);
            }
        }
    }
    int i = 0;
    for (char gene : sequence) {
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
    for (char gene : sequence) {
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

// If the first location is negative, this means the sequence that the function searches for is the in the reverse complementary order
vector<tuple<std::tuple<int, int>, tuple<int, int>>> Sequence::indexof(const Sequence &other) const {
    if (other.size() > size()) {
        throw invalid_argument("the size of the sequence to be located is greater than the src sequence");
    }
    vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>> rval;
    // By using the frame size, scanning through the sequence to see if the matching sequence has been found
    int frameSize = other.size();
    for (int i = 0; i < size(); ++i) {
        int start = i;
        int end = (i + frameSize) % size();
        if (compareTo(other, start, end)) {
            // Here are values for recording the location range for the sub-sequence
            auto po = make_tuple(start, end);
            auto rpo = make_tuple(-((size() - end) % size()) - 1, -((size() - start) % size()) - 1);
            rval.push_back(make_tuple(po, rpo));
        }
    }
    for (int i = -1; i > -size() - 1; --i) {
        int start = i;
        int end = -(((-i - 1) + frameSize) % size() + 1);
        if (compareTo(other, start, end)) {
            // Here are values for recording the location range for the sub-sequence
            auto po = make_tuple(start, end);
            auto rpo = make_tuple((size() + end + 1) % size(), (size() + start + 1) % size());
            rval.push_back(make_tuple(po, rpo));
        }
    }
    return rval;
}

tuple<tuple<int, int>, tuple<int, int>> Sequence::locate(const Sequence &other) const {
    if (other.size() > size()) {
        throw invalid_argument("the size of the sequence to be located is greater than the src sequence");
    }
    int frameSize = other.size();
    for (int i = 0; i < size(); ++i) {
        int start = i;
        int end = (i + frameSize) % size();
        if (compareTo(other, start, end)) {
            // Here are values for recording the location range for the sub-sequence
            auto po = make_tuple(start, end);
            auto rpo = make_tuple(-((size() - end) % size()) - 1, -((size() - start) % size()) - 1);
            return make_tuple(po, rpo);
        }
    }
    for (int i = -1; i > -size() - 1; --i) {
        int start = i;
        int end = -(((-i - 1) + frameSize) % size() + 1);
        if (compareTo(other, start, end)) {
            // Here are values for recording the location range for the sub-sequence
            auto po = make_tuple((size() + end + 1) % size(), (size() + start + 1) % size());
            auto rpo = make_tuple(start, end);
            return make_tuple(po, rpo);
        }
    }
    return make_tuple(make_tuple(size(), size()), make_tuple(size(), size()));
}

int Sequence::syncStart(const Sequence &oriC) {
    tuple<int, int> startLocatoin = std::get<0>(locate(oriC));
    if (std::get<0>(startLocatoin) != size() && std::get<1>(startLocatoin) != size()) {
        int start = std::get<0>(startLocatoin);
        int end = std::get<1>(startLocatoin);
        if (start < 0) {
            // The reverse complementary order
            startIndexR = -end - 1;
            startIndex = size() + end;
        } else {
            // The normal order
            startIndex = start;
            startIndexR = size() - start;
        }
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

bool Sequence::compareTo(const Sequence & other, int start, int end) const {
    bool isEqual = true;
    int j = 0;
    if (start < 0 && end < 0) {
        if (start > end) {
            for (int i = start; i > end; --i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
        } else {
            for (int i = start; i > -size() - 1; --i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
            for (int i = -1; i > end; --i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
        }
    } else {
        if (start >= end) {
            for (int i = start; isEqual && i < size(); ++i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
            for (int i = 0; isEqual && i < end; ++i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
        } else {
            for (int i = start; isEqual && i < end; ++i, ++j) {
                if (toupper(other[j]) != toupper((*this)[i])) {
                    isEqual = false;
                }
            }
        }
    }
    return isEqual;
}
