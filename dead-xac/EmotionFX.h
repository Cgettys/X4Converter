#pragma once

enum EmoFxChunkIds
{
    EMOFX_CHUNK_XAC_MESH                  = 1,
    EMOFX_CHUNK_XAC_BONE_INFLUENCES       = 2,
    EMOFX_CHUNK_XAC_MATERIAL_DEFINITION   = 3,
    EMOFX_CHUNK_XAC_METADATA              = 7,
    EMOFX_CHUNK_XAC_NODE_HIERARCHY        = 0xB,
    EMOFX_CHUNK_XAC_MORPH_TARGETS         = 0xC,
    EMOFX_CHUNK_XAC_MATERIAL_TOTALS       = 0xD,
    EMOFX_MAX_CHUNK
};

enum EmoFxVertexElemUsage
{
    EMOFX_DECLUSAGE_POSITION,
    EMOFX_DECLUSAGE_NORMAL,
    EMOFX_DECLUSAGE_TANGENT,
    EMOFX_DECLUSAGE_TEXCOORD,
    EMOFX_DECLUSAGE_COLOR32,
    EMOFX_DECLUSAGE_INFLUENCERANGEIDX,
    EMOFX_DECLUSAGE_COLOR128
};

enum EmoFxPhonemes
{
    EMOFX_PHONEME_NEUTRAL                           = 0x001,
    EMOFX_PHONEME_M_B_P_X                           = 0x002,
    EMOFX_PHONEME_AA_AO_OW                          = 0x004,
    EMOFX_PHONEME_IH_AE_AH_EY_AY_H                  = 0x008,
    EMOFX_PHONEME_AW                                = 0x010,
    EMOFX_PHONEME_N_NG_CH_J_DH_D_G_T_K_Z_ZH_TH_S_SH = 0x020,
    EMOFX_PHONEME_IY_EH_Y                           = 0x040,
    EMOFX_PHONEME_UW_UH_OY                          = 0x080,
    EMOFX_PHONEME_F_V                               = 0x100,
    EMOFX_PHONEME_L_EL                              = 0x200,
    EMOFX_PHONEME_W                                 = 0x400,
    EMOFX_PHONEME_R_ER                              = 0x800
};
