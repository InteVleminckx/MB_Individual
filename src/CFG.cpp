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

    //Sorteren van de productions.
    sort(gProductions.begin(), gProductions.end());

}

CFG::CFG(const string &jsonfile)
{
    //Inlezen JSON file
    ifstream  input(jsonfile);

    json file;
    input >> file;

    //eerst kennen we de variable toe de vector
    for (auto &var : file["Variables"]) gVariables.push_back(var);
    //Sorteren van de vector
    sort(gVariables.begin(), gVariables.end());

    //Toekenen van de terminals
    for (auto &ter : file["Terminals"]) gTerminals.push_back(ter);
    //Sorteren van de vector
    sort(gTerminals.begin(), gTerminals.end());

    //Toekenen van de productions
    for(auto &pro : file["Productions"])
    {
        if (pro["body"].empty()) gProductions.emplace_back(pro["head"], vector<string>{""});
        else gProductions.emplace_back(pro["head"], pro["body"]);
    }

    sort(gProductions.begin(), gProductions.end());
    //Toekenen van het startsymbool
    gStartsymbol = file["Start"];
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

void CFG::print(bool cnf)
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
    if (cnf)
    {
        for (const auto& pro : gProductions )
        {
            string body;
            int i = 0;

            for (auto &var : pro.second)
            {
                i++;
                if (i != pro.second.size()) body += var + " ";
                else body += var;
            }

            cout << "\t" << pro.first << "  -> `" << body << "`" << endl;
        }
    }
    else
    {
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
    }
    cout << "}" << endl;

    //Printen van het Startsymbool
    cout << "S = " << gStartsymbol << endl;

}

bool CFG::accepts(const string input)
{
    //aanmaken van de tabel
    createTable(input.size());

    //aanmaken vector van elke input char, symmetrie met de tabel dan
    for (auto const chr : input) gInputstring.push_back(chr);

    directDerivation();

    while (gRowpointer != (int) gInputstring.size()) fillingTable();
    printTable();


    clearAll();
    return false;
}

void CFG::createTable(const int stringLenght)
{
    for (int i = 0; i < stringLenght; ++i)
    {
        vector<set<string>> createRow;
        for (int j = 0; j < stringLenght; ++j)
        {
            set<string> createSet;
            createRow.push_back(createSet);
        }
        gCYK_table.push_back(createRow);
    }
}

void CFG::clearAll() {
    //tabel helemaal op het einde terug leegmaken
    gCYK_table = vector<vector<set<string>>>();

    //vector van input ook leegmaken
    gInputstring = vector<char>();

    //rowpointer resetten
    gRowpointer = 0;
}

void CFG::directDerivation()
{
    for (int i = 0; i < (int) gInputstring.size(); i++) searchInputInProduction(gInputstring[i], gCYK_table[0][i]);
    gRowpointer++;
}

void CFG::searchInputInProduction(const char terminal, set<string>& variables)
{
    for (auto const &pro : gProductions)
    {
        for (auto const &body : pro.second)
        {
            if (body[0] == terminal) variables.insert(pro.first);
        }
    }
}

void CFG::fillingTable()
{
    //Berekenen aantal subtables
    int numberTables = gInputstring.size() - gRowpointer;

    for (int i = 0; i < numberTables; ++i) 
    {
        createTop(createSubTable(i), i);
    }
    gRowpointer++;

}

vector<vector<set<string>>> CFG::createSubTable(const int colom)
{
    vector<vector<set<string>>> subTable;

    for (int i = 0; i < gRowpointer; ++i) 
    {
        vector<set<string>> row;
        for (int j = colom; j <= gRowpointer + colom - i; j++)
        {
           row.push_back(gCYK_table[i][j]);
        }
        subTable.push_back(row);
    }

    return subTable;
    
}

void CFG::createTop(vector<vector<set<string>>> subTable, int topColom)
{

    for (int i = 0; i < (int) subTable.size(); ++i)
    {
        for (auto const &first : subTable[i][0])
        {
            vector<string> creations;
            for (auto const &second : subTable[subTable.size()-1 - i][subTable[subTable.size()-1-i].size()-1])
            {
                compareCreationWithProductions(vector<string>{first, second}, pair<int, int>{subTable.size(), topColom});
            }
        }
    }
}

