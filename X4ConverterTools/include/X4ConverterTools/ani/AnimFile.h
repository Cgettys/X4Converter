//
// Created by cg on 3/13/19.
//

#pragma once

#include <boost/format.hpp>
#include <iostream>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include "Header.h"
#include "AnimDesc.h"

#include "../pugixml.hpp"

namespace ani {
    class AnimFile {
    public:
        AnimFile() = default;
        explicit AnimFile(Assimp::IOStream *pStream);

        ~AnimFile();

        std::string validate(); // Debug method
        Header getHeader() const;

        void setHeader(Header header);

        void WriteAnims(pugi::xml_node tgtNode) const;

    protected:
        Header header;
        std::vector<AnimDesc> descs;
    };
}