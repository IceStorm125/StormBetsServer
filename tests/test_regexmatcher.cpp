#include "test_regexmatcher.h"

#include <QTest>
Test_RegexMatcher::Test_RegexMatcher()
{

}

void Test_RegexMatcher::isStringPositiveNumber()
{
    RegexMatcher matcher;
    QCOMPARE(matcher.isStringPositiveNumber("12"), true);
    QCOMPARE(matcher.isStringPositiveNumber("2432"), true);
    QCOMPARE(matcher.isStringPositiveNumber("+12"), false);
    QCOMPARE(matcher.isStringPositiveNumber("-12"), false);
    QCOMPARE(matcher.isStringPositiveNumber("012"), false);
    QCOMPARE(matcher.isStringPositiveNumber("1-2"), false);
    QCOMPARE(matcher.isStringPositiveNumber("00"), false);
    QCOMPARE(matcher.isStringPositiveNumber("0"), false);
    QCOMPARE(matcher.isStringPositiveNumber("10-"), false);
    QCOMPARE(matcher.isStringPositiveNumber("-"), false);
    QCOMPARE(matcher.isStringPositiveNumber("aefs"), false);
}
