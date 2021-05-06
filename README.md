# COGSFinalProject
Decision Tree Programs for Sport Analysis
Author: Tyler Baughcome

# Resources: 
International football results from 1872 to 2020
https://www.kaggle.com/martj42/international-football-results-from-1872-to-2017

# Description of Decision Tree Data Structure:
  The decision tree data structure implemented here is a simple non-binary tree.
  Each decision tree node has children to which it points, and those children propagate until
  no conditions are available or until they exceed a certain degree of certainty with one of
  their outcomes. The memory is allocated and de-allocated explicitly for each node in the tree,
  where each node also has a pointer to its parent node. The tree also takes two extra parameters
  in addition to the data and conditions that are used to avoid overfitting the data, that is extracting
  certainty from standalone or events with a minimal precedent. This is done with a "prune certainty", which
  truncates a path if that path has a certainty greater than the prune certainty, and it also accomplishes
  this with a parameter that details the minimum number of occurences an outcome must have for it to be valid. 
  For example, if a team wins 5 times out of 7, and the "min_occurences" member variable is equal to 5, then the
  outcome is valid and is added to the tree, but if it were equal to 8 or greater, then it would not be added.
  A final note on the decision tree is that a root node must be specified. For instance, for all the conditions, the
  tree must begin at a certain condition in order to begin building the tree. While everything could just be put under 
  one dummy root node, this approach allows for smaller decision trees so that we may eventually find the optimal one to
  describe the scenario at hand.

# Important Functions
  Parameters
   The number of conditions in the data table => c
   The number of possible outcomes for each line of data => o
   The number of lines of data in the data table => n
   The number of unique features in the data set => u
  build_decision_tree:
    This function is a utility for the constructor, where it builds each path in a depth-first
    fashion. The paths are built such that each condition connects to another unique for every 
    addition, and every possible permutation of the conditions should be explored unless a path
    is at some point truncated as a result of having a particularly certain outcome.
    Note that it has an approximate runtime of around O(u * c^2 * o).
  get_certainties:
    This function asserts the certainty of the outcomes for a new node by assessing the compounded
    certainty of all the previous conditions on the path. It does so by examining certain indices in 
    the data that are relavant to the path being examined.
    Note that it has an approximate runtime of O(n * c + o)
  get_best_paths:
    This function identifies the path(s) with the greatest degree of certainty. If there is a tie
    among the paths for the greatest degree of certainty, every path with that certainty is stored
    until a path with a greater degree of certainty is found. 
    Note that the approximate runtime of this function is O(u * c), where c reflects the maxiumum
    number of children a node could have as it could not have more children than there are conditions.
# Tests and Data Collected
Hypothetical Basketball Team
  In the file entitiled *Joe.txt* a data table and query can be found. The table specifies the conditions
  and results of a basketball team's last 10 or so games, and the query specifies a certain set of conditions
  that align with teh precedents. With the query given, one can use the decision tree to explore the certainty,
  more or less, of possible outcomes, particularly for different root nodes. 
  For instance, in this example, using the program, we will find that having conditions of
  Location: Away 
  Opposing Center: Tall
  will in conjunction provide a certainty of a win of .75 or 75% when the root node is the condition
  with respect to the location. This seems pretty good in terms of asserting certainty, but if we were
  to look at this tree, it would be slightly large, and perhaps not the optimal one.
  This is proven when the root condition refers to the time when the team plays. At 9pm, the team has lost 
  100% of their games, and thus we only require one condition to produce a certain outcome, which is a 
  representation of what we may call a *good* decision tree.
  Note that, if interested, there are some images showing the structure of certain trees with different nodes
  for this example, images that came from the class for which this project was completed.
Euro 2020/2021
  The ultimate goal of this project was to try and predict the outcome of a large sporting event using this decision
  tree data strucutre and algorithm. As a football fan, I chose to pursue the possible outcome of the 2020/2021 Euros
  which will occur in the summer of 2021 after being postponed as a result of the COVID-19 pandemic. Using a massive data
  table of international football results from 1872-present day, I executed the program *EURO_Main.cpp* for every game in the 
  group stage and the subsequent predicted games to ultimately produce a winner. It is important to note that the data I used was
  somewhat limited in terms of conditions, and after making some conditions more concrete, such as the score, and removing other less
  revealing conditions that required more detail, as such as the city in which a match was played, there were only 3 conditions, excluding
  the outcome that were used to predict results, but they were usually quite revealing. It is important to note as well that the home team
  in the query was always used as a reference, so the conditions were listed for that team. This does mean that the away team wasn't examined, but
  it doesn't matter because the teams only play each other and their outcomes will be the opposite unless a draw occured, where it was the same.
  The conditions used were the following: Home/Away, Tournament Competition?, and Neutral Venue?. Basically, whether the team was playing at home
  or away, whether the game was in a tournament rather than begin an exhibition, and whether the game occured at a neutral venue determined the results.
  Furthermore, if the match did occur at a neutral venue, whether the game was home or away was not considered, as a neutral venue gives no considerable
  advantage either way with respect to the location most of the time. Examining each match under these conditions, the outcome of every game in the group stage
  was determined based on the highest certainty of an outcome, and the results were recorded. In the *Euros* folder, the results of all such games
  in the group stage are recorded with the certainties of the outcomes, and a table showing the final results of the group stage are shown as well.
  Next, the matches of the knockout phase were determined using the rules of the competition, and the results from the group stage were added to 
  the data table. The same process was repeated until a champion was found.
  A final note on this process is in the variance in the root condition used for each match. Sometimes, no meaningful data was accrued that superceded
  the minumum number of occurences for the outcomes (that is "min_occurences" = 3), in which case the root condition was changed. Always starting with
  Home/Away, I would then proceed to Tournament Competition? and Neutral Venue? until a meaningful result was found. If no meaningul result was found, as
  either the consequence of the two teams not playing that often historically or sometimes as the result of limited games at neutral venues, a draw was assumed
  and a coin flipped if a knockout phase game. Because many of the games for the Euros do occur at neutral venues, though only slightly more than half this year,
  this occured fairly often. Note as well that data from the past fifty years only was used.
# Conclusion
Much was learned from decision trees in this project. One can easily see the limitations of using such black and white conditions and outcomes 
to determine the outcomes of events that simply aren't so black and white, a false dilemma. In addition, the number of conditions that are present
allow for more accurate conclusions, but it is particularly difficult to collect and account for all conditions, and many conditions lie on a gradient, not
a finite number of features that occur. Yes, begin home or away, the time at which an event occurs, and other factors are plain, but the mental states 
of the players, their leadership and communications skills, and more are not things that can effectively be described so categorically. Such items, particularly
with regard to sports, lie on a spectrum and are subjective, and with a decision tree, those features that "lie on a spectrum" technically have an infinite 
number of features, and thus a decision tree is not always the optimal way to determine such outcomes, especially in sports. 
Furthermore, in terms of runtime and memory usage on a compute, it becomes evident that smaller decision trees are better. For instance, in the hypothetical
basketball example, we saw how sometimes it only takes one condition to find a quite certain and sufficiently numerous outcome, but identifying the best tree
then becomes a recursive process. 
Ultimately, there are some features of a decision tree that are not implemented here that would certaintly improve the program. For instance, many decision
trees use weighting to ascertain the importance of each condition. After witnessing the results, it becomes apparent that some conditions are more important
than others, and weighting is a technique that would allow for more accurate results, but it is not implemented here.
  

  
