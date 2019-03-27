//
// Created by cg on 3/13/19.
//

#pragma once

#include <boost/format.hpp>
#include <iostream>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include "AniHeader.h"
#include "AniAnimDesc.h"

#include "../pugixml.hpp"
// TODO superclass for XMF/ANI?

class AniFile {
public:

    explicit AniFile(Assimp::IOStream* pStream);
    ~AniFile();

    std::string validate(); // Debug method
    AniHeader getHeader() const;
    void setHeader(AniHeader header);
    void WriteAnims(pugi::xml_node tgtNode) const;
protected:
    AniHeader header;
    std::vector<AniAnimDesc> descs;
};