
#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>

// TODO proper namespacing
// TODO destroy constructor
// TODO is there a better way than this?
// TODO check/warn on used unknowns
class AniHeader {
public:
    AniHeader();
    explicit AniHeader(Assimp::StreamReader<>& reader);

    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
protected:
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
public:
    uint16_t getNumStates() const;

    void setNumStates(uint16_t numStates);

    uint16_t getStateDataOffset() const;

    void setStateDataOffset(uint16_t stateDataOffset);

    uint8_t getUsedUnknown1() const;

    void setUsedUnknown1(uint8_t usedUnknown1);

};
