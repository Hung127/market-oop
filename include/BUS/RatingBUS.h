#ifndef RATING_BUS_H
#define RATING_BUS_H

#include "../DAO/RatingDAO.h"

class RatingBUS {
   private:
    RatingDAO dao;

   public:
    RatingBUS() : dao("ratings.txt") {}

    bool rate(const std::string& userId, const std::string& targetId, int score,
              const std::string& comment, const std::string& type);

    double avgRating(const std::string& targetId, const std::string& type);
};

#endif
