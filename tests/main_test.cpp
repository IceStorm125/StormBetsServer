#include "regexmatcher.h"

#include <gtest/gtest.h>

TEST(RegexMatcherTest, PositiveIntegerString)
    {
        EXPECT_TRUE(RegexMatcher::isStringPositiveNumber("123"));
    }

    TEST(RegexMatcherTest, NegativeIntegerString)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("-123"));
    }

    TEST(RegexMatcherTest, StringWithLeadingZeros)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("00123"));
    }

    TEST(RegexMatcherTest, EmptyString)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber(""));
    }

    TEST(RegexMatcherTest, StringWithLetters)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("123abc"));
    }
    
    TEST(RegexMatcherTest, StringWithSpaces)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("    "));
    }

    TEST(RegexMatcherTest, StringWithDots)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber(".123"));
    }

    TEST(RegexMatcherTest, StringStartsWithDot)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber(".1234"));
    }

    TEST(RegexMatcherTest, StringStartsWithMinusAndDot)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("-.1234"));
    }

    TEST(RegexMatcherTest, StringStartsWithMinusAndDot2)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("-0.1234"));
    }

    TEST(RegexMatcherTest, StringWithDash)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("123-456"));
    }

    TEST(RegexMatcherTest, StringWithPlus)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("123+456"));
    }

    TEST(RegexMatcherTest, StringWithPlusAndLeadingSpace)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber(" 123+456"));
    }

    TEST(RegexMatcherTest, StringWithLeadingLetter)
    {
        EXPECT_FALSE(RegexMatcher::isStringPositiveNumber("a123"));
    }


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
   
    return RUN_ALL_TESTS();
}