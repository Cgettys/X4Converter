#include <X4ConverterTools/StdInc.h>

ComponentPartLod *ComponentPart::GetLod(int lodIndex) {
    for (ComponentPartLod &lod: Lods) {
        if (lod.LodIndex == lodIndex) {
            return &lod;
        }
    }
    return nullptr;
}
