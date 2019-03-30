//
// Created by cg on 3/13/19.
//

#pragma once

#include <boost/format.hpp>
#include <iostream>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include <X4ConverterTools/Component.h>
#include "Header.h"
#include "AnimDesc.h"

#include "../pugixml.hpp"

namespace ani {
    class AnimFile {
    public:
        AnimFile() = default;
        AnimFile(Assimp::IOStream *pStream);
        explicit AnimFile(Assimp::IOStream *pStream, const std::string& xmlPath);

        ~AnimFile();

        std::string validate(); // Debug method
        Header getHeader() const;

        void setHeader(Header header);

        void WriteAnims(pugi::xml_node tgtNode) const;


    protected:
        Header header;
        std::vector<AnimDesc> descs;

        // TODO objects plz
        std::map<std::string,std::map<std::string, std::pair<int,int> >> meta;
    };
}