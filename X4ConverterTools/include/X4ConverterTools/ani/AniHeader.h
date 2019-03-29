
#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>

// TODO proper namespacing
// TODO check/warn on used unknowns
class AniHeader {
public:
    AniHeader();
    explicit AniHeader(Assimp::StreamReaderLE& reader);

    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
protected:
public:
    int getNumAnims() const;

    void setNumAnims(int NumAnims);

    int getKeyOffsetBytes() const;

    void setKeyOffsetBytes(int KeyOffsetBytes);

    int getVersion() const;

    void setVersion(int Version);

    int getPadding() const;

    void setPadding(int Padding);

protected:
    int NumAnims;
    int KeyOffsetBytes;
    int Version;
    int Padding;


};
