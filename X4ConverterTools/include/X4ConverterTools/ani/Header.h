#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <boost/format.hpp>
#include <iostream>
#include <pugixml.hpp>

namespace ani {
class Header {
 public:
  Header();

  explicit Header(Assimp::StreamReaderLE &reader);

  void WriteGameFiles(Assimp::StreamWriterLE &writer);

  std::string validate();// Debug method - throws exception if invalid, else returns human readable string
 protected:
 public:
  [[nodiscard]] int getNumAnims() const;

  void setNumAnims(int NumAnims);

  [[nodiscard]] int getKeyOffsetBytes() const;

  [[nodiscard]] int getVersion() const;

  void setVersion(int Version);

  [[nodiscard]] int getPadding() const;

  void setPadding(int padding);

 protected:
  int NumAnims = 0;
  int KeyOffsetBytes = 16;
  int Version = 1;
  int Padding = 0;

};
}