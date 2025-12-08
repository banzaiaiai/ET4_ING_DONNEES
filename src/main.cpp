#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

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

  ofs << "ID\t";
  ofs << "GENRE\t";
  ofs << "NAME\t";
  ofs << "AVGRATTING\t";
  ofs << "NBRATTINGS\n";

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

void addEstimate(){

  ifstream productions("data/parsed/productionCinématographique.tsv");
  if (productions.fail()) {
    std::cerr << "error opening productionCinématographique.tsv" << std::endl;
    return;
  }

  ifstream titlesRatings("data/parsed/titlesWithRatings.tsv");
  if(titlesRatings.fail()){
    std::cerr << "error opening titlesWithRatings.tsv" << std::endl;
    return;
  }

  std::ofstream ofs("data/parsed/titlesRatingsEstimates.tsv");
  if(ofs.fail()){
    std::cerr << "Error: Cannot create file " << "data/parsed/titlesRatingsEstimates.tsv" << std::endl;
    return;
  }

  // Step 1: Load productions data into a hash map
  // Key: TITRE (normalized), Value: pair<REALISATEUR, DEVIS>
  unordered_map<string, pair<string, string>> productionsMap;
  
  string line;
  // Skip header line in productions file
  getline(productions, line);
  
  while(getline(productions, line)){
    stringstream ss(line);
    vector<string> items;
    string tmp;
    while(getline(ss, tmp, '\t')){
      items.push_back(tmp);
    }
    if(items.size() >= 5){
      // Normalize title for matching (convert to lowercase)
      string title = items[1];
      transform(title.begin(), title.end(), title.begin(), ::tolower);
      // Store TITRE -> (RÉALISATEUR, DEVIS)
      productionsMap[title] = make_pair(items[2], items[4]);
    }
  }
  productions.close();
  
  std::cout << "Loaded " << productionsMap.size() << " productions into memory" << std::endl;
  
  // Step 2: Write header to output file
  getline(titlesRatings, line); // Read header from titlesRatings
  ofs << line << '\t' << "REALISATEUR\t" << "DEVIS\n";
  
  // Step 3: Process titlesRatings and join with productions
  int matchCount = 0;
  int totalCount = 0;
  
  while(getline(titlesRatings, line)){
    stringstream ss(line);
    vector<string> items;
    string tmp;
    while(getline(ss, tmp, '\t')){
      items.push_back(tmp);
    }
    
    if(items.size() >= 3){
      totalCount++;
      
      // Write all existing fields
      ofs << line;
      
      // Normalize title for matching
      string title = items[2];
      transform(title.begin(), title.end(), title.begin(), ::tolower);
      
      // Check if production data exists and add it
      auto it = productionsMap.find(title);
      if(it != productionsMap.end()){
        ofs << '\t' << it->second.first;  // RÉALISATEUR
        ofs << '\t' << it->second.second; // DEVIS
        matchCount++;
      } else {
        // No production data found, add empty fields
        ofs << '\t' << "\\N\t\\N";
      }
      ofs << '\n';
    }
  }
  
  titlesRatings.close();
  ofs.close();
  
  std::cout << "File titlesRatingsEstimates.tsv created successfully" << std::endl;
  std::cout << "Matched " << matchCount << " out of " << totalCount << " titles with production data" << std::endl;

}

int main(int argc, char *argv[]) {
  
  parseTitles();
  addRatings();
  addEstimate();

  return 0;
}
