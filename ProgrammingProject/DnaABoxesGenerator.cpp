//
//  DnaABoxesGenerator.cpp
//  ProgrammingProject
//
//  Created by Yahui Liang on 1/27/19.
//  Copyright © 2019 Yahui Liang. All rights reserved.
//

#include "DnaABoxesGenerator.hpp"

using namespace std;

// The recursive function for generating 9-mer DnaA boxes with all possibilities
// tmpStored vector is for tracking boxes added so far.
static vector<string> gen9MerDnaABoxes(const vector<vector<char>> &gen9MerDnaAs, int position) {
    vector<string> seqs;
    if (position < gen9MerDnaAs.size()) {
        vector<string> subResult = gen9MerDnaABoxes(gen9MerDnaAs, position + 1);
        if (subResult.size() > 0) {
            for (string &tmp : subResult) {
                for (char gene : gen9MerDnaAs[position]) {
                    string cpy = gene + tmp;
                    seqs.push_back(move(cpy));
                }
            }
        } else {
            for (char gene : gen9MerDnaAs[position]) {
                char cstring[5];
                cstring[0] = gene;
                cstring[1] = '\0';
                string tmp(cstring);
                seqs.push_back(move(tmp));
            }
        }
        
    }
    return seqs;
}

// The function for generating 9-mer DnaA boxes with all possibilities
set<Sequence> gen9MerDnaABoxes() {
    const static vector<vector<char>> gen9MerDnaAs = {{'T'}, {'G'}, {'T'}, {'G', 'C'}, {'G'}, {'A'}, {'T', 'C'}, {'A'}, {'T', 'A', 'C', 'G'}};
    const vector<string> boxes = gen9MerDnaABoxes(gen9MerDnaAs, 0);
    set<Sequence> ans;
    for (string box : boxes) {
        Sequence tmp(box);
        Sequence tmpr = -tmp;
        ans.insert(move(tmp));
        ans.insert(move(tmpr));
    }
    return ans;
}
