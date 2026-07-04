#include <iostream>
#include <sys/cdefs.h>
#include "parser.h"

HttpRequest Parser::parse(const std::string& raw)
{
  HttpRequest req;
  std::vector<std::string> lines = splitLines(raw);

  if (lines.empty())
  {
    return req;
  }

  parseRequestLine(lines[0], req);          // insert vector of lines from splitLines(raw);

  for (size_t i = 1; i < lines.size(); i++) // I need to start at 1 because the header
  {                                         // line does not start on line 0
    parseHeaderLine(lines[i], req);
  }

  return req;
}

std::vector<std::string> Parser::splitLines(const std::string& raw)
{
  std::vector<std::string> lines;
  size_t lineStart = 0;

  for (size_t i = 0; i + 1 < raw.size(); i++) // from the start of raw vector to end, i++
  {
    if (raw[i] == '\r' && raw[i + 1] == '\n')
    {
      std::string line = raw.substr(lineStart, i - lineStart);
      
      if (line.empty())
      {
        break; 
      }

      lines.push_back(line);
      lineStart = i + 2;
      i = lineStart - 1;
    }
  }

  return lines;
}

void Parser::parseRequestLine(const std::string& line, HttpRequest& req)
{
  size_t firstSpace = 0;
  for (size_t i = 0; i < line.size(); i++)
  {
    if (line[i] == ' ')
    {
      firstSpace = i;
      break;
    }
  }

  req.method = line.substr(0, firstSpace);

  size_t secondSpace = 0;
  for (size_t i = firstSpace + 1; i < line.size(); i++)
  {
    if (line[i] == ' ')
    {
      secondSpace = i;
      break;
    }
  }

  req.path = line.substr(firstSpace + 1, secondSpace - (firstSpace + 1));
}

void Parser::parseHeaderLine(const std::string& line, HttpRequest& req)
{
  size_t colonPos = 0;
  for (size_t i = 0; i < line.size(); i++)
  {
    if (line[i] == ':')
    {
      colonPos = i;
      break;
    }
  }

  std::string key = line.substr(0, colonPos);
  std::string value = line.substr(colonPos + 2);

  req.headers[key] = value;
}

