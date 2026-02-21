#include "ProjectManager.h"
#include "constants.h"
#include "Entity.h"
#include <fstream>
#include <sstream>

void SaveProjectToFile(string filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) return;

    outFile << projectBackdrops.size() << endl;
    for (const auto& bd : projectBackdrops) {
        outFile << bd.name << endl;
    }

    outFile << selectedBackdropIndex << endl;

    outFile << now_sprite.name << "|" << now_sprite.x << "|" << now_sprite.y << endl;

    outFile.close();
}

void LoadProjectFromFile(string filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) return;

    projectBackdrops.clear();

    int backdropCount;
    if (!(inFile >> backdropCount)) return;
    string dummy;
    getline(inFile, dummy);

    for (int i = 0; i < backdropCount; i++) {
        string bName;
        getline(inFile, bName);
        AddBackdropToProject(nullptr, bName, false, true);
    }

    inFile >> selectedBackdropIndex;
    getline(inFile, dummy);

    string spriteLine;
    if (getline(inFile, spriteLine)) {
        stringstream ss(spriteLine);
        string nameStr, xStr, yStr;
        if (getline(ss, nameStr, '|') && getline(ss, xStr, '|') && getline(ss, yStr, '|')) {
            now_sprite.name = nameStr;
            now_sprite.x = stoi(xStr);
            now_sprite.y = stoi(yStr);
        }
    }

    inFile.close();
}

void NewProject() {
    active_blocks.clear();
    allCharacters.clear();
}