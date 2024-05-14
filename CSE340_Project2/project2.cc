/* 

   Arsalan Nawabi
        
        &

   Junichi Koizumi

/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <vector>

using namespace std;






class Rule {
public:
    Token leftHand;
    std::vector<Token> rightHand;
};

std::vector<Rule> ruleList;
std::vector<int> indexLHS;
std::vector<int> indexRHS;
std::vector<std::string> indexList;
std::vector<std::string> Terminals;
std::vector<std::string> Nonterminals;
std::vector<std::string> sortedNonterminals;
std::vector<std::vector<std::string>> firstSets;
std::vector<std::vector<std::string>> followSets;




LexicalAnalyzer* lexer = new LexicalAnalyzer();


int index(const std::string& target) {
    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i] == target) {
            return i;
        }
    }
    return -1;
}

int goToRuleLeft(const std::string& lhs) {
    for (int i = 0; i < ruleList.size(); i++) {
        if (ruleList[i].leftHand.lexeme == lhs) {
            return i;
        }
    }
    return -1;
}

int findSortedNonterminalIndex(const std::vector<std::string>& sorted, const std::string& target) {
    for (int i = 0; i < sorted.size(); i++) {
        if (sorted[i] == target) {
            return i;
        }
    }
    return -1;
}

bool isInNonterminals(const std::string& target) {
    for (const auto& nonterminal : Nonterminals) {
        if (nonterminal == target) {
            return true;
        }
    }
    return false;
}



bool isElement(const std::string& target, const std::vector<std::string>& list) {
    for (const auto& element : list) {
        if (target == element) {
            return true;
        }
    }
    return false;
}

bool isInTerminal(const std::string& target) {
    for (const auto& terminal : Terminals) {
        if (terminal == target) {
            return true;
        }
    }
    return false;
}

void addToSet(std::vector<std::string>& existing, const std::vector<std::string>& toInput) {
    for (const auto& item : toInput) {
        if (!isElement(item, existing) && item != "#") {
            existing.push_back(item);
        }
    }
}

void addToSetFollow(std::vector<std::string>& existing, const std::vector<std::string>& toInput) {
    for (const auto& item : toInput) {
        if (!isElement(item, existing) && item != "#") {
            existing.push_back(item);
        }
    }
}

bool rhsTrue(const bool generating[], const std::vector<Token>& rhs) {
    for (const auto& token : rhs) {
        if (!generating[index(token.lexeme)]) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> removeRepeats(const std::vector<std::string>& list) {
    std::vector<std::string> newList;
    for (const auto& item : list) {
        if (!isElement(item, newList)) {
            newList.push_back(item);
        }
    }
    return newList;
}

void makeTerminals() {
    for (const auto& rule : ruleList) {
        for (const auto& token : rule.rightHand) {
            if (!isInNonterminals(token.lexeme) && !isInTerminal(token.lexeme)) {
                Terminals.push_back(token.lexeme);
            }
        }
    }
}

void makeNonterminals() {
    Nonterminals.push_back(ruleList[0].leftHand.lexeme);

    for (int i = 1; i < ruleList.size(); i++) {
        if (!isInNonterminals(ruleList[i].leftHand.lexeme)) {
            Nonterminals.push_back(ruleList[i].leftHand.lexeme);
        }
    }
}

bool hasEpsilon(const Rule& rule) {
    for (const auto& token : rule.rightHand) {
        if (!isElement("#", firstSets[index(token.lexeme)])) {
            return false;
        }
    }
    return true;
}




void CalculatingFirst(){

    
    vector<string> set;
    //initialize the firstSets
    for(int i = 0; i < indexList.size(); i++) {
        firstSets.push_back(set);
    }

    for (int n = 0; n < ruleList.size(); n++){
    for (int i = 0; i < ruleList.size(); i++){
       
        Rule rule = ruleList[i];
      
        int indexThisLHS = index(rule.leftHand.lexeme);

       
        if (rule.rightHand.size() == 0){
            if (!isElement("#", firstSets[indexThisLHS])){
                firstSets[indexThisLHS].push_back("#");
            } 
        }
        else { 
            
            Token thisRight = rule.rightHand[0];
            int indexStartRHS = index(thisRight.lexeme);

            bool hasAllEpsilon = hasEpsilon(rule);

           
            if (isInTerminal(thisRight.lexeme)){
                //if not already in this rule's (lhs) firstset, add this terminal to it
                if (!isElement(thisRight.lexeme, firstSets[indexThisLHS])){
                    firstSets[indexThisLHS].push_back(thisRight.lexeme);
                    //add then stop
                } 
            }
            else if(hasAllEpsilon && !isElement("#", firstSets[indexThisLHS])) {
                firstSets[indexThisLHS].push_back("#");
            }
           
            else if(isInNonterminals(thisRight.lexeme) && isElement("#", firstSets[indexStartRHS])){
               
                int k = 0; 

               
                while (k < rule.rightHand.size()){ 

                    if(isInTerminal(rule.rightHand[k].lexeme)) {
                        if(!isElement(rule.rightHand[k].lexeme, firstSets[indexThisLHS])) {
                            firstSets[indexThisLHS].push_back(rule.rightHand[k].lexeme);
                        }
                        break;
                    }                
                    
                   
                    int indexThisRHS = index(rule.rightHand[k].lexeme);

                    
                    if (isInNonterminals(rule.rightHand[k].lexeme) && isElement("#", firstSets[indexThisRHS])){
                       
                        addToSet(firstSets[indexThisLHS], firstSets[indexThisRHS]);
                    }
                    else if (isInTerminal(rule.rightHand[k].lexeme)){
                        
                        if (!isElement(rule.rightHand[k].lexeme, firstSets[indexThisLHS])){
                            firstSets[indexThisLHS].push_back(rule.rightHand[k].lexeme);
                            break;
                            
                        } 
                    }
                    else if (isInNonterminals(rule.rightHand[k].lexeme) && !isElement("#", firstSets[indexThisRHS])){
                       
                        addToSet(firstSets[indexThisLHS], firstSets[indexThisRHS]);
                        break;
                    }
                    k++;
                }
            }
            
            else if (isInNonterminals(thisRight.lexeme) && !isElement("#", firstSets[indexStartRHS])){
                
                if (firstSets[indexStartRHS].size() != 0){
                   
                    addToSet(firstSets[indexThisLHS], firstSets[indexStartRHS]);
                   

                }   
                
            }
        }

    }

    }

}



bool HasUselessRules() {
    int size  = indexList.size();

   
    bool generatingIndex[indexList.size()];
    //initialize to false first
    for (int i = 0; i < indexList.size(); i++) {
        generatingIndex[i] = false;
    }
    for(int i = 0; i < indexList.size(); i++) {
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generatingIndex[i] = true;
        }
    }
    
   
    for(int n = 0; n < ruleList.size(); n++) {
        //for each rule
        for(int i = 0; i < ruleList.size(); i++) {

           
            if(ruleList[i].rightHand.size() == 0) {
                generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
            }
            else{
               
                if(rhsTrue(generatingIndex, ruleList[i].rightHand) == true) {
                    generatingIndex[index(ruleList[i].leftHand.lexeme)] = true;
                }
            }
        }
    }

    
    vector<bool> generatableList(ruleList.size(), false); 
  
    for(int i = 0; i < ruleList.size(); i++) {
        
        if(generatingIndex[index(ruleList[i].leftHand.lexeme)] && rhsTrue(generatingIndex, ruleList[i].rightHand)) {
            generatableList[i] = true;
        }
    }

    vector<Rule> generatingRules;
    for(int i = 0; i < ruleList.size(); i++) {
       
        if(generatableList[i]) {
            generatingRules.push_back(ruleList[i]);
        }
    }
  

    bool reachableIndex[indexList.size()];
   
    for (int i = 0; i < indexList.size(); i++) {
        reachableIndex[i] = false;
    }


  
    if (generatingIndex[index(ruleList[0].leftHand.lexeme)]){
        
      
        if (generatingRules.size() != 0){
            reachableIndex[index(generatingRules[0].leftHand.lexeme)] = true;
        }

       
        for(int n = 0; n < generatingRules.size(); n++) {
            for(int i = 0; i < generatingRules.size(); i++) {
               
                if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
                    if (generatingRules[i].rightHand.size() == 0){
                        reachableIndex[index("#")] = true;
                    }
                    else{
                        //for each rhs, make reachable
                        for(int j = 0; j < generatingRules[i].rightHand.size(); j++) {
                            reachableIndex[index(generatingRules[i].rightHand[j].lexeme)] = true;
                        }
                    }
                }
        
            }
        }
    }

    

  
    vector<bool> reachableList(generatingRules.size(), false); 
   
    for(int i = 0; i < generatingRules.size(); i++) {
       
        if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
            reachableList[i] = true;
        }
    }

    bool hasUseless = false;
    // Check for unreachable rules
    for (int i = 0; i < generatingRules.size(); i++) {
        if (!reachableList[i]) {
            hasUseless = true;
            break;
        }
    }
    if (hasUseless) {
        return true;
    }
    else{
        return false;
    }
}



bool HasLeftRecursion() {
    for (const auto& nonterminal : Nonterminals) {
        for (const auto& rule : ruleList) {
            if (rule.leftHand.lexeme == nonterminal && rule.rightHand.size() > 0 && rule.rightHand[0].lexeme == nonterminal) {
                return true; // Left recursion found
            }
        }
    }
    return false; // No left recursion
}






void ReadGrammar() {
    indexList.push_back("#");
    indexList.push_back("$");

    while (true) {
        Rule current;
        bool endOfRule = false;

        Token token = lexer->GetToken();

        if (token.token_type == HASH) {
            break; // End of grammar definition
        } else if (token.token_type == ARROW) {
            continue; // Skip arrow tokens
        } else if (token.token_type == ID) {
            if (index(token.lexeme) == -1) {
                indexList.push_back(token.lexeme);
            }

            current.leftHand = token;

            while (!endOfRule) {
                Token rhsToken = lexer->GetToken();

                if (rhsToken.token_type == STAR) {
                    endOfRule = true; // End of the current rule
                } else if (rhsToken.token_type == ARROW) {
                    continue; // Skip arrow tokens
                } else if (rhsToken.token_type == ID) {
                    if (index(rhsToken.lexeme) == -1) {
                        indexList.push_back(rhsToken.lexeme);
                    }
                    current.rightHand.push_back(rhsToken);
                } else {
                    continue; // Skip other tokens
                }
            }
        }
        ruleList.push_back(current); // Add the parsed rule to the rule list
    }

    makeNonterminals();
    makeTerminals();

    // Update sortedNonterminals list
    for (const auto& indexItem : indexList) {
        if (!isElement(indexItem, sortedNonterminals) && isInNonterminals(indexItem)) {
            sortedNonterminals.push_back(indexItem);
        }
    }
}







// Task 1
void printTerminalsAndNoneTerminals()
{
    for(int i = 0; i < Terminals.size(); i++) {
        cout << Terminals[i] << " ";
    }

    for(int i = 0; i < sortedNonterminals.size(); i++) {
        cout << sortedNonterminals[i] << " ";
    }
}






// Task 2
void RemoveUselessSymbols()
{
    int size  = indexList.size();

   
    bool generatingIndex[indexList.size()];
  
    for (int i = 0; i < indexList.size(); i++) {
        generatingIndex[i] = false;
    
        if(isInTerminal(indexList[i]) || indexList[i] == "#" || indexList[i] == "$") {
            generatingIndex[i] = true;
        }
    }
    
  
    for(int i = 0; i < ruleList.size(); i++) {
        
        for(int j = 0; j < ruleList.size(); j++) {

            
            if(ruleList[j].rightHand.size() == 0) {
                generatingIndex[index(ruleList[j].leftHand.lexeme)] = true;
            }
            else{
              
                if(rhsTrue(generatingIndex, ruleList[j].rightHand) == true) {
                    generatingIndex[index(ruleList[j].leftHand.lexeme)] = true;
                }
            }
        }
    }

   
    vector<bool> generatableList(ruleList.size(), false); 
    vector<Rule> generatingRules;
    for(int i = 0; i < ruleList.size(); i++) {
       
        if(generatingIndex[index(ruleList[i].leftHand.lexeme)] && rhsTrue(generatingIndex, ruleList[i].rightHand)) {
            generatableList[i] = true;
        }
        if(generatableList[i]) {
            generatingRules.push_back(ruleList[i]);
        }
    }
    
    
    bool reachableIndex[indexList.size()];
    
    for (int i = 0; i < indexList.size(); i++) {
        reachableIndex[i] = false;
    }
   
    if (generatingIndex[index(ruleList[0].leftHand.lexeme)]){

       
        if (generatingRules.size() != 0){
            reachableIndex[index(generatingRules[0].leftHand.lexeme)] = true;
        }

       
        for(int n = 0; n < generatingRules.size(); n++) {
            for(int i = 0; i < generatingRules.size(); i++) {
                
              
                if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
                    if (generatingRules[i].rightHand.size() == 0){
                        reachableIndex[index("#")] = true;
                    }
                    else{
                        
                        for(int j = 0; j < generatingRules[i].rightHand.size(); j++) {
                            reachableIndex[index(generatingRules[i].rightHand[j].lexeme)] = true;
                        }
                    }
                }
        
            }
        }
    }


    vector<bool> reachableList(generatingRules.size(), false);
    for(int i = 0; i < generatingRules.size(); i++) {
       
        if(reachableIndex[index(generatingRules[i].leftHand.lexeme)]) {
            reachableList[i] = true;
        }
    }

   
    for (int i = 0; i < generatingRules.size(); i++) {
       
        if (reachableList[i]) {

            Rule rule = generatingRules[i];
            cout << rule.leftHand.lexeme << " -> ";
            if (rule.rightHand.size() == 0){
                cout << "#";
            }
            else{
                for (int j = 0; j < rule.rightHand.size(); j++) {
                    Token token = rule.rightHand[j];
                    cout << token.lexeme << " ";
                }
            }
            cout << endl;
        }
    }
}



void reOrder(vector<string> &list) {
    for (size_t i = 0; i < list.size(); ++i) {
        for (size_t j = 0; j < list.size() - 1; ++j) {
            if (index(list[j]) > index(list[j + 1])) {
                swap(list[j], list[j + 1]);
            }
        }
    }
}



// Task 3
void CalculateFirstSets() {
    
    CalculatingFirst();

   
    for (size_t i = 0; i < firstSets.size(); ++i) {
        reOrder(firstSets[i]);
    }

    
    for (size_t i = 0; i < indexList.size(); ++i) {
        if (isInNonterminals(indexList[i])) {
            cout << "FIRST(" << indexList[i] << ") = { ";
            for (size_t j = 0; j < firstSets[i].size(); ++j) {
                cout << firstSets[i][j];
                if (j != firstSets[i].size() - 1) {
                    cout <<  ", ";
                }
            }
            cout << " }" << endl;
        }
    }
}







// Task 4 
void CalculateFollowSets(){

    CalculatingFirst();

    


    vector<string> set;
   
    for(int i = 0; i < indexList.size(); i++) {
        followSets.push_back(set);
    }
   
    for(int i = 0; i < indexList.size(); i++) {
        if (isInNonterminals(indexList[i])){
            followSets[i].push_back("$"); 
            break;
        }
    }


  
    for (int n = 0; n < ruleList.size(); n++) {
    for (int i = 0; i < ruleList.size(); i++) {
        
        Rule rule = ruleList[i];
       
        int thisRHSSize = rule.rightHand.size();
        int rhsLastIndex = thisRHSSize - 1;
        
        
        for (int p = rule.rightHand.size() - 1; p >= 0; p--) {

            string thisRHS = rule.rightHand[p].lexeme;
            if (!(p-1 >= 0)){
                continue; 
            }
            string nextRHS = rule.rightHand[p-1].lexeme;

           
            if  (((p-1) >= 0 && isInTerminal(thisRHS) && isInTerminal(nextRHS)) || ((p-1) >= 0 && isInNonterminals(thisRHS) && isInTerminal(nextRHS)) ) {
                continue;
            
            
                
            }

           
            else if ((p-1) >= 0 && isInTerminal(thisRHS) && isInNonterminals(rule.rightHand[p-1].lexeme)){

               
                string thisTerminal = thisRHS;
                int t = 1; 

                do { 
                    int indexNextRHS = index(rule.rightHand[p-t].lexeme);

                   
                    if (!isElement(thisTerminal, followSets[indexNextRHS])){
                        followSets[indexNextRHS].push_back(thisTerminal);
                    }
                   
                    if (!isElement("#", firstSets[index(rule.rightHand[p-t].lexeme)])){
                        break;
                    }
                    t++;
                    
                }
                
                while ((p-t) >= 0 && isInNonterminals(rule.rightHand[p-t].lexeme) 
                && isElement("#", firstSets[index(rule.rightHand[p-t-1].lexeme)]));
                
            }
           
            else if ((p-1) >= 0 && isInNonterminals(thisRHS) && isInNonterminals(rule.rightHand[p-1].lexeme)){

               
                int k = 1;
                
                int indexThisRHS = index(thisRHS);

                do { 
                    int indexFollow = index(rule.rightHand[p-k].lexeme);
                   
                    addToSetFollow(followSets[indexFollow], firstSets[indexThisRHS]);                
                    
                    if (!isElement("#", firstSets[index(rule.rightHand[p-k].lexeme)])){
                        break;
                    }
                    k++;
                    
                } 
                while ((p-k) >= 0 && isInNonterminals(rule.rightHand[p-k].lexeme)  && isElement("#", firstSets[index(rule.rightHand[p-k-1].lexeme)]));
          
          
            }
        }

        
    }
    }

    
    for (int n = 0; n < ruleList.size(); n++) {
    for (int i = 0; i < ruleList.size(); i++) {
        
        Rule rule1 = ruleList[i];

        int lastindex = rule1.rightHand.size() - 1;
        if (lastindex < 0){
            continue;
        }

       
        if (isInNonterminals(rule1.rightHand[lastindex].lexeme)){

            
            do {

                if (lastindex< 0){
                    break;
                }

               
                int indexFollowLHS = index(rule1.leftHand.lexeme);
                
                int indexFollowRHS = index(rule1.rightHand[lastindex].lexeme);
               
                addToSetFollow(followSets[indexFollowRHS], followSets[indexFollowLHS]);
               
                if (!isElement("#", firstSets[indexFollowRHS])){
                    break;
                }
                lastindex--;  
            } 
            while (lastindex >= 0 && isInNonterminals(rule1.rightHand[lastindex].lexeme) && isElement("#", firstSets[index(rule1.rightHand[lastindex+1].lexeme)]));

        }
        

    }
    }


    for(int i = 0; i < followSets.size(); i++) {
        reOrder(followSets[i]);
    }

    for (int i = 0; i < indexList.size(); i++){
        if (isInNonterminals(indexList[i])){
            cout << "FOLLOW(" << indexList[i] << ") = { "; 
            for (int j = 0; j < followSets[i].size(); j++){
                cout << followSets[i][j];
                if (j != followSets[i].size() - 1){
                    cout <<  ", ";
                }
            }
            cout << " }";
            cout << endl;
        }
    }
}


// Task 5
void CheckIfGrammarHasPredictiveParser()
{
   
    if ((HasUselessRules()) || (HasLeftRecursion()) ) {
        cout << "NO\n";
        return;
    }
   
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}