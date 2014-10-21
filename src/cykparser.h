#include <iostream>
#include <cstdlib>

class ParseTreeNode {
  
  char * label; 
  vector < ParseTreeNode > children;
  ParseTreeNode *parent;
 public:
  ParseTreeNode ();
  ParseTreeNode (char * label);
  void setLabel( char *label);
  void add( ParseTreeNode child );
  void setParent(ParseTreeNode *parent);
  void print(int tab_level);
  char * getLabel();
  char * flatRepr();
  vector < ParseTreeNode > getChildren();
  ParseTreeNode* getParent();
  ParseTreeNode clone();
  
};

ParseTreeNode::ParseTreeNode(){
  this -> label = (char *)malloc(100*sizeof(char));
  this -> label[0] = '\0';
  
}
ParseTreeNode::ParseTreeNode( char * label ){
  this -> label = label;
  this -> label[strlen(this->label)] = '\0';
}
void ParseTreeNode::setLabel(char *label){ 
  this -> label = label;
}
void ParseTreeNode::add( ParseTreeNode child ) {
  this -> children.push_back(child);
}
ParseTreeNode ParseTreeNode::clone(){
  ParseTreeNode c;
  strcat(c.label , this -> label);
  //  copy.setLabel(label)
  for ( int i =0; i < this -> children.size(); i++){
    c.add(children[i]);
  }
  //  copy.children = children;
  c.parent = parent;
  return c;

}
void ParseTreeNode::setParent(ParseTreeNode *parent){
  this -> parent = parent;
}
void print_tabs(int tab_level){
  if (tab_level > 0){
    cout << "|";
  }
  for (int i =0; i < tab_level; i++){
    cout << "-";
  }
}
void ParseTreeNode::print(int tablevel){
  print_tabs(tablevel);
  cout << label << endl;
  for ( int i = 0; i < children.size(); i++){
    children[i].print(tablevel + 1);
  }
}
char * ParseTreeNode::getLabel() {
  return label;
}
char * ParseTreeNode::flatRepr(){
  char * buf;
  buf = (char *) malloc(1000*sizeof(char));
  buf[0] = '\0';
  strcat(buf, "(");
  strcat(buf, label);

  for ( int i = 0; i< children.size(); i++){
    strcat(buf, " ");
    char * childbuf = children[i].flatRepr();
    strcat(buf, childbuf);
  }
  strcat(buf, ")");
  
  return buf;
}

vector<ParseTreeNode> ParseTreeNode::getChildren(){
  return children;
}
ParseTreeNode* ParseTreeNode::getParent(){
  return parent;
}


vector<char*> stringToVec(char * input, char delimiter){
  vector <char *> output;
  char * buf; 
  buf = (char *)malloc(20*sizeof(char));
  buf[0] = '\0';
  for (int i=0; i < strlen(input); i++){
    char ch = input[i];
    
    if (strlen(buf) > 0 && (ch == delimiter || delimiter == '+')){
      output.push_back(strdup(buf));
      buf[0] = '\0';
    }
    int buf_len = strlen(buf);
    buf[buf_len] = ch;
    buf[buf_len + 1] = '\0';
    
  }
  output.push_back(strdup(buf));
  return output;
}

class CYKCell {
  vector <ParseTreeNode> productions;
 public:
  CYKCell () {};
  void setProductions ( vector <ParseTreeNode> productions ){
    this -> productions = productions;
  };
  vector<ParseTreeNode> getProductions(){
    return productions;
  };
  void add(ParseTreeNode p){
    productions.push_back(p);
  }
};



class CYKParser {
 Grammar g;
 public:
  CYKParser (){}; 
  CYKParser (Grammar g) { this -> g = g; };
  vector<ParseTreeNode> parse(vector <char *> input);
  void parseMulti(CYKCell ***cellarray, int i, int j);
  void getMatchingProductions(CYKCell ***cellarray, CYKCell *left, CYKCell *down, int i, int j);
  vector<vector<char *> > fillUni(char * token);
};

vector<vector<char *> >  unionProductions(vector <vector<char *> > a, vector <vector<char *> > b, bool mask,char * maskstring ){
  for ( int i =0; i < b.size(); i++){
    if (mask){
      vector<char *> rule = b[i];
      rule[1] = maskstring;
      a.push_back(rule);
      
    }else{
      a.push_back(b[i]);
    
    }
  }
  return a;
}

void printProductions(vector<vector<char *> > a){
  for (int i = 0; i< a.size(); i++)
    printRule(a[i]);
}

vector<vector<char *> > CYKParser::fillUni(char * token) {
  vector<char *> searchFor;
  searchFor.push_back(token);
  vector <vector<char *> > productions = g.reverseSearch(searchFor);
  char *candidate = "@c";
  searchFor.clear();
  searchFor.push_back(candidate);
  vector <vector<char *> > other_productions = g.reverseSearch(searchFor);
  productions = unionProductions(productions, other_productions, true, token);
  return productions;
}


