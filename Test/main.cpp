//
//  main.cpp
//  Test
//
//  Created by Yahui Liang on 1/14/19.
//  Copyright © 2019 Yahui Liang. All rights reserved.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "Sequence.hpp"
#include "DnaABoxesGenerator.hpp"
#include "catch.hpp"
#include <set>
#include <iostream>

using namespace std;

TEST_CASE("The sequence can be initialized from the string if") {
    SECTION("The valid string is provided") {
        string gene = "TTATCCACA";
        Sequence seq(gene);
        REQUIRE(seq.size() == gene.size());
        REQUIRE(seq.toString() == gene);
        REQUIRE(seq.getName() == "unknown sequence");
    }
    
    SECTION("The valid string and the valid name are provided") {
        string gene = "TTATCCACA";
        Sequence seq(gene, "test sequence");
        REQUIRE(seq.size() == gene.size());
        REQUIRE(seq.toString() == gene);
        REQUIRE(seq.getName() == "test sequence");
    }
    
    SECTION("Nothing is provided") {
        Sequence seq;
        REQUIRE(seq.size() == 0);
        REQUIRE(seq.toString() == "");
        REQUIRE(seq.getName() == "unknown sequence");
    }
}

TEST_CASE("Copy Operation") {
    Sequence a("TTATCCACA");
    Sequence b = a;
    REQUIRE(a == b);
    Sequence c;
    c = a;
    REQUIRE(a == c);
}

TEST_CASE("Move Operation") {
    Sequence a("TTATCCACA");
    Sequence b = move(a);
    REQUIRE(b.size() == 9);
    REQUIRE(b.toString() == "TTATCCACA");
    REQUIRE(a.size() == 0);
    REQUIRE(a.toString() == "");
    a = Sequence("TTATCCACA");
    Sequence c;
    c = move(a);
    REQUIRE(c.size() == 9);
    REQUIRE(c.toString() == "TTATCCACA");
    REQUIRE(a.size() == 0);
    REQUIRE(a.toString() == "");
}

TEST_CASE("Sequence Reverse Complementary") {
    SECTION("Normal sequence reverse") {
        Sequence seq("TTATCCACA");
        Sequence seqr = -seq;
        REQUIRE(seq == seqr);
        REQUIRE(seqr.getName() == seq.getName() + " reversed complementary");
    }
    
    SECTION("Empty sequence reverse") {
        Sequence seq("");
        Sequence seqr = -seq;
        REQUIRE(seq == seqr);
        REQUIRE(seqr.getName() == seq.getName() + " reversed complementary");
    }
}

TEST_CASE("Sequence Compare Operation") {
    SECTION("Compare with longer sequence") {
        Sequence a("A");
        Sequence b("CT");
        REQUIRE(a < b);
    }
    
    SECTION("Compare with equal length sequence") {
        Sequence a("AT");
        Sequence b("CT");
        REQUIRE(a < b);
    }
    
    SECTION("Equality by using compare operator") {
        Sequence a("CT");
        Sequence b("CT");
        REQUIRE((!(a < b) && !(b < a)));
    }
}

TEST_CASE("Index synchronizing") {
    SECTION("Without reversed complementary") {
        Sequence seq("TTATCCACA");
        Sequence oriC("ACATT");
        seq.syncStart(oriC);
        Sequence result = seq(0, 0);
        REQUIRE(result.toString() == "ACATTATCC");
    }
    
    SECTION("With reversed complementary") {
        Sequence seq("TTATCCACA");
        Sequence oriC("AATGT");
        seq.syncStart(oriC);
        Sequence result = seq(0, 0);
        REQUIRE(result.toString() == "ACATTATCC");
    }
}

