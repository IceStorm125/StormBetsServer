#include "curlClient.h"

#include <memory>

CURLClient::CURLClient()
{

}

CURLClient::~CURLClient()
{

}

namespace
{
std::size_t callback(
        const char *in,
        std::size_t size,
        std::size_t num,
        std::string *out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}
}

int CURLClient::getJsonDataFromURL(const std::string &URL, std::string &outResponce)
{
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_perform(curl);
    outResponce = *httpData;

    long responceCode{0};
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responceCode);
    curl_easy_cleanup(curl);

    return responceCode;
}


