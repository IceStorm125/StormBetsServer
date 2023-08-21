#include "requester.h"
#include <iostream>
#include <memory>

#include <QDebug>

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


Requester::Requester()
{

}



