// File: ParseResultBase.h
// Martin Fracker
// CSCE 463-500 Spring 2017
#pragma once

class ParseResultBase
{
public:
  bool Success = false;

  virtual ~ParseResultBase() = default;
};
