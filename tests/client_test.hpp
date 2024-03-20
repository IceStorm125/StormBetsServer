#include <gtest/gtest.h>
#include "boostClient.h"
#include "curlClient.h"

#include <memory>

enum HttpCode
{
    SUCCES = 200,
    NOT_FOUND = 404
};

TEST(BoostClientTest, TestSuccessfulHttpRequest)
{
    std::unique_ptr<Client> client = std::make_unique<BoostClient>();
    std::string response;
    int result = client->getJsonDataFromURL("https://api.rivalry.com/api/v1/tournaments/", response);

    EXPECT_EQ(result, HttpCode::SUCCES);
}

TEST(BoostClientTest, TestFailedHttpRequest)
{
    std::unique_ptr<Client> client = std::make_unique<BoostClient>();
    std::string response;
    int result = client->getJsonDataFromURL("https://api.rivalry.com/api/v1/badword/", response);

    EXPECT_EQ(result, HttpCode::NOT_FOUND);
}

TEST(CURLClientTest, TestSuccessfulHttpRequest)
{
    std::unique_ptr<Client> client = std::make_unique<CURLClient>();
    std::string response;
    int result = client->getJsonDataFromURL("https://api.rivalry.com/api/v1/tournaments/", response);

    EXPECT_EQ(result, HttpCode::SUCCES);
}

TEST(CURLClientTest, TestFailedHttpRequest)
{
    std::unique_ptr<Client> client = std::make_unique<CURLClient>();
    std::string response;
    int result = client->getJsonDataFromURL("https://api.rivalry.com/api/v1/badword/", response);

    EXPECT_EQ(result, HttpCode::NOT_FOUND);
}

