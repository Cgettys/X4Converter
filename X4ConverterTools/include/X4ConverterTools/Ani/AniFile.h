//
// Created by cg on 3/13/19.
//

#pragma once

#include <boost/format.hpp>
#include <iostream>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

#pragma pack(push)
#pragma pack(1)
// TODO proper namespacing
// TODO is there a better way than this?
// TODO check/warn on used unknowns
struct AniHeader {
    // idx 0x00 - 0x01 at a minimum
    uint16_t numStates;;// 16 bit speculated based on assets/structures/production/PROD_GEN_HULLPARTS_DATA.ANI
    // Might actually be 32 bits
    // idx 0x02-0x03
    uint8_t padding0[2];
    //idx 0x04-0x05
    uint16_t stateDataOffset;
    //idx 0x06-0x0F
    uint8_t padding1[2];
    uint8_t usedUnknown1;// TODO figure this out; every known file has this as a 1
    uint8_t padding2[7];
};

#pragma pack(pop)
// TODO refactor out
class AniStateDesc {
public:

    virtual ~AniStateDesc();

    static AniStateDesc *ReadFromIOStream(Assimp::IOStream *pStream);
};


// TODO classes for the structs?
// TODO superclass for XMF/ANI?

class AniFile {
public:
    virtual ~AniFile();

    static AniFile *ReadFromIOStream(Assimp::IOStream *pStream);
    // TODO explicitly delete constructor nonsense?
    std::string validate(); // Debug method

private:
//    AniFile();
    AniHeader *header;
public:
    AniHeader *getHeader() const;

    void setHeader(AniHeader *header);
};