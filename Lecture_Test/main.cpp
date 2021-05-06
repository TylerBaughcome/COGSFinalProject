#include "tree.h"
#include <map>
#include <vector>
#include <cctype>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <set>
//For weighting certain input, weights must be specified coming in
std::string lowerfy(const std::string& s){
	std::string ret="";
	for(int i=0;i<s.length();i++){
		ret+=tolower(s[i]);
	};
return ret;
}



int main(int argc, char *argv[]){
if(argc!=2){
	std::cerr << "Need an input file.";
	exit(1);
};
if(!argv[1]){
	std::cerr << "ERROR: The file could not be opened for reading.";
	exit(1);
};
std::vector<std::vector<std::string> > dataTable;
/*Using vector approach, everything sequence must correspond to the conditions given (not likely we are going to know all the conditions we want => thus conditions are limited to what we can know)*/
/*Also have to ensure that no features or conditions in text file are two words (can change at end by looking for capital letters, the start of a capital letter meaning a new word)*/
//get counts of occurences to examine certainty/probability rates
//associate a type of phenomenon with all the outcomes for all conditions;
//need to build a tree out of the phenomenon that ocurr
//change to user interface, use stdout for now
std::string condition;
std::vector<std::string> conditions;
std::ifstream dataFile(argv[1]);
//certain types of conditions are factors that produce the final output
//a general condition has specific conditions associated with it (could be object-oriented if need be)
while(condition!="quit"){
	dataFile >> condition;	
	if(condition!="quit"){
	std::cout << condition << std::endl;
	conditions.push_back(condition);
	};
};
std::string first_element;
while(dataFile >> first_element){
	if(first_element=="query"){
		break;
	};
	std::vector<std::string> subVec;
	subVec.push_back(first_element);
	std::string sub;
	for(int i=1;i<conditions.size();i++){
		dataFile >> sub;
		subVec.push_back(sub);
	};
	dataTable.push_back(subVec);
	subVec.clear();
};
//read in the query;
std::vector<std::string> query_features;
std::string query;
while(dataFile >> query){
	query_features.push_back(query);
};
//have data
//checking data
for(int i=0;i<dataTable.size();i++){
 for(int j=0;j<dataTable[i].size();j++){
	std::cout << dataTable[i][j] << ' ';	
 };
 std::cout << std::endl;
};
std::cout << "Query:";
for(int i = 0;i<query_features.size();i++){
	std::cout << ' ' << query_features[i];
};
std::cout << std::endl;
//need to specify a root_condition_index for the tree in addition to the iterator associated with that "index"
DecisionTree<std::string> dt(conditions,dataTable,5,4,.75);
//Possible root_condition_indices: [0..5]
//Printing outcomes
dt.print_best_paths_for_query(query_features);
//Visualize tree with sideways print and print_all_paths
//dt.print_sideways(std::cout);
//Not exactly working
dt.print_all_paths(std::cout);
return 0;
}
