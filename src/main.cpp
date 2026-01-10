#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct TitleData {
    string line;
    string formattedTitle;
    int date;
    bool matched;
    string realisateur;
    string devis;
};

string formatTitle(string in) {
    transform(in.begin(), in.end(), in.begin(), ::toupper);
    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
    return in;
}

string moveArticleToFront(string title) {
    size_t openParen = title.rfind('(');
    if (openParen != string::npos && openParen > 0) {
        size_t closeParen = title.rfind(')');
        if (closeParen != string::npos && closeParen > openParen) {
            string article = title.substr(openParen + 1, closeParen - openParen - 1);
            string titlePart = title.substr(0, openParen);
            while (!titlePart.empty() && titlePart.back() == ' ') {
                titlePart.pop_back();
            }
            title = article + titlePart;
        }
    }
    return title;
}

void parseTitles() {
    ifstream titres("data/title.basics.tsv");
    if (titres.fail()) {
        cerr << "Error opening data/title.basics.tsv" << endl;
        return;
    }

    ofstream ofs("data/parsed/titles.tsv");
    if (ofs.fail()) {
        cerr << "Error: Cannot create file data/parsed/titles.tsv" << endl;
        return;
    }

    string line;
    int count = 0;
    getline(titres, line);

    while (getline(titres, line)) {
        stringstream ss(line);
        vector<string> items;
        string tmp;
        while (getline(ss, tmp, '\t')) {
            items.push_back(tmp);
        }

        if (items.size() >= 7 && (items[1] == "movie" || items[1] == "tvMovie" || items[1] == "video")) {
            ofs << items[0] << '\t';
            ofs << items[1] << '\t';
            ofs << items[3] << '\t';
            ofs << formatTitle(items[3]) << '\t';
            ofs << items[5] << '\n';
            count++;
        }
    }

    ofs.close();
    cout << "File titles.tsv created successfully" << endl;
    cout << "Parsed " << count << " lines from title.basics.tsv" << endl;
}

void parseProdCine() {
    cout << "Converting Excel file to TSV..." << endl;

    int result = system("venv/bin/python3 src/excel_to_tsv.py");

    if (result != 0) {
        cerr << "Error: Failed to convert Excel file to TSV" << endl;
        cerr << "Please ensure Python 3 and openpyxl are installed" << endl;
        return;
    }

    cout << "Excel file converted successfully" << endl;
}

void addRatings() {
    ifstream ratings("data/title.ratings.tsv");
    if (ratings.fail()) {
        cerr << "Error opening data/title.ratings.tsv" << endl;
        return;
    }

    ifstream titles("data/parsed/titles.tsv");
    if (titles.fail()) {
        cerr << "Error opening data/parsed/titles.tsv" << endl;
        return;
    }

    ofstream ofs("data/parsed/titlesWithRatings.tsv");
    if (ofs.fail()) {
        cerr << "Error: Cannot create file data/parsed/titlesWithRatings.tsv" << endl;
        return;
    }

    ofs << "ID\tGENRE\tNAME\tFORMATTEDNAME\tDATE\tAVGRATING\tNBRATINGS\n";

    unordered_map<string, pair<string, string>> ratingsMap;
    string line;
    getline(ratings, line);

    while (getline(ratings, line)) {
        stringstream ss(line);
        vector<string> items;
        string tmp;
        while (getline(ss, tmp, '\t')) {
            items.push_back(tmp);
        }
        if (items.size() >= 3) {
            ratingsMap[items[0]] = make_pair(items[1], items[2]);
        }
    }
    ratings.close();

    cout << "Loaded " << ratingsMap.size() << " ratings into memory" << endl;

    int matchCount = 0;
    int totalCount = 0;

    while (getline(titles, line)) {
        stringstream ss(line);
        vector<string> items;
        string tmp;
        while (getline(ss, tmp, '\t')) {
            items.push_back(tmp);
        }

        if (items.size() >= 5) {
            totalCount++;

            ofs << items[0] << '\t';
            ofs << items[1] << '\t';
            ofs << items[2] << '\t';
            ofs << items[3] << '\t';
            ofs << items[4] << '\t';

            auto it = ratingsMap.find(items[0]);
            if (it != ratingsMap.end()) {
                ofs << it->second.first << '\t';
                ofs << it->second.second << '\n';
                matchCount++;
            } else {
                ofs << "\\N\t\\N\n";
            }
        }
    }

    titles.close();
    ofs.close();

    cout << "File titlesWithRatings.tsv created successfully" << endl;
    cout << "Matched " << matchCount << " out of " << totalCount << " titles with ratings" << endl;
}