void CFG::compareCreationWithProductions(const vector<string> creation, pair<int, int> topPlace) {

    for (int i = 0; i < (int) gProductions.size(); ++i)
    {
        if (creation == gProductions[i].second)
        {
            gCYK_table[topPlace.first][topPlace.second].insert(gProductions[i].first);
        }
    }
}

void CFG::toCNF()
{
//    cout << "Original CFG:\n\n";
//    print(true);
//    cout << "\n-------------------------------------\n\n";
//    RemoveNullProductions();
}

void CFG::RemoveNullProductions()
{

//    set<string> nullables{};
//    int original = gProductions.size();
//    int created;
//
//    cout << " >> Eliminating epsilon productions\n";
//
//    //Code
//    //-----------------------------------------------//
//
//    bool erased = false;
//    //zoeken naar nullables
//    while (!erased)
//    {
//        erased = true;
//        for (int i = 0; i < gProductions.size(); ++i)
//        {
//            if (gProductions[i].second.empty())
//            {
//                addProductions(gProductions[i].first);
//                nullables.insert(gProductions[i].first);
//                gProductions.erase(gProductions.begin()+i);
//                erased = false;
//                break;
//            }
//        }
//    }
//
//
//
//
//
//
//    //-----------------------------------------------//
//    //printing
//    cout << "  Nullables are {";
//    int i = 0;
//    for (auto const &nullable : nullables)
//    {
//        i++;
//        if (i != nullables.size()) cout << nullable + ", ";
//        else cout << nullable + "}\n";
//    }
//    created = gProductions.size();
//    cout << "  Created " << created << " productions, original had " << original << endl;
//    print(true);

}

void CFG::addProductions(const string nullable)
{
//    for (auto const &pro : gProductions)
//    {
//        for (int i = 0; i < pro.second.size(); ++i)
//        {
//
//
//
//        }
//    }

}

void CFG::printTable()
{
    bool accept = false;

    vector<pair<int, int>> stringDistance;
    for (int i = 0; i < gCYK_table.size(); ++i)
    {
        for (int j = 0; j < gCYK_table[i].size()- i; ++j)
        {
            if (i == 0) stringDistance.push_back(pair<int, int>{j, gCYK_table[i][j].size()});
            else
            {
                if (stringDistance[j].second < gCYK_table[i][j].size())
                {
                    stringDistance[j].second = gCYK_table[i][j].size();
                }
            }
        }
    }
    string table;
    for (int i = gCYK_table.size()-1; 0 <= i; --i)
    {
        for (int j = 0; j < gCYK_table[i].size()- i; ++j)
        {
            table += "| {";
            int counter{};
            for (auto const &chr : gCYK_table[i][j])
            {
                table+=chr;
                table += (counter+1 != gCYK_table[i][j].size()) ? "," : "} ";
                counter++;
                if (chr == gStartsymbol && i == gCYK_table.size()-1) accept = true;
            }
            if (gCYK_table[i][j].empty()) table+= "}";
            for (int k = 0; k < stringDistance[j].second*2 - gCYK_table[i][j].size()*2; ++k) table += " ";
        }
        table += "| \n";
    }
    cout << table << boolalpha << accept << endl;

}

void CFG::ll()
{
    cout << ">>> Building LL(1) Table" << endl;
    set<string> newVar;
    vector<production> newPro;
    elemLeftRecursion(newVar, newPro);
    if (!newVar.empty()) addNewProductions(newVar, newPro);
    firstAndFollow();
    vector<vector<string>> parserTable;
    vector<int> longestString;
    for (int i = 0; i < gTerminals.size() + 2; ++i)
    {
        if (i == gTerminals.size() + 1) longestString.push_back(5); //<EOS>
        else longestString.push_back(1); //min lengt variable + terminal
    }
    createParserTable(parserTable, longestString);
    printParserTable(parserTable, longestString);
}

