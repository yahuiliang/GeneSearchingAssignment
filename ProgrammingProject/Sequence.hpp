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

/**
 * The structure represents the double-stranded Sequence
 */
struct Sequence {
    // Here are constructor declarations
    Sequence(); // Just initialize the empty sequence
    Sequence(const Sequence & src); // The copy constructor for the sequence
    Sequence(Sequence && src); // The move constructor of the sequence
    Sequence(const std::string & genes);
    Sequence(const std::string & genes, const std::string & name);
    // Overriden operators
    Sequence & operator=(Sequence && src); // The move assign operator
    Sequence & operator=(const Sequence & src); // The copy assign operator
    bool operator==(const Sequence & other) const; // The compare equality operator
    bool operator<(const Sequence & other) const; // The comparison operator
    Sequence operator-() const; // The function should be the reverse complement sequence
    // This is mainly for getting the single gene from the sequence
    // If the index overflows, the invalid arg exception will be thrown
    // The sequence itself was circular
    const char & operator[](int index) const;
    // Get the sub-sequence within a range
    // When the start and the end are both negative, it searches in the reverse complementary order
    // If the index overflows, the invalid arg exception will be thrown
    // The range will be between start and (end - 1)
    // The end index will not be included in the returned sequence
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
    int size() const; // returns the size of the sequence
    std::string getName() const; // gets the name of the sequence
    // Convert the sequence to the string format
    std::string toString() const;
    // The string will be highlighted in certain sections by using locations provided
    std::string toHighlightenedString(std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>> locations) const;
    // Scan the sequence and compute the composition
    Distribution computeDistribution() const;
    // Locates DNA boxes in the sequence
    std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>> locateBoxes(const std::set<Sequence> & boxes) const;
    // Locate the sequence in the current sequence
    // Returns the location where the other sequence is located
    std::tuple<std::tuple<int, int>, std::tuple<int, int>> locate(const Sequence & other) const;
    // The method is for synchronizing the start of the sequence
    // If no sequence matched with the provided one, the startIndex will remain same
    int syncStart(const Sequence & oriC);
private:
    std::string name; // the name of the sequence
    
    // Double-strands sequence is immutable
    std::vector<char> sequence;
    std::vector<char> rsequence;
    
    int startIndex = 0; // The start index of the sequence
    int startIndexR = 0; // The start index of the sequence in the reversed complementary order

    Sequence sub(int start, int end) const;
    bool compareTo(const Sequence & other, int start, int end) const; // Compare if the marked section is equal between sequences
};

#endif /* Sequence_hpp */
