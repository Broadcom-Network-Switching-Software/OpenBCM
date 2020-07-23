
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 
#include <shared/utilex/utilex_framework.h>
#include <bcm/oam.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>









static void
dnx_oam_piecewise_reverse_buffer(
    const uint8 *buffer,
    uint8 *target_buffer,
    uint32 start_index,
    uint32 data_length)
{
    int index, target_index;
    for (index = start_index; index < start_index + data_length; index++)
    {
        target_index = 15 + index - 2 * (index & 0xF);
        target_buffer[target_index] = buffer[index - start_index];
    }
}

int
dnx_oam_oamp_crc16(
    int unit,
    const uint8 *buff,
    uint32 start_index,
    uint32 data_length,
    uint16 *calculated_crc)
{
    uint8 new_crc[16], data[OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION * UTILEX_NOF_BITS_IN_BYTE], crc[16];
    uint8 *buffer = NULL;
    uint16 return_val;
    int bit, byte_iteration, cur_byte, byte, byte_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(buffer, OAM_OAMP_CRC_BUFFER_SIZE, "Buffer to use as input for CRC calculations",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    dnx_oam_piecewise_reverse_buffer(buff, buffer, start_index, data_length);

    
    for (bit = 0; bit < 16; bit++)
    {
        crc[bit] = 0x1;
    }

    for (byte_iteration = 0; byte_iteration < (OAM_OAMP_CRC_BUFFER_SIZE / OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION);
         byte_iteration++)
    {

        
        for (cur_byte = 0; cur_byte < OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION; cur_byte++)
        {
            byte = cur_byte + byte_iteration * OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION;
            byte_val = buffer[byte];

            for (bit = 0; bit < UTILEX_NOF_BITS_IN_BYTE; bit++)
            {
                
                data[cur_byte * UTILEX_NOF_BITS_IN_BYTE + bit] = (byte_val >> bit) & 0x1;
            }
        }

        
        new_crc[0] = crc[1] ^ crc[11] ^ crc[15] ^ crc[3] ^ crc[6] ^
            crc[9] ^ data[0] ^ data[100] ^ data[102] ^ data[104] ^
            data[111] ^ data[112] ^ data[114] ^ data[120] ^ data[122] ^
            data[123] ^ data[124] ^ data[125] ^ data[126] ^ data[16] ^
            data[20] ^ data[24] ^ data[27] ^ data[28] ^ data[35] ^
            data[36] ^ data[38] ^ data[42] ^ data[43] ^ data[44] ^
            data[48] ^ data[49] ^ data[51] ^ data[58] ^ data[64] ^
            data[65] ^ data[67] ^ data[68] ^ data[71] ^ data[72] ^
            data[74] ^ data[79] ^ data[80] ^ data[81] ^ data[82] ^
            data[83] ^ data[86] ^ data[88] ^ data[90] ^ data[91] ^ data[93] ^ data[96] ^ data[97] ^ data[98];
        new_crc[1] = crc[10] ^ crc[12] ^ crc[2] ^ crc[4] ^ crc[7] ^
            data[1] ^ data[101] ^ data[103] ^ data[105] ^ data[112] ^
            data[113] ^ data[115] ^ data[121] ^ data[123] ^ data[124] ^
            data[125] ^ data[126] ^ data[127] ^ data[17] ^ data[21] ^
            data[25] ^ data[28] ^ data[29] ^ data[36] ^ data[37] ^
            data[39] ^ data[43] ^ data[44] ^ data[45] ^ data[49] ^
            data[50] ^ data[52] ^ data[59] ^ data[65] ^ data[66] ^
            data[68] ^ data[69] ^ data[72] ^ data[73] ^ data[75] ^
            data[80] ^ data[81] ^ data[82] ^ data[83] ^ data[84] ^
            data[87] ^ data[89] ^ data[91] ^ data[92] ^ data[94] ^ data[97] ^ data[98] ^ data[99];
        new_crc[2] = crc[0] ^ crc[11] ^ crc[13] ^ crc[3] ^ crc[5] ^
            crc[8] ^ data[100] ^ data[102] ^ data[104] ^ data[106] ^
            data[113] ^ data[114] ^ data[116] ^ data[122] ^ data[124] ^
            data[125] ^ data[126] ^ data[127] ^ data[18] ^ data[2] ^
            data[22] ^ data[26] ^ data[29] ^ data[30] ^ data[37] ^
            data[38] ^ data[40] ^ data[44] ^ data[45] ^ data[46] ^
            data[50] ^ data[51] ^ data[53] ^ data[60] ^ data[66] ^
            data[67] ^ data[69] ^ data[70] ^ data[73] ^ data[74] ^
            data[76] ^ data[81] ^ data[82] ^ data[83] ^ data[84] ^
            data[85] ^ data[88] ^ data[90] ^ data[92] ^ data[93] ^ data[95] ^ data[98] ^ data[99];
        new_crc[3] = crc[0] ^ crc[1] ^ crc[12] ^ crc[14] ^ crc[4] ^
            crc[6] ^ crc[9] ^ data[100] ^ data[101] ^ data[103] ^
            data[105] ^ data[107] ^ data[114] ^ data[115] ^ data[117] ^
            data[123] ^ data[125] ^ data[126] ^ data[127] ^ data[19] ^
            data[23] ^ data[27] ^ data[3] ^ data[30] ^ data[31] ^
            data[38] ^ data[39] ^ data[41] ^ data[45] ^ data[46] ^
            data[47] ^ data[51] ^ data[52] ^ data[54] ^ data[61] ^
            data[67] ^ data[68] ^ data[70] ^ data[71] ^ data[74] ^
            data[75] ^ data[77] ^ data[82] ^ data[83] ^ data[84] ^
            data[85] ^ data[86] ^ data[89] ^ data[91] ^ data[93] ^ data[94] ^ data[96] ^ data[99];
        new_crc[4] = crc[0] ^ crc[1] ^ crc[10] ^ crc[13] ^ crc[15] ^
            crc[2] ^ crc[5] ^ crc[7] ^ data[100] ^ data[101] ^
            data[102] ^ data[104] ^ data[106] ^ data[108] ^ data[115] ^
            data[116] ^ data[118] ^ data[124] ^ data[126] ^ data[127] ^
            data[20] ^ data[24] ^ data[28] ^ data[31] ^ data[32] ^
            data[39] ^ data[4] ^ data[40] ^ data[42] ^ data[46] ^
            data[47] ^ data[48] ^ data[52] ^ data[53] ^ data[55] ^
            data[62] ^ data[68] ^ data[69] ^ data[71] ^ data[72] ^
            data[75] ^ data[76] ^ data[78] ^ data[83] ^ data[84] ^
            data[85] ^ data[86] ^ data[87] ^ data[90] ^ data[92] ^ data[94] ^ data[95] ^ data[97];
        new_crc[5] = crc[0] ^ crc[14] ^ crc[15] ^ crc[2] ^ crc[8] ^
            crc[9] ^ data[100] ^ data[101] ^ data[103] ^ data[104] ^
            data[105] ^ data[107] ^ data[109] ^ data[111] ^ data[112] ^
            data[114] ^ data[116] ^ data[117] ^ data[119] ^ data[120] ^
            data[122] ^ data[123] ^ data[124] ^ data[126] ^ data[127] ^
            data[16] ^ data[20] ^ data[21] ^ data[24] ^ data[25] ^
            data[27] ^ data[28] ^ data[29] ^ data[32] ^ data[33] ^
            data[35] ^ data[36] ^ data[38] ^ data[40] ^ data[41] ^
            data[42] ^ data[44] ^ data[47] ^ data[5] ^ data[51] ^
            data[53] ^ data[54] ^ data[56] ^ data[58] ^ data[63] ^
            data[64] ^ data[65] ^ data[67] ^ data[68] ^ data[69] ^
            data[70] ^ data[71] ^ data[73] ^ data[74] ^ data[76] ^
            data[77] ^ data[80] ^ data[81] ^ data[82] ^ data[83] ^
            data[84] ^ data[85] ^ data[87] ^ data[90] ^ data[95] ^ data[97];
        new_crc[6] = crc[0] ^ crc[1] ^ crc[10] ^ crc[15] ^ crc[3] ^
            crc[9] ^ data[101] ^ data[102] ^ data[104] ^ data[105] ^
            data[106] ^ data[108] ^ data[110] ^ data[112] ^ data[113] ^
            data[115] ^ data[117] ^ data[118] ^ data[120] ^ data[121] ^
            data[123] ^ data[124] ^ data[125] ^ data[127] ^ data[17] ^
            data[21] ^ data[22] ^ data[25] ^ data[26] ^ data[28] ^
            data[29] ^ data[30] ^ data[33] ^ data[34] ^ data[36] ^
            data[37] ^ data[39] ^ data[41] ^ data[42] ^ data[43] ^
            data[45] ^ data[48] ^ data[52] ^ data[54] ^ data[55] ^
            data[57] ^ data[59] ^ data[6] ^ data[64] ^ data[65] ^
            data[66] ^ data[68] ^ data[69] ^ data[70] ^ data[71] ^
            data[72] ^ data[74] ^ data[75] ^ data[77] ^ data[78] ^
            data[81] ^ data[82] ^ data[83] ^ data[84] ^ data[85] ^ data[86] ^ data[88] ^ data[91] ^ data[96] ^ data[98];
        new_crc[7] = crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[2] ^
            crc[4] ^ data[102] ^ data[103] ^ data[105] ^ data[106] ^
            data[107] ^ data[109] ^ data[111] ^ data[113] ^ data[114] ^
            data[116] ^ data[118] ^ data[119] ^ data[121] ^ data[122] ^
            data[124] ^ data[125] ^ data[126] ^ data[18] ^ data[22] ^
            data[23] ^ data[26] ^ data[27] ^ data[29] ^ data[30] ^
            data[31] ^ data[34] ^ data[35] ^ data[37] ^ data[38] ^
            data[40] ^ data[42] ^ data[43] ^ data[44] ^ data[46] ^
            data[49] ^ data[53] ^ data[55] ^ data[56] ^ data[58] ^
            data[60] ^ data[65] ^ data[66] ^ data[67] ^ data[69] ^
            data[7] ^ data[70] ^ data[71] ^ data[72] ^ data[73] ^
            data[75] ^ data[76] ^ data[78] ^ data[79] ^ data[82] ^
            data[83] ^ data[84] ^ data[85] ^ data[86] ^ data[87] ^ data[89] ^ data[92] ^ data[97] ^ data[99];
        new_crc[8] = crc[1] ^ crc[11] ^ crc[12] ^ crc[2] ^ crc[3] ^
            crc[5] ^ data[100] ^ data[103] ^ data[104] ^ data[106] ^
            data[107] ^ data[108] ^ data[110] ^ data[112] ^ data[114] ^
            data[115] ^ data[117] ^ data[119] ^ data[120] ^ data[122] ^
            data[123] ^ data[125] ^ data[126] ^ data[127] ^ data[19] ^
            data[23] ^ data[24] ^ data[27] ^ data[28] ^ data[30] ^
            data[31] ^ data[32] ^ data[35] ^ data[36] ^ data[38] ^
            data[39] ^ data[41] ^ data[43] ^ data[44] ^ data[45] ^
            data[47] ^ data[50] ^ data[54] ^ data[56] ^ data[57] ^
            data[59] ^ data[61] ^ data[66] ^ data[67] ^ data[68] ^
            data[70] ^ data[71] ^ data[72] ^ data[73] ^ data[74] ^
            data[76] ^ data[77] ^ data[79] ^ data[8] ^ data[80] ^
            data[83] ^ data[84] ^ data[85] ^ data[86] ^ data[87] ^ data[88] ^ data[90] ^ data[93] ^ data[98];
        new_crc[9] = crc[0] ^ crc[12] ^ crc[13] ^ crc[2] ^ crc[3] ^
            crc[4] ^ crc[6] ^ data[101] ^ data[104] ^ data[105] ^
            data[107] ^ data[108] ^ data[109] ^ data[111] ^ data[113] ^
            data[115] ^ data[116] ^ data[118] ^ data[120] ^ data[121] ^
            data[123] ^ data[124] ^ data[126] ^ data[127] ^ data[20] ^
            data[24] ^ data[25] ^ data[28] ^ data[29] ^ data[31] ^
            data[32] ^ data[33] ^ data[36] ^ data[37] ^ data[39] ^
            data[40] ^ data[42] ^ data[44] ^ data[45] ^ data[46] ^
            data[48] ^ data[51] ^ data[55] ^ data[57] ^ data[58] ^
            data[60] ^ data[62] ^ data[67] ^ data[68] ^ data[69] ^
            data[71] ^ data[72] ^ data[73] ^ data[74] ^ data[75] ^
            data[77] ^ data[78] ^ data[80] ^ data[81] ^ data[84] ^
            data[85] ^ data[86] ^ data[87] ^ data[88] ^ data[89] ^ data[9] ^ data[91] ^ data[94] ^ data[99];
        new_crc[10] = crc[0] ^ crc[1] ^ crc[13] ^ crc[14] ^ crc[3] ^
            crc[4] ^ crc[5] ^ crc[7] ^ data[10] ^ data[100] ^
            data[102] ^ data[105] ^ data[106] ^ data[108] ^ data[109] ^
            data[110] ^ data[112] ^ data[114] ^ data[116] ^ data[117] ^
            data[119] ^ data[121] ^ data[122] ^ data[124] ^ data[125] ^
            data[127] ^ data[21] ^ data[25] ^ data[26] ^ data[29] ^
            data[30] ^ data[32] ^ data[33] ^ data[34] ^ data[37] ^
            data[38] ^ data[40] ^ data[41] ^ data[43] ^ data[45] ^
            data[46] ^ data[47] ^ data[49] ^ data[52] ^ data[56] ^
            data[58] ^ data[59] ^ data[61] ^ data[63] ^ data[68] ^
            data[69] ^ data[70] ^ data[72] ^ data[73] ^ data[74] ^
            data[75] ^ data[76] ^ data[78] ^ data[79] ^ data[81] ^
            data[82] ^ data[85] ^ data[86] ^ data[87] ^ data[88] ^ data[89] ^ data[90] ^ data[92] ^ data[95];
        new_crc[11] = crc[0] ^ crc[1] ^ crc[14] ^ crc[15] ^ crc[2] ^
            crc[4] ^ crc[5] ^ crc[6] ^ crc[8] ^ data[101] ^
            data[103] ^ data[106] ^ data[107] ^ data[109] ^ data[11] ^
            data[110] ^ data[111] ^ data[113] ^ data[115] ^ data[117] ^
            data[118] ^ data[120] ^ data[122] ^ data[123] ^ data[125] ^
            data[126] ^ data[22] ^ data[26] ^ data[27] ^ data[30] ^
            data[31] ^ data[33] ^ data[34] ^ data[35] ^ data[38] ^
            data[39] ^ data[41] ^ data[42] ^ data[44] ^ data[46] ^
            data[47] ^ data[48] ^ data[50] ^ data[53] ^ data[57] ^
            data[59] ^ data[60] ^ data[62] ^ data[64] ^ data[69] ^
            data[70] ^ data[71] ^ data[73] ^ data[74] ^ data[75] ^
            data[76] ^ data[77] ^ data[79] ^ data[80] ^ data[82] ^
            data[83] ^ data[86] ^ data[87] ^ data[88] ^ data[89] ^ data[90] ^ data[91] ^ data[93] ^ data[96];
        new_crc[12] = crc[11] ^ crc[2] ^ crc[5] ^ crc[7] ^ data[100] ^
            data[107] ^ data[108] ^ data[110] ^ data[116] ^ data[118] ^
            data[119] ^ data[12] ^ data[120] ^ data[121] ^ data[122] ^
            data[125] ^ data[127] ^ data[16] ^ data[20] ^ data[23] ^
            data[24] ^ data[31] ^ data[32] ^ data[34] ^ data[38] ^
            data[39] ^ data[40] ^ data[44] ^ data[45] ^ data[47] ^
            data[54] ^ data[60] ^ data[61] ^ data[63] ^ data[64] ^
            data[67] ^ data[68] ^ data[70] ^ data[75] ^ data[76] ^
            data[77] ^ data[78] ^ data[79] ^ data[82] ^ data[84] ^
            data[86] ^ data[87] ^ data[89] ^ data[92] ^ data[93] ^ data[94] ^ data[96] ^ data[98];
        new_crc[13] = crc[0] ^ crc[12] ^ crc[3] ^ crc[6] ^ crc[8] ^
            data[101] ^ data[108] ^ data[109] ^ data[111] ^ data[117] ^
            data[119] ^ data[120] ^ data[121] ^ data[122] ^ data[123] ^
            data[126] ^ data[13] ^ data[17] ^ data[21] ^ data[24] ^
            data[25] ^ data[32] ^ data[33] ^ data[35] ^ data[39] ^
            data[40] ^ data[41] ^ data[45] ^ data[46] ^ data[48] ^
            data[55] ^ data[61] ^ data[62] ^ data[64] ^ data[65] ^
            data[68] ^ data[69] ^ data[71] ^ data[76] ^ data[77] ^
            data[78] ^ data[79] ^ data[80] ^ data[83] ^ data[85] ^
            data[87] ^ data[88] ^ data[90] ^ data[93] ^ data[94] ^ data[95] ^ data[97] ^ data[99];
        new_crc[14] = crc[1] ^ crc[13] ^ crc[4] ^ crc[7] ^ crc[9] ^
            data[100] ^ data[102] ^ data[109] ^ data[110] ^ data[112] ^
            data[118] ^ data[120] ^ data[121] ^ data[122] ^ data[123] ^
            data[124] ^ data[127] ^ data[14] ^ data[18] ^ data[22] ^
            data[25] ^ data[26] ^ data[33] ^ data[34] ^ data[36] ^
            data[40] ^ data[41] ^ data[42] ^ data[46] ^ data[47] ^
            data[49] ^ data[56] ^ data[62] ^ data[63] ^ data[65] ^
            data[66] ^ data[69] ^ data[70] ^ data[72] ^ data[77] ^
            data[78] ^ data[79] ^ data[80] ^ data[81] ^ data[84] ^
            data[86] ^ data[88] ^ data[89] ^ data[91] ^ data[94] ^ data[95] ^ data[96] ^ data[98];
        new_crc[15] = crc[0] ^ crc[10] ^ crc[14] ^ crc[2] ^ crc[5] ^
            crc[8] ^ data[101] ^ data[103] ^ data[110] ^ data[111] ^
            data[113] ^ data[119] ^ data[121] ^ data[122] ^ data[123] ^
            data[124] ^ data[125] ^ data[15] ^ data[19] ^ data[23] ^
            data[26] ^ data[27] ^ data[34] ^ data[35] ^ data[37] ^
            data[41] ^ data[42] ^ data[43] ^ data[47] ^ data[48] ^
            data[50] ^ data[57] ^ data[63] ^ data[64] ^ data[66] ^
            data[67] ^ data[70] ^ data[71] ^ data[73] ^ data[78] ^
            data[79] ^ data[80] ^ data[81] ^ data[82] ^ data[85] ^
            data[87] ^ data[89] ^ data[90] ^ data[92] ^ data[95] ^ data[96] ^ data[97] ^ data[99];

        
        for (bit = 0; bit < 16; bit++)
        {
            crc[bit] = new_crc[bit];
        }
    }

    
    return_val = 0;
    for (bit = 0; bit < 16; bit++)
    {
        return_val += (new_crc[bit] & 0x1) << bit;
    }
    SHR_FREE(buffer);
    *calculated_crc = return_val;

exit:
    SHR_FUNC_EXIT;
}


