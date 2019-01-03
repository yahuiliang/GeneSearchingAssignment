//
//  main.cpp
//  ProgramminngProject
//
//  Created by Yahui Liang on 11/29/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "Sequence.hpp"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <regex>
#include <vector>
#include <tuple>

using namespace std;
using namespace RestClient;

/**
 * Download the gen file from the database
 * The accession number should be provided
 */
static void downloadGenFile(const string &accessionNumber);
static void printLocation(const string &seqName, const tuple<int, int> location);
static void reportRelativeLocations(const Sequence &seq, const Sequence &oriC_1, const Sequence &oriC_2, const Sequence &dnaA);
static vector<Sequence> gen9MerDnaABoxes();
static vector<tuple<int, int>> search9MerDnaABoxes(const Sequence &oriC_1);

#warning the vector data structure may be switched to others to maximize the performance
#warning how to determine which part is the oriC sequence. By hardcoding?
int main(int argc, const char * argv[]) {
    // Determine if the number of arguments is valid
    if (argc < 2) {
        cout << "The program accepts the accession number" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Redirect the output
    //    ofstream out("out.txt");
    //    cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    
    // Get the accession number
    string accessionnNumber(argv[1]);
    
    // Download the gen file
    // TODO: handle exceptional cases here
    // The file is downloaded to the same directory as the executable binary
    
    // Check if the file has existed
    // If not, install the file from the database
    ifstream sequenceFile("sequence.fasta");
    if (!((bool)sequenceFile)) {
        downloadGenFile(accessionnNumber);
    }
    
    // Read the sequence from the file
    Sequence sequence;
    sequence.readSequence("sequence.fasta");
    // Read the oriC_1 sequence from the file
    Sequence oriC_1;
    oriC_1.readSequence("oriC_1.fasta");
    // Read the oriC_2 sequence from the file
    Sequence oriC_2;
    oriC_2.readSequence("oriC_2.fasta");
    // Read the dnaA sequence from the file
    Sequence dnaA;
    dnaA.readSequence("dnaA.fasta");
    
    // Print out the distribution report
    cout << ">Computing the distribution of the sequence..." << endl;
    Sequence::Distribution distribution = sequence.computeDistribution();
    cout << distribution.toString() << endl;
    cout << endl;
    // Report locations of oriCs and dnaA
    reportRelativeLocations(sequence, oriC_1, oriC_2, dnaA);
    cout << endl;
    // Print out highlightened 9Mer boxes
    vector<tuple<int, int>> locations;
    cout << ">DnaA boxes " << oriC_1.getName() << endl;
    locations = search9MerDnaABoxes(oriC_1);
    cout << oriC_1.toHighlightenedString(locations) << endl;
    cout << ">DnaA boxes " << oriC_2.getName() << endl;
    locations = search9MerDnaABoxes(oriC_2);
    cout << oriC_2.toHighlightenedString(locations) << endl;
    cout << endl;
    return 0;
}

static void reportRelativeLocations(const Sequence &seq, const Sequence &oriC_1, const Sequence &oriC_2, const Sequence &dnaA) {
    tuple<int, int> oriC_1_location = seq.locate(oriC_1);
    printLocation(oriC_1.getName(), oriC_1_location);
    tuple<int, int> oriC_2_location = seq.locate(oriC_2);
    printLocation(oriC_2.getName(), oriC_2_location);
    tuple<int, int> dnaA_location = seq.locate(dnaA);
    printLocation(dnaA.getName(), dnaA_location);
    
    bool beforeC1, beforeC2;
    if (get<0>(dnaA_location) < get<0>(oriC_1_location)) {
        // dnaA is before oriC_1
        beforeC1 = true;
    } else {
        // dnaA is after oriC_1
        beforeC1 = false;
    }
    
    if (get<0>(dnaA_location) < get<0>(oriC_2_location)) {
        // dnaA is before oriC_2
        beforeC2 = true;
    } else {
        // dnaA is after oriC_2
        beforeC2 = false;
    }
    
    // Determine relative locations
    // TODO: please double check the information reported is correct
    if (beforeC1 && beforeC2) {
        cout << "dnaA is before oriC_1 and oriC_2" << endl;
    } else if (beforeC1 && !beforeC2) {
        cout << "dnaA is before oriC_1 and after oriC_2" << endl;
    } else if (!beforeC1 && beforeC2) {
        cout << "dnaA is after oriC_1 and before oriC_2" << endl;
    } else if (!beforeC1 && !beforeC2) {
        cout << "dnaA is after oriC_1 and oriC_2" << endl;
    }
}

static void printLocation(const string &seqName, const tuple<int, int> location) {
    // Play around with them
    int start = get<0>(location);
    int end = get<1>(location);
    cout << "start:" << start << " end:" << end << endl;
}

static void downloadGenFile(const string &accessionNumber) {
    // Send the HTTP GET request to download the file
    cout << "downloading the gene file..." << endl;
    char tmp[500];
    sprintf(tmp, "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=nucleotide&id=%s&rettype=fasta&retmode=text", accessionNumber.c_str());
    string url(tmp);
    Response r = get(url);
    // Read the name of the file
    auto pair = r.headers.find("Content-Disposition");
    string contentDispositionField = pair->second;
    regex filenameRegex("filename=\"(.*)\";?");
    std::smatch m;
    regex_search(contentDispositionField, m, filenameRegex);
    auto itr = m.begin() + 1;
    string filename = *itr;
    // Write the file body to the local disk
    ofstream genFile(filename.c_str());
    if (!genFile.is_open()) {
        cout << "The gene file cannot be written to the disk" << endl;
        exit(EXIT_FAILURE);
    }
    genFile << r.body << endl;
    genFile.close();
    cout << filename << " is downloaded" << endl;
}

// The recursive function for generating 9-mer DnaA boxes with all possibilities
// tmpStored vector is for tracking boxes added so far.
static vector<Sequence> gen9MerDnaABoxes(const vector<vector<char>> &gen9MerDnaAs, vector<Sequence> &&tmpStored, int position) {
    vector<Sequence> seqs;
    if (position < gen9MerDnaAs.size()) {
        vector<char> genes = gen9MerDnaAs[position];
        for (Sequence &tmp : tmpStored) {
            for (char gene : genes) {
                Sequence newSeq = tmp;
                newSeq + gene; // The add operation is implemented by using "+" operator
                seqs.push_back(move(newSeq));
            }
        }
        return gen9MerDnaABoxes(gen9MerDnaAs, move(seqs), position + 1);
    } else {
        return tmpStored;
    }
}

// The function for generating 9-mer DnaA boxes with all possibilities
static vector<Sequence> gen9MerDnaABoxes() {
    const static vector<vector<char>> gen9MerDnaAs = {{'T'}, {'G'}, {'T'}, {'G', 'C'}, {'G', 'A'}, {'A'}, {'T', 'C'}, {'A'}, {'T', 'A', 'C'}};
    return gen9MerDnaABoxes(gen9MerDnaAs, {Sequence()}, 0);
}

static vector<tuple<int, int>> search9MerDnaABoxes(const Sequence &seq) {
    vector<Sequence> boxes = gen9MerDnaABoxes();
    // Search for 9-mer boxes in oriC
    vector<tuple<int, int>> locations;
    for (Sequence &box : boxes) {
        vector<tuple<int, int>> tmps = seq.indexof(box);
        for (tuple<int, int> tmp : tmps) {
            Sequence tmpseq = seq(get<0>(tmp), get<1>(tmp));
            locations.push_back(tmp);
        }
    }
    return locations;
}