void CFG::elemLeftRecursion(set<string> &newVar, vector<production> &newPro)
{
   //controlleer of het eerste symbool van de body == head

    //vector van alfa's en beta's
    vector<vector<string>> alfa, beta;
    map<string, vector<vector<string>>> allPro;
    string prev = gProductions[0].first;
    vector<vector<string>> prod;
    for (auto const &pro : gProductions)
    {
        if (pro.first == prev) prod.push_back(pro.second);

        else
        {
            allPro.insert({prev, prod});
            prev = pro.first;
            prod = {};
            prod.push_back(pro.second);
        }
    }
    allPro.insert({prev, prod});

    string newVari;

    for (auto const &it : allPro)
    {
        for (auto const &at : it.second)
        {
            if (at[0] == "") beta.push_back(at);

            else if (it.first == at[0])
            {
                newVari = it.first + "\'";
                vector<string> prods;
                for (int i = 1; i < at.size(); ++i) prods.push_back(at[i]);
                alfa.push_back(prods);
                newVar.insert(newVari);
            }
            else beta.push_back(at);
        }

        for (int i = 0; i < beta.size(); ++i)
        {
            if (newVari == it.first + "\'")
            {
                vector<string> produ = beta[i];
                produ.push_back(it.first + "\'");
                newPro.push_back(production{it.first, produ});
            }
        }

        for (int i = 0; i < alfa.size(); ++i)
        {
            if (newVari == it.first + "\'")
            {
                vector<string> produ = alfa[i];
                produ.push_back(it.first + "\'");
                newPro.push_back(production{it.first + "\'", produ});
            }
        }
        if (newVari == it.first + "\'") newPro.push_back(production{it.first + "\'", vector<string>{""}});
    }
}

void CFG::addNewProductions(set<string> &newVar, vector<production> &newPro)
{
    for (auto const & var : newVar) gVariables.push_back(var);

    vector<production> filterdPro;

    for (auto const & var : newVar)
    {
        string oldVar = var;
        oldVar.pop_back();

        for (auto const & pro : gProductions)
        {
            if (pro.first != oldVar) filterdPro.push_back(pro);
        }
    }

    gProductions = filterdPro;

    for (auto const & pro : newPro) gProductions.push_back(pro);
}

void CFG::firstAndFollow()
{

    //TODO: nog uitleg zetten in de first function
    for (const auto &var : gVariables) first(var);

    //TODO: nog uitleg zetten in de follow function
    for (const auto &var : gVariables) follow(var);

    string firstS;

    for (const auto & setVal : gFirst)
    {
        bool hasEps = false;
        firstS += "\t" + setVal.first + ": {";

        for (auto it = setVal.second.begin(); it != setVal.second.end(); ++it)
        {
            auto next = it;
            ++next;

            if (*it == "") hasEps = true;

            if (next != setVal.second.end() && *it != "") firstS += *it + ", ";
            else if (next == setVal.second.end() && hasEps) firstS += *it + ", " + " " + "}\n";
            else if (next == setVal.second.end() && !hasEps) firstS += *it + "}\n";
        }
        hasEps = false;
    }

    string followS;

    for (const auto & setVal : gFollow)
    {
        followS += "\t" + setVal.first + ": {";

        for (auto it = setVal.second.begin(); it != setVal.second.end(); ++it)
        {
            auto next = it;
            ++next;

            if (next != setVal.second.end() && *it != "" && *it == "$") followS += "<EOS>, ";
            else if (next != setVal.second.end() && *it != "" && *it != "$") followS += *it + ", ";
            else followS += *it + "}\n";
        }
    }


    cout << " >> FIRST:\n" << firstS << " >> FOLLOW:\n" << followS;

}

set<string> CFG::first(const string &var)
{
    // TODO: nesting nog te goei maken zodat het niet te diep genest is.
    for (const auto &pro : gProductions)
    {
        if (pro.first == var && pro.second[0] != "")
        {
            // Het is een variable
            if (isVariable(pro.second[0]))
            {
                bool containsEps = true;
                int i = 0;
                while (containsEps)
                {
                    set<string> setLeft = first(pro.second[i]);
                    containsEps = containsEpsilon(setLeft);
                    for (const auto & setVal : setLeft)
                    {
                        if (!containsEps) gFirst[var].insert(setVal);
                        else if (i == pro.second.size()-1) gFirst[var].insert(setVal);
                        else if (setVal != "") gFirst[var].insert(setVal);
                    }

                    if (containsEps) i++;
                    if (i == pro.second.size()) containsEps = false;
                }
            }

            // Het is een terminal
            else if (isTerminal(pro.second[0])) gFirst[var].insert(pro.second[0]);
        }

        // epsilon
        else if (pro.first == var && pro.second[0] == "")
        {
            gFirst[var].insert("");
        }
    }

    return gFirst[var];
}

