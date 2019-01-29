//
//  DnaABoxesGenerator.hpp
//  ProgrammingProject
//
//  Created by Yahui Liang on 1/27/19.
//  Copyright © 2019 Yahui Liang. All rights reserved.
//

#ifndef DnaABoxesGenerator_hpp
#define DnaABoxesGenerator_hpp

#include "Sequence.hpp"
#include <stdio.h>
#include <vector>
#include <set>
#include <string>

/**
 * Generates 9MerDnaA boxes
 *
 * @return the set of DNA boxes
 */
std::set<Sequence> gen9MerDnaABoxes();

#endif /* DnaABoxesGenerator_hpp */
