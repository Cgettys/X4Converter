#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>

// TODO check/warn on used unknowns
namespace ani {
    class Header {
    public:
        Header() = default;
        explicit Header(Assimp::StreamReaderLE &reader);

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
        int NumAnims = 0;
        int KeyOffsetBytes = 16;
        int Version = 1;
        int Padding = 0;


    };
}