bool CFG::isVariable(const string &input)
{

    for (auto const & var : gVariables)
        if (var == input) return true;


    return false;
}

bool CFG::isTerminal(const string &input)
{
    for (auto const & ter : gTerminals)
        if (ter == input) return true;

    return false;
}

bool CFG::containsEpsilon(const set<string> &setLeft)
{
    for (const auto & elem : setLeft)
        if (elem == "") return true;

    return false;
}

set<string> CFG::follow(const string &var)
{

    /**
     * Toegevoegde "regels" gebasseerd om het huidige algoritme om een duidelijk overzicht te creeëren:
     * regel_1: geval var niet laatste in de production + first van follow bevat geen epsilon
     *          -> voeg first van de volgende toe aan follow
     * regel_2: geval var bevindt zich op de laatste plaats in de production
     *          -> voeg de follow toe van de head van de production
     * regel_3: geval var bevindt zich niet op de laatste plaats in de production + first van follow bevat epsilon
     */

    if (var == gStartsymbol) gFollow[var].insert("$");

    //zoeken de var in productions
    for (const auto & pro : gProductions)
    {
        for (int i = 0; i < pro.second.size(); ++i)
        {
            // regel_1 of regel_3
            if (pro.second[i] == var && i != pro.second.size()-1) addFollow_rule_1_or_3(pro, var, i);

            // regel_2
            else if (pro.second[i] == var && i == pro.second.size()-1)
            {
                addFollow_rule_2(pro, var);
            }
        }
    }

    return gFollow[var];

}

void CFG::addFollow_rule_1_or_3(const production &pro, const string &var, const int i)
{
    //Variable
    if (isVariable(pro.second[i+1]))
    {
        set<string> next = gFirst[pro.second[i+1]];
        //regel_1
        if (!containsEpsilon(next))
        {
            for (const auto & setVal : next) gFollow[var].insert(setVal);
        }

        //regel_3
        else addFollow_rule_3(pro, var, i, next);

    }
    //Terminal (regel_1)
    else gFollow[var].insert(pro.second[i+1]);
}

void CFG::addFollow_rule_2(const production &pro, const string &var)
{
    //bepalen de follow van de head en voegen deze toe
    //als de head hetzelfde is als het achterste van de body gaat dit een inf loop geven
    if (pro.first != var)
    {
        for (const auto & setVal : follow(pro.first)) gFollow[var].insert(setVal);
    }
}

void CFG::addFollow_rule_3(const production &pro, const string &var, const int i, set<string> next)
{
    //TODO: nestings diepte inorde krijgen.
    for (int j = i+1; j < pro.second.size() ; ++j)
    {
        for (const auto & setVal : next)
        {
            if (setVal != "") gFollow[var].insert(setVal);
        }


        // bevat geen epsilon meer dus kunnen hoeven niet meer voort te gaan
        if (!containsEpsilon(next)) break;

        //zitten nog niet op de laatste plaats in de body
        if (j+1 != pro.second.size())
        {
            if (isVariable(pro.second[j+1])) next = gFirst[pro.second[j+1]];
            else
            {
                gFollow[var].insert(pro.second[j+1]);
                break;
            }
        }

        //zitten op de laatste plaats in de body
        else
        {
            if (isVariable(pro.second[j]))
            {
                set<string> setL = gFirst[pro.second[j]];
                for (const auto & setVal : setL)
                {
                    if (setVal != "") gFollow[var].insert(setVal);
                }

                if (containsEpsilon(setL)) addFollow_rule_2(pro, var);

            }
            else gFollow[var].insert(pro.second[j]);
        }
    }
}

