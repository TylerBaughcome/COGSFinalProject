#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip>
/*This header file is comprised by the DecisionTree class and its utility node
 * class. The decision tree, in general, is a tool to examine possible outcomes 
 * relative to established precedents in order to ultimately, as the name suggests,
 * make a decision. The implementation of the decision tree here is a non-binary tree
 * that propagates with the nodes having children. When a node does not have a child, there
 * are no more possibilities to explore an outcome of some degree of certainty is found. Per
 * a data table that the user provides, the decision tree is constructed, and the user can then
 * examine all outcomes that fit a custom query.*/
template <class T>
class DecisionTreeNode{
	/*This class represents the nodes that make up a decision tree. It stores
	 * important information regarding potential outcomes and the subsequent nodes
	 * to which it points, its children. Note that everything is left public as the
	 * user is not supposed to access this class, and it is only used in constructing
	 * the decision tree. */
	public:
	//CONSTRUCTORS
	DecisionTreeNode(const T& p, const T& i, std::map<T,float> c)
			{item = i; parent_condition=p;outcome_certainties =c;}
	DecisionTreeNode(const T& p){item = p; parent_condition=p;}
	DecisionTreeNode(const T& p, std::map<T,float> c){item = p; parent_condition=p;outcome_certainties=c;}

	//Member variables
	std::vector<DecisionTreeNode<T>*> children;
	//Pointers to subseqent nodes
	T item;
	//The feature that describes the condition the node represents
	T parent_condition;
	//The condition to which the feature refers
	DecisionTreeNode<T>* parent;
	//The parent node
	std::map<T, float> outcome_certainties;
	//All possible outcomes at this node from its path and their certainties (% occurence)
};
	
	
template <class T>
class DecisionTree{
	/* This class represents a decision tree. Comprised of a series of DecisionTreeNode's,
	 * each node in the tree is dynamically allocated memory and properly de-allocated once
	 * the DecisionTree goes out of scope. In terms of the behavior of the tree, the tree will
	 * examine all possible combinations of conditions by forming paths in the tree, but once a
	 * certain degree of certainty is found, the node is "pruned" so as not to overfit the data.
	 * This degree of certainty can be specified by the user, as well as the minimum number of
	 * occurences an outcome needs in order to be valid, another safety net to avoid overfitting.
	 * Ultimately, the DecisionTree class forms all paths that will lead to the certainty the user
	 * is looking for provided there is a path as such that exists. */ 
	private:
	//MEMBER VARIABLES
	int size_;
	//To track the size of the tree
	int min_occurences;
	//Minimum number of occurences an outcome needs (overfitting avoidance)	
	float prune_certainty;
	//Certainty at which pruning occurs (overfitting avoidance)
	DecisionTreeNode<T>* root;
	//The root node of the tree (all other nodes can be accessed from the root)
	
	//UTILITIES
	void print_sideways(std::ostream& ostr,DecisionTreeNode<T>* p, int depth)const;	
	//A recursive utility for the "print_sideways" public option

	void build_decision_tree(const std::vector<T>& conditions,
	       	const std::vector<std::vector<T> >& data, DecisionTreeNode<T>* p, const std::vector<int>&
		conditions_found, const std::vector<T>& features_path,int root_condition_index);
	//A utility for the constructor, this builds the decision tree in a depth-first fashion

	void destroy_tree(DecisionTreeNode<T>* p);
	//Utility for the destructor to de-allocate the assigned memory


	std::map<T,std::map<T, float> > get_certainties(const std::vector<T>& prior_features,
			const std::vector<int>& feature_indices,
		       	const std::vector<std::vector<T> >& data);
	//A recursive utiltiy to "get_certainties"'s public option
	void print_all_paths(std::ostream& ostr, DecisionTreeNode<T>* p,
		       	const std::vector<DecisionTreeNode<T>* >& path)const;
	//A recursive utility "print_all_paths"'s public option 

