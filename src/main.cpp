#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// // Lecture TSV
// std::vector<std::vector<std::string>>
// read_tsv(std::ifstream *ifs, std::vector<std::vector<std::string>> items) {
//   if (ifs->fail()) {
//     std::cerr << "error" << std::endl;
//     return {};
//   }
//   std::string line;
//   while (getline(*ifs, line)) {
//     std::stringstream ss(line);
//     std::vector<std::string> item;
//     std::string tmp;
//     while (getline(ss, tmp, '\t')) {
//       item.push_back(tmp);
//     }
//     items.push_back(item);
//   }
//   return items;
// }


void parseTitles(){

  ifstream titres("data/title.basics.tsv");
  if (titres.fail()) {
    std::cerr << "error" << std::endl;
    return;
  }

  std::ofstream ofs("data/parsed/titles.tsv");
  if(ofs.fail()){
    std::cerr << "Error: Cannot create file " << "data/parsed/titles.tsv" << std::endl;
    return;
  }

  string line;
  while(getline(titres,line)){
    stringstream ss(line);
    vector<string> items;
    string tmp;
    while(getline(ss,tmp,'\t')){
      items.push_back(tmp);
    }
    if(items[1]== "video" || items[1]== "movie" || items[1]== "tvMovie"){
      ofs << items[0]<<'\t';//ID
      ofs << items[1]<<'\t';//Type
      ofs << items[3]<<'\n';//Title
    }
  }

  ofs.close();
  std::cout << "File created successfully" << std::endl;
}

void addRatings(){

  ifstream ratings("data/title.ratings.tsv");
  if (ratings.fail()) {
    std::cerr << "error" << std::endl;
    return;
  }

  ifstream titles("data/parsed/titles.tsv");
  if(titles.fail()){
    std::cerr << "error" << std::endl;
    return;
  }

  std::ofstream ofs("data/parsed/titlesWithRatings.tsv");
  if(ofs.fail()){
    std::cerr << "Error: Cannot create file " << "data/parsed/titlesWithRatings.tsv" << std::endl;
    return;
  }

  // Step 1: Load all ratings into a hash map for O(1) lookup
  // Key: tconst, Value: pair<averageRating, numVotes>
  unordered_map<string, pair<string, string>> ratingsMap;
  
  string line;
  // Skip header line in ratings file
  getline(ratings, line);
  
  while(getline(ratings, line)){
    stringstream ss(line);
    vector<string> items;
    string tmp;
    while(getline(ss, tmp, '\t')){
      items.push_back(tmp);
    }
    if(items.size() >= 3){
      // Store tconst -> (averageRating, numVotes)
      ratingsMap[items[0]] = make_pair(items[1], items[2]);
    }
  }
  ratings.close();
  
  std::cout << "Loaded " << ratingsMap.size() << " ratings into memory" << std::endl;
  
  // Step 2: Read titles and join with ratings
  int matchCount = 0;
  int totalCount = 0;
  
  while(getline(titles, line)){
    stringstream ss(line);
    vector<string> items;
    string tmp;
    while(getline(ss, tmp, '\t')){
      items.push_back(tmp);
    }
    
    if(items.size() >= 3){
      totalCount++;
      string tconst = items[0];
      
      // Write title info
      ofs << items[0] << '\t'; // ID
      ofs << items[1] << '\t'; // Type
      ofs << items[2] << '\t'; // Title
      
      // Check if rating exists and add it
      auto it = ratingsMap.find(tconst);
      if(it != ratingsMap.end()){
        ofs << it->second.first << '\t';  // averageRating
        ofs << it->second.second << '\n'; // numVotes
        matchCount++;
      } else {
        // No rating found, add empty fields or placeholders
        ofs << "\\N\t\\N\n";
      }
    }
  }
  
  titles.close();
  ofs.close();
  
  std::cout << "File created successfully" << std::endl;
  std::cout << "Matched " << matchCount << " out of " << totalCount << " titles with ratings" << std::endl;

}

// vector<vector<string>> parseTitleBasics(){

//   std::ifstream titres("data/title.basics.tsv");

//   vector<vector<string>> titleBasics;
//   titleBasics = read_tsv(&titres, titleBasics);
//   vector<vector<string>> retour;
//     for(auto film:titleBasics){
//       vector<string> tmpfilm;
//       for(int i=0;i<film.size();i++){
//         if(i==0 || i==3){
//           tmpfilm.push_back(film[i]);
//         }
//       }
//       retour.push_back(tmpfilm);
//     }
//   return retour;
// }



// vector<vector<string>> joinTitleRating(vector<vector<string>> titles){
//   int i=0; //Titles
//   int j=0; //Ratings

//    std::ifstream titleRatings("data/title.ratings.tsv");

//   vector<vector<string>> ratings;
//   ratings = read_tsv(&titleRatings, ratings);


//   for(i;i<titles.size();i++){
    
//     while (j < ratings.size() && ratings[j][0]!=titles[i][0]){
//       j++;
//     }
    
//     // Only add ratings if we found a match
//     if(j < ratings.size() && ratings[j][0] == titles[i][0]){
//       titles[i].push_back(ratings[j][1]);
//       titles[i].push_back(ratings[j][2]);
//     }
    
//   }

//   return titles;
  
// }

int main(int argc, char *argv[]) {
  
  // vector<vector<string>> titles = parseTitleBasics();
  // vector<vector<string>> ratings = joinTitleRating(titles);

  // for(int i=0;i<100;i++){
  //   for(auto value:ratings[i]){
  //     cout<<value<<" ";
  //   }
  //   cout<<"\n";
  // }

  parseTitles();
  addRatings();

  return 0;
}
