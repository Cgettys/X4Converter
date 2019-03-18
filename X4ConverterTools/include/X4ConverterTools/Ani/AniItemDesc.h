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
    // 34 bytes, at least 0-3 strings?
    std::vector<std::string> words0;
    std::vector<std::string> words1;

    //    char words0[64];
//    char words1[64];
    uint8_t padding0[32];
};
