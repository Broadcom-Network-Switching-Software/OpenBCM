/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pp.c
 * Purpose:     Diagnostic helper routines
 * Notes:	Retrieve, check, or print Packet Pool table for Hercules
 */

/* 
 * This sequence of masks comes from an adaptation of the gen_ecc_table.cpp
 * file in the hercules/rtl/pp directory.  
 * Inserting this code segment at line 132 (before the fclose) will
 * produce the necessary numbers in the output file mmu_pp_ecc_gen_tbl.vh:
 *
 *  fprintf (gen_fh, "\n") ;
 *  for (unsigned chkbit = 0; chkbit < NUM_CHK_BITS; chkbit++) {
 *      unsigned int chunks[6] = { 0, 0, 0, 0, 0, 0 };
 *      fprintf (gen_fh, "ecc_xor_mask[%d] = { ", chkbit);
 *      for (int syn_idx = NUM_SYNDROMES-1; syn_idx >= 0; syn_idx--) {
 *          chunks[syn_idx/32] |= ((synd_list [syn_idx] >> chkbit) & 1) << (syn_idx % 32);
 *      }
 *      for (int ch_idx = 0; ch_idx < 6; ch_idx++) 
 *          fprintf (gen_fh, "0x%08x, ", chunks[ch_idx]) ;
 *      fprintf (gen_fh, "};\n") ;
 *  }
 */

#include <sal/types.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/hercules.h>
   
#include <appl/diag/pp.h>

static uint32 ecc_xor_mask[DIAG_PP_ECC_BITS][DIAG_PP_ECC_CHUNKS] = { 
    { 0xefdf7bb4, 0x9a46910f, 0xa769a476, 0x3b4d23dd, 
      0xef769eed, 0x00000103 },
    { 0xdfbef76a, 0x5525488f, 0x56d5526d, 0xb6aa93bb, 
      0xdeed5dda, 0x00000083 },
    { 0xbf7deed9, 0x2c93244f, 0xcdb2c95b, 0x6d964b76, 
      0xbddb3bb6, 0x00000043 },
    { 0x7efbddc7, 0xe388e22f, 0x3b8e38b8, 0xdc71c6ee, 
      0x7bb8f771, 0x00000823 },
    { 0xfdf7bc3f, 0xe0781e1e, 0xf87e0787, 0xc3f03de1, 
      0xf787ef0f, 0x00000412 },
    { 0xfbef83ff, 0x1ff801fd, 0xf801ff80, 0xc00ffc1f, 
      0xf07fe0ff, 0x00000209 },
    { 0xf7e07fff, 0x0007fffb, 0x07ffff80, 0x3ffffc00, 
      0x0fffe000, 0x00000e04 },
    { 0xf01fffff, 0xfffffff7, 0x0000007f, 0xfffffc00, 
      0x00001fff, 0x000001fc },
    { 0x0fffffff, 0xfffffff0, 0xffffffff, 0x000003ff, 
      0x00000000, 0x00000ffc }
};

/* Masks to XOR a word into a bit */
static uint32 compress_xor_mask[5] = { 
    0xaaaaaaaa, 0x44444444, 0x10101010, 0x01000100, 0x00010000 
};

/*
 * Function:
 *	diag_mem_pp_word_check
 *
 * Description:
 *	Verify the ECC on a Hercules PP entry word.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      word_info - packet pool pre-digested word and syndrome info.
 *      address - entry address from which word sourced (needed for ECC).
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Syndrome mismatch
 *
 * Notes:
 */

