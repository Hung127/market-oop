#ifndef RATING_DTO_H
#define RATING_DTO_H

#include <string>

class RatingDTO {
   private:
    std::string userId;
    std::string targetId;
    int score;
    std::string comment;
    std::string type;

   public:
    RatingDTO(const std::string& u, const std::string& t, int s, const std::string& c,
              const std::string& tp)
        : userId(u), targetId(t), score(s), comment(c), type(tp) {}

    std::string getUserId() const {
        return userId;
    }
    std::string getTargetId() const {
        return targetId;
    }
    int getScore() const {
        return score;
    }
    std::string getComment() const {
        return comment;
    }
    std::string getType() const {
        return type;
    }

    void setScore(int s) {
        score = s;
    }
    void setComment(const std::string& c) {
        comment = c;
    }
};

#endif
