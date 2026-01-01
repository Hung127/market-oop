#include <gtest/gtest.h>

#include "TestsCommon.h"

// Test SearchHelper editDistance
TEST(SearchHelper_EditDistance, ExactMatch) {
    int dist = Utils::SearchHelper::editDistance("hello", "hello");
    EXPECT_EQ(dist, 0);
}

TEST(SearchHelper_EditDistance, OneInsertion) {
    int dist = Utils::SearchHelper::editDistance("hello", "helllo");
    EXPECT_EQ(dist, 1);
}

TEST(SearchHelper_EditDistance, OneDeletion) {
    int dist = Utils::SearchHelper::editDistance("hello", "hllo");
    EXPECT_EQ(dist, 1);
}

TEST(SearchHelper_EditDistance, OneSubstitution) {
    int dist = Utils::SearchHelper::editDistance("hello", "hallo");
    EXPECT_EQ(dist, 1);
}

TEST(SearchHelper_EditDistance, CompletelyDifferent) {
    int dist = Utils::SearchHelper::editDistance("abc", "xyz");
    EXPECT_EQ(dist, 3);
}

TEST(SearchHelper_EditDistance, EmptyStrings) {
    EXPECT_EQ(Utils::SearchHelper::editDistance("", ""), 0);
    EXPECT_EQ(Utils::SearchHelper::editDistance("hello", ""), 5);
    EXPECT_EQ(Utils::SearchHelper::editDistance("", "world"), 5);
}

// Test SearchHelper toLower
TEST(SearchHelper_ToLower, MixedCase) {
    EXPECT_EQ(Utils::SearchHelper::toLower("HeLLo WoRLd"), "hello world");
}

TEST(SearchHelper_ToLower, AllUpper) {
    EXPECT_EQ(Utils::SearchHelper::toLower("ALLCAPS"), "allcaps");
}

TEST(SearchHelper_ToLower, AllLower) {
    EXPECT_EQ(Utils::SearchHelper::toLower("alllower"), "alllower");
}

TEST(SearchHelper_ToLower, EmptyString) {
    EXPECT_EQ(Utils::SearchHelper::toLower(""), "");
}

TEST(SearchHelper_ToLower, SpecialCharacters) {
    EXPECT_EQ(Utils::SearchHelper::toLower("Test123!@#"), "test123!@#");
}

// Test SearchHelper containsIgnoreCase
TEST(SearchHelper_Contains, ExactMatch) {
    EXPECT_TRUE(Utils::SearchHelper::containsIgnoreCase("hello world", "world"));
}

TEST(SearchHelper_Contains, CaseInsensitive) {
    EXPECT_TRUE(Utils::SearchHelper::containsIgnoreCase("Hello World", "WORLD"));
    EXPECT_TRUE(Utils::SearchHelper::containsIgnoreCase("HELLO", "hello"));
}

TEST(SearchHelper_Contains, NotFound) {
    EXPECT_FALSE(Utils::SearchHelper::containsIgnoreCase("hello world", "xyz"));
}

TEST(SearchHelper_Contains, EmptyKeyword) {
    EXPECT_TRUE(Utils::SearchHelper::containsIgnoreCase("anything", ""));
}

TEST(SearchHelper_Contains, EmptyText) {
    EXPECT_FALSE(Utils::SearchHelper::containsIgnoreCase("", "keyword"));
}

// Test SearchHelper equalsIgnoreCase
TEST(SearchHelper_Equals, ExactMatch) {
    EXPECT_TRUE(Utils::SearchHelper::equalsIgnoreCase("test", "test"));
}

TEST(SearchHelper_Equals, CaseInsensitive) {
    EXPECT_TRUE(Utils::SearchHelper::equalsIgnoreCase("Test", "TEST"));
    EXPECT_TRUE(Utils::SearchHelper::equalsIgnoreCase("TeSt", "tEsT"));
}

TEST(SearchHelper_Equals, NotEqual) {
    EXPECT_FALSE(Utils::SearchHelper::equalsIgnoreCase("test", "testing"));
}

TEST(SearchHelper_Equals, EmptyStrings) {
    EXPECT_TRUE(Utils::SearchHelper::equalsIgnoreCase("", ""));
}

// Test SearchHelper similarityScore
TEST(SearchHelper_Similarity, IdenticalStrings) {
    double score = Utils::SearchHelper::similarityScore("test", "test");
    EXPECT_DOUBLE_EQ(score, 1.0);
}

TEST(SearchHelper_Similarity, CompletelyDifferent) {
    double score = Utils::SearchHelper::similarityScore("abc", "xyz");
    EXPECT_LT(score, 0.5);
}