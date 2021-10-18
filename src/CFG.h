//
// Created by inte on 10/8/21.
//

#ifndef MB_PROGRAMMEEROPDRACHT_2_CFG_H
#define MB_PROGRAMMEEROPDRACHT_2_CFG_H

#include <iostream>
#include <vector>
#include <fstream>
#include "json.hpp"
#include <iomanip>


using namespace std;
using json = nlohmann::json;

//production bestaat uit een head en body
typedef pair<string, vector<string>> production;

struct Transition;
class PDA;

class CFG {

    vector<string> gVariables;
    vector<string> gTerminals;
    vector<production> gProductions;
    string gStartsymbol;

    /**
     * Zoekt alles productions van het start symbool
     * @param startState: string, start state van de pda
     * @param startStack: string, start stack
     * @param states: vector<string>, alle states van de pda
     */
    void searchProdStartSymbol(const string& startState, const string& startStack, const vector<string>& states);

    /**
     * Maakt alle resterende productions na die van het start symbool
     * @param transitions: vector<Transition*>, alle transities van de pda
     */
    void createProductions(const vector<Transition*>& transitions, vector<string> states);

    /**
     * de replacement == leeg == epsilon
     * @param transition: transition
     */
    void replacement_rule_1(Transition* transition);

    /**
     * de replacement == lengte 1
     * @param transition: transition
     */
    void replacement_rule_2(Transition* transition, vector<string> &states);

    /**
     * de replacement > lengte 1
     * @param transition: transition
     */
    void replacement_rule_3(Transition* transition, vector<string> &states);

    /**
     * Verschuift telkens de states in de vector 1 naar voor en plaatst
     * de voorste naar achter
     * @param states: vector<string>, vector van states
     */
    void shiftStates(vector<string> &states);

public:

    CFG(PDA &pda);

    void print();

};

#endif //MB_PROGRAMMEEROPDRACHT_2_CFG_H
