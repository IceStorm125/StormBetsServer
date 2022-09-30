#ifndef TEST_REGEXMATCHER_H
#define TEST_REGEXMATCHER_H
#include <QObject>

#include "regexmatcher.h"

class Test_RegexMatcher : public QObject
{
    Q_OBJECT
public:
    explicit Test_RegexMatcher();

private slots:
    void isStringPositiveNumber();

};

#endif // TEST_REGEXMATCHER_H
