#include "DAO/RatingDAO.h"

#include <fstream>
#include <sstream>

RatingDAO::RatingDAO(const std::string& file) : filename(file) {
    loadFromFile();
}

void RatingDAO::loadFromFile() {
    ratings.clear();
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string u, t, sStr, c, tp;
        getline(ss, u, '|');
        getline(ss, t, '|');
        getline(ss, sStr, '|');
        getline(ss, c, '|');
        getline(ss, tp, '|');

        int s = std::stoi(sStr);
        ratings.push_back(std::make_shared<RatingDTO>(u, t, s, c, tp));
    }
}

void RatingDAO::saveToFile() {
    std::ofstream file(filename);
    for (auto& r : ratings) {
        file << r->getUserId() << "|" << r->getTargetId() << "|" << r->getScore() << "|"
             << r->getComment() << "|" << r->getType() << "\n";
    }
}

std::vector<std::shared_ptr<RatingDTO>> RatingDAO::getAll() const {
    return ratings;
}

std::shared_ptr<RatingDTO> RatingDAO::findExisting(const std::string& userId,
                                                   const std::string& targetId,
                                                   const std::string& type) {
    for (auto& r : ratings) {
        if (r->getUserId() == userId && r->getTargetId() == targetId && r->getType() == type) {
            return r;
        }
    }
    return nullptr;
}

void RatingDAO::addRating(const std::shared_ptr<RatingDTO>& r) {
    ratings.push_back(r);
    saveToFile();
}
