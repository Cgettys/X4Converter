#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>

// TODO better name
class AniItemDesc {
public:
    AniItemDesc() = default;

    explicit AniItemDesc(Assimp::StreamReader<> &reader);

    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
protected:
    uint8_t padding0[160];
};