	void get_best_paths(const std::vector<T>& query, const std::vector<DecisionTreeNode<T>*>&  current_path,
				std::vector<std::vector<DecisionTreeNode<T>* > >& best_paths,
				float& best_certainty, DecisionTreeNode<T>* p,std::vector<T>& outcomes)const;
	//A recursive utility "print_best_paths_for_query"'s public option 
	bool is_rearranged_path(
			const std::vector<std::vector<DecisionTreeNode<T>* > >& best_paths,
					const std::vector<DecisionTreeNode<T>* > path)const;
	//A private utility to assert whether a path is non-unique (a re-arranged sequence)
	void print_path_to_parent(DecisionTreeNode<T>* p)const;
	//A private utility for debugging to print the path to the root node

	public:
	//CONSTRUCTORS
	DecisionTree(const std::vector<T>& conditions, const std::vector<std::vector<T> >& data,
	       	int root_condition_index,int min_occur, float prune);
	//ACCESSORS
	int get_size()const{return size_;}

	//PUBLIC UTILITIES
	void print_best_paths_for_query(const std::vector<T>& query)const;
	//Prints the paths that generate the highest degree of certainty
	void print_sideways(std::ostream& ostr)const;
	//A utility to print the shape of the tree sideways (Note this does not work well with many children)
	void print_all_paths(std::ostream& ostr)const;
	//A utility print all the paths in the tree from the root if tree cannot be visualized with "print_sideways"
	
	//DESTRUCTOR
	~DecisionTree(){this->destroy_tree(root);}
};

template <class T>
void DecisionTree<T>::print_path_to_parent(DecisionTreeNode<T>* p)const{
	/*This function simply prints the path to the root from the node passed in.
	 * It is not publicly available and was used for debugging.*/
	DecisionTreeNode<T>* temp = p;
	while(temp->parent){
	std::cout<< temp->item << ' ';
	temp=temp->parent;
	};
}



template <class T>
typename std::map<T,std::map<T,float> >
DecisionTree<T>::get_certainties(const std::vector<T>& prior_features, const std::vector<int>& feature_indices,
			const std::vector<std::vector<T> >& data){
	/*This function asserts the likely outcomes of the features provided. This is done by passing in a vector
	 * of indices that represent the features also passed in as the templated class, but an extra index in 
	 * "feature_indices" represents the new outcome that we would like to assert. Essentially, the function
	 * is characterized by the question: *How many times do the things found out these indices occur as the
	 *  combination shown in the features? */
	std::map<T,std::map<T,int> > outcomes;
	//map outcome conditions to number of results for each outcome for each set of conditions
	std::string checker = "";
	/*For every set of features in the data table, we want to see those instances that
	 * adhere to the features specified. */
	for(int i = 0;i<data.size();i++){
		int candidate = true;
		checker = "";
		for(int j=0;j<prior_features.size();j++){
			int index = feature_indices[j];
			checker +=prior_features[j]+' ';
			if(data[i][index] != prior_features[j]){
			/*If an entry for those indices in the data is not in our features,
			 * then we don't need to examine that line of data.*/
				candidate = false;
				break;
			};
		};
		if(candidate){
			//If it is a candidate, we increment the outcome associated with the set of features
			int index = feature_indices.back();
			//take the most recently added index
			outcomes[data[i][index]][data[i].back()]++;
		};
	};
	typename std::map<T,std::map<T,int> >::iterator outcomes_itr;
	std::map<T,std::map<T,float> > ret_certainties;
	for(outcomes_itr = outcomes.begin();outcomes_itr!=outcomes.end();outcomes_itr++){
		/*Here, we check if the outcomes meet the rquirement specified by "min_occurences"*/
		typename std::map<T,int>::iterator outcomes_itr2=outcomes_itr->second.begin();
		int denom = 0;
		for(;outcomes_itr2!=outcomes_itr->second.end();outcomes_itr2++){
			denom+= outcomes_itr2->second;
		};
		for(outcomes_itr2=outcomes_itr->second.begin();
		    outcomes_itr2!=outcomes_itr->second.end();outcomes_itr2++){
			if(denom >=min_occurences){
			//overfitting restriction => want at least this many occurences
			ret_certainties[outcomes_itr->first][outcomes_itr2->first] = 
				(float)outcomes_itr2->second/denom;
			};
		};
	};
	return ret_certainties;
	
}


