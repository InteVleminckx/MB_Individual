//
// Created by inte on 10/8/21.
//

#include "CFG.h"
#include "PDA.h"

CFG::CFG(PDA &pda)
{

    //Startsymbool is altijd S
    gStartsymbol = "S";

    //De terminals van de CFG zijn gelijk aan het alphabet van de PDA
    gTerminals = pda.getAlphabet();

    //Sorteren van de Terminals
    sort(gTerminals.begin(), gTerminals.end());

    //De variable worden gemaakt door elke mogelijke combinatie tussen een state en
    //stack inputs + het startsymbool

    gVariables.push_back(gStartsymbol);

    for (const auto& stackinput : pda.getStackAlphabet())
    {
        for (const auto& state1 : pda.getStates())
        {
            for (const auto& state2 : pda.getStates())
            {
                gVariables.push_back("[" + state1 + "," + stackinput + "," + state2 + "]");
            }
        }
    }

    //Sorteren van de variablen
    sort(gVariables.begin(), gVariables.end());

    //Enkel nog de Productions toekennen

    //1) Zoeken eerst naar de production(s) van het start symbool
    searchProdStartSymbol(pda.getStartState(), pda.getStartStack(), pda.getStates());

    //2) Uit elke transitie de productions afleiden
    createProductions(pda.getTransitions(), pda.getStates());

    //Sorteren van de productions
    sort(gProductions.begin(), gProductions.end());

}

void CFG::searchProdStartSymbol(const string& startState, const string& startStack, const vector<string>& states)
{
    for  (const auto& state : states)
    {
        production prod;
        prod.first = gStartsymbol;
        prod.second = {"[" + startState + "," + startStack + "," +  state + "]"};
        gProductions.push_back(prod);
    }

}

void CFG::createProductions(const vector<Transition *>& transitions, vector<string> states)
{
    //We kunnen dit opdelen in 3 delen
    //1) de replacement == leeg == epsilon
    //2) de replacement == lengte 1
    //3) de replacement > lengte 1

    //We lopen over de transities
    for (auto transition : transitions)
    {
        //replacement rule 1
        if (transition->getReplacement().empty()) replacement_rule_1(transition);

        //replacement rule 2
        else if (transition->getReplacement().size() == 1) replacement_rule_2(transition, states);

        //replacement rule 3
        else replacement_rule_3(transition, states);
    }

}

void CFG::replacement_rule_1(Transition *transition)
{
    //2 gevallen
    //1) geen input == epsilon
    //2) input

    string head = "[" + transition->getFrom() +
                  "," + transition->getStackTop() +
                  "," + transition->getTo() +
                  "]";

    string body;

    //1)
    if (transition->getInput().empty()) body = "";

    //2)
    else body = transition->getInput();

    production prod;

    prod.first = head; prod.second.push_back(body);

    gProductions.push_back(prod);

}

void CFG::replacement_rule_2(Transition *transition, vector<string> &states)
{
    //Omdat er 1 replacement is hebben we hebben p productions
    //Met p = aantal states

    for (const auto& state : states)
    {
        string head = "[" + transition->getFrom() +
                      "," + transition->getStackTop() +
                      "," + state +
                      "]";

        string body = transition->getInput() +
                      "[" + transition->getTo() +
                      "," + transition->getReplacement()[0] +
                      "," + state +
                      "]";

        production prod;
        prod.first = head; prod.second.push_back(body);
        gProductions.push_back(prod);
    }

}

void CFG::replacement_rule_3(Transition *transition, vector<string> &states)
{
    //Omdat er meerde replacements zijn hebben we p^n productions
    //Met p = aantal states en n = aantal replacements


    for (const auto& state1 : states)
    {
        string head = "[" + transition->getFrom() +
                      "," + transition->getStackTop() +
                      "," + state1 +
                      "]";


        string prev;

        vector<string> copyStates = states;

        for (int s = 0; s < (int) states.size(); ++s) {
            string body = transition->getInput();
            for (int i = 0; i < (int) transition->getReplacement().size(); ++i)
            {
                //3 delen:
                //1) het begin waar binnen de [haakjes als eerste de To moet komen]
                //2) het gelus tussen de replacements tot de voor laatste waarbij
                //   de eerste tussen de haakjes gelijk moet zijn als de laatste van de vorige
                //3) de laatste van de haakjes op het einde van de replacements == de laatste
                //   van de head

                //1)
                if (i == 0)
                {
                    body += "[" + transition->getTo() +
                            "," + transition->getReplacement()[i] +
                            "," + copyStates[s] +
                            "]";

                    prev = copyStates[s];
                }

                //2)
                else if (i < (int) transition->getReplacement().size()-1)
                {
                    body += "[" + prev +
                            "," + transition->getReplacement()[i] +
                            "," + copyStates[s] +
                            "]";

                    prev = copyStates[s];
                }

                    //3)
                else
                {
                    body += "[" + prev +
                            "," + transition->getReplacement()[i] +
                            "," + state1 +
                            "]";
                }
                shiftStates(copyStates);
            }
            production prod;
            prod.first = head; prod.second.push_back(body);
            gProductions.push_back(prod);
        }
    }
}

void CFG::shiftStates(vector<string> &states)
{

    vector<string> newVector;

    for (int i = 1; i < (int) states.size() + 1; ++i)
    {
        if (i != (int) states.size()) newVector.push_back(states[i]);
        else newVector.push_back(states[0]);
    }

    states = newVector;

}

void CFG::print()
{

    //Printen van de Variables
    cout << "V = {";
    for (const auto& var : gVariables)
    {
        if (var != gVariables[gVariables.size() - 1]) cout << var << ", ";
        else cout << var;
    }
    cout << "}" << endl;

    //Printen van de Terminals
    cout << "T = {";
    for (const auto& ter : gTerminals)
    {
        if (ter != gTerminals[gTerminals.size() - 1]) cout << ter << ", ";
        else cout << ter;
    }
    cout << "}" << endl;

    //Printen van de Productions
    cout << "P = {" << endl;
    for (const auto& pro : gProductions )
    {
        string body;
        for (auto &var : pro.second)
        {
            if (var != pro.second[pro.second.size()-1]) body += var + " ";
            else body += var;
        }

        cout << "\t" << pro.first << "  -> `" << body << "`" << endl;
    }
    cout << "}" << endl;

    //Printen van het Startsymbool
    cout << "S = " << gStartsymbol << endl;

}




