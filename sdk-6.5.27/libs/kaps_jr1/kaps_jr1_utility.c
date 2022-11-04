

#include "kaps_jr1_utility.h"
#include "kaps_jr1_portable.h"

void
kaps_jr1_convert_dm_to_xy(
    uint8_t * data_d,
    uint8_t * data_m,
    uint8_t * data_x,
    uint8_t * data_y,
    uint32_t dataLenInBytes)
{
    uint32_t i;

    for (i = 0; i < dataLenInBytes; i++)
    {
        data_x[i] = data_d[i] & (~data_m[i]);
        data_y[i] = (~data_d[i]) & (~data_m[i]);
    }
}

void
kaps_jr1_convert_xy_to_dm(
    uint8_t * data_x,
    uint8_t * data_y,
    uint8_t * data_d,
    uint8_t * data_m,
    uint32_t dataLenInBytes)
{
    uint32_t i;

    for (i = 0; i < dataLenInBytes; ++i)
    {
        data_d[i] = data_x[i] & (~data_y[i]);
        data_m[i] = (~data_x[i]) & (~data_y[i]);
    }
}



uint32_t
KapsJr1ReadBitsInArrray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos)
{
    uint32_t value = 0;
    uint32_t len;
    uint32_t curr_bit_pos, curr_byte_pos;
    uint32_t mask_data = (uint32_t) ~ 0;
    int32_t counter = 0, total_bits = 0;

    curr_byte_pos = ArrSize - 1 - (EndPos >> 3);
    curr_bit_pos = 7 - (EndPos & 0x7);

    len = EndPos - StartPos + 1;
    counter = len + curr_bit_pos;
    if (len != 32)
        mask_data = ~((uint32_t)~0 << len);
    while (counter > 0 && total_bits < 32)
    {
        value = (value << 8) | (Arr[curr_byte_pos++]);
        counter -= 8;
        total_bits += 8;
    }

    if (len + curr_bit_pos > 32)
    {
        value <<= 8 - (StartPos & 0x7);
        value |= ((Arr[curr_byte_pos] >> (StartPos & 0x7)) & 0xFF);
    }
    else
    {
        value = value >> (StartPos & 0x7);
    }

    return (value & mask_data);
}



void
KapsJr1WriteBitsInArray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos,
    uint32_t Value)
{
    uint32_t startByteIdx;
    uint32_t endByteIdx;
    uint32_t byte;
    uint32_t len;
    uint8_t maskValue;

    kaps_jr1_assert((EndPos >= StartPos), "endbit cannot be less than startbit");
    kaps_jr1_assert((EndPos < (ArrSize << 3)), "endBit exceeds the arr size");

    len = EndPos - StartPos + 1;
    kaps_jr1_assert((len <= 32), "Can not write more than 32-bit at a time !");
    
    if (len != 32)
    {
        kaps_jr1_assert(Value <= (uint32_t) (~((uint32_t)~0 << len)), "Value is too big to write in the bit field!");
    }

    startByteIdx = ArrSize - ((StartPos >> 3) + 1);
    endByteIdx = ArrSize - ((EndPos >> 3) + 1);

    if (startByteIdx == endByteIdx)
    {
        maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
        maskValue |= ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        return;
    }
    if (StartPos & 7)
    {
        maskValue = (uint8_t) ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        startByteIdx--;
        Value >>= (8 - (StartPos & 7));
    }
    for (byte = startByteIdx; byte > endByteIdx; byte--)
    {
        Arr[byte] = (uint8_t) (Value);
        Value >>= 8;
    }
    maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
    Arr[byte] &= maskValue;
    Arr[byte] |= Value;
}


