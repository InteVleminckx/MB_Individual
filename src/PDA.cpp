//
// Created by inte on 10/18/21.
//

#include "PDA.h"

Transition::Transition(const string& from, const string& input, const string& stacktop, const string& to, const vector<string>& replacement)
{
    gFrom = from;
    gInput = input;
    gStacktop = stacktop;
    gTo = to;
    gReplacement = replacement;
}

PDA::PDA() = default;

PDA::PDA(const string& jsonfile)
{
    //Inlezen JSON file
    ifstream  input(jsonfile);

    json file;
    input >> file;

    //Kennen de states toe.
    for (const auto& state : file["States"]) gStates.push_back(state);

    //Kennen het alphabet toe.
    for (const auto& alph : file["Alphabet"]) gAlphabet.push_back(alph);

    //Kennen het StackAlphabet toe.
    for (const auto& sAlph : file["StackAlphabet"]) gStackAlphabet.push_back(sAlph);

    //Kennen het StartState toe.
    gStartState = file["StartState"];

    //Kennen het StartStack toe.
    gStartStack = file["StartStack"];

    //Kenne de transitie toe.
    for(const auto& trans : file["Transitions"])
    {
        gTransitions.push_back(new Transition(trans["from"],
                                              trans["input"],
                                              trans["stacktop"],
                                              trans["to"],
                                              trans["replacement"]));
    }

    cout << endl;

}

CFG PDA::toCFG()
{
    PDA thispda;

    thispda.gStates = gStates;
    thispda.gAlphabet = gAlphabet;
    thispda.gStackAlphabet = gStackAlphabet;
    thispda.gTransitions = gTransitions;
    thispda.gStartState = gStartState;
    thispda.gStartStack = gStartStack;

    return CFG(thispda);
}