template <class T>
void DecisionTree<T>::build_decision_tree(const std::vector<T>& conditions,
	       	const std::vector<std::vector<T> >& data, DecisionTreeNode<T>* p,
		const std::vector<int>& conditions_found, const std::vector<T>& features_path,
		int root_condition_index){
	/* This function is a utility for the constructor and recursively builds the decision tree.*/
	if(!p){
		//BASE CASE
		return;
	};
	for(int i=0;i<conditions.size()-1;i++){
		//for every possible condition (note that last entry, the outcome, is not considered)
		if(std::find(conditions_found.begin(),conditions_found.end(),i)==conditions_found.end()){
			if((conditions_found.size()==0&&i==root_condition_index)||conditions_found.size()>0){
			/*If the condition has not been found, as in it is not on the current path, we can
			 * create a new node.*/
			std::vector<int> conditions_found_copy = conditions_found;
			conditions_found_copy.push_back(i);
			std::map<T,std::map<T,float> > certainties = get_certainties(features_path,
					conditions_found_copy,data);
			typename std::map<T,std::map<T,float> >::iterator itr;
			for(itr = certainties.begin();itr!=certainties.end();itr++){
				/*Create a feature for every feature associated with a condition*/
				DecisionTreeNode<T>* new_node = 
					new DecisionTreeNode<T>(conditions[i], itr->first,itr->second);
				size_++;
				p->children.push_back(new_node);
				//add new node to current node's children
				new_node->parent =p;
				//assign parent
				bool make_leaf = false;
				typename std::map<T,float>::iterator prune_checker;
				for(prune_checker =itr->second.begin();prune_checker!=itr->second.end();
						prune_checker++){
					/*If the node has an outcome with a certainty >= "prune_certainty",
					 * we can make the new node a leaf.*/
					if(prune_checker->second >= prune_certainty){
						make_leaf = true;
						break;
					};
					
				};
				std::vector<T> features_path_copy = features_path;
				features_path_copy.push_back(itr->first);
				//copy the new feature to the features on the path
				if(!make_leaf){
				/*If the node is a leaf, then there is no need to continue adding to the path.*/
				build_decision_tree(conditions,data,new_node, conditions_found_copy,
						features_path_copy,root_condition_index);
				//Continue building with the node just created, thereby doing a depth-first build
				};
			};
			};
		};
	};
}	

template<class T>
DecisionTree<T>::DecisionTree(const std::vector<T>& conditions, const std::vector<std::vector<T> >& data,
	       	int root_condition_index,int min_occur, float prune){
	/* The constructor allocated memory for the root, which serves as a dummy. Each decision tree starts 
	 * at one of the condition specified and builds from there, where the "root condition" technically serves
	 * as the root of the tree even though it does not technically offer any information about the outcome alone.
	 * This simply allows us to build from a certain condition to look for optimal trees.*/
	prune_certainty=prune;
	min_occurences = min_occur;
	//dummy_root
	std::vector<int> conditions_found;
	std::vector<T> features_path;
	root = new DecisionTreeNode<T>(conditions[root_condition_index]);
	//dummy_root node with starting condition
	size_=1;
	this->build_decision_tree(conditions, data, root,conditions_found, features_path,root_condition_index);

}


template<class T> void DecisionTree<T>::print_sideways(std::ostream& ostr) const {
    //DRIVER FOR PRINT_SIDEWAYS
    if(this->size_==0){
    	ostr << "Tree is empty.\n";
	return;
    };
    print_sideways(ostr, this->root, 0);
}

