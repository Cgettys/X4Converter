#pragma once

#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamWriter.h>
#include <assimp/StreamReader.h>
#include "Header.h"
#include "AnimDesc.h"

#include <pugixml.hpp>

namespace ani {
    class AnimFile {
    public:
        AnimFile();

        AnimFile(Assimp::IOStream *pStream);

        AnimFile(pugi::xml_node node);

        ~AnimFile();

        std::string validate(); // Debug method
        Header getHeader() const;

        void setHeader(Header header);

        void WriteIntermediateRepr(const std::string &xmlPath, pugi::xml_node tgtNode) const;

        void HandleConnection(pugi::xml_node tgtNode, pugi::xml_node conn) const;

        void WriteGameFiles(Assimp::StreamWriterLE &writer, pugi::xml_node node);

    protected:
        Header header = Header();
        std::vector<AnimDesc> descs;

    };
}