//
//  main.cpp
//  Test
//
//  Created by Yahui Liang on 1/14/19.
//  Copyright © 2019 Yahui Liang. All rights reserved.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "Sequence.hpp"
#include "catch.hpp"
#include <iostream>

using namespace std;

TEST_CASE("The sequence can be initialized from the string") {
    SECTION("The valid string is provided") {
        string gene = "TTATCCACA";
        Sequence seq(gene);
        REQUIRE(seq.size() == gene.size());
        REQUIRE(seq.toString() == gene);
    }
}

TEST_CASE("The sequence locate the other sequence at the right location") {
    SECTION("The provided sequence is larger than the source sequence") {
        Sequence src("TTATCCACA");
        Sequence other("TTATCCACAA");
        try {
            src.locate(other);
        } catch (invalid_argument & err) {
            SUCCEED("The invalid argument exception is thrown when the other sequence is larger than the src sequence");
        }
    }
    
    SECTION("The provided sequence is identical to the source sequence") {
        Sequence src("TTATCCACA");
        Sequence other("TTATCCACA");
        auto location = src.locate(other);
        int start = get<0>(get<0>(location));
        int end = get<1>(get<0>(location));
        REQUIRE(start == end);
        REQUIRE(start == 0);
        start = get<0>(get<1>(location));
        end = get<1>(get<1>(location));
        REQUIRE(start == end);
        REQUIRE(start == -1);
    }
    
    SECTION("The provided sequence is identical to the source sequence and it is in the reverse complementary form") {
        Sequence src("TTATCCACA");
        Sequence other = -src;
        auto location = src.locate(other);
        int start = get<0>(get<0>(location));
        int end = get<1>(get<0>(location));
        REQUIRE(start == end);
        REQUIRE(start == 0);
        start = get<0>(get<1>(location));
        end = get<1>(get<1>(location));
        REQUIRE(start == end);
        REQUIRE(start == -1);
    }
    
    SECTION("The sequence to be located is in the middle of the sequence") {
        Sequence src("TTATCCACA");
        SECTION("Without reverse complementary") {
            Sequence other("TC");
            auto location = src.locate(other);
        }
        SECTION("With reverse complementary") {
            
        }
    }
}
