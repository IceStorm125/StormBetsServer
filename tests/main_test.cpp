#include <gtest/gtest.h>

#include "regex_test.hpp"
#include "client_test.hpp"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
   
    return RUN_ALL_TESTS();
}