#ifndef RATING_DAO_H
#define RATING_DAO_H

#include <memory>
#include <string>
#include <vector>

#include "../DTO/RatingDTO.h"

class RatingDAO {
   private:
    std::vector<std::shared_ptr<RatingDTO>> ratings;
    std::string filename;

   public:
    RatingDAO(const std::string& file = "ratings.txt");
    void loadFromFile();
    void saveToFile();

    std::vector<std::shared_ptr<RatingDTO>> getAll() const;
    std::shared_ptr<RatingDTO> findExisting(const std::string& userId, const std::string& targetId,
                                            const std::string& type);

    void addRating(const std::shared_ptr<RatingDTO>& r);
};

#endif
