#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

namespace xmf {
// TODO better name
class IXmf {
  virtual void ReadBinary(Assimp::StreamReaderLE &reader) = 0;

  virtual void WriteBinary(Assimp::StreamWriterLE &reader) const = 0;
};
}