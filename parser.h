#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>

struct HttpRequest
{
  std::string method;
  std::string path;
  std::map<std::string, std::string> headers;
};

class Parser 
{
  public:
    static HttpRequest parse(const std::string& raw);
    static std::vector<std::string> splitLines(const std::string& raw);
    static void parseRequestLine(const std::string& line, HttpRequest& req);
    static void parseHeaderLine(const std::string& line, HttpRequest& req);

};

#endif
