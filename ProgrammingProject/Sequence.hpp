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
#include <set>
#include <tuple>
#include <string>

struct Sequence {
    // Here are constructor declarations
    Sequence(); // Just initialize the empty sequence
    Sequence(const Sequence & src); // The copy constructor for the sequence
    Sequence(Sequence && src); // The move constructor of the sequence
    Sequence(const std::string & genes);
    Sequence(const std::string & genes, const std::string & name);
    // Overriden operators
    Sequence & operator=(Sequence && src);
    Sequence & operator=(const Sequence & src);
    bool operator==(const Sequence & other) const;
    bool operator<(const Sequence & other) const;
    Sequence operator-() const; // The function should be the reverse complement sequence
    // This is mainly for getting the single gene from the sequence
    // If the index overflows, the index will start from zero
    // The sequence itself was circular
    const char & operator[](int index) const;
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
    // Convert the sequence to the string format
    std::string toString() const;
    std::string toHighlightenedString(std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>> locations) const;
    // Scan the sequence and compute the composition
    Distribution computeDistribution() const;
    std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>> locateBoxes(const std::set<Sequence> & boxes) const;
    std::tuple<std::tuple<int, int>, std::tuple<int, int>> locate(const Sequence & other) const;
    // The method is for synchronizing the start of the sequence
    // If no sequence matched with the provided one, the startIndex will remain same
    int syncStart(const Sequence & oriC);
private:
    std::string name;
    
    // Double-strands sequence is immutable
    std::vector<char> sequence;
    std::vector<char> rsequence;
    
    int startIndex = 0;
    int startIndexR = 0;

    Sequence sub(int start, int end) const;
    bool compareTo(const Sequence & other, int start, int end) const;
};

#endif /* Sequence_hpp */
