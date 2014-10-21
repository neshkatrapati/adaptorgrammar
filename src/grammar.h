#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string.h>
using namespace std;


class Grammar {
 private:
  vector<vector<char *> > rules;
  bool isCNF;
 public:
  char * startSymbol;
  Grammar ();
  Grammar (FILE *fp);
  vector <vector<char *> > getRules ();
  void setRules(vector <vector<char *> > rules);
  friend ostream& operator<<(ostream& os, const Grammar& g);
  Grammar toCNF();
  void ruleToCNF(int rule_id, vector < int > *prunables);
  bool isNonTerminal(char * candidate); 
  vector<vector <char *> > getRules ( char * candidate);
  bool hasRule(vector<char *> rule);
  int findRule(vector<char *> rule);
  Grammar pruneGrammar(vector <int> *prunables);
  vector<vector <char *> > reverseSearch(vector<char *> rhs);
  char * getStartSymbol() { return this -> startSymbol; };
};

Grammar::Grammar(){
  isCNF = false;
  startSymbol = (char *) malloc(20 * sizeof(char));
  startSymbol[0] = '\0';
}
char * shiftString(char *str){
  char * newString; 
  newString = (char *) malloc(20 * sizeof(char));
  newString[0] = '\0';
  for(int i=0; i < strlen(str) - 1; i++){
    newString[i] = str[i+1];
  }
  return newString;
}
Grammar::Grammar(FILE *fp) {
  size_t len = 0;
  ssize_t read;
  char *line = NULL; 
  char * lhs;
  char rhs [100];
  lhs = (char *) malloc(20 * sizeof(char));
  int offset = 0, charcount=0;
  while ((read = getline(&line, &len, fp)) != -1){
    vector<char *> rule;
    sscanf(line, "%s -> %s%n", lhs, rhs, &charcount);
    if(lhs[0] == '!'){
      lhs = shiftString(lhs);
      this -> startSymbol = strdup(lhs);
      this -> startSymbol[strlen(this -> startSymbol)] = '\0';
      //      cout << this -> startSymbol << endl;
    }
    offset = charcount;
    rule.push_back(strdup(lhs));
    rule.push_back(strdup(rhs));
    while ((sscanf(line + offset, "%s%n", rhs, &charcount) == 1)){
      rule.push_back(strdup(rhs));
      offset += charcount;
    }
    rules.push_back(rule);
  }
  isCNF = false;
}

ostream& operator<<(ostream& os, const Grammar& g){
  for (size_t i = 0; i < g.rules.size(); i++){
    for (size_t j = 0; j < g.rules[i].size(); j++){
	os << g.rules[i][j] << " ";
      }
      os << endl;
  }
  return os;
}
bool Grammar::isNonTerminal(char * candidate) {
  for (size_t i = 0; i < rules.size(); i++){
    if (strcmp(rules[i][0],candidate) == 0) {
      return true;
    }
  }
  return false;
}
vector <vector<char *> > Grammar::getRules(char * candidate) {
  vector <vector<char *> > allRules;
  for (size_t i = 0; i < rules.size(); i++){
    if (strcmp(rules[i][0],candidate) == 0) {
      allRules.push_back(rules[i]);
    }
  }
  return allRules;
}

  
void printRule(vector<char*> rule){
    for (size_t j = 0; j < rule.size(); j++){
	cout << rule[j] << " ";
    }
    cout << endl;
}

int Grammar::findRule(vector<char *> rule){
  //      cout << "R:"<< " "; 
  //      printRule(rule);

  int check = -100;
  for ( int i=0; i < rules.size(); i++ ) {
    if ( rule.size() == rules[i].size()){
      int check = i;
      for (int j = 0; j < rule.size(); j++){
	if (strcmp(rule[j], rules[i][j]) != 0){
	  check = -100;
	  break;
	}
      }
      if (check != -100) {
	//	cout << "S:" << check << endl;
	return check;
      }
    }
  }
  return -1;
}

bool Grammar::hasRule(vector<char *> rule){
  int ruleIndex = findRule(rule);
  if (ruleIndex == -1){
    return false;
  }
  return true;
}