TEST_CASE("Sequence fetching") {
    Sequence seq("TTATCCACA");
    SECTION("Single fetching") {
        SECTION("In range") {
            char gene = seq[4];
            REQUIRE(gene == 'C');
            gene = seq[0];
            REQUIRE(gene == 'T');
            gene = seq[seq.size() - 1];
            REQUIRE(gene == 'A');
            gene = seq[-1];
            REQUIRE(gene == 'T');
            gene = seq[-4];
            REQUIRE(gene == 'G');
            gene = seq[-seq.size()];
            REQUIRE(gene == 'A');
        }
        
        SECTION("Out of range") {
            SECTION("Without reverse") {
                try {
                    seq[seq.size()];
                } catch (const invalid_argument & err) {
                    REQUIRE(string(err.what()) == "the index is out of range");
                }
            }
            
            SECTION("With reverse") {
                try {
                    seq[-seq.size() - 1];
                } catch (const invalid_argument & err) {
                    REQUIRE(string(err.what()) == "the index is out of range");
                }
            }
        }
    }
    
    SECTION("Range fetching") {
        SECTION("In range") {
            Sequence result = seq(0, 0);
            Sequence other = seq;
            REQUIRE(result == other);
            
            result = seq(3, 5);
            other = Sequence("TC");
            REQUIRE(result == other);
            
            result = seq(5, 3);
            other = Sequence("CACATTA");
            REQUIRE(result == other);
            
            result = seq(seq.size() - 2, 3);
            other = Sequence("CATTA");
            REQUIRE(result == other);
            
            result = seq(-1, -1);
            other = seq;
            REQUIRE(result == other);
            
            result = seq(-3, -5);
            other = Sequence("TG");
            REQUIRE(result == other);
            
            result = seq(-5, -3);
            other = Sequence("GATAATG");
            REQUIRE(result == other);
            
            result = seq(-seq.size() + 2, -3);
            other = Sequence("TAATG");
            REQUIRE(result == other);
        }
        
        SECTION("Out of range") {
            try {
                seq(0, seq.size());
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The end index is out of range");
            }
            
            try {
                seq(seq.size(), 0);
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The start index is out of range");
            }
            
            try {
                seq(0, -seq.size() - 1);
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The end index is out of range");
            }
            
            try {
                seq(-seq.size() - 1, 0);
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The start index is out of range");
            }
            
            try {
                seq(0, -seq.size());
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The start index and the end index should both have the same sign");
            }
            
            try {
                seq(-seq.size(), 0);
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "The start index and the end index should both have the same sign");
            }
        }
    }
}

TEST_CASE("The sequence compute the equality with the other sequence if") {
    SECTION("They are equal") {
        SECTION("Without reverse complementary") {
            Sequence a("TTATCCACA");
            Sequence b("TTATCCACA");
            REQUIRE(a == b);
        }
        
        SECTION("With reverse complementary") {
            Sequence a("TTATCCACA");
            Sequence b("TGTGGATAA");
            REQUIRE(a == b);
        }
    }
    
    SECTION("The other sequence is not equal") {
        SECTION("And the other sequence's size is not equal to the src") {
            Sequence a("TTATCCACA");
            Sequence b("TTACCACA");
            REQUIRE(!(a == b));
        }
        
        SECTION("And the other sequence's size is equal, but genes are not equal") {
            Sequence a("TTATCCACA");
            Sequence b("TTATACACA");
            REQUIRE(!(a == b));
        }
    }
}

TEST_CASE("The sequence locate the other sequence if") {
    SECTION("The provided sequence is larger than the source sequence") {
        Sequence src("TTATCCACA");
        Sequence other("TTATCCACAA");
        try {
            src.locate(other);
        } catch (const invalid_argument & err) {
            REQUIRE(string(err.what()) == "the size of the sequence to be located is greater than the src sequence");
        }
    }
    SECTION("The provided sequence is empty") {
        Sequence src("TTATCCACA");
        Sequence other;
        try {
            src.locate(other);
        } catch (const invalid_argument & err) {
            REQUIRE(string(err.what()) == "the other sequence is empty");
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
            auto normal = get<0>(location);
            auto reverse = get<1>(location);
            int start = get<0>(normal);
            int end = get<1>(normal);
            REQUIRE(start == 3);
            REQUIRE(end == 5);
            start = get<0>(reverse);
            end = get<1>(reverse);
            REQUIRE(start == -5);
            REQUIRE(end == -7);
        }
        SECTION("With reverse complementary") {
            Sequence other("GA");
            auto location = src.locate(other);
            auto normal = get<0>(location);
            auto reverse = get<1>(location);
            int start = get<0>(normal);
            int end = get<1>(normal);
            REQUIRE(start == 3);
            REQUIRE(end == 5);
            start = get<0>(reverse);
            end = get<1>(reverse);
            REQUIRE(start == -5);
            REQUIRE(end == -7);
        }
    }
    
    SECTION("The sequence to be located is at two sides of the sequence") {
        Sequence src("TGTGGACAC");
        SECTION("Without reverse complementary") {
            Sequence other("CT");
            auto location = src.locate(other);
            auto normal = get<0>(location);
            auto reverse = get<1>(location);
            int start = get<0>(normal);
            int end = get<1>(normal);
            REQUIRE(start == 8);
            REQUIRE(end == 1);
            start = get<0>(reverse);
            end = get<1>(reverse);
            REQUIRE(start == -9);
            REQUIRE(end == -2);
        }
        SECTION("With reverse complementary") {
            Sequence other("AG");
            auto location = src.locate(other);
            auto normal = get<0>(location);
            auto reverse = get<1>(location);
            int start = get<0>(normal);
            int end = get<1>(normal);
            REQUIRE(start == 8);
            REQUIRE(end == 1);
            start = get<0>(reverse);
            end = get<1>(reverse);
            REQUIRE(start == -9);
            REQUIRE(end == -2);
        }
    }
}

