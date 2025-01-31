#pragma once

#include <string>

namespace RequestParser
{
    std::string ParseContentType(std::string path);

    std::string ParseMethod(std::string req);
}