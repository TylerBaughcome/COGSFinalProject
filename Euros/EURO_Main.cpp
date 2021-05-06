#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include "tree.h"
#include "date.h"
/* This program asserts the probable outcome of a certain football match using a decision tree.
 * Using a data table and a query passed in via the command line, all historical precedents of the
 * conditions associated with that match are used to form a decision tree and produce an outcome.*/

std::string string_replace(const std::string& a, char replace, char new_char){
/* This is a utility function to remove spaces for entries in the table*/
std::string ret="";
for(int i=0;i<a.size();i++){
	if(a[i] == replace){
		ret+= new_char;
	}
	else{
		ret+=a[i];
	};
};
return ret;
}


void format_data(const std::string team_a, const std::string team_b,
		std::ifstream& inFile,
		std::vector<std::string>& conditions, std::vector<std::vector<std::string> >& data,
		const Date& starting_year){
	/*This function formats all the data in the data file to produce a data table of strings that can
	 * later be used to identify appropriate matchups and conditions for the query match. The way this 
	 * function specifically stores the data is based on an expected structure of the data file.*/
	std::string first_line;
	inFile >> first_line;
	std::string condition="";
	for(int i=0;i<first_line.length();i++){
	/*The first line holds the generic conditions that describes the data; store those first.*/
		if(first_line[i] ==','){
			conditions.push_back(condition);
			condition = "";
		}
		else{
			condition+=first_line[i];
		};
	};
	std::vector<std::string> current_line;
	char sub;
	std::string curr_string;
	while(std::getline(inFile,curr_string)){
	if(curr_string.length()==0){
		continue;
		};
	std::string term="";
	for(int i=0;i<curr_string.length();i++){
		/*Each entry in row separated by a comma*/
		if(curr_string[i] ==','){
			current_line.push_back(string_replace(term,' ','_'));
			term = "";
		}
		else{
			term+=curr_string[i];
		};
	};	
	current_line.push_back(string_replace(term,' ','_'));
	//need to check if valid date in range
	std::string date_string = current_line[0];
	std::string sub = "";
	std::string date_nums[3];
	int date_type = 0;
	for(int i=0;i<date_string.length();i++){
		if(date_string[i]=='-'){
			date_nums[date_type] = sub;
			sub ="";
			date_type++;
		}
		else{
			sub += date_string[i];
		};
	};
	date_nums[date_type] = sub;
	int date_ints[3];
	for(int i=0;i<3;i++){
		std::stringstream ss;
		ss << date_nums[i];
		int sub_date;
		ss >> sub_date;
		date_ints[i] = sub_date;
	};
	Date date_check(date_ints[2], date_ints[1],date_ints[0]);
	current_line.push_back(string_replace(term,' ','_'));
	if(!(date_check < starting_year)){	
	/* If the date of the match is before the specified starting year, we can add it to
	 * the data.*/
	data.push_back(current_line);
	};
	current_line.clear();
	};
}

void organize_data(std::vector<std::vector<std::string> >& organized_data,
		const std::vector<std::vector<std::string> >& all_data,
		const std::string& team_a, const std::string& team_b){
	/* This function organizes the data into the conditions we want to examine. Specifically for
	 * football matches pertaining to the given data set, there are a certain set of conditions and 
	 * an outcome we want to identify. These conditions are the following:
	 * Home/Away, Tournament Competition?, Neutral Venue
	 * The outcome we search for is simply Win, Loss, or Draw. Note that this is few conditions, but
	 * the data file doesn't offer much in the way of specific conditions.*/
std::vector<std::string> current_line;
for(int i = 0;i<all_data.size();i++){
  /*If the match pertains to the two teams we are looking for, store the data.*/
  if((all_data[i][1]==team_a && all_data[i][2] == team_b)||
     (all_data[i][1] == team_b && all_data[i][2] == team_a)){
  std::string outcome;
  bool a_is_home_team;
  for(int j=0;j<all_data[i].size();j++){
	if(j==1){
		/*The first entry pertains to the home team, where we use team_a, the 
		 * home team in our query, as our reference.*/ 
		if(all_data[i][j]==team_a){
			current_line.push_back("Home");
			a_is_home_team = true;
		}
		else{
			current_line.push_back("Away");
			a_is_home_team = false;
		};
		j++;
	}
	else if(j==3){
		//Assert scores to determine outcome
		std::stringstream score_home;
		std::stringstream score_away;
		score_home << all_data[i][j];
		score_away << all_data[i][j+1];
		//String to int conversion
		int scoreH;
		int scoreA;
		score_home >> scoreH;
		score_away >> scoreA;
		std::string margin;
		if(scoreH > scoreA){
			/*If team_a is the home team in this instance, then a win
			 * and vice versa.*/
			if(a_is_home_team){
			outcome = "Win";
			}
			else{
			outcome = "Loss";
			};
		}
		else if(scoreH < scoreA){
			/*If team_a is the home team in this instance, then a loss
			 * and vice versa.*/
			if(a_is_home_team){
			outcome = "Loss";
			}
			else{
			outcome = "Win";
			};
		}
		else{
			//If the scores are even, then outcome is a draw
			outcome = "Draw";
		};
		j++;
	}
	else if(j==5 || j==all_data[i].size()-1){
		if(j==5){
			/* The fifth entry pertains to the competition. As teams perform
			 * different in friendlies than in tournaments, assert whether or not
			 * game is a friendly (an exhibition) or an official match.*/
			if(all_data[i][j]!="Friendly"){
				current_line.push_back("Yes");
			}
			else{
				current_line.push_back("No");
			};
		}
		else{
		current_line.push_back(string_replace(all_data[i][j],' ', '_'));
		};
	};
  };
  //Add to organized_data and clear current line
  current_line.push_back(outcome);
  organized_data.push_back(current_line);
  current_line.clear();
  };
};

}