void Grammar::ruleToCNF(int rule_id, vector<int> *prunables) {
  vector<char*> rule = rules[rule_id];
  vector<vector<char*> > productions = getRules(rule[0]);
  for(int i=0; i< productions.size(); i++){
    if ( productions[i].size() == 2) {
      if ( isNonTerminal(productions[i][1]) ) {
	vector<vector<char *> > redprods = getRules(productions[i][1]);
	for ( int j =0; j < redprods.size(); j++){
	  redprods[j][0] = rule[0];
	  if (hasRule(redprods[j]) == false){
	    rules.push_back(redprods[j]);
	  }
 	    int ruleIndex = findRule(productions[i]);
	    if (ruleIndex > -1){
	      prunables -> push_back(ruleIndex);
	  }	  
	}
      }
    }
    else if (productions[i].size() > 2) {
      bool isCase1 = false;
      vector <char *> newProduction;
      newProduction.push_back(productions[i][0]);
      int ruleIndex = findRule (productions[i]);
      for(int j=1; j < productions[i].size(); j++){
	if ( ! isNonTerminal (productions[i][j])){
	  cout << j << endl;
	  vector <char *> transferRule;
	  char temp[50];
	  sprintf(temp, "NT_%s", productions[i][j]);
	  transferRule.push_back(strdup(temp));
	  transferRule.push_back(productions[i][j]);
	  rules.push_back(transferRule);
	  newProduction.push_back(strdup(temp));
	  isCase1 = true;
	} else {
	  newProduction.push_back(productions[i][j]);
	}
      }
      if (isCase1){
	if (hasRule(newProduction) == false){
	  rules.push_back(newProduction);
	  rules.erase(rules.begin() + ruleIndex);
	}
      }else if (productions[i].size() > 3) {
	char temp[50];
	int randv = rand() % 100; 
	sprintf(temp, "NT_%c", productions[i][0][0]);
	vector < char *> newProduction, changedProduction;
	newProduction.push_back(strdup(temp));
	int ruleIndex = findRule (productions[i]);
	int productionSize = productions[i].size();
	for ( int j =1; j < productions[i].size() -1; j ++){
	  char append[5] = "\0";
	  append[0] = productions[i][j][0];
	  strcat(newProduction[0], append);
	  //	  strcat(append, productions[i][j][0]);
	  newProduction.push_back(productions[i][j]);
	}

	changedProduction.push_back(productions[i][0]);
	changedProduction.push_back(newProduction[0]);
	changedProduction.push_back(productions[i][productionSize - 2]);
	if (hasRule(newProduction) == false){
	  rules.push_back(newProduction);
	  rules.push_back(changedProduction);
	  rules.erase(rules.begin() + ruleIndex);
	}
      }
    }
  }
}


Grammar Grammar::pruneGrammar(vector<int> *prunables){
  Grammar newGrammar;
  
  for (int i = 0; i < rules.size(); i++) {
    bool check = false;
    for ( int j = 0; j < prunables -> size(); j++){
      if ( i == prunables -> at(j)){
	check = true;
	break;
      }
    }
    
    if ( check == false ){
      newGrammar.rules.push_back(rules[i]);
    }
  }
  return newGrammar;
  
}

Grammar Grammar::toCNF () {
   Grammar cnfGrammar;
   vector<int> prunables;
   for (size_t i = 0; i < rules.size(); i++){
     ruleToCNF(i, &prunables);
  }
   cnfGrammar = pruneGrammar(&prunables);
   //   cout << this -> startSymbol;
   cnfGrammar.startSymbol = this -> startSymbol;
   return cnfGrammar;
}
vector <vector<char *> > Grammar::reverseSearch(vector<char *> rhs){
  
  vector <vector<char *> > output;
  int check = 1;
  int loopcount = 0;
  for (int i=0; i<rules.size(); i++){
    vector<char *> rule = rules[i];
    check = 1;
    loopcount = 0;
    for ( int j =1; j < rule.size(); j++){
      if (strcmp(rule[j], rhs[j - 1]) != 0){
	check = 0;
	break;
      }
      loopcount ++;
    }
    if (check == 1 && loopcount > 0) {
      output.push_back(rule);
    }
  }
  return output;
}
