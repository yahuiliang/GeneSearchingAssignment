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
#include "DnaABoxesGenerator.hpp"
#include <iostream>
#include <sstream>
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
 *
 * @return the sequence downloaded from the database
 */
static Sequence downloadGenFile(const string &accessionNumber);

/**
 * Prints the location of the gene section
 *
 * @param seqName is the name of the sequence
 * @param location is the location tuple of the section
 */
static void printLocation(const string &seqName, const tuple<tuple<int, int>, tuple<int, int>> location);

/**
 * Reports relative locations of OriC and DnaA
 *
 * @param seq is the sequence which needs to be located with oriC1, oriC2, and dnaA
 * @param oriC_1 is the oriC_1 sequence to be located
 * @param oriC_2 is the oriC_2 sequence to be located
 * @param dnaA is the dnaA sequence to be located
 */
static void reportRelativeLocations(const Sequence &seq, const Sequence &oriC_1, const Sequence &oriC_2, const Sequence &dnaA);

/**
 * Search 9MerDna boxes from the provided sequence
 *
 * @param seq is the sequence where to search boxes from
 * @return the array of locations where dna boxes are at
 */
static vector<tuple<tuple<int, int>, tuple<int, int>>> search9MerDnaABoxes(const Sequence &seq);

static string oriC1Str =
"caggaccggggatcaatcggggaaagTGTGAATAActtttcggaagtcatacacagtctg"
"tccacaTGTGGATAGgctgtgtttcctgtctttttcacaacTTATCCACAaatccacagg"
"ccctactattacttctactattttttataaatatatatattaatacattatccgttagga"
"ggataaaa";

static string oriC2Str =
"ttatgacacctccctcgaggaatagctgttaaagacagtcttacttattatatttgcgtt"
"acctattcattgtcaacttcactagtgcttttatttcttgcaaccataataggataccat"
"accttttcaactttcgaaaccttattttttagattccttaattttacggaaaaaagacaa"
"attcaaacaatttgcccctaaaatcacgcaTGTGGATATctttttcggctttttttaGTA"
"TCCACAgaggTTATCGACAacattttcacattaccaaccccTGTGGACAAggttttttca"
"acaggttgtccgcttTGTGGATAAgattgtgacaaccattgcaagctctcgtttattttg"
"gtattatatttgtgttttaactcttgattactaatcctacctttcctctTTATCCACAaa"
"gTGTGGATAAgttgtggattgatttcacacagcttgtgtagaaggTTGTCCACAagttgt"
"gaaatttgtcgaaaagctatttatctactatattatatgttttcaacatttaatgtgtac"
"gaatggtaagcgccatttgctctttttttgtgttctataacagagaaagacgccattttc"
"taagaaaaggagggacgtgccggaag";