template <class T> void DecisionTree<T>::print_all_paths(std::ostream& ostr, DecisionTreeNode<T>* p,
		const std::vector<DecisionTreeNode<T>*>& path)const{
/* This recursive function prints all the paths in the tree using a depth-first search.*/
if(p->children.size()==0){
//Base case: if leaf is found, just print the accumulated path
ostr << "General Outcomes for path:";
	for(int i = 1;i<path.size();i++){
		ostr << ' ' << path[i]->item;
	};
	ostr << std::endl;
	typename std::map<T,float>::iterator itr;
	for(itr = p->outcome_certainties.begin();itr!=p->outcome_certainties.end();itr++){
		ostr << "   "<< itr->first << ' ' << itr->second << std::endl;
	};

};
for(int i=0;i<p->children.size();i++){
	/*Continue searching down each of the children.*/
	std::vector<DecisionTreeNode<T>* > copy = path;
	copy.push_back(p->children[i]);
	print_all_paths(ostr, p->children[i], copy);
};
}

	
template <class T> void DecisionTree<T>::print_all_paths(std::ostream& ostr)const{
//DRIVER FOR PRINT_ALL_PATHS
std::vector<DecisionTreeNode<T>*> path;
path.push_back(root);
this->print_all_paths(ostr,root,path);
}



template <class T>
void DecisionTree<T>::get_best_paths(const std::vector<T>& query, 
			const std::vector<DecisionTreeNode<T>* >& current_path,
			std::vector<std::vector<DecisionTreeNode<T>* > >& best_paths, float& best_certainty,
			DecisionTreeNode<T>* p, std::vector<T>& outcomes)const{
	/*This recursive function is a utility for the "print_best_paths_for_query" function. Using a depth-first
	 * search, for those paths that match the query in whatever order, even if a truncated version of the path
	 * the query suggests, the certainty of the outcome of that path is evaluated. The highest certainty and 
	 * its outcome is appropriately tracked.*/
if(!p){
	return;
};
if(p->children.size()==0){
	//BASE CASE
	/* If a path that adheres to the query has reached a leaf, then evaluate its certainty and outcomes.*/
	typename std::map<T,float>::iterator itr;
	float max_certainty = 0;
	T certainty_elem;
	for(itr = p->outcome_certainties.begin();itr!=p->outcome_certainties.end();itr++){
		//Find max certainty of outcomes at this node
		if(itr->second > max_certainty){
			certainty_elem = itr->first;
			max_certainty = itr->second;	
		};
	};
	if(max_certainty == best_certainty){
	/*If merely equal, then can add path to best paths*/
	outcomes.push_back(certainty_elem);
	best_paths.push_back(current_path);
	}
	else if(max_certainty > best_certainty){
	//If higher, clear best_paths and add this path to best_path to set new standard of certainty
	best_paths.clear();
	outcomes.clear();
	best_certainty = max_certainty;
	outcomes.push_back(certainty_elem);
	best_paths.push_back(current_path);
	};
};
//only continue searching path if next entry is in query as well
for(int i=0;i<p->children.size();i++){
	if(std::find(query.begin(),query.end(),p->children[i]->item) != query.end()){
	std::vector<DecisionTreeNode<T>* > copy = current_path;
	copy.push_back(p->children[i]);
	get_best_paths(query, copy, best_paths, best_certainty, p->children[i],outcomes);
	};
};
}



