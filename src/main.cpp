#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// Lecture TSV
std::vector<std::vector<std::string>>
read_tsv(std::ifstream *ifs, std::vector<std::vector<std::string>> items) {
  if (ifs->fail()) {
    std::cerr << "error" << std::endl;
    return {};
  }
  std::string line;
  while (getline(*ifs, line)) {
    std::stringstream ss(line);
    std::vector<std::string> item;
    std::string tmp;
    while (getline(ss, tmp, '\t')) {
      item.push_back(tmp);
    }
    items.push_back(item);
  }
  return items;
}
int main(int argc, char *argv[]) {
  std::ifstream titres("../data/title.basics.tsv");
  std::ifstream production("../data/production cinématographique - liste "
                           "des premiers films(8).tsv");
  std::vector<std::vector<std::string>> titre;
  titre = read_tsv(&titres, titre);
  for (const auto &item : titre) {
    for (const auto &field : item) {
      std::cout << field << " ";
    }
    std::cout << std::endl;
  }
  return 0;
}