static string dnaAStr =
"ATGGAAAATATATTAGACCTGTGGAACCAAGCCCTTGCTCAAATCGAAAAAAAGTTGAGCAAACCGAGTT"
"TTGAGACTTGGATGAAGTCAACCAAAGCCCACTCACTGCAAGGCGATACATTAACAATCACGGCTCCCAA"
"TGAATTTGCCAGAGACTGGCTGGAGTCCAGATACTTGCATCTGATTGCAGATACTATATATGAATTAACC"
"GGGGAAGAATTGAGCATTAAGTTTGTCATTCCTCAAAATCAAGATGTTGAGGACTTTATGCCGAAACCGC"
"AAGTCAAAAAAGCGGTCAAAGAAGATACATCTGATTTTCCTCAAAATATGCTCAATCCAAAATATACTTT"
"TGATACTTTTGTCATCGGATCTGGAAACCGATTTGCACATGCTGCTTCCCTCGCAGTAGCGGAAGCGCCC"
"GCGAAAGCTTACAACCCTTTATTTATCTATGGGGGCGTCGGCTTAGGGAAAACACACTTAATGCATGCGA"
"TCGGCCATTATGTAATAGATCATAATCCTTCTGCCAAAGTGGTTTATCTGTCTTCTGAGAAATTTACAAA"
"CGAATTCATCAACTCTATCCGAGATAATAAAGCCGTCGACTTCCGCAATCGCTATCGAAATGTTGATGTG"
"CTTTTGATAGATGATATTCAATTTTTAGCGGGGAAAGAACAAACCCAGGAAGAATTTTTCCATACATTTA"
"ACACATTACACGAAGAAAGCAAACAAATCGTCATTTCAAGTGACCGGCCGCCAAAGGAAATTCCGACACT"
"TGAAGACAGATTGCGCTCACGTTTTGAATGGGGACTTATTACAGATATCACACCGCCTGATCTAGAAACG"
"AGAATTGCAATTTTAAGAAAAAAGGCCAAAGCAGAGGGCCTCGATATTCCGAACGAGGTTATGCTTTACA"
"TCGCGAATCAAATCGACAGCAATATTCGGGAACTCGAAGGAGCATTAATCAGAGTTGTCGCTTATTCATC"
"TTTAATTAATAAAGATATTAATGCTGATCTGGCCGCTGAGGCGTTGAAAGATATTATTCCTTCCTCAAAA"
"CCGAAAGTCATTACGATAAAAGAAATTCAGAGGGTAGTAGGCCAGCAATTTAATATTAAACTCGAGGATT"
"TCAAAGCAAAAAAACGGACAAAGTCAGTAGCTTTTCCGCGTCAAATCGCCATGTACTTATCAAGGGAAAT"
"GACTGATTCCTCTCTTCCTAAAATCGGTGAAGAGTTTGGAGGACGTGATCATACGACCGTTATTCATGCG"
"CATGAAAAAATTTCAAAACTGCTGGCAGATGATGAACAGCTTCAGCAGCATGTAAAAGAAATTAAAGAAC"
"AGCTTAAATAG";

int main(int argc, const char * argv[]) {
    // Determine if the number of arguments is valid
    if (argc < 2) {
        cout << "The program accepts the accession number" << endl;
        exit(EXIT_FAILURE);
    }
    try {
        // Get the accession number
        string accessionnNumber(argv[1]);
        
        auto start = high_resolution_clock::now();
        // Download the sequence
        Sequence sequence = downloadGenFile(accessionnNumber);
        // Read the oriC_1 sequence from the file
        Sequence oriC_1(oriC1Str, "oriC_1 of B. subtilis subsp. subtilis str. 168, NC_000964.3");
        // Read the oriC_2 sequence from the file
        Sequence oriC_2(oriC2Str, "oriC_2 of B. subtilis subsp. subtilis str. 168, NC_000964.3");
        // Read the dnaA sequence from the file
        Sequence dnaA(dnaAStr, "dnaA gene of B subtilis subsp. subtilis str. 168");
        
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
    } catch (const string & err) {
        cout << err << endl;
    }
    
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

static Sequence downloadGenFile(const string &accessionNumber) {
    // Send the HTTP GET request to download the file
    cout << "downloading the gene file..." << endl;
    char tmp[500];
    sprintf(tmp, "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=nucleotide&id=%s&rettype=fasta&retmode=text", accessionNumber.c_str());
    string url(tmp);
    Response r = get(url);
    if (r.code != 200) {
        throw string("The file cannot be fetched from the database.");
    }
    // Read the name of the file
    auto pair = r.headers.find("Content-Disposition");
    string contentDispositionField = pair->second;
    regex filenameRegex("filename=\"(.*)\";?");
    std::smatch m;
    regex_search(contentDispositionField, m, filenameRegex);
    auto itr = m.begin() + 1;
    string filename = *itr;
    cout << filename << " is downloaded" << endl;
    istringstream f(r.body);
    string name;
    getline(f, name);
    name.erase(0, 1);
    
    char c;
    string seqStr;
    while (f.get(c)) {
        if (c != '\n') {
            seqStr += c;
        }
    }
    
    return Sequence(seqStr, name);
}

static vector<tuple<tuple<int, int>, tuple<int, int>>> search9MerDnaABoxes(const Sequence &seq) {
    set<Sequence> boxes = gen9MerDnaABoxes();
    // Search for 9-mer boxes in oriC
    vector<tuple<tuple<int, int>, tuple<int, int>>> locations;
    vector<tuple<tuple<int, int>, tuple<int, int>>> tmps = seq.locateBoxes(boxes);
    for (tuple<tuple<int, int>, tuple<int, int>> tmp : tmps) {
        locations.push_back(tmp);
    }
    return locations;
}