int main(int argc, char* argv[]){
	if(argc!=3){
		std::cerr << "Only arguments should be a file for reading the data and a query file, respectively.";
		exit(1);
	};
	//Read files
	std::ifstream inFile(argv[1]);
	std::ifstream query(argv[2]);
	std::string team_a;
	std::string team_b;
	//Get teams
	query >> team_a >> team_b;
	std::vector<std::string> query_features;
	std::string sub;
	int years_to_examine = 0;
	int this_year = 2021;
	int this_month = 5;
	int this_day = 1;
	//Arbitrary date set to somewhere around the time this was written
	//Simple request for how many years should be examined
	std::cout << "How many prior years would you like to examine? (please enter an integer value)" << std::endl;
	std::cin >> years_to_examine; 
	while(query >> sub){
		//Get the features of the query
		query_features.push_back(sub);
	};
	if(std::find(query_features.begin(),query_features.end(),"FALSE")!=query_features.end()){
	/*If the query match is not at a neutral venue, then add "Home" condition using team_a,
	 * the home team, as a reference.*/
	query_features.insert(query_features.begin(),"Home");
	};
	//Use simple Date class to make minimum date
	Date starting_date(this_day, this_month, this_year-years_to_examine);
	std::vector<std::string> base_conditions;
	std::vector<std::vector<std::string> > all_data;
	//Format the data
	format_data(team_a,team_b, inFile, base_conditions, all_data, starting_date);
	all_data.erase(all_data.begin());
	//always an odd empty read for std::getline; just remove that element
	std::vector<std::vector<std::string> > organized_data;
	//Organize the data
	organize_data(organized_data,all_data,team_a,team_b);
	/*Some cleanup for the conditions to have the conditions we want, as some of 
	 * the conditions specified in the data file are not all that meaningful for
	 * our purposes (such as venue city)*/
	std::vector<std::string> final_conditions = base_conditions;
	final_conditions.erase(final_conditions.begin());
	final_conditions.erase(final_conditions.begin());
	final_conditions.erase(final_conditions.begin());
	final_conditions.erase(final_conditions.begin());
	final_conditions.insert(final_conditions.begin(),"Home/Away");
	final_conditions.erase(final_conditions.begin()+1);
	final_conditions.erase(final_conditions.begin()+2);
	final_conditions.erase(final_conditions.begin()+2);
	final_conditions.erase(final_conditions.begin()+2);
	//final_conditions.insert(final_conditions.begin()+1,"Margin of Victory/Defeat");
	final_conditions.push_back("Tournament Competition?");
	final_conditions.push_back("Neutral_Location");
	final_conditions.push_back("Outcome");
	//Optional printing of organized data table (uncomment to print)
	/*
	for(int i = 0;i<final_conditions.size();i++){
		std::cout << final_conditions[i] << ' ';
	};
	std::cout << std::endl;
	for(int i=0;i<organized_data.size();i++){
	 for(int j=0;j<organized_data[i].size();j++){
		 std::cout << organized_data[i][j] << ' ';	
	 };
	 std::cout << std::endl;
	};*/
	//Initialize the decision tree
	DecisionTree<std::string> dt(final_conditions,organized_data,1,3,.3);
	//Optionally print all paths (uncomment to print)
	//dt.print_all_paths(std::cout);
	std::cout << std::endl << "Results with respect to " <<team_a << " vs. " << team_b
	<< "\n  *Note that the outcome is listed for the home team listed in the query." << std::endl;
	std::cout << std::endl;
	/*Print the most certain outcomes for the query.*/
	dt.print_best_paths_for_query(query_features);
	return 0;
}