int 
diag_mem_pp_word_check(int unit, diag_mem_pp_word_t *word_info, 
                           int entry_addr)
{
    uint32 synd_bit, chunk, cx;
    uint32 xor_work, shift;
    uint32 our_syndrome = 0; 

    COMPILER_REFERENCE(unit);

    for (synd_bit = 0; synd_bit < DIAG_PP_ECC_BITS; synd_bit++) {
        xor_work = 0;
        for (chunk = 0; chunk < DIAG_PP_WORD_SIZE; chunk++) {
            xor_work ^= word_info->data[chunk] & ecc_xor_mask[synd_bit][chunk];
        }
        xor_work ^= entry_addr & 
            ecc_xor_mask[synd_bit][DIAG_PP_ECC_CHUNKS - 1];

        for (cx = 0, shift = 1; cx < 5; cx++, shift *= 2) {
            xor_work ^= (xor_work & compress_xor_mask[cx]) >> shift;
        }

        our_syndrome |= (xor_work & 0x1) << synd_bit;
    }

    if ( our_syndrome != word_info->syndrome) {
        /* Mismatch, now what? */
        /* return SOC_E_BAD_SYNDROME; */
        return SOC_E_INTERNAL;
    }
    
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_mem_pp_word_decomp
 *
 * Description:
 *	Separate a PP entry into the four word structures.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      data - raw uint32 form of the PP entry
 *      entry - return structure for assembled data
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_mem_pp_word_decomp(int unit, uint32 *data, 
                            diag_mem_pp_entry_t *entry)
{
    int ix, jx, dx = 0;;

    for (ix = 0; ix < DIAG_PP_WORDS_PER_ENTRY; ix++) {
        int shift = (ix * DIAG_PP_ECC_BITS);
        uint32 word_mask = 0xffffffff >> shift;
        for (jx = 0; jx < DIAG_PP_WORD_SIZE; jx++) {
            entry->words[ix].data[jx] = ((data[dx] >> shift) & word_mask) |
                ((data[dx + 1] << (32 - shift)) & ~word_mask);
            dx++;
        }

        entry->words[ix].syndrome = 
            (data[dx] >> shift) & DIAG_PP_ECC_MASK;
    }
    
    /* Leftover adjustment for the last syndrome, which spans two uint32's */

    entry->words[DIAG_PP_WORDS_PER_ENTRY - 1].syndrome = 
        (entry->words[DIAG_PP_WORDS_PER_ENTRY - 1].syndrome & 0x1f) |
        (data[dx] << 5);

    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_mem_pp_byte_array
 *
 * Description:
 *	Convert decomposed PP entry into byte array.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      entry - return structure for assembled data
 *      data - uint8 return array
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_mem_pp_byte_array(int unit, diag_mem_pp_entry_t *entry, uint8 *data)
{
    int          word_ptr, data_ptr, ret_ptr = 0;
    uint32       data_bytes;
    
    for (word_ptr = (DIAG_PP_WORDS_PER_ENTRY -1); word_ptr >= 0; word_ptr--) {
        for (data_ptr = (DIAG_PP_WORD_SIZE - 1); data_ptr >= 0; data_ptr--) {
            data_bytes = soc_htonl(entry->words[word_ptr].
                                   data[data_ptr]);
            sal_memcpy(&data[ret_ptr], &data_bytes, 4);
            ret_ptr += 4;
        }
    }
 
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_mem_pp_entry_check
 *
 * Description:
 *	Check ECC for the whole entry.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      entry - pre_digested words and syndromes
 *      address - entry address from which word sourced (needed for ECC).
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_mem_pp_entry_check(int unit, diag_mem_pp_entry_t *entry, 
                            int entry_addr)
{
    int word;

    for (word=0; word < DIAG_PP_WORDS_PER_ENTRY; word++) {
        SOC_IF_ERROR_RETURN(diag_mem_pp_word_check(unit, &(entry->words[word]),
                                                  entry_addr));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_mem_pp_check
 *
 * Description:
 *	Check ECC for a range of the PP table.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      start - entry index to begin check
 *      end - entry index to finish check
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_XXX		Various failures
 *
 * Notes:
 */

int 
diag_mem_pp_check(int unit, int start, int end)
{
#ifdef BCM_HERCULES_SUPPORT
    uint32 chip_entry[SOC_MAX_MEM_WORDS];
    diag_mem_pp_entry_t decomp_entry;
    int ix;
    int index_min = soc_mem_index_min(unit, MEM_PPm);
    int index_max = soc_mem_index_max(unit, MEM_PPm);

    if ( start < index_min ) {
        start = index_min;
    }

    if ( end > index_max ) {
        end = index_max;
    }

    for (ix = start; ix < end; ix++) {
        int entry_addr = soc_mem_addr(unit, MEM_PPm, 0, 0, ix);
        
	SOC_IF_ERROR_RETURN(READ_MEM_PPm(unit, 0, ix, chip_entry));

        SOC_IF_ERROR_RETURN(diag_mem_pp_word_decomp(unit, chip_entry, 
                                                   &decomp_entry));
        SOC_IF_ERROR_RETURN(diag_mem_pp_entry_check(unit, &decomp_entry, 
                                                   entry_addr));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_dump_xq_packet
 *
 * Purpose:
 *	Print the packet for an XQ entry in a given port.
 *
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      blk - selected block to dump XQ packets.
 *      xq_ptr - selected XQ entry indicating packet.
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_dump_xq_packet(int unit, soc_block_t blk, int xq_ptr)
{
#ifdef BCM_HERCULES_SUPPORT
    int                 offset, size, bytes, rv;
    int                 pp_ptr, pp_data_p, pkt_data_p = 0;
    uint32              read_data[SOC_MAX_MEM_WORDS];
    uint8               *pkt_data;
    diag_mem_pp_entry_t  dpp_entry;
    uint8               pp_data[DIAG_PP_DATA_BYTES];


    SOC_IF_ERROR_RETURN(READ_MEM_XQm(unit, blk, xq_ptr, read_data));
    offset = soc_mem_field32_get(unit, MEM_XQm, read_data, OFFSETf);
    bytes = size = soc_mem_field32_get(unit, MEM_XQm, read_data, SIZEf);
    pp_ptr = soc_mem_field32_get(unit, MEM_XQm, read_data, PKTPTRf);
    
    pkt_data = sal_alloc(size,"XQPacket");
    if (!pkt_data) {
        return SOC_E_MEMORY;
    }

    while (bytes != 0) {
        int copy_bytes = bytes;
        
        rv = READ_MEM_PPm(unit, blk, pp_ptr, read_data);
        if (SOC_FAILURE(rv)) {
            sal_free(pkt_data);
            return rv;
        }
        rv = diag_mem_pp_word_decomp(unit, read_data, &dpp_entry);
        if (SOC_FAILURE(rv)) {
            sal_free(pkt_data);
            return rv;
        }

        rv = diag_mem_pp_byte_array(unit, &dpp_entry, pp_data);
        if (SOC_FAILURE(rv)) {
            sal_free(pkt_data);
            return rv;
        }

        if (offset) {
            pp_data_p = offset * DIAG_PP_XQ_OFFSET_BYTES;
        } else {
            pp_data_p = 0;
        }

        if (copy_bytes > (DIAG_PP_DATA_BYTES - pp_data_p)) {
            copy_bytes = (DIAG_PP_DATA_BYTES - pp_data_p);
        }

        sal_memcpy(&(pkt_data[pkt_data_p]), &(pp_data[pp_data_p]), copy_bytes);
        pkt_data_p += copy_bytes;
        bytes -= copy_bytes;

        if (bytes != 0) {
            offset = 0; /* We've use this up */
            /* Follow to next PP entry */
            rv = READ_MEM_LLAm(unit, blk, pp_ptr, read_data);
            if (SOC_FAILURE(rv)) {
                sal_free(pkt_data);
                return rv;
            }
            pp_ptr = soc_mem_field32_get(unit, MEM_LLAm, read_data, NEXTPTRf);
        }
    }
    
    cli_out("XQ Packet:  index[%d]\n", xq_ptr);
    soc_dma_dump_pkt(unit, "  ", pkt_data, size, TRUE);
    cli_out("\tCRC(len-4) = 0x%08x; CRC(len) = 0x%08x\n", 
            ~_shr_crc32(~0, pkt_data, size - 4), 
            ~_shr_crc32(~0, pkt_data, size));

    sal_free(pkt_data);
#endif
    return SOC_E_NONE;
}
 
/*
 * Function:
 *	diag_dump_cos_packets
 *
 * Description:
 *	Print the packets for selected COS in a given port.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      port - selected port to dump XQ packets
 *      cos_start - begin with dumping this COS
 *      cos_end - finish with dumping this COS
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_dump_cos_packets(int unit, soc_port_t port, int cos_start, int cos_end)
{
#ifdef BCM_HERCULES_SUPPORT
    int                 cos, ptr_total, limit;
    int                 num_cos = NUM_COS(unit);
    int                 cos_read_ptr, cos_write_ptr, cos_ptr_wrap;
    int                 cos_ptr_start[SOC_MAX_NUM_COS];
    int			cos_ptr_end[SOC_MAX_NUM_COS];
    uint32		regval;
    uint32              read_data[SOC_MAX_MEM_WORDS];
    int                 ptr_max = soc_mem_index_max(unit, MEM_XQm);
    soc_block_t         blk = SOC_PORT_BLOCK(unit, port);
	
    ptr_total = 0;
    cos_ptr_start[0] = -1;
    /* Should this limit be cos_end? */
    for (cos = 0; cos < num_cos; cos++) {
        SOC_IF_ERROR_RETURN(READ_MMU_PKTLMTCOS_UPPERr(unit, port, cos, &regval));
        limit = soc_reg_field_get(unit, MMU_PKTLMTCOS_UPPERr, regval, LIMITf);

        if (limit != 0) {
            if (ptr_total == 0) {
                cos_ptr_start[cos] = 0;
            } else {
                cos_ptr_start[cos] = cos_ptr_end[cos - 1] + 1;
            }
            ptr_total += limit;

            if (ptr_total > ptr_max) {
                cos_ptr_end[cos] = ptr_max;
                break;
            } else {
                cos_ptr_end[cos] = cos_ptr_start[cos] + limit - 1;
            }
        } else {
            if (cos != 0) {
                cos_ptr_end[cos] = cos_ptr_start[cos] = cos_ptr_end[cos - 1];
            } else {
                cos_ptr_end[cos] = cos_ptr_start[cos];
            }
        }
    }

    for (cos = cos_start; cos <= cos_end; cos++) {
        SOC_IF_ERROR_RETURN(READ_MEM_XQ_PTRSm(unit, blk, cos, read_data));
        cos_read_ptr = 
            soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, RDPTRf);
        cos_write_ptr = 
            soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, WRPTRf);
        cos_ptr_wrap =
            soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, WRPTRWRAPf) !=
            soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, RDPTRWRAPf);

        if ((cos_read_ptr == cos_write_ptr) && !cos_ptr_wrap) {
            /* Nothing in queue */
            continue;
        }

        do {
            SOC_IF_ERROR_RETURN(diag_dump_xq_packet(unit, port, cos_read_ptr));

            cos_read_ptr++;
            if (cos_read_ptr > cos_ptr_end[cos]) {
                /* Wrap */
                cos_read_ptr = cos_ptr_start[cos];
            }
        } while (cos_read_ptr != cos_write_ptr);
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_dump_ib_packet
 *
 * Description:
 *	Print the packet for the given port and ingress buffer range.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      blk - selected block to dump XQ packets.
 *      start - start index in Ingress Buffer
 *      end - end index in Ingress Buffer
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_dump_ib_packet(int unit, soc_block_t blk, int start, int end, 
                   int final_lanes)
{
#ifdef BCM_HERCULES_SUPPORT
    uint32              read_data[SOC_MAX_MEM_WORDS];
    uint32              flipped_data[2];
    int                 ptr_max = soc_mem_index_max(unit, MEM_INGBUFm);
    uint8               *pkt_data;
    int                 size, read_ptr, pkt_data_p = 0;

    
    if (end >= start) {
        size = end - start + 1;
    } else {
        size = ptr_max - start + 1 + end;
    }

    size *= 8;
    size -= final_lanes;

    if (size > (9 * 1024)) { /* Jumbo packets are max size */
        cli_out("IngBuf Packet oversized, skipping range[%d,%d]\n", 
                start, end);
        return SOC_E_NONE;
    }

    pkt_data = sal_alloc(size,"IBPacket");
    if (!pkt_data) {
        return SOC_E_MEMORY;
    }

    read_ptr = start;
    while (1) {
        int rv;

        if ((rv = (READ_MEM_INGBUFm(unit, blk, read_ptr, read_data))) < 0) {
            sal_free(pkt_data);
            return rv;
        }

        /* FIXME:  Fix up this on real chip! */
        flipped_data[0] = read_data[1];
        flipped_data[1] = read_data[0];

        if (read_ptr == end) {
            sal_memcpy(&(pkt_data[pkt_data_p]), flipped_data, 8 - final_lanes);
            break;
        } else {
            sal_memcpy(&(pkt_data[pkt_data_p]), flipped_data, 8);
            pkt_data_p += 8;
            if (read_ptr == ptr_max) {
                read_ptr = 0;
            } else {
                read_ptr++;
            }
        }
    }

    cli_out("IngBuf Packet:  range[%d,%d]\n", start, end);
    soc_dma_dump_pkt(unit, "  ", pkt_data, size, TRUE);
    sal_free(pkt_data);
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_dump_ib_packets
 *
 * Description:
 *	Print the packets buffered for a given port.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      port - selected port to dump Ingress Buffer packets
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_dump_ib_packets(int unit, soc_port_t port)
{
#ifdef BCM_HERCULES_SUPPORT
    uint32              read_data[SOC_MAX_MEM_WORDS];
    int                 ptr_max = soc_mem_index_max(unit, MEM_INGBUFm);
    int                 start_index, end_index, wrap;
    uint32              info_addr;
    int			blk = SOC_PORT_BLOCK(unit, port);
    
    read_data[0] = 0;
    start_index = 0;
    wrap = FALSE;
    while (!wrap) {
        end_index = start_index;
        do {
            /* Only retrieve the info here */
            info_addr = soc_mem_addr(unit, MEM_INGBUFm, 0, blk, end_index) + 2;
            SOC_IF_ERROR_RETURN(soc_hercules_mem_read_word(
                                unit, info_addr, read_data));

            if ((read_data[0] & DIAG_INGBUF_EOF_BIT) != 0) {
                int final_lanes = read_data[0] & DIAG_INGBUF_LANES_MASK;
                /* Dump the present packet */
                SOC_IF_ERROR_RETURN(
                    diag_dump_ib_packet(unit, blk, start_index, end_index,
                                       final_lanes));
                start_index = end_index + 1;
                break;
            } else {
                end_index++;
                if (end_index > ptr_max ) {
                    end_index = 0;
                    wrap = TRUE;
                }
            }
        } while (end_index != start_index);
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *	diag_set_xq_errors
 *
 * Purpose:
 *	Introduce errors for an XQ packet in a given port.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      blk - selected block to dump XQ packets.
 *      xq_ptr - selected XQ entry indicating packet.
 *      errors - number of bits to invert
 *      start_bit - first bit of the packet to trash
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_set_xq_errors(int unit, soc_block_t blk, int xq_ptr,
                   int errors, int start_bit)
{
#ifdef BCM_HERCULES_SUPPORT
    int                 offset, bytes;
    int                 pp_ptr;
    uint32              read_data[SOC_MAX_MEM_WORDS];
    int                 bit_bonus, the_chunk, the_word;
    int                 entry_bit, entry_word; 
    int                 word_bit, word_addr;
    soc_mem_info_t      *meminfo = &SOC_MEM_INFO(unit, MEM_PPm);

    SOC_IF_ERROR_RETURN(READ_MEM_XQm(unit, blk, xq_ptr, read_data));
    offset = soc_mem_field32_get(unit, MEM_XQm, read_data, OFFSETf);
    bytes = soc_mem_field32_get(unit, MEM_XQm, read_data, SIZEf);
    pp_ptr = soc_mem_field32_get(unit, MEM_XQm, read_data, PKTPTRf);

    if (BYTES2BITS(bytes) < start_bit) {
        start_bit = BYTES2BITS(bytes);
    }

    start_bit += offset * BYTES2BITS(DIAG_PP_XQ_OFFSET_BYTES);

    while (start_bit > (DIAG_PP_WORD_BITS * DIAG_PP_WORDS_PER_ENTRY)) {
        /* Follow to next PP entry */
        SOC_IF_ERROR_RETURN(READ_MEM_LLAm(unit, blk, pp_ptr, read_data));
        pp_ptr = soc_mem_field32_get(unit, MEM_LLAm, read_data, NEXTPTRf);
        start_bit -= (DIAG_PP_WORD_BITS * DIAG_PP_WORDS_PER_ENTRY);
    }

    the_chunk = start_bit / DIAG_PP_WORD_BITS;
    bit_bonus = (3 - the_chunk) * (DIAG_PP_WORD_BITS + DIAG_PP_ECC_BITS);
    start_bit %= DIAG_PP_WORD_BITS;

    the_word = BITS2WORDS(start_bit);
    bit_bonus += WORDS2BITS(DIAG_PP_WORD_SIZE - the_word);
    start_bit = 32 - (start_bit % 32);
    
    entry_bit = start_bit + bit_bonus;
    entry_word = entry_bit / 32;
    word_bit = entry_bit % 32;

    word_addr = soc_mem_addr(unit, MEM_PPm, 0, blk, 0) + 
        (pp_ptr * meminfo->gran) + entry_word;

    /* Fun with A0 PP mem read problem */
    SOC_IF_ERROR_RETURN(soc_hercules_mem_read_word(unit, 
                                     word_addr, &(read_data[0])));
    SOC_IF_ERROR_RETURN(soc_hercules_mem_read_word(unit, 
                                     word_addr, &(read_data[0])));
    
    /* Do we need to endian swap the bytes? */
    while (errors--) {
        read_data[0] ^= (1 << word_bit);
        cli_out("Predicted error at entry %d, data word %d, bit %d\n",
                pp_ptr, entry_bit / (DIAG_PP_WORD_BITS + DIAG_PP_ECC_BITS),
                entry_bit % (DIAG_PP_WORD_BITS + DIAG_PP_ECC_BITS));
        entry_bit++;
        word_bit--;
        start_bit++;
        if ( word_bit  == 0 ) {
            break; /* Give up crossing word-boundaries */
        }
        if ( (start_bit % DIAG_PP_WORD_BITS) == 0 ) {
            break; /* Give up crossing data-boundaries */
        }
    }
    
    SOC_IF_ERROR_RETURN(soc_hercules_mem_write_word(unit,
                                     word_addr, &(read_data[0])));
#endif
    return SOC_E_NONE;
}



/*
 * Function:
 *	diag_set_cos_errors
 *
 * Purpose:
 *	Introduce errors for XQ packets in selected COS's in a given port.
 *
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      port - selected port to trash XQ packets.
 *      cos_sel - COS to analyze
 *      errors - number of bits to invert
 *      packet - which packet number in the port/cos queue to trash
 *      start_bit - first bit of the packet to trash
 *      xq_ptr - selected XQ entry indicating packet.
 *
 * Returns:
 *	SOC_E_NONE		Success
 *	SOC_E_INTERNAL		Chip access failure
 *
 * Notes:
 */

int 
diag_set_cos_errors(int unit, soc_port_t port, int cos_sel,
                   int errors, int packet, int start_bit)
{
#ifdef BCM_HERCULES_SUPPORT
    int                 cos, ptr_total, limit;
    int                 num_cos = NUM_COS(unit);
    int                 cos_read_ptr, cos_write_ptr, cos_ptr_wrap;
    int                 previous_cos_read_ptr;
    int                 cos_ptr_start[SOC_MAX_NUM_COS];
    int			cos_ptr_end[SOC_MAX_NUM_COS];
    uint32		regval;
    uint32              read_data[SOC_MAX_MEM_WORDS];
    int                 ptr_max = soc_mem_index_max(unit, MEM_XQm);
    int                 cur_pkt;
    int			blk = SOC_PORT_BLOCK(unit, port);
	
    ptr_total = 0;
    cos_ptr_start[0] = -1;
    /* Should this limit be cos_end? */
    for (cos = 0; cos < num_cos; cos++) {
        SOC_IF_ERROR_RETURN(READ_MMU_PKTLMTCOS_UPPERr(unit, port, cos, &regval));
        limit = soc_reg_field_get(unit, MMU_PKTLMTCOS_UPPERr, regval, LIMITf);

        if (limit != 0) {
            if (ptr_total == 0) {
                cos_ptr_start[cos] = 0;
            } else {
                cos_ptr_start[cos] = cos_ptr_end[cos - 1] + 1;
            }
            ptr_total += limit;

            if (ptr_total > ptr_max) {
                cos_ptr_end[cos] = ptr_max;
                break;
            } else {
                cos_ptr_end[cos] = cos_ptr_start[cos] + limit - 1;
            }
        } else {
            if (cos != 0) {
                cos_ptr_end[cos] = cos_ptr_start[cos] = cos_ptr_end[cos - 1];
            } else {
                cos_ptr_end[cos] = cos_ptr_start[cos];
            }
        }
    }

    cos = cos_sel;
    SOC_IF_ERROR_RETURN(READ_MEM_XQ_PTRSm(unit, blk, cos, read_data));
    cos_read_ptr = 
        soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, RDPTRf);
    cos_write_ptr = 
        soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, WRPTRf);
    cos_ptr_wrap =
        soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, WRPTRWRAPf) !=
        soc_mem_field32_get(unit, MEM_XQ_PTRSm, read_data, RDPTRWRAPf);

    if ((cos_read_ptr == cos_write_ptr) && !cos_ptr_wrap) {
        /* Nothing in queue */
        return SOC_E_NONE;
    }

    cur_pkt = 0;
    /* Note XQ used before current read pointer, reverse wrap if needed */
    if (cos_read_ptr == cos_ptr_start[cos]) {
        previous_cos_read_ptr = cos_ptr_end[cos];
    } else {
        previous_cos_read_ptr = cos_read_ptr - 1;
    }

    do {
        if ( cur_pkt++ == packet ) {
            SOC_IF_ERROR_RETURN(diag_set_xq_errors(unit, blk, 
                                                   cos_read_ptr,
                                                   errors, start_bit));
        }
        cos_read_ptr++;
        if (cos_read_ptr > cos_ptr_end[cos]) {
            /* Wrap */
            cos_read_ptr = cos_ptr_start[cos];
        }
    } while (cos_read_ptr != cos_write_ptr);

    /* The XQ memory does not latch the read it needs, thus reading it
     * will probably corrupt the system.  If we are only using one COS,
     * then it is possible to fix this problem.
     * Read to restore HW state to prevent pointer crash in XQ. 
     */
    SOC_IF_ERROR_RETURN(READ_MEM_XQm(unit, blk, 
                                     previous_cos_read_ptr, read_data));
#endif
    return SOC_E_NONE;
}

