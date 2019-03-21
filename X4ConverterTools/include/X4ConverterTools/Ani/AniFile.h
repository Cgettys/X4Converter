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
#include "AniKeyFrameDesc.h"

// TODO classes for the structs?
// TODO superclass for XMF/ANI?

class AniFile {
public:

    explicit AniFile(Assimp::IOStream* pStream);
    ~AniFile();
    // TODO explicitly delete constructor nonsense?
    std::string validate(); // Debug method, callee frees
    AniHeader getHeader() const;
    void setHeader(AniHeader header);
protected:
    AniHeader header;
    std::vector<AniKeyFrameDesc> descs;
};