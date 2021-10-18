//
// Created by inte on 10/18/21.
//

#ifndef PDA2CFG_PDA_H
#define PDA2CFG_PDA_H

#include <iostream>
#include <vector>
#include <fstream>
#include "json.hpp"
#include "CFG.h"
#include <iomanip>


using namespace std;
using json = nlohmann::json;

struct Transition {

private:
    string gFrom;
    string gInput;
    string gStacktop;
    string gTo;
    vector<string> gReplacement;

public:
    /**
     * Constructor voor een transitie aan te maken
     * @param from: string
     * @param input: string
     * @param stacktop: string
     * @param to: string
     * @param replacement: vector<string>
     */
    Transition(const string& from, const string& input, const string& stacktop, const string& to, const vector<string>& replacement);

    /**
     * @return gFrom
     */
    string getFrom() {return gFrom;}

    /**
     * @return gInput
     */
    string getInput() {return gInput;}

    /**
     * @return gStacktop
     */
    string getStackTop() {return gStacktop;}

    /**
     * @return gTo
     */
    string getTo() {return gTo;}

    /**
     * @return gReplacement
     */
    vector<string> getReplacement() {return gReplacement;}


};

class PDA {

    vector<string> gStates;
    vector<string> gAlphabet;
    vector<string> gStackAlphabet;
    vector<Transition*> gTransitions;
    string gStartState;
    string gStartStack;

    PDA();

public:

    /**
     * Constructor om alles uit het input bestand te lezen en de juiste
     * waardes toe te kennen aan de juiste variable om een PDA op te stellen
     * @param jsonfile, jsonfile input bestand.
     */
    PDA(const string& jsonfile);

    /**
     * Creeert een CFG van de PDA
     * @return: de omgezetten PDA in een CFG
     */
    CFG toCFG();

    /**
     * @return: vector<string>, states van de PDA
     */
    vector<string> getStates() {return gStates;}

    /**
     * @return: vector<string>, alphabet van de PDA
     */
    vector<string> getAlphabet() {return gAlphabet;}

    /**
     * @return: vector<string>, stackAlphabet van de PDA
     */
    vector<string> getStackAlphabet() {return gStackAlphabet;}

    /**
     * @return: vector<Transition*>, transitions van de PDA
     */
    vector<Transition*> getTransitions() {return gTransitions;}

    /**
     * @return: string, startstate van de PDA
     */
    string getStartState() {return gStartState;}

    /**
     * @return: string, startstack van de PDA
     */
    string getStartStack() {return gStartStack;}


};


#endif //PDA2CFG_PDA_H
