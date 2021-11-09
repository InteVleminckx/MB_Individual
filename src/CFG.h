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
#include <set>

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
    vector<vector<set<string>>> gCYK_table;
    vector<char> gInputstring;
    int gRowpointer{};
    map<string, set<string>> gFirst;
    map<string, set<string>> gFollow;


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

    /**
     * Stelt een lege tabel op basis van de string lengte.
     * @param stringLenght: int, lengte van de input string
     *
     */
    void createTable(const int stringLenght);

    /**
     * Zoekt in de productions of er een body is met de gegeven input.
     * @param terminal: char, de input van de string
     * @param variables: set<string>, de head(s) die gevonden worden voor een bepaalde input.
     */
    void searchInputInProduction(const char terminal, set<string>& variables);

    /**
     * Vult de eerste rij van de tabel. == Basis case
     */
    void directDerivation();

    /**
     * Vult de andere rijen. == induction case
     */
    void fillingTable();

    /**
     * Genereert een subtable waarbij de top, de plaats is die we gaan invullen
     * @param colom: int, colom plaats van de top
     * @return vector<vector<set<string>>>: de subtabel die gemaakt is.
     */
    vector<vector<set<string>>> createSubTable(const int colom);

    /**
     * berekent de top van de subTable, dit is ook de waarde die we in de algemene tabel gaan invullen;
     * @param subTable: vector<vector<set<string>>>, de subtabel die wel gemaakt hebben
     * @param topColom: int, plaats van de colom
     */
    void createTop(vector<vector<set<string>>> subTable, int topColom);

    /**
     * Kijkt na op de gemaakte production in de productions zit, zo ja wordt de head toegevoegd
     * @param creation: vector<string>, de gemaakte production
     * @param topPlace: int, de plaats waar deze moet worden toegevoegd
     */
    void compareCreationWithProductions(vector<string> const creation, pair<int, int> topPlace);

    /**
     * Verwijderd alle productions naar epsilon
     */
    void RemoveNullProductions();

    /**
     * Wijzigt de productions waar de nullable in stond
     */
    void addProductions(string const nullable);

    /**
     *  Maakt elke mogelijke productie recursief aan
     */
    void recProduction();

    /**
     * Reset all global variables die hergebruikt moeten worden bij het
     * controlleren van een nieuwe input
     */
    void clearAll();

    /**
     * Print de tabel af
     */
    void printTable();

    /**
     * Controlleert of er left recursion aanwezig is, als dat zo is wordt dit verwijderd.
     * @param newVar: vector<string>, hier worden de nieuwe variable toegevoegd, moesten deze aangemaakt worden.
     * @param newPro: vector<production>, hier worden nieuwe productions toegevoegd, moesten deze aangemaakt worden.
     */
    void elemLeftRecursion(set<string> &newVar, vector<production> &newPro);

    /**
     * Voegt de nieuwe productions en aangemaakte variable toe aan huidige productions en variable.
     * @param newVar: set<string>, de nieuwe variable
     * @param newPro: vector<production>, de nieuwe productions
     */
    void addNewProductions(set<string> &newVar, vector<production> &newPro);

    /**
     * bepaalt de first en follow sets (roept de functies hiervoor aan)
     */
    void firstAndFollow();

    /**
     * Bepaalt de first set voor een variable
     * @param var: string, de variable waarvan we de set willen bepalen
     * @return: set<string>, wordt gebruikt voor recursie
     */
    set<string> first(const string &var);

    /**
     * Zegt als de gegeven input een variable is
     * @param input: string, de input die we willen controlleren
     * @return: bool, true als het een variable is anders false
     */
    bool isVariable(const string &input);

    /**
     * Zegt als de gegeven input een terminal is
     * @param input: string, de input die we willen controlleren
     * @return: bool, true als het een terminal is anders false
     */
    bool isTerminal(const string &input);

    /**
     * Controlleert of de set een epsilon heeft
     * @param setLeft: set<string>, de set die gecontrolleert moet worden
     * @return: bool, true als het een epsilon bevat anders false
     */
    bool containsEpsilon(const set<string> &setLeft);

    /**
     * Bepaalt de follow set voor een variable
     * @param var: string, de variable waarvan we de set willen bepalen
     * @return: set<string>
     */
    set<string> follow(const string &var);

    /**
     * Eerste 'regel' om een follow te creeëren
     * @param pro: production, de production waar de gezochte variable inzit
     * @param var: string, de variable waar we de follow set van zoeken
     * @param i: int, de plaats waar de var zich bevindt in de production
     */
    void addFollow_rule_1_or_3(const production &pro, const string &var, const int i);

    /**
    * Tweede 'regel' om een follow te creeëren
    * @param pro: production, de production waar de gezochte variable inzit
    * @param var: string, de variable waar we de follow set van zoeken
    */
    void addFollow_rule_2(const production &pro, const string &var);

    /**
    * Derde 'regel' om een follow te creeëren
    * @param pro: production, de production waar de gezochte variable inzit
    * @param var: string, de variable waar we de follow set van zoeken
    * @param i: int, de plaats waar de var zich bevindt in de production
    * @param next, set<string>, de first set van het volgende symbool
    */
    void addFollow_rule_3(const production &pro, const string &var, const int i, set<string> next);

    void createParserTable(vector<vector<string>> &parserTable);

    pair<int, int> searchRowandColom(const string &variable, const string &terminal);


public:

    CFG(PDA &pda);

    CFG(const string& jsonfile);

    /**
     * Kijkt of een string aanvaardt wordt daar de CFG
     * @param input: string, de input die gecontrolleerd moet worden
     * @return bool: zegt of de string aanvaardt wordt of niet.
     */
    bool accepts(const string input);

    /**
     * Zet een CFG om naar een CNF
     */
    void toCNF();

    /**
     * LL(1) Parser
     */
    void ll();

    void print(bool cnf = false);

};

#endif //MB_PROGRAMMEEROPDRACHT_2_CFG_H