TEST_CASE("The sequence locate boxes if") {
    Sequence src("TGTGGACAC");
    SECTION("boxes are empty") {
        SECTION("if the box set is empty") {
            set<Sequence> boxes;
            auto locations = src.locateBoxes(boxes);
            REQUIRE(locations.size() == 0);
        }
        
        SECTION("if the box set contains empty box") {
            try {
                set<Sequence> boxes;
                boxes.insert(Sequence());
                src.locateBoxes(boxes);
            } catch (const invalid_argument & err) {
                REQUIRE(string(err.what()) == "the size of boxes set to be located is empty");
            }
        }
    }
    
    SECTION("valid boxes are provided") {
        Sequence box1("TG");
        Sequence box2("AC");
        Sequence box3("CT");
        
        set<Sequence> boxes;
        boxes.insert(box1);
        boxes.insert(box2);
        boxes.insert(box3);
        
        vector<tuple<tuple<int, int>, tuple<int, int>>> locations = src.locateBoxes(boxes);
        
        REQUIRE(locations.size() == 5);
        
        auto location1 = locations[0];
        auto normal = get<0>(location1);
        auto reverse = get<1>(location1);
        int start = get<0>(normal);
        int end = get<1>(normal);
        REQUIRE(start == 0);
        REQUIRE(end == 2);
        start = get<0>(reverse);
        end = get<1>(reverse);
        REQUIRE(start == -8);
        REQUIRE(end == -1);
        
        auto location2 = locations[1];
        normal = get<0>(location2);
        reverse = get<1>(location2);
        start = get<0>(normal);
        end = get<1>(normal);
        REQUIRE(start == 2);
        REQUIRE(end == 4);
        start = get<0>(reverse);
        end = get<1>(reverse);
        REQUIRE(start == -6);
        REQUIRE(end == -8);
        
        auto location3 = locations[2];
        normal = get<0>(location3);
        reverse = get<1>(location3);
        start = get<0>(normal);
        end = get<1>(normal);
        REQUIRE(start == 5);
        REQUIRE(end == 7);
        start = get<0>(reverse);
        end = get<1>(reverse);
        REQUIRE(start == -3);
        REQUIRE(end == -5);
        
        auto location4 = locations[3];
        normal = get<0>(location4);
        reverse = get<1>(location4);
        start = get<0>(normal);
        end = get<1>(normal);
        REQUIRE(start == 7);
        REQUIRE(end == 0);
        start = get<0>(reverse);
        end = get<1>(reverse);
        REQUIRE(start == -1);
        REQUIRE(end == -3);
        
        auto location5 = locations[4];
        normal = get<0>(location5);
        reverse = get<1>(location5);
        start = get<0>(normal);
        end = get<1>(normal);
        REQUIRE(start == 8);
        REQUIRE(end == 1);
        start = get<0>(reverse);
        end = get<1>(reverse);
        REQUIRE(start == -9);
        REQUIRE(end == -2);
    }
}

TEST_CASE("Sequence highlight string operation") {
    Sequence seq("TGTGGATAA");
    SECTION("General Case") {
        set<Sequence> boxes;
        boxes.insert(Sequence("GA"));
        boxes.insert(Sequence("AT"));
        auto locations = seq.locateBoxes(boxes);
        string highlightedString = seq.toHighlightenedString(locations);
        REQUIRE(highlightedString == "TgtgGATaA");
    }
    
    SECTION("The reversed order location") {
        vector<tuple<tuple<int, int>, tuple<int, int>>> locations;
        locations.push_back(make_tuple(make_tuple(7, 2), make_tuple(-8, -3)));
        string highlightedString = seq.toHighlightenedString(locations);
        REQUIRE(highlightedString == "TGtggatAA");
    }
}

TEST_CASE("Sequence distribution computation") {
    Sequence seq("TGTGGATACT");
    Sequence::Distribution result = seq.computeDistribution();
    REQUIRE(result.APercent == 20);
    REQUIRE(result.CPercent == 10);
    REQUIRE(result.GPercent == 30);
    REQUIRE(result.TPercent == 40);
    REQUIRE(result.GCPercent == 40);
}

TEST_CASE("Generate DNA boxes") {
    auto boxes = gen9MerDnaABoxes();
    Sequence seq("TGTGGATAT");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("GTATCCACA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TTATCGACA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TGTGGACAA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TGTGGATAA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TTATCCACA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TGTGGATAA");
    REQUIRE(boxes.find(seq) != boxes.end());
    seq = Sequence("TGTGGATAG");
    REQUIRE(boxes.find(seq) != boxes.end());
}