template <class T> 
bool DecisionTree<T>::is_rearranged_path(const std::vector<std::vector<DecisionTreeNode<T>* > >& best_paths,
					const std::vector<DecisionTreeNode<T>* > path)const{
/* This function merely asserts whether the path has multiple variations for all those identified.
 * If there is no other path that is a mere re-arrangement of the one passed in, then it is a unique
 * path and false is returned.*/
std::vector<DecisionTreeNode<T>* > path_copy = path;
std::sort(path_copy.begin(),path_copy.end());
std::vector<DecisionTreeNode<T>* > copy_util;
for(int i=0;i<best_paths.size();i++){
	if(best_paths[i].size()==path.size()){
		copy_util = best_paths[i];
		std::sort(copy_util.begin(),copy_util.end());
		//Sort then check every index => O(nlogn)
		bool is_same=true;
		for(int j=0;j<copy_util.size();j++){
			if(copy_util[i]!=path_copy[i]){
				is_same =false;
				break;
			};
		};
		if(is_same){
			return true;
		};
	};
};
return false;
	
}

template <class T>
void DecisionTree<T>::print_best_paths_for_query(const std::vector<T>& query)const{
	/*This function prints the paths that lead to the most "certain" outcome for a 
	 * certain query. Note that it will print only those paths with the highest
	 * certainty, so unless there is a tie for the highest certainty among several
	 * paths, it will output only one path.*/
	std::vector<std::vector<DecisionTreeNode<T>* > > best_paths;
	std::vector<DecisionTreeNode<T>*> starter_path;
	starter_path.push_back(root);
	float best_certainty = -1.0;
	std::vector<T> outcomes;
	//Pass "best_paths" in as reference to recursive utility
	this->get_best_paths(query, starter_path, best_paths, best_certainty, root,outcomes);
	if(best_paths.size()==0){
		/*If no paths beat the best_certainty of -1, then no paths matched the query as a path has 
		 at least a certainty of 0.*/
		std::cerr << "No outcomes matched the conditions in the input query." << std::endl;
		exit(1);
	};
	//need to print only unique paths 
	std::cout << "For best_certainty of " << std::setprecision(3)
	       	<< (float)best_certainty << " with root_condition of " <<
		best_paths[0][0]->item << std::endl;
	std::vector<std::vector<DecisionTreeNode<T>* > > final_best_paths;
	for(int i=0;i<best_paths.size();i++){
		if(!is_rearranged_path(final_best_paths,best_paths[i])){
			//If the path is not one that we have already seen, then add it to final paths
			final_best_paths.push_back(best_paths[i]);
		};
	};	
	//Print the paths with the associated outcome and its certainty
	for(int i=0;i<final_best_paths.size();i++){
		std::cout << "Outcome of " << outcomes[i] << " with the following features:\n";
		for(int j=1;j<final_best_paths[i].size();j++){
			std::cout << "   Condition " <<final_best_paths[i][j]->parent_condition 
			       << ": " <<final_best_paths[i][j]->item << std::endl;
		};
		std::cout << std::endl;
	};
}


template<class T> void DecisionTree<T>::print_sideways
(std::ostream& ostr,DecisionTreeNode<T>* p, int depth) const {
   /* The print_sideways function prints the tree to a stream and 
    * represents the structure of the tree along with all the keys 
    * associated with each node. It is accomplished using an in-order
    * traversal of each node.*/
    if(!p->children.size()==0){ 
	    //Left 
    for(int i=0;i<p->children.size()/2;i++){
	      print_sideways(ostr, p->children[i], depth+1);	      
    };
    //Root
    ostr << std::string(depth,'\t');
	ostr<<p->item;
    ostr << std::endl;
    //Right
    if(!p->children.size()==0){
    for(int i=p->children.size()/2;i<p->children.size();i++){
	      print_sideways(ostr, p->children[i], depth+1);	      
    };
    };
   };

}

template <class T>
void DecisionTree<T>::destroy_tree(DecisionTreeNode<T>* p){
/*This recursive function is a simple utility for the destructor. Using
 * a depth-first search, we delete the node once only if all its children 
 * have been deleted.*/
if(p){
for(int i=0;i<p->children.size();i++){
	destroy_tree(p->children[i]);
};
delete p;
};
}


