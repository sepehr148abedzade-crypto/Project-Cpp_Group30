#include "ProjectManager.h"
#include "constants.h"
#include "Entity.h"
#include <fstream>
#include <sstream>

void SaveProject(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (auto& ch : allCharacters) {
        file << "CHARACTER|" << ch.name << "|" << ch.x << "|" << ch.y << "|" << ch.size << "|" << ch.isvisible << "\n";
    }

    for (auto& block : active_blocks) {
        file << "BLOCK|" << block.id << "|" << block.rect.x << "|" << block.rect.y;
        for (const auto& val : block.values) {
            file << "|" << val;
        }
        file << "\n";
    }

    file.close();
}

void LoadProject(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    active_blocks.clear();
    allCharacters.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, '|');

        if (type == "CHARACTER") {
            Character ch;
            std::string xStr, yStr, sizeStr, visStr;
            std::getline(ss, ch.name, '|');
            std::getline(ss, xStr, '|'); ch.x = std::stod(xStr);
            std::getline(ss, yStr, '|'); ch.y = std::stod(yStr);
            std::getline(ss, sizeStr, '|'); ch.size = std::stoi(sizeStr);
            std::getline(ss, visStr, '|'); ch.isvisible = (visStr == "1");
            allCharacters.push_back(ch);
        } else if (type == "BLOCK") {
            Blocks b;
            std::string idStr, xStr, yStr;
            std::getline(ss, idStr, '|'); b.id = std::stoi(idStr);
            std::getline(ss, xStr, '|'); b.rect.x = std::stoi(xStr);
            std::getline(ss, yStr, '|'); b.rect.y = std::stoi(yStr);
            std::string val;
            while (std::getline(ss, val, '|')) {
                b.values.push_back(val);
            }
            active_blocks.push_back(b);
        }
    }
    file.close();
}

void NewProject() {
    active_blocks.clear();
    allCharacters.clear();
}