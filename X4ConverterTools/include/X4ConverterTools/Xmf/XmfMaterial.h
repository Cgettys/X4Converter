#pragma once

struct XmfMaterial
{
            XmfMaterial     ();
            XmfMaterial     ( int firstIndex, int numIndices, const std::string& name );

    int     FirstIndex;
    int     NumIndices;
    char    Name[128];
};
