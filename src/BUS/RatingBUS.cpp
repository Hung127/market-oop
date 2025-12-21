#include "BUS/RatingBUS.h"

bool RatingBUS::rate(const std::string& userId, const std::string& targetId, int score,
                     const std::string& comment, const std::string& type) {
    if (score < 1 || score > 5)
        return false;
    auto existed = dao.findExisting(userId, targetId, type);
    if (existed) {
        existed->setScore(score);
        existed->setComment(comment);
        dao.saveToFile();
        return true;
    }
    dao.addRating(std::make_shared<RatingDTO>(userId, targetId, score, comment, type));
    return true;
}

double RatingBUS::avgRating(const std::string& targetId, const std::string& type) {
    auto all = dao.getAll();
    double sum = 0;
    int cnt = 0;
    for (auto& r : all) {
        if (r->getTargetId() == targetId && r->getType() == type) {
            sum += r->getScore();
            cnt++;
        }
    }
    return cnt == 0 ? 0 : sum / cnt;
}
