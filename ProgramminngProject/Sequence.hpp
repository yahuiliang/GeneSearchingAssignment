//
//  Sequence.hpp
//  ProgramminngProject
//
//  Created by Yahui Liang on 11/30/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#ifndef Sequence_hpp
#define Sequence_hpp

#include <stdio.h>
#include <vector>
#include <tuple>
#include <string>

struct Sequence {
    // Here are constructor declarations
    Sequence(); // Just initialize the empty sequence
    ~Sequence(); // The destructor of the sequence
    Sequence(const Sequence &src); // The copy constructor for the sequence
    Sequence(Sequence &&src); // The move constructor of the sequence
    // Overriden operators
    Sequence &operator=(Sequence &&src);
    Sequence &operator=(const Sequence &src);
    bool operator==(const Sequence &other) const;
    // The function will push an element to the sequence. The reference to itself will be returned. The returned value can be
    // assigned to either the new object or the referece of the object.
    Sequence &operator+(char gene);
    Sequence operator-() const; // The function should be the reverse complement sequence
    // This is mainly for getting the single gene from the sequence
    // If the index overflows, the index will start from zero
    // The sequence itself was circular
    char &operator[](unsigned int index);
    const char &operator[](unsigned int index) const;
    // Get the sub-sequence within a range
    // When the start and the end are both negative, it searches in the reverse complementary order
    Sequence operator()(int start, int end) const;
    // The structure for shown the distribution of the sequence
    struct Distribution {
        double APercent = 0;
        double CPercent = 0;
        double GPercent = 0;
        double TPercent = 0;
        double GCPercent = 0;
        std::string toString();
    };
    int size() const;
    std::string getName() const;
    // Reads the sequence from the file
    void readSequence(const std::string &filename);
    // Convert the sequence to the string format
    std::string toString() const;
    std::string toHighlightenedString(std::vector<std::tuple<int, int>> locations) const;
    // Scan the sequence and compute the composition
    Distribution computeDistribution() const;
    // (0, 0) will be returned if there is no frame matching with the sequence
    // The base of the index is 0
    // The difference between `indexof` and `locate` is that `locate` only return the location of the first occurence
    std::vector<std::tuple<int, int>> indexof(const Sequence &other) const;
    std::tuple<int, int> locate(const Sequence &other) const;
    // The method is for synchronizing the start of the sequence
    // If no sequence matched with the provided one, the startIndex will remain same
    int syncStart(const Sequence &oriC);
private:
    std::string name;
    std::vector<char> *sequence = nullptr;
    int startIndex = 0;
    
    char &get(unsigned int index) const;
    void indexof(std::vector<std::tuple<int, int>> &rval, const Sequence &other) const;
    std::tuple<int, int> firstOccurence(const Sequence &other) const;
};

#endif /* Sequence_hpp */
