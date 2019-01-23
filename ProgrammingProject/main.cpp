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
#include <chrono>

using namespace std::chrono;
using namespace std;
using namespace RestClient;

/**
 * Download the gen file from the database
 * The accession number should be provided
 */
static void downloadGenFile(const string & dest, const string &accessionNumber);
static void printLocation(const string &seqName, const tuple<tuple<int, int>, tuple<int, int>> location);
static void reportRelativeLocations(const Sequence &seq, const Sequence &oriC_1, const Sequence &oriC_2, const Sequence &dnaA);
static vector<Sequence> gen9MerDnaABoxes();
static vector<tuple<tuple<int, int>, tuple<int, int>>> search9MerDnaABoxes(const Sequence &seq);

int main1() {
    Sequence tmp1("TGTGGATADT");
    Sequence tmp2 = -tmp1 + 'A';
    cout << (tmp1 == tmp2) << endl;
    return 0;
}

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
    
    string baseDir = "genes/";
    
    // Download the gen file
    // The file is downloaded to the same directory as the executable binary
    // Check if the file has existed
    // If not, install the file from the database
    ifstream sequenceFile(baseDir + "sequence.fasta");
    if (!((bool)sequenceFile)) {
        downloadGenFile(baseDir + "sequence.fasta", accessionnNumber);
    }
    
    auto start = high_resolution_clock::now();
    // Read the sequence from the file
    Sequence sequence;
    sequence.readSequence(baseDir + "sequence.fasta");
    // Read the oriC_1 sequence from the file
    Sequence oriC_1;
    oriC_1.readSequence(baseDir + "oriC_1.fasta");
    // Read the oriC_2 sequence from the file
    Sequence oriC_2;
    oriC_2.readSequence(baseDir + "oriC_2.fasta");
    // Read the dnaA sequence from the file
    Sequence dnaA;
    dnaA.readSequence(baseDir + "dnaA.fasta");
    
    // Sync the start index of the sequence
    sequence.syncStart(oriC_1);
    
    // Print out the distribution report
    cout << ">Computing the distribution of the sequence..." << endl;
    Sequence::Distribution distribution = sequence.computeDistribution();
    cout << distribution.toString() << endl;
    cout << endl;
    // Report locations of oriCs and dnaA
    reportRelativeLocations(sequence, oriC_1, oriC_2, dnaA);
    cout << endl;
    // Print out highlightened 9Mer boxes
    cout << ">DnaA boxes " << oriC_1.getName() << endl;
    vector<tuple<tuple<int, int>, tuple<int, int>>> locations = search9MerDnaABoxes(oriC_1);
    cout << oriC_1.toHighlightenedString(locations) << endl;
    cout << ">DnaA boxes " << oriC_2.getName() << endl;
    locations = search9MerDnaABoxes(oriC_2);
    cout << oriC_2.toHighlightenedString(locations) << endl;
    cout << ">" << dnaA.getName() << endl;
    cout << dnaA.toString() << endl;
    cout << endl;
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    // To get the value of duration use the count()
    // member function on the duration object
    cout << "Execution time:" << duration.count() << "ms" << endl;
    
    return 0;
}

static int getGeneStartIndex(tuple<tuple<int, int>, tuple<int, int>> location) {
    int returnVal = get<0>(get<0>(location));
    if (returnVal < 0) {
        // The position
        returnVal = get<1>(get<0>(location));
    }
    return returnVal;
}

static void reportRelativeLocations(const Sequence &seq, const Sequence &oriC_1, const Sequence &oriC_2, const Sequence &dnaA) {
    cout << ">locatinng " << oriC_1.getName() << "..." << endl;
    auto oriC_1_location = seq.locate(oriC_1);
    printLocation(oriC_1.getName(), oriC_1_location);
    
    cout << ">locatinng " << oriC_2.getName() << "..." << endl;
    auto oriC_2_location = seq.locate(oriC_2);
    printLocation(oriC_2.getName(), oriC_2_location);
    
    cout << ">locatinng " << dnaA.getName() << "..." << endl;
    auto dnaA_location = seq.locate(dnaA);
    printLocation(dnaA.getName(), dnaA_location);
    
    int oriC2Start = getGeneStartIndex(oriC_2_location);
    int dnaAStart = getGeneStartIndex(dnaA_location);
    
    cout << "Relative locations: ";
    if (dnaAStart < oriC2Start) {
        cout << "OriC1 DnaA OriC2 (Circular)" << endl;
    } else {
        cout << "OriC1 OriC2 DnaA (Circular)" << endl;
    }
}

static void printLocation(const string &seqName, const tuple<tuple<int, int>, tuple<int, int>> location) {
    // Play around with them
    int start = get<0>(get<0>(location));
    int end = get<1>(get<0>(location));
    cout << "start:" << start << " end:" << end << endl;
    start = get<0>(get<1>(location));
    end = get<1>(get<1>(location));
    cout << "start:" << start << " end:" << end << " (reverse complementary)" << endl;
}

static void downloadGenFile(const string & dest, const string &accessionNumber) {
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
    ofstream genFile(dest.c_str());
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
                cstring[1] = NULL;
                string tmp(cstring);
                seqs.push_back(move(tmp));
            }
        }
        
    }
    return seqs;
}

// The function for generating 9-mer DnaA boxes with all possibilities
static vector<Sequence> gen9MerDnaABoxes() {
    const static vector<vector<char>> gen9MerDnaAs = {{'T'}, {'G'}, {'T'}, {'G', 'C'}, {'G'}, {'A'}, {'T', 'C'}, {'A'}, {'T', 'A', 'C', 'G'}};
    const vector<string> boxes = gen9MerDnaABoxes(gen9MerDnaAs, 0);
    vector<Sequence> ans;
    for (string box : boxes) {
        Sequence tmp(box);
        ans.push_back(move(tmp));
    }
    return ans;
}

static vector<tuple<tuple<int, int>, tuple<int, int>>> search9MerDnaABoxes(const Sequence &seq) {
    vector<Sequence> boxes = gen9MerDnaABoxes();
    // Search for 9-mer boxes in oriC
    vector<tuple<tuple<int, int>, tuple<int, int>>> locations;
    for (Sequence &box : boxes) {
        vector<tuple<tuple<int, int>, tuple<int, int>>> tmps = seq.indexof(box);
        for (tuple<tuple<int, int>, tuple<int, int>> tmp : tmps) {
            locations.push_back(tmp);
        }
    }
    return locations;
}