void CFG::createParserTable(vector<vector<string>> &parserTable, vector<int> &longestString)
{

    //opstellen van parse table
    for (int i = 0; i < gVariables.size()+1; ++i)
    {
        vector<string> row;

        for (int j = 0; j < gTerminals.size()+2; ++j)
        {

            if (i == 0 && j == 0) row.push_back("clear");
            else if (i == 0 && j < gTerminals.size() + 1) row.push_back(gTerminals[j-1]);
            else if (i == 0 && j == gTerminals.size() + 1) row.push_back("<EOS>");
            else if (i > 0 && j == 0) row.push_back(gVariables[i-1]);
            else if (i > 0 && j > 0) row.push_back("");
        }
        parserTable.push_back(row);
    }


    for (const auto &pro : gProductions)
    {
        string first = pro.second[0];

        //variable
        if (isVariable(first))
        {

            string str = "\'";
            for (const auto & bodyVal : pro.second)
            {
                if (bodyVal != pro.second[pro.second.size()-1]) str += bodyVal + " ";
                else str += bodyVal;
            }
            str += "\'";



            // neemt terminals uit follow van de variable (body)
            for (auto setVal : gFollow[first])
            {
                pair<int, int> rowAcolom = searchRowandColom(pro.first, setVal, parserTable);
                if (str.size() > longestString[rowAcolom.second]) longestString[rowAcolom.second] = str.size();
                parserTable[rowAcolom.first][rowAcolom.second] = str;
            }
        }

        //terminal
        else if (isTerminal(first))
        {
            // neemt production met terminal
            string str = "\'";
            pair<int, int> rowAcolom = searchRowandColom(pro.first, first, parserTable);
            for (const auto & bodyVal : pro.second)
            {
                if (bodyVal != pro.second[pro.second.size()-1]) str += bodyVal + " ";
                else str += bodyVal;
            }
            str += "\'";

            if (str.size() > longestString[rowAcolom.second]) longestString[rowAcolom.second] = str.size();

            parserTable[rowAcolom.first][rowAcolom.second] = str;
        }

        //epsilon
        else
        {
            // neemt follow van de head
            for (auto setVal : gFollow[pro.first])
            {
                if (setVal == "$") setVal = "<EOS>";
                pair<int, int> rowAcolom = searchRowandColom(pro.first, setVal, parserTable);
                if (setVal.size() > longestString[rowAcolom.second]) longestString[rowAcolom.second] = setVal.size();
                parserTable[rowAcolom.first][rowAcolom.second] = " ";
            }
        }
    }

}

pair<int, int> CFG::searchRowandColom(const string &variable, const string &terminal, const vector<vector<string>> & parseTable) {
    pair<int, int> rowAcolom;

    for (int i = 0; i < parseTable.size(); ++i)
    {
        if (parseTable[i][0] == variable)
        {
            rowAcolom.first = i;
            break;
        }
     }

    for (int i = 0; i < parseTable[0].size(); ++i)
    {
        if (parseTable[0][i] == terminal)
        {
            rowAcolom.second = i;
            break;
        }
    }

    return rowAcolom;

}

void CFG::printParserTable(const vector<vector<string>> &parserTable, vector<int> &longestString)
{

    cout << ">>> Table is built.\n\n" << "-------------------------------------\n" << endl;

    string table;
    string streepjes;
    for (int i = 0; i < parserTable.size(); ++i)
    {
        if (i == 0)
        {
            table += "     ";
            streepjes += "|-";
            for (int j = 0; j < longestString[0]; ++j) streepjes += "-";
            streepjes += "--";
        }
        else table += "| " + parserTable[i][0] + "  ";

        for (int j = 1; j < parserTable[i].size(); ++j)
        {
            string text = parserTable[i][j];
            int extraSpace = longestString[j] - text.size();
            table += "| " +text + "  ";

            if (i == 0)
            {
                streepjes += "|---";
                for (int k = 0; k < longestString[j]; ++k) streepjes += "-";
            }
            for (int b = 0; b < extraSpace; ++b)
            {
                table += " ";
//                if (i == 0 && j == 1) streepjes += "-";
            }
        }
        table += "|\n";

        if (i == 0)
        {
            streepjes += "|";
            table += streepjes + "\n";
        }
    }
    table += streepjes;
    cout << table;

}

