// File: IStringParser.h
// Martin Fracker
// CSCE 463-500 Spring 2017

#pragma once

#include "ParseResultBase.h"
#include <string>

class IStringParser
{
public:
  virtual ~IStringParser() = default;

  virtual ParseResultBase* Parse(std::string) = 0;
};