void printCellArray(CYKCell ***cellarray, int len){
  for ( int i =0; i < len; i ++){
    for (int j = 0; j < i; j++){
      cout << "\t";
    }
    for ( int j = i ; j < len ; j ++){
      vector <ParseTreeNode> productions = cellarray[i][j] -> getProductions();
      int production_size =  productions.size();
      cout << '[';
      if (production_size > 0) {
	for ( int k =0; k< production_size; k++){
	  cout << productions[k].flatRepr() << ",";
	}
      }
      cout << "]\t";
    }
    cout << endl;
  }
}


/* vector<ParseTreeNode> convertProductionsIntoTrees(vector<vector<char *> > productions){ */
/*   vector<ParseTreeNode> parseTreeNodes; */
/*   for(int i=0; i < productions.size(); i++){ */
/*     ParseTreeNode p; */
/*     p.setLabel() */
/*   } */
/* } */

void CYKParser::getMatchingProductions (CYKCell ***cellarray, CYKCell *left, CYKCell *down, int p, int q) {
  vector<ParseTreeNode> leftProductions = left -> getProductions();
  vector<ParseTreeNode> downProductions = down -> getProductions();
  vector<char *> candidateProduction;

  for (int i = 0; i < leftProductions.size(); i++){
    ParseTreeNode leftProduction = leftProductions[i];
    candidateProduction.push_back(leftProduction.getLabel());
    candidateProduction.push_back(NULL);
    
    for ( int j = 0; j < downProductions.size(); j ++){
      ParseTreeNode downProduction = downProductions[j];
      candidateProduction[1] = downProduction.getLabel();
      vector<vector <char *> > candidateProductions = g.reverseSearch(candidateProduction);
      //      printRule(candidateProduction);
      if ( candidateProductions.size() > 0){
	//cout << endl;
	//cout << "Found " << candidateProductions.size() << " Matching Rules" << endl ;
	for ( int k =0; k < candidateProductions.size(); k++){
	  ParseTreeNode parent;
	  parent.setLabel(candidateProductions[k][0]);
	  parent.add(leftProduction.clone());
	  parent.add(downProduction);
	  cellarray[p][q] -> add(parent);
	  //	  printRule(candidateProductions[k]);
	  //parent.print(0);
	  //cout << endl;
	}
	//cout << endl;
      }
      //      cout << leftProduction.getLabel() << " " << downProduction.getLabel() << endl;
      //      printRule(candidateProductions);
    
    }
    candidateProduction.clear();
  }
  
}

void CYKParser::parseMulti(CYKCell ***cellarray, int i, int j){
  for ( int k = i; k < j; k ++ ){
    CYKCell *left = cellarray[i][k];
    CYKCell *down = cellarray[k+1][j];
    getMatchingProductions (cellarray, left, down, i, j);
    // cout << left -> getProductions().size() << " " << down -> getProductions().size()<< endl;
    
  }
}

vector<ParseTreeNode> getFinalParses(CYKCell *cell, char * startSymbol){
  vector <ParseTreeNode> productions  = cell -> getProductions();
  vector <ParseTreeNode> newProductions;
  for(int i=0; i<productions.size(); i++){
    if (strcmp(productions[i].getLabel(), startSymbol) == 0){
      newProductions.push_back(productions[i]);
    }
  }
  return newProductions;
}

vector<ParseTreeNode> CYKParser::parse (vector <char *> input){
  int input_len = input.size();
  CYKCell *** cellarray = new CYKCell**[input_len];
  for ( int i =0; i < input_len; i++){
    cellarray[i] = new CYKCell*[input_len];
    for (int j = 0; j < input_len; j++){
      cellarray[i][j] = new CYKCell();
    }
  }
  for ( int i =0; i< input_len; i++){
    for (int j =0; j < input_len - i; j++){
      //      cout << j << " - " << j + i <<endl;
      if ( i == 0 ){
	vector<vector<char*> > uniprods = fillUni(input[j]);
	vector<ParseTreeNode> pv;
	for ( int k=0; k<uniprods.size(); k++){
	  ParseTreeNode p(uniprods[k][0]);
	  ParseTreeNode c(uniprods[k][1]);
	  p.add(c);
	  c.setParent(&p);
	  pv.push_back(p);
	}
	cellarray[j][j] -> setProductions(pv);
      }
      else {
	parseMulti(cellarray, j, j + i);
      }
    }
    //    cout << endl;
  }
  vector<ParseTreeNode> newParses = getFinalParses(cellarray[0][input_len -1], g.startSymbol);
  return newParses;
}