void 
KapsJr1WriteBitsGeneric(
    uint8_t *out_data,
    uint32_t out_data_size_in_bits,
    uint32_t out_data_end_bit_pos,
    uint32_t out_data_start_bit_pos,
    uint8_t *in_data,
    uint32_t in_data_size_in_bits,
    uint32_t in_data_end_bit_pos,
    uint32_t in_data_start_bit_pos)
{
    uint32_t out_data_size_in_bytes = (out_data_size_in_bits + 7) / 8 ;
    uint32_t in_data_size_in_bytes = (in_data_size_in_bits + 7) / 8;
    int32_t tmp, numOfBits;
    uint32_t value = 0;
    uint32_t cur_out_bit = out_data_end_bit_pos;
    uint32_t cur_in_data_end_bit = in_data_end_bit_pos;

    numOfBits = out_data_end_bit_pos - out_data_start_bit_pos + 1;

    kaps_jr1_assert(numOfBits == (in_data_end_bit_pos - in_data_start_bit_pos + 1), 
                "Number of bits to write should be equal to number of bits to be read\n");

    if (numOfBits) {
        tmp = numOfBits;
        while (tmp >= 32) {
            value = KapsJr1ReadBitsInArrray(in_data, in_data_size_in_bytes, cur_in_data_end_bit,
                                     cur_in_data_end_bit - 31);

            KapsJr1WriteBitsInArray(out_data, out_data_size_in_bytes, cur_out_bit, cur_out_bit - 31, value);

            tmp -= 32;
            cur_in_data_end_bit -= 32;
            cur_out_bit -= 32;
        }

        if (tmp > 0) {
            value = KapsJr1ReadBitsInArrray(in_data, in_data_size_in_bytes, cur_in_data_end_bit,
                                     cur_in_data_end_bit - tmp + 1);
            
            KapsJr1WriteBitsInArray(out_data, out_data_size_in_bytes, cur_out_bit, 
                                    cur_out_bit - tmp + 1, value);
        }

        
    }

}



void
kaps_jr1_FillOnes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_jr1_assert((data != NULL), "Null Data Pointer !");
    kaps_jr1_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = ((startBit + 8) >> 3);
    numOfBytes = (((startBit - endBit) + 8) >> 3);

    if (numOfBytes == 1)
    {
        KapsJr1WriteBitsInArray(data, datalen, startBit, endBit, (~(0xFFFFFFFF << ((startBit - endBit) + 1))));
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsJr1WriteBitsInArray(&data[datalen - startByte], 1, (startBit & 0x7), 0, (~(0xFFFFFFFF << ((startBit + 1) & 0x7))));
        startByte--;
        numOfBytes--;
    }
    if (numOfBytes > 1)
    {
        kaps_jr1_memset(&data[datalen - startByte], 0xff, numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsJr1WriteBitsInArray(data, datalen, (startByte << 3) - 1, endBit, (~(0xFFFFFFFF << ((startByte << 3) - endBit))));
    }
}


void
kaps_jr1_FillZeroes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_jr1_assert((data != NULL), "Null Data Pointer !");
    kaps_jr1_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = (startBit + 8) >> 3;
    numOfBytes = ((startBit - endBit) + 8) >> 3;

    if (numOfBytes == 1)
    {
        KapsJr1WriteBitsInArray(data, datalen, startBit, endBit, 0);
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsJr1WriteBitsInArray(&data[datalen - startByte], 1, (startBit & 0x7), 0, 0);
        startByte--;
        numOfBytes--;
    }
    if (numOfBytes > 1)
    {
        kaps_jr1_memset(&data[datalen - startByte], 0x0, numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsJr1WriteBitsInArray(data, datalen, (startByte << 3) - 1, endBit, 0);
    }
}


void
kaps_jr1_CopyData(
    uint8_t * o_data,
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_jr1_assert((o_data != NULL), "Null Output Data Pointer !");
    kaps_jr1_assert((data != NULL), "Null Data Pointer !");
    kaps_jr1_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = (startBit + 8) >> 3;
    numOfBytes = ((startBit - endBit) + 8) >> 3;

    if (numOfBytes == 1)
    {
        KapsJr1WriteBitsInArray(o_data, datalen, startBit, endBit, KapsJr1ReadBitsInArrray(data, datalen, startBit, endBit));
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsJr1WriteBitsInArray(&o_data[datalen - startByte], 1, (startBit & 0x7), 0,
                         KapsJr1ReadBitsInArrray(&data[datalen - startByte], 1, (startBit & 0x7), 0));
        startByte--;
        numOfBytes--;
    }

    if (numOfBytes > 1)
    {
        kaps_jr1_memcpy(&o_data[datalen - startByte], &data[datalen - startByte], numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsJr1WriteBitsInArray(o_data, datalen, (startByte << 3) - 1, endBit,
                         KapsJr1ReadBitsInArrray(data, datalen, (startByte << 3) - 1, endBit));
    }
}


uint32_t
kaps_jr1_hex_to_int(
    uint8_t c)
{
    uint32_t ret = 0;
    
    if ('0' <= c && c <= '9')
        ret = c - '0';
    else if ('A' <= c && c <= 'Z')
        ret = c - 'A' + 10;
    else if ('a' <= c && c <= 'z')
        ret = c - 'a' + 10;

    return ret;
}

