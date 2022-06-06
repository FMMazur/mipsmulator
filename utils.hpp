#ifndef UTILS_HXX
#define UTILS_HXX

#include <string>
#include <string_view>

namespace utils
{
  static inline std::string &ltrim(std::string &str)
  {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [&](unsigned char ch)
                                        { return !std::isspace(ch); }));

    return str;
  }

  // trim from end (in place)
  static inline std::string &rtrim(std::string &str)
  {
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch)
                           { return !std::isspace(ch); })
                  .base(),
              str.end());

    return str;
  }

  // trim from both ends (in place)
  static inline std::string trim(std::string &str)
  {
    ltrim(str);
    rtrim(str);

    return str;
  }

  static inline std::string &replace(std::string &str, std::string replace, char to)
  {
    std::replace_if(
        str.begin(), str.end(), [&](unsigned char ch)
        { return replace.find(ch) != std::string::npos; },
        to);

    return str;
  }
} // namespace utils

#endif // UTILS_HXX