void addEstimate() {
    ifstream productions("data/parsed/productionCinématographique.tsv");
    if (productions.fail()) {
        cerr << "Error opening data/parsed/productionCinématographique.tsv" << endl;
        return;
    }

    ifstream titlesRatings("data/parsed/titlesWithRatings.tsv");
    if (titlesRatings.fail()) {
        cerr << "Error opening data/parsed/titlesWithRatings.tsv" << endl;
        return;
    }

    ofstream ofs("data/final/donnes.tsv");
    if (ofs.fail()) {
        cerr << "Error: Cannot create file data/final/donnes.tsv" << endl;
        return;
    }

    string line;
    vector<TitleData> allTitles;
    unordered_map<string, vector<int>> titleIndex;

    string headerLine;
    getline(titlesRatings, headerLine);

    while (getline(titlesRatings, line)) {
        stringstream ss(line);
        vector<string> items;
        string tmp;
        while (getline(ss, tmp, '\t')) {
            items.push_back(tmp);
        }

        if (items.size() >= 5) {
            TitleData td;
            td.line = line;
            td.formattedTitle = items[3];
            td.matched = false;
            td.realisateur = "\\N";
            td.devis = "\\N";

            try {
                td.date = stoi(items[4]);
            } catch (...) {
                td.date = 0;
            }

            int idx = allTitles.size();
            allTitles.push_back(td);
            titleIndex[td.formattedTitle].push_back(idx);
        }
    }
    titlesRatings.close();

    cout << "Loaded " << allTitles.size() << " titles into memory" << endl;

    getline(productions, line);

    int matchCount = 0;
    int prodCount = 0;

    while (getline(productions, line)) {
        stringstream ss(line);
        vector<string> items;
        string tmp;
        while (getline(ss, tmp, '\t')) {
            items.push_back(tmp);
        }

        if (items.size() >= 5) {
            prodCount++;

            string formattedTitle = formatTitle(moveArticleToFront(items[1]));

            int prodDate = 0;
            try {
                prodDate = stoi(items[4]);
            } catch (...) {
                prodDate = 0;
            }

            auto it = titleIndex.find(formattedTitle);
            if (it != titleIndex.end()) {
                int bestIdx = -1;
                int bestDiff = 999999;

                for (int idx : it->second) {
                    if (!allTitles[idx].matched) {
                        int diff = abs(prodDate - allTitles[idx].date);
                        if (diff < bestDiff) {
                            bestDiff = diff;
                            bestIdx = idx;
                        }
                    }
                }

                if (bestIdx != -1) {
                    allTitles[bestIdx].matched = true;
                    allTitles[bestIdx].realisateur = items[2];
                    allTitles[bestIdx].devis = items[3];
                    matchCount++;
                }
            }
        }
    }
    productions.close();

    ofs << headerLine << "\tREALISATEUR\tDEVIS\n";

    for (const auto& td : allTitles) {
        ofs << td.line << '\t' << td.realisateur << '\t' << td.devis << '\n';
    }

    ofs.close();

    cout << "File donnes.tsv created successfully" << endl;
    cout << "Matched " << matchCount << " productions out of " << prodCount << " to " << allTitles.size() << " titles" << endl;
}

int main() {
    parseTitles();
    parseProdCine();
    addRatings();
    addEstimate();
    return 0;
}
