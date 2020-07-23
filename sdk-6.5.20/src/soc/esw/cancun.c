/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cancun.c
 * Purpose:     CANCUN API and routines
 *              This module provides API's for all CANCUN related operations
 */
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/util.h>
#include <soc/debug.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/esw/cancun.h>
#include <soc/esw/cancun_feature_map.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif

#include <soc/devids.h>

/* Environment switch */
#define _PCID_TEST 0

/* Local defines */
#define BYTES_PER_UINT32    (sizeof(uint32))

/*
 * Variable:
 *      soc_cancun_info
 * Purpose:
 *      One entry for each SOC device containing port information
 *      for that device. 
 */
static soc_cancun_t *soc_cancun_info[BCM_MAX_NUM_UNITS];

/* Local functions */
static int _soc_cancun_alloc(soc_cancun_t** cancun);
static int _soc_cancun_file_pio_load(int unit, uint8* buf, int buf_words);
static int _soc_cancun_file_cmh_load(int unit, uint8* buf, int buf_words);
static int _soc_cancun_file_cch_load(int unit, uint8* buf, int buf_words);
static int _soc_cancun_file_ceh_load(int unit, uint8* buf, int buf_words);
static int _soc_cancun_cih_load(int unit, uint8* buf, int num_data_blobs);
static int _soc_cancun_cih_pio_load(int unit, uint8* buf, int length,
                                    uint32 flags);
static int _soc_cancun_cih_tcam_write(int unit, uint8 *buf);
static int _soc_cancun_hash(soc_cancun_hash_table_t* t, uint32 mem,
                            soc_field_t field, uint32* key);
static uint32* _soc_cancun_hash_find_entry(uint32 *hash_entry, uint32 src_mem,
                                           uint32 src_field, int src_app);
static int _soc_cancun_cmh_list_update(soc_cancun_cmh_t *cmh);
static int _soc_cancun_memcpy_letohl(uint32 *des, uint32 *src, uint32 word_len);
static int _soc_cancun_enum_check(int unit, int regmem, int enum_val, int enum_type);
static int _soc_cancun_cih_mem_load(int unit, uint8 *buf);
static int _soc_cancun_cch_dest_set (int unit,
               soc_cancun_cch_map_t *cch_map, uint64 data);
static int _soc_cancun_cch_dest_set_with_idx (int unit,
               soc_cancun_cch_map_t *cch_map, uint64 data, uint32 val_idx);


/* ZLIB CRC-32 table*/
static uint32 soc_crc32_tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
    0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
    0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
    0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
    0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
    0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
    0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
    0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
    0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
    0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
    0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
    0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
    0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
    0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
    0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
    0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
    0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
    0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
    0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
    0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
    0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
    0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
    0x2d02ef8dL
};

/*
 * Function:
 *      soc_cancun_init
 * Purpose:
 *      Initialization sequence for loading all applied CANCUN loadable files
 * Parameters:
 *      unit  - (IN) Unit ID
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
*/
int soc_cancun_init (uint32 unit) {
    soc_cancun_t* cc = soc_cancun_info[unit];
    char *cancun_path;
    uint32 val, cancun_path_len = 0;

    if(cc == NULL) {

        if(_soc_cancun_alloc(&cc)) {
            return SOC_E_MEMORY;
        }

        cc->unit = unit;
        cc->flags = SOC_CANCUN_FLAG_LOAD_DEFAULT_ALL;

        cancun_path = soc_property_get_str(unit, "cancun_dir");
        if(cancun_path != NULL) {
            cancun_path_len = sal_strlen(cancun_path);
            if(cancun_path_len < CANCUN_FILENAME_SIZE) {
                sal_memcpy(cc->default_path, cancun_path, cancun_path_len);
            }
        } else {
            sal_memset(cc->default_path, 0, CANCUN_FILENAME_SIZE);
        }

        val = soc_property_get(unit, "cancun_load_skip", 0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_CIH)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_CIH) : (~0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_CMH)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_CMH) : (~0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_CCH)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_CCH) : (~0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_CFH)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_CFH) : (~0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_CEH)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_CEH) : (~0);
        cc->flags &= (val & SOC_PROPERTY_CANCUN_LOAD_SKIP_ISPF)?
                     (~SOC_CANCUN_FLAG_LOAD_DEFAULT_ISPF) : (~0);
        val = soc_property_get(unit, "cancun_debug_mode", 0);
        cc->flags |= (val & SOC_PROPERTY_CANCUN_DEBUG_MODE)?
                     SOC_CANCUN_FLAG_DEBUG_MODE: 0;
        cc->flags |= (val & SOC_PROPERTY_CANCUN_FILE_VALIDITY)?
                     SOC_CANCUN_FLAG_SKIP_VALIDITY: 0;
        cc->flags |= SOC_CANCUN_FLAG_INITIALIZED;
        soc_cancun_info[unit] = cc;
    }

    if((cc->flags & SOC_CANCUN_FLAG_INITIALIZED) == 0) {
        return SOC_E_INIT;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_WARM_BOOT(unit)) {
            (void)soc_cancun_scache_alloc(unit);
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_deinit
 * Purpose:
 *      De-initialization CANCUN control structures
 * Parameters:
 *      unit  - (IN) Unit ID
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
*/
void soc_cancun_deinit(uint32 unit) {
    soc_cancun_t* cc = soc_cancun_info[unit];

    LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
        "De-initialize CANCUN for UNIT %d\n"), unit));

    if(cc != NULL) {
        if(cc->cih != NULL) {
            sal_free(cc->cih);
        }

        if(cc->cmh != NULL) {
            if(cc->cmh->cmh_table != NULL) {
                sal_free(cc->cmh->cmh_table);
            }
            if(cc->cmh->cmh_list != NULL) {
                sal_free(cc->cmh->cmh_list);
            }
            sal_free(cc->cmh);
        }

        if(cc->cch != NULL) {
            if(cc->cch->cch_table != NULL) {
                sal_free(cc->cch->cch_table);
            }
            if(cc->cch->pseudo_regs != NULL) {
                sal_free(cc->cch->pseudo_regs);
            }
            sal_free(cc->cch);
        }

        if(cc->ceh != NULL) {
            if(cc->ceh->ceh_table != NULL) {
                sal_free(cc->ceh->ceh_table);
            }
            sal_free(cc->ceh);
        }

        if(cc->flow_db != NULL) {
            if(cc->flow_db->flow_map != NULL) {
                sal_free(cc->flow_db->flow_map);
            }
            sal_free(cc->flow_db);
        }

        if(cc->ispf != NULL) {
            sal_free(cc->ispf);
        }

        sal_free(cc);

        soc_cancun_info[unit] = (soc_cancun_t*) NULL;
    }
}

/*
 * Function:
 *      soc_cancun_info_get
 * Purpose:
 *      Get cancun info
 * Parameters:
 *      unit  - (IN)     Unit ID
 *      cc    - (IN/OUT) cancun info
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
*/
int soc_cancun_info_get(uint32 unit, soc_cancun_t** cc) {

    soc_cancun_t* pcc = soc_cancun_info[unit];

    if(pcc == NULL) {
        return SOC_E_INTERNAL;
    }

    *cc = soc_cancun_info[unit];

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_status_get
 * Purpose:
 *      Initialization sequence for loading all applied CANCUN loadable files
 * Parameters:
 *      unit  - (IN) Unit ID
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
*/
int soc_cancun_status_get(uint32 unit, soc_cancun_t** cc) {

    soc_cancun_t* pcc = soc_cancun_info[unit];

    if(pcc == NULL) {
        return SOC_E_UNIT;
    } else if (!(pcc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    *cc = soc_cancun_info[unit];

    return SOC_E_NONE;
}

void soc_cancun_buf_swap32(uint8 *buf, long buf_size)
{
    long i;
    uint32 *fword = (uint32 *)buf;
    for (i = 0; i < ((buf_size + 3) / 4); i++) {
        *fword = soc_letohl(*fword);
        fword ++;
    }
    return ;
}

int soc_cancun_chip_rev_validate(const uint32 chip_rev_id,
    const uint16 dev_id, const uint8 rev_id) {
    uint16 cancun_dev_id = chip_rev_id >> 16;
    uint8 cancun_rev_id = chip_rev_id & 0xff;

    if(chip_rev_id == ((dev_id << 16) | rev_id)) {
        return TRUE;
    }


    if ((cancun_dev_id == BCM56870_DEVICE_ID) &&
            (cancun_rev_id == BCM56870_A0_REV_ID)) {
        if ((dev_id == BCM56873_DEVICE_ID) && (rev_id == BCM56873_A0_REV_ID)) {
            return TRUE;
        }
    }
    if ((cancun_dev_id == BCM56770_DEVICE_ID) &&
            (cancun_rev_id == BCM56770_A0_REV_ID)) {
        if ((dev_id == BCM56771_DEVICE_ID) && (rev_id == BCM56771_A0_REV_ID)) {
            return TRUE;
        }
    }

#ifdef BCM_HURRICANE4_SUPPORT
    if ((cancun_dev_id == BCM56275_DEVICE_ID) &&
        (cancun_rev_id == BCM56275_A0_REV_ID)) {
        if (((dev_id == BCM56273_DEVICE_ID) && (rev_id == BCM56273_A0_REV_ID)) ||
            ((dev_id == BCM56274_DEVICE_ID) && (rev_id == BCM56274_A0_REV_ID)) ||
            ((dev_id == BCM56276_DEVICE_ID) && (rev_id == BCM56276_A0_REV_ID)) ||
            ((dev_id == BCM56277_DEVICE_ID) && (rev_id == BCM56277_A0_REV_ID)) ||
            ((dev_id == BCM56278_DEVICE_ID) && (rev_id == BCM56278_A0_REV_ID)) ||
            ((dev_id == BCM56273_DEVICE_ID) && (rev_id == BCM56273_A1_REV_ID)) ||
            ((dev_id == BCM56274_DEVICE_ID) && (rev_id == BCM56274_A1_REV_ID)) ||
            ((dev_id == BCM56275_DEVICE_ID) && (rev_id == BCM56275_A1_REV_ID)) ||
            ((dev_id == BCM56276_DEVICE_ID) && (rev_id == BCM56276_A1_REV_ID)) ||
            ((dev_id == BCM56277_DEVICE_ID) && (rev_id == BCM56277_A1_REV_ID)) ||
            ((dev_id == BCM56278_DEVICE_ID) && (rev_id == BCM56278_A1_REV_ID)) ||
            ((dev_id == BCM56279_DEVICE_ID) && (rev_id == BCM56279_A1_REV_ID)) ||
            ((dev_id == BCM56575_DEVICE_ID) && (rev_id == BCM56575_A1_REV_ID))) {
            return TRUE;
        }
    }
#endif /* BCM_HURRICANE4_SUPPORT */

    if ((cancun_dev_id == BCM56370_DEVICE_ID) &&
            (cancun_rev_id == BCM56370_A0_REV_ID)) {
        if (((dev_id == BCM56371_DEVICE_ID) && (rev_id == BCM56371_A0_REV_ID)) ||
            ((dev_id == BCM56372_DEVICE_ID) && (rev_id == BCM56372_A0_REV_ID)) ||
            ((dev_id == BCM56374_DEVICE_ID) && (rev_id == BCM56374_A0_REV_ID)) ||
            ((dev_id == BCM56375_DEVICE_ID) && (rev_id == BCM56375_A0_REV_ID)) ||
            ((dev_id == BCM56376_DEVICE_ID) && (rev_id == BCM56376_A0_REV_ID)) ||
            ((dev_id == BCM56377_DEVICE_ID) && (rev_id == BCM56377_A0_REV_ID)) ||
            ((dev_id == BCM56577_DEVICE_ID) && (rev_id == BCM56577_A0_REV_ID)) ||
            ((dev_id == BCM56578_DEVICE_ID) && (rev_id == BCM56578_A0_REV_ID)) ||
            ((dev_id == BCM56579_DEVICE_ID) && (rev_id == BCM56579_A0_REV_ID)) ||
            ((dev_id == BCM56370_DEVICE_ID) && (rev_id == BCM56370_A1_REV_ID)) ||
            ((dev_id == BCM56371_DEVICE_ID) && (rev_id == BCM56371_A1_REV_ID)) ||
            ((dev_id == BCM56372_DEVICE_ID) && (rev_id == BCM56372_A1_REV_ID)) ||
            ((dev_id == BCM56374_DEVICE_ID) && (rev_id == BCM56374_A1_REV_ID)) ||
            ((dev_id == BCM56375_DEVICE_ID) && (rev_id == BCM56375_A1_REV_ID)) ||
            ((dev_id == BCM56376_DEVICE_ID) && (rev_id == BCM56376_A1_REV_ID)) ||
            ((dev_id == BCM56377_DEVICE_ID) && (rev_id == BCM56377_A1_REV_ID)) ||
            ((dev_id == BCM56577_DEVICE_ID) && (rev_id == BCM56577_A1_REV_ID)) ||
            ((dev_id == BCM56578_DEVICE_ID) && (rev_id == BCM56578_A1_REV_ID)) ||
            ((dev_id == BCM56579_DEVICE_ID) && (rev_id == BCM56579_A1_REV_ID)) ||
            ((dev_id == BCM56370_DEVICE_ID) && (rev_id == BCM56370_A2_REV_ID)) ||
            ((dev_id == BCM56371_DEVICE_ID) && (rev_id == BCM56371_A2_REV_ID)) ||
            ((dev_id == BCM56372_DEVICE_ID) && (rev_id == BCM56372_A2_REV_ID)) ||
            ((dev_id == BCM56374_DEVICE_ID) && (rev_id == BCM56374_A2_REV_ID)) ||
            ((dev_id == BCM56375_DEVICE_ID) && (rev_id == BCM56375_A2_REV_ID)) ||
            ((dev_id == BCM56376_DEVICE_ID) && (rev_id == BCM56376_A2_REV_ID)) ||
            ((dev_id == BCM56377_DEVICE_ID) && (rev_id == BCM56377_A2_REV_ID)) ||
            ((dev_id == BCM56577_DEVICE_ID) && (rev_id == BCM56577_A2_REV_ID)) ||
            ((dev_id == BCM56578_DEVICE_ID) && (rev_id == BCM56578_A2_REV_ID)) ||
            ((dev_id == BCM56579_DEVICE_ID) && (rev_id == BCM56579_A2_REV_ID))) {
            return TRUE;
        }
    }

#ifdef BCM_FIREBOLT6_SUPPORT
    if ((cancun_dev_id == BCM56470_DEVICE_ID) &&
            (cancun_rev_id == BCM56470_A0_REV_ID)) {
        if (((dev_id == BCM56471_DEVICE_ID) && (rev_id == BCM56471_A0_REV_ID)) ||
            ((dev_id == BCM56472_DEVICE_ID) && (rev_id == BCM56472_A0_REV_ID)) ||
            ((dev_id == BCM56475_DEVICE_ID) && (rev_id == BCM56475_A0_REV_ID))) {
            return TRUE;
        }
    }
#endif

    return FALSE;
}


/*
 * Function:
 *      soc_cancun_file_info_get
 * Purpose:
 *      Decode a packaged format CANCUN files and save it into file structure
 * Parameters:
 *      unit        - (IN) Unit ID
 *      ccf         - (OUT) Pointer to CANCUN file structure
 *      filename    - (IN) Filename string if available
 *      buf         - (IN) Pointer to buffer of a CANCUN loadable file
 *      buf_bytes   - (IN) Length of buffer in bytes
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_INTERNAL - NULL file structure pointer
 *      SOC_E_BADID    - File validity check fails
 */
int soc_cancun_file_info_get(int unit, soc_cancun_file_t* ccf, char *filename,
        uint8 *buf, long buf_bytes) {

    soc_cancun_t *cc = soc_cancun_info[unit];
    soc_cancun_file_header_t *ccfh = (soc_cancun_file_header_t *) buf;
    uint16 dev_id;
    uint8 rev_id;
    uint32 crc, *file_crc;
    uint8 *cur_buf;
    long cur_buf_size;

    if (ccf == NULL) {
        return SOC_E_INTERNAL;
    }

    crc = soc_cancun_crc32(0, buf, buf_bytes - 4);

    /* convert file header into correct endianness */
    cur_buf = buf;
    cur_buf_size = SOC_CANCUN_FILE_HEADER_OFFSET;
    soc_cancun_buf_swap32(cur_buf, cur_buf_size);
    sal_memcpy(&ccf->header, ccfh, sizeof(soc_cancun_file_header_t));

    cur_buf += cur_buf_size;
    cur_buf_size = buf_bytes - cur_buf_size;
    /* check if CEH or CFH, don't swap rest of file */
    if ((ccfh->file_type != SOC_CANCUN_FILE_ID_CEH) &&
        (ccfh->file_type != SOC_CANCUN_FILE_ID_CFH)) { 
        soc_cancun_buf_swap32(cur_buf, cur_buf_size);
    } else {
        /* swap the checksum */
        soc_cancun_buf_swap32(buf + buf_bytes - 4, 4);
    }

    if (filename) {
        sal_strncpy(ccf->filename, filename, sal_strlen(filename)+1);
    }

    ccf->valid = 0;
    /* 1. File identifier */
    if(ccfh->file_identifier != SOC_CANCUN_FILE_ID) {
        if (filename) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: %s is not a CANCUN file\n"), filename));
        } else {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: Not a CANCUN file: 0x%08x. Abort\n"), ccfh->file_identifier));
        }
        return SOC_E_INTERNAL;
    }
    /* 2. File type */
    switch(ccfh->file_type) {
        case SOC_CANCUN_FILE_ID_CIH:
            ccf->type = CANCUN_SOC_FILE_TYPE_CIH;
            break;
        case SOC_CANCUN_FILE_ID_CMH:
            ccf->type = CANCUN_SOC_FILE_TYPE_CMH;
            break;
        case SOC_CANCUN_FILE_ID_CCH:
            ccf->type = CANCUN_SOC_FILE_TYPE_CCH;
            break;
        case SOC_CANCUN_FILE_ID_CFH:
            ccf->type = CANCUN_SOC_FILE_TYPE_CFH;
            break;
        case SOC_CANCUN_FILE_ID_CEH:
            ccf->type = CANCUN_SOC_FILE_TYPE_CEH;
            break;
        case SOC_CANCUN_FILE_ID_ISPF:
            ccf->type = CANCUN_SOC_FILE_TYPE_ISPF;
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: Invalid file type. Abort\n")));
            return SOC_E_INTERNAL;
    }

    /* CANCUN file validation */
    if((cc->flags & SOC_CANCUN_FLAG_SKIP_VALIDITY) == 0) {
        /* 3. File length */
        if(ccfh->file_length != BYTES2WORDS(buf_bytes)) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: File length mismatch. Abort\n")));
            return SOC_E_INTERNAL;
        }
        /* 4. HW version */
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if(!soc_cancun_chip_rev_validate(ccfh->chip_rev_id, dev_id, rev_id)) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: HW version mismatch. Abort\n")));
            return SOC_E_INTERNAL;
        }
        /* 5. CRC */
        file_crc = (uint32 *) (buf + buf_bytes - 4);
        if (crc != *file_crc) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: CRC check fails (crc 0x%08x, file_crc 0x%08x. Abort\n"), crc, *file_crc));
            return SOC_E_INTERNAL;
        }
    }

    ccf->valid = 1;
    ccf->format = CANCUN_SOC_FILE_FORMAT_PACK;
    ccf->status = CANCUN_SOC_FILE_LOAD_NONE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_file_load
 * Purpose:
 *      Loading a CANCUN loadable file into device or internal structures. This
 *      function also reports file type and format if input content is in
 *      packaged format, or it keep them unchanged
 * Parameters:
 *      unit        - (IN) Unit ID
 *      buf         - (IN) Pointer to buffer of a CANCUN loadable file
 *      buf_bytes   - (IN) Length of buffer in bytes
 *      type        - (INOUT) File type
 *      format      - (INOUT) File format
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNIT  - Invalid unit ID
 *      SOC_E_PARAM - Invalid input buffer
 *      SOC_E_BADID - File validity check fails
 *      SOC_E_FAIL  - Loading fails
 */

int soc_cancun_file_load(int unit, uint8* buf, long buf_bytes, uint32* type,
                          uint32* format) {
    soc_cancun_t *cc;
    soc_cancun_file_t ccf_file, *ccf;
    uint32 status = SOC_CANCUN_LOAD_STATUS_NOT_LOADED;
    int rv = SOC_E_NONE;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    } else if (buf == NULL) {
        return SOC_E_PARAM;
    }

    /* Get file information if input PACK or UNKNOWN format */
    sal_memset(&ccf_file, 0, sizeof(soc_cancun_file_t));
    if (*format == CANCUN_SOC_FILE_FORMAT_PACK ||
        *format == CANCUN_SOC_FILE_FORMAT_UNKNOWN) {
        rv = soc_cancun_file_info_get(unit, &ccf_file, NULL, buf, buf_bytes);
        if(rv == SOC_E_NONE) {
            *type = ccf_file.type;
            *format = ccf_file.format;

            if(*format == CANCUN_SOC_FILE_FORMAT_PACK) {
                buf += SOC_CANCUN_FILE_HEADER_OFFSET;
                buf_bytes -= (SOC_CANCUN_FILE_HEADER_OFFSET + 1);
            } else {
                return SOC_E_BADID;
            }
        } else {
            return rv;
        }
    }

    if(*type == CANCUN_SOC_FILE_TYPE_CIH) {
        soc_cancun_file_branch_id_e branch_id = 0;
        ccf = &cc->cih->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        if(*format == CANCUN_SOC_FILE_FORMAT_PIO) {
            cc->cih->status = status;
            rv = _soc_cancun_file_pio_load(unit, buf,
                                           (buf_bytes/BYTES_PER_UINT32));

        } else if(*format == CANCUN_SOC_FILE_FORMAT_PACK) {
            if(SOC_WARM_BOOT(unit)) {
                rv = SOC_E_NONE;
            } else {
                cc->cih->status = status;
                rv = _soc_cancun_cih_load(unit, buf,
                                      ccf_file.header.num_data_blobs);
            }

        } else {
            return SOC_E_PARAM;
        }

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->cih->version = ccf_file.header.version;
            cc->flags |= SOC_CANCUN_FLAG_CIH_LOADED;

        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_CIH_LOADED;
        }
        cc->cih->status = status;
        rv = soc_cancun_branch_id_get(unit,
                       CANCUN_SOC_FILE_TYPE_CIH, &branch_id);
        if (branch_id == CANCUN_FILE_BRANCH_ID_HGoE) {
            SOC_FEATURE_SET(unit, soc_feature_higig_over_ethernet);
        }


    } else if (*type == CANCUN_SOC_FILE_TYPE_CMH) {
        ccf = &cc->cmh->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        cc->cmh->status = status;
        rv = _soc_cancun_file_cmh_load(unit, buf, (buf_bytes/BYTES_PER_UINT32));

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->cmh->version = ccf_file.header.version;
            cc->cmh->sdk_version = ccf_file.header.sdk_version;
            cc->flags |= SOC_CANCUN_FLAG_CMH_LOADED;
        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_CMH_LOADED;
        }
        cc->cmh->status = status;

    } else if (*type == CANCUN_SOC_FILE_TYPE_CCH) {
        ccf = &cc->cch->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        cc->cch->status = status;
        rv = _soc_cancun_file_cch_load(unit, buf, (buf_bytes/BYTES_PER_UINT32));

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->cch->version = ccf_file.header.version;
            cc->cch->sdk_version = ccf_file.header.sdk_version;
            cc->flags |= SOC_CANCUN_FLAG_CCH_LOADED;
        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_CCH_LOADED;
        }
        cc->cch->status = status;

    } else if (*type == CANCUN_SOC_FILE_TYPE_CEH) {
        ccf = &cc->ceh->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        cc->ceh->status = status;
        rv = _soc_cancun_file_ceh_load(unit, buf, (buf_bytes/BYTES_PER_UINT32));

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->ceh->version = ccf_file.header.version;
            cc->ceh->sdk_version = ccf_file.header.sdk_version;
            cc->flags |= SOC_CANCUN_FLAG_CEH_LOADED;
        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_CEH_LOADED;
        }
        cc->ceh->status = status;

    } else if (*type == CANCUN_SOC_FILE_TYPE_CFH)  {
        ccf =  &cc->flow_db->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        cc->flow_db->status = status;
        rv = _soc_flow_db_load(unit, buf, (buf_bytes/BYTES_PER_UINT32));

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->flow_db->version = ccf_file.header.version;
            cc->flags |= SOC_CANCUN_FLAG_CFH_LOADED;
        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_CFH_LOADED;
        }
        cc->flow_db->status = status;

    } else if (*type == CANCUN_SOC_FILE_TYPE_ISPF) {
        ccf = &cc->ispf->file;
        status = SOC_CANCUN_LOAD_STATUS_IN_PROGRESS;
        if(*format == CANCUN_SOC_FILE_FORMAT_PIO) {
            cc->ispf->status = status;
            rv = _soc_cancun_file_pio_load(unit, buf,
                    (buf_bytes/BYTES_PER_UINT32));

        } else if(*format == CANCUN_SOC_FILE_FORMAT_PACK) {
            if(SOC_WARM_BOOT(unit)) {
                rv = SOC_E_NONE;
            } else {
                cc->ispf->status = status;
                /* For ISPF header, the sdk_version field will contain
                 * the ISPF version number instead */
                cc->ispf->ispf_version = ccf_file.header.sdk_version;
                rv = _soc_cancun_cih_load(unit, buf,
                                      ccf_file.header.num_data_blobs);
            }

        } else {
            return SOC_E_PARAM;
        }

        if(rv == SOC_E_NONE) {
            status = SOC_CANCUN_LOAD_STATUS_LOADED;
            cc->ispf->version = ccf_file.header.version;
            cc->flags |= SOC_CANCUN_FLAG_ISPF_LOADED;

        } else {
            status = SOC_CANCUN_LOAD_STATUS_FAILED;
            cc->flags &= ~SOC_CANCUN_FLAG_ISPF_LOADED;
        }
        cc->ispf->status = status;
    } else {
        LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "ERROR: can't recognize file type enum %d\n"), *type));
        return SOC_E_PARAM;
    }

    /* File successfully loaded here. Update ccf */
    if(ccf) {
        sal_memcpy(ccf, &ccf_file, sizeof(soc_cancun_file_t));
        ccf->status = status;
    }

    return rv;
}


/*
 * Function:
 *      soc_cancun_cmh_list
 * Purpose:
 *      Return the array that contain memory and field pairs covered in CMH
 * Parameters:
 *      unit        - (IN) Unit ID
 *      cmh_list    - (INOUT) pointer to a buffer to hold CMH list.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
 *
 * Note: Output format of CANCUN coverage list
 *      +-------------------+-----------------+--------------+--------------+
 *      | mem_num           | mem_1           | field_num    | field_1      |
 *      +-------------------+-----------------+--------------+--------------+
 *      | field_2           | ...             | mem_n        | field_num_n  |
 *      +-------------------+-----------------+--------------+--------------+
 *      | field_n1          | field_n2        | ...          |              |
 *      +-------------------+-----------------+--------------+--------------+
*/
int soc_cancun_cmh_list (int unit, uint32** cmh_list) {
    soc_cancun_t *pcc = soc_cancun_info[unit];
    soc_cancun_cmh_t *cmh;

    if(pcc == NULL) {
        return SOC_E_UNIT;
    } else if (!(pcc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    cmh = pcc->cmh;
    if(cmh != NULL) {
        if(cmh->cmh_list != NULL) {
            *cmh_list = cmh->cmh_list;
            return SOC_E_NONE;
        }
    }
    return SOC_E_INIT;
}

/*
 * Function:
 *      soc_cancun_cmh_check
 * Purpose:
 *      Check if a mem/field is covered by CMH mapping
 * Parameters:
 *      unit  - (IN) Unit ID
 *      mem   - (IN) Memory ID
 *      field - (IN) Field ID
 * Returns:
 *      SOC_E_NONE - input mem/field is covered by CMH
 *      SOC_E_FAIL - input mem/field is NOT covered by CMH
 *
 * Note: For cmh_list, the first word is the number of the entries, and each two
 *       words after it is the pair of memory and field pair in enumeration
 *      +-------------------+-----------------+--------------+--------------+
 *      | entry_num         | mem_1           | field_1      | app_1        |
 *      +-------------------+-----------------+--------------+--------------+
 *      | ...               | mem_n           | field_n      | app_n        |
 *      +-------------------+-----------------+--------------+--------------+
*/
int soc_cancun_cmh_check (int unit, soc_mem_t mem, soc_field_t field) {
    soc_cancun_t *pcc = soc_cancun_info[unit];
    soc_cancun_cmh_t *cmh;
    uint32 entry_num = 0, i;
    uint32 *entry = NULL;

    if(pcc == NULL) {
        return SOC_E_UNIT;
    } else if (!(pcc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    cmh = pcc->cmh;
    if(cmh != NULL) {
        if(cmh->cmh_list != NULL) {
            entry = cmh->cmh_list;
            entry_num = *entry++;
            for(i = 0; i < entry_num; i++, entry += SOC_CANCUN_CMH_LIST_ENTRY_SIZE) {
                if(mem == *(entry + SOC_CANCUN_CMH_LIST_ENTRY_OFFSET_MEM) &&
                   field == *(entry + SOC_CANCUN_CMH_LIST_ENTRY_OFFSET_FIELD)) {
                    LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "%s.%s is covered by CMH\n"),
                            SOC_MEM_NAME(unit, mem),
                            SOC_FIELD_NAME(unit, field)));
                    return SOC_E_NONE;
                }
            }
            return SOC_E_FAIL;
        }
    }
    return SOC_E_INIT;
}

/*
 * Function:
 *      soc_cancun_cmh_mem_set
 * Purpose:
 *      Configure memory field values according to internal CMH tables
 * Parameters:
 *      unit  - (IN) Unit ID
 *      mem   - (IN) Memory ID. Input INVALIDm if not applied
 *      index - (IN) Memory entry index
 *      field - (IN) Field ID. Input INVALIDf if not applied
 *      input - (IN) This parameter can be field value or application
 *
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
 *      SOC_E_INTERNAL  - CMH tables are not loaded
 *      SOC_E_PARAM     - Input parameters are invalid
 *      SOC_E_NOT_FOUND - Input mem/reg/field are not found in CMH
 *
 * Note:
 *      CMH entry format is shown as following. For more details, please refer
 *      to section 4.1.1 in document "Trident3 SOC Layer Support for CANCUN".
 *
 *      +-------------------+-----------------+--------------+--------------+
 *      | entry flags/size  | src_mem         | src_field    | src_value_num|
 *      +-------------------+-----------------+--------------+--------------+
 *      | /removed/         | dest_mem_num    | src_val_0    | ...          |
 *      +-------------------+-----------------+--------------+--------------+
 *      | src_val_n         | des_mem_0       | field_num_0  | field_0      |
 *      +-------------------+-----------------+--------------+--------------+
 *      | ...               | field_m         | dest_val_00  | ...          |
 *      +-------------------+-----------------+--------------+--------------+
 *      | dest_val_0m       | dest_val_10     | ...          | dest_val_1m  |
 *      +-------------------+-----------------+--------------+--------------+
 *      | dest_mem_1        | ...                                           |
 *      +-------------------+-----------------------------------------------+
 */
int soc_cancun_cmh_mem_set (int unit, soc_mem_t mem, int index,
                            soc_field_t field, uint32 input) {
    soc_cancun_t* cc;
    soc_cancun_cmh_t* cmh;
    soc_cancun_cmh_map_t* cmh_map;
    soc_cancun_hash_table_t* hash_table_header;
    uint32 *hash_table_entry, *p;
    uint32 entry_num, value_ind, mem_num, field_num, value_num;
    uint32 i, j, k, offset, key, app;
    soc_mem_t d_mem;
    soc_field_t fields[SOC_MAX_MEM_FIELD_WORDS];
    uint32 values[SOC_MAX_MEM_FIELD_WORDS];
    int rv, dest_index = index;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    /* deal with EGR_PORT.PORT_TYPE covered by CMH format 2 */
    if(mem == EGR_PORTm && field == PORT_TYPEf) {
        switch(input) {
        case 0: /* Ethernet */
            if(IS_CPU_PORT(unit, index)) {
                app = CANCUN_APP__EGR_PORT__CPU_ETHERNET_PORT;
            } else {
                app = CANCUN_APP__EGR_PORT__ETHERNET;
            }
            break;
        case 1: /* HG2 */
            if(IS_CPU_PORT(unit, index)) {
                app = CANCUN_APP__EGR_PORT__CPU_HG2_PORT;
            } else {
                app = CANCUN_APP__EGR_PORT__HG2_PORT;
            }
            break;
        case 2: /* Loopback */
            app = CANCUN_APP__EGR_PORT__LOOPBACK;
            break;
        case 4: /* Cascade */
            app = CANCUN_APP__EGR_PORT__CASCADED_PORT;
            break;
        default:
            return SOC_E_PARAM;
            break;
        };
        rv = soc_cancun_app_dest_entry_set (unit, mem, index, field, app, input);
        
        
        (void)soc_cancun_app_dest_entry_set (unit, EGR_PORTm, index, INVALIDf, CANCUN_APP__EGR_PORT__QOS_LAYERED_RESOLUTION, 0);
        
        return rv;
    }

    cmh = cc->cmh;
    hash_table_header = (soc_cancun_hash_table_t*) cmh->cmh_table;
    hash_table_entry = &hash_table_header->table_entry;

    rv = _soc_cancun_hash(hash_table_header, mem, field, &key);
    if(rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if(offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    p = hash_table_entry + offset;
    entry_num = *p++;
    cmh_map = (soc_cancun_cmh_map_t*) p;
    if (entry_num != 0) {
        for(i = 0; i < entry_num; i++) {
            if(cmh_map->src_mem == mem && cmh_map->src_field == field &&
               (cmh_map->format == 1 || cmh_map->format == 5)) {

                /* check if value is in value list */
                value_num = cmh_map->src_value_num;
                p = &(cmh_map->dest_map_entry);
                for(j = 0; j < value_num; j++) {
                    if(input == *(p+j)) {
                        break;
                    }
                }

                if(j >= value_num) {
                    return SOC_E_PARAM;
                } else {
                    value_ind = j;
                }

                /* writing mapped mem/field */
                mem_num = cmh_map->dest_mem_num;
                p = &(cmh_map->dest_map_entry) + value_num;
                for(j = 0; j < mem_num; j++) {
                    d_mem = *p++;
                    if(cmh_map->format == 5) {
                        dest_index = *p++;
                    } else {
                        dest_index = index;
                    }
                    field_num = *p++;
                    for(k = 0; k < field_num; k++) {
                        fields[k] = *p++;
                    }
                    p += value_ind * field_num;
                    for(k = 0; k < field_num; k++) {
                        values[k] = *p++;
                    }

                    rv = soc_mem_fields32_modify(unit, d_mem, dest_index, field_num,
                                                 fields, values);

                    /* Debug log */
                    if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                        LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                            "For index %d, mem %s, field %s, value %d\nCMH writes:\n\tmem:   %s[%d]\n"),
                             index,
                             SOC_MEM_NAME(unit, mem),
                             SOC_FIELD_NAME(unit, field), input,
                             SOC_MEM_NAME(unit, d_mem),
                             dest_index));

                        for(k = 0; k < field_num; k++) {
                            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                                "\tfield: %s, value: %d\n"),
                                 SOC_FIELD_NAME(unit, fields[k]), values[k]));
                        }
                    }
                }
                return SOC_E_NONE;
            }
            cmh_map = (soc_cancun_cmh_map_t *) \
                      ((uint32 *)cmh_map + cmh_map->entry_size);
        }
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NOT_FOUND;
}

int soc_cancun_cmh_mem_get (int unit, soc_mem_t mem, int index,
                            soc_field_t field, uint32* value) {

    /* No deprecated fields for now. No need to be implement */

    return SOC_E_NONE;
}

STATIC
int _soc_cancun_ceh_hash_key_get(int unit, soc_cancun_hash_table_t *hash_tbl,
                                      char *obj_name,
                                      uint32 *key)
{
    char *cp;
    uint32 len;
    uint32 hash = 0;

    if ((hash_tbl == NULL) ||
        (key == NULL)) {
        return SOC_E_PARAM;
    }
    /* hash function */
    cp = obj_name;
    len = sal_strlen(obj_name);
    if (len == 0) {
        return SOC_E_PARAM;
    }
    while (*cp != '\0') {
        hash += *cp++;
    }

    *key = (hash_tbl->pb * hash) % hash_tbl->pd;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_ceh_obj_field_get
 * Purpose:
 *      Get the Cancun encoded object field definition
 * Parameters:
 *      unit        - (IN) Unit ID
 *      obj_name    - (IN) object name
 *      field_name  - (IN) object field name
 *      field_info  - (OUT) Retrieved field information
 * Returns:
 *      SOC_E_XXX
 */
int soc_cancun_ceh_obj_field_get (int unit, char *obj_name,
                                      char *field_name,
                                      soc_cancun_ceh_field_info_t *info)
{

    soc_cancun_t* cc;
    soc_cancun_ceh_t* ceh;
    soc_cancun_hash_table_t* hash_table_header;
    uint32 *entry_location_tbl;
    uint32 entry_num;
    uint32 i, j, offset, key;
    uint32 *curr_p;
    soc_cancun_ceh_object_t *obj_p;
    soc_cancun_ceh_field_t  *fld_p;
    char *str_mem_offset;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    ceh = cc->ceh;
    hash_table_header = SOC_CANCUN_CEH_HASH_HDR_TBL(ceh->ceh_table);
    entry_location_tbl = SOC_CANCUN_CEH_OBJ_ENTRY_LOC_TBL(ceh->ceh_table);

    /* hash key generation. The hash function must be same as in CEH */
    SOC_IF_ERROR_RETURN(_soc_cancun_ceh_hash_key_get(unit,hash_table_header,
                                      obj_name, &key));

    /* the hash key is the index to the entry location table where each location
     * could have one or more entries with the same hash key value. The location
     * is represented by an offset value from the start of entry_location_tbl
     * to the start of the entry.
     */
    offset = *(entry_location_tbl + key);
    if(offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    /* point to the location where the first entry starts */
    curr_p = entry_location_tbl + offset;
    entry_num = *curr_p++;
    if (!entry_num) {
        return SOC_E_NOT_FOUND;
    }
    /* start of string offset location */
    str_mem_offset = (char *)SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);

    for (i = 0; i < entry_num; i++) {
        obj_p = (soc_cancun_ceh_object_t *)curr_p;
        fld_p = (soc_cancun_ceh_field_t *)(obj_p + 1);
        if (sal_strcmp(obj_name, str_mem_offset + obj_p->name_addr) == 0) {
            /* found object */
            for (j = 0; j < obj_p->num_fields; j++) {
                if (sal_strcmp(field_name,
                    str_mem_offset + (fld_p + j)->name_addr) == 0) {
                    info->offset = (fld_p + j)->offset;
                    info->width  = (fld_p + j)->width;
                    info->value  = (fld_p + j)->value;
                    info->flags  = 0;
                    return SOC_E_NONE;
                }
            }
        }
        /* next entry with the same hash key */
        curr_p = (uint32 *)(fld_p + obj_p->num_fields);
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_cancun_ceh_obj_field_id_list_get
 * Purpose:
 *      Get a list of field IDs for the given object 
 * Parameters:
 *      unit         - (IN) Unit ID
 *      obj_name     - (IN) object name
 *      size         - (IN/OUT) size of field_id array buffer 
 *      field_id_arr - (OUT) field_id array buffer 
 * Returns:
 *      SOC_E_XXX
 */
int soc_cancun_ceh_obj_field_id_list_get (int unit, char *obj_name,
                                      int *size,
                                      uint32 *field_id_arr)
{

    soc_cancun_t* cc;
    soc_cancun_ceh_t* ceh;
    soc_cancun_hash_table_t* hash_table_header;
    uint32 *entry_location_tbl;
    uint32 entry_num;
    uint32 i, j, offset, key;
    uint32 *curr_p;
    soc_cancun_ceh_object_t *obj_p;
    soc_cancun_ceh_field_t  *fld_p;
    char *str_mem_offset;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    ceh = cc->ceh;
    hash_table_header = SOC_CANCUN_CEH_HASH_HDR_TBL(ceh->ceh_table);
    entry_location_tbl = SOC_CANCUN_CEH_OBJ_ENTRY_LOC_TBL(ceh->ceh_table);

    /* hash key generation. The hash function must be same as in CEH */
    SOC_IF_ERROR_RETURN(_soc_cancun_ceh_hash_key_get(unit,hash_table_header,
                                      obj_name, &key));

    /* the hash key is the index to the entry location table where each location
     * could have one or more entries with the same hash key value. The location
     * is represented by an offset value from the start of entry_location_tbl
     * to the start of the entry.
     */
    offset = *(entry_location_tbl + key);
    if(offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    /* point to the location where the first entry starts */
    curr_p = entry_location_tbl + offset;
    entry_num = *curr_p++;
    if (!entry_num) {
        return SOC_E_NOT_FOUND;
    }
    /* start of string offset location */
    str_mem_offset = (char *)SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);

    for (i = 0; i < entry_num; i++) {
        obj_p = (soc_cancun_ceh_object_t *)curr_p;
        fld_p = (soc_cancun_ceh_field_t *)(obj_p + 1);
        if (sal_strcmp(obj_name, str_mem_offset + obj_p->name_addr) == 0) {
            /* found object */
            if (field_id_arr == NULL) {
                /* return size only */
                *size = obj_p->num_fields;
                return SOC_E_NONE;
            } 
            for (j = 0; (j < obj_p->num_fields) && (j < *size); j++) {
                field_id_arr[j] = (fld_p + j)->name_addr;
            }
            return SOC_E_NONE;
        }
        /* next entry with the same hash key */
        curr_p = (uint32 *)(fld_p + obj_p->num_fields);
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_cancun_ceh_obj_field_id_get
 * Purpose:
 *      Get the field ID for the given field name 
 * Parameters:
 *      unit        - (IN) Unit ID
 *      field_name  - (IN) object field name
 *      field_id  - (OUT) Retrieved field id
 * Returns:
 *      SOC_E_XXX
 */
int soc_cancun_ceh_obj_field_id_get (int unit,
                                      char *field_name,
                                      uint32 *field_id)
{

    soc_cancun_t* cc;
    soc_cancun_ceh_t* ceh;
    uint32 offset;
    char *str_tbl;
    uint32 str_tbl_size;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    ceh = cc->ceh;

    str_tbl_size = *SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);

    /* start of string offset location */
    str_tbl = (char *)SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);
    offset  = 4;
    while ((offset < str_tbl_size) && (*(str_tbl + offset) != 0)) {
        if (sal_strcmp(field_name, str_tbl + offset) == 0) {
            *field_id = offset;
            return SOC_E_NONE;
        }
        offset += sal_strlen(str_tbl + offset) + 1;
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_cancun_ceh_obj_field_name_get
 * Purpose:
 *      Get the object field name for the given field_id
 * Parameters:
 *      unit        - (IN) Unit ID
 *      field_id    - (IN) field id
 *      size        - (IN) size of the field name buffer
 *      field_name  - (OUT) field name buffer
 * Returns:
 *      SOC_E_XXX
 */
int soc_cancun_ceh_obj_field_name_get (int unit,
                                      uint32 field_id,
                                      int *size,
                                      char *field_name)
{

    soc_cancun_t* cc;
    soc_cancun_ceh_t* ceh;
    char *str_tbl;
    uint32 str_tbl_size;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    ceh = cc->ceh;

    str_tbl_size = *SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);

    /* start of string offset location */
    str_tbl = (char *)SOC_CANCUN_CEH_STR_NAME_TBL(ceh->ceh_table);

    /* validate field_id */
    if ((field_id < 4) || (field_id >= str_tbl_size) ) {
        return SOC_E_NOT_FOUND;
    }
    /* return size */
    if (field_name == NULL) {
        *size = sal_strlen(str_tbl + field_id) + 1;
        return SOC_E_NONE;
    }

    if (sal_strlen(str_tbl + field_id) < *size) {
       sal_strcpy(field_name, str_tbl + field_id);
    } else {
       sal_strncpy(field_name, str_tbl + field_id, *size - 1);
       field_name[*size - 1] = 0;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_app_dest_entry_set
 * Purpose:
 *      Write to destination fields according to source table and application
 * Parameters:
 *      unit        - (IN) Unit ID
 *      mem         - (IN) Memory ID or Register ID
 *      app         - (IN) CANCUN application ID
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL - input mem/app is NOT covered by CANCUN
*/
int soc_cancun_app_dest_entry_set (int unit, soc_mem_t mem, int index, soc_field_t field, int app, uint32 input) {
    soc_cancun_t *cc;
    soc_cancun_cmh_t *cmh;
    soc_cancun_cmh_map_t *cmh_map = NULL;
    soc_cancun_hash_table_t *hash_table_header;
    uint32 *hash_table_entry, *hash_entry, *p;
    uint32 d_mem, d_field_num, key, offset, value_ind=0, value_num = 0;
    int i, j, rv, cmh_d_mem_is_reg = 0;
    soc_field_t fields[SOC_MAX_MEM_FIELD_WORDS];
    uint32 values[SOC_MAX_MEM_FIELD_WORDS];

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_CMH_LOADED)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CMH file is not loaded\n")));
        return SOC_E_INIT;
    }

    cmh = cc->cmh;
    hash_table_header = (soc_cancun_hash_table_t*) cmh->cmh_table;
    hash_table_entry = &hash_table_header->table_entry;
    rv = _soc_cancun_hash(hash_table_header, mem, app, &key);
    if(rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if(offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    hash_table_entry = &hash_table_header->table_entry + offset;
    hash_entry = _soc_cancun_hash_find_entry(hash_table_entry, mem, field, app);
    if(hash_entry == NULL) {
        return SOC_E_NOT_FOUND;
    }
    cmh_map = (soc_cancun_cmh_map_t *) hash_entry;

    p = &cmh_map->dest_map_entry;
    if(cmh_map->format == 3 || cmh_map->format == 2) {
        if(cmh_map->format == 2) {
            value_num = cmh_map->src_value_num;
            for(i = 0; i < value_num; i++) {
                if(input == *(p+i)) {
                    break;
                }
            }

            if(i >= value_num) {
                return SOC_E_PARAM;
            } else {
                value_ind = i;
            }
            p += value_num;
        }
        
        for(i = 0; i < cmh_map->dest_mem_num; i++) {
            d_mem = *p++;
            d_field_num = *p++;
            for(j = 0; j < d_field_num; j++) {
                fields[j] = *p++;
            }
            if(cmh_map->format == 2) {
                p += value_ind * d_field_num;
            }
            for(j = 0; j < d_field_num; j++) {
                values[j] = *p++;
            }
            if(cmh_map->format == 2) {
                p += (value_num - value_ind - 1) * d_field_num;
            }

            cmh_d_mem_is_reg = (d_mem & SOC_CANCUN_FLAG_REG_ENUM)? 1: 0;
            
            if(cmh_d_mem_is_reg) {
                d_mem &= ~(SOC_CANCUN_FLAG_REG_ENUM);
                rv = soc_reg_fields32_modify(unit, d_mem, REG_PORT_ANY, 
                                    d_field_num, fields, values);
            } else {
                rv = soc_mem_fields32_modify(unit, d_mem, index, d_field_num,
                                         fields, values);
            }

            if(rv != SOC_E_NONE) {
                return rv;
            }

            /* Debug log */
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "For index %d, mem %s, app %d\nCMH writes:\n\t%s:   %s\n"),
                     index,
                     SOC_MEM_IS_VALID(unit, mem)? SOC_MEM_NAME(unit, mem):
                                                  SOC_REG_NAME(unit,mem),
                     app,
                     cmh_d_mem_is_reg? "reg": "mem",
                     cmh_d_mem_is_reg? SOC_REG_NAME(unit, d_mem): 
                                       SOC_MEM_NAME(unit, d_mem)));

                for(j = 0; j < d_field_num; j++) {
                    LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "\tfield: %s, value: %d\n"),
                         SOC_FIELD_NAME(unit, fields[j]), values[j]));
                }
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CMH format %d is not supported for soc_cancun_app_dest_entry_set() yet\n"),
            cmh_map->format));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_app_dest_entry_get
 * Purpose:
 *      Return CANCUN destination entry by input source table and application
 * Parameters:
 *      unit        - (IN) Unit ID
 *      mem         - (IN) Memory ID or Register ID
 *      app         - (IN) CANCUN application ID
 *      dest_entry  - (OUT) CANCUN destination information entry
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL - input mem/app is NOT covered by CMH
*/
int soc_cancun_app_dest_entry_get (int unit, soc_mem_t mem, int index, soc_field_t field, int app,
                                   soc_cancun_dest_entry_t *dest_entry) {

    soc_cancun_t *cc;
    soc_cancun_cmh_t *cmh;
    soc_cancun_cmh_map_t *cmh_map = NULL;
    soc_cancun_hash_table_t *hash_table_header;
    uint32 *hash_table_entry, *hash_entry, *p;
    uint32 key, offset;
    int i, rv;

    cc = soc_cancun_info[unit];
    if(cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_CMH_LOADED)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CMH file is not loaded\n")));
        return SOC_E_INIT;
    }

    cmh = cc->cmh;
    hash_table_header = (soc_cancun_hash_table_t*) cmh->cmh_table;
    hash_table_entry = &hash_table_header->table_entry;
    rv = _soc_cancun_hash(hash_table_header, mem, app, &key);
    if(rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if(offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    hash_table_entry = &hash_table_header->table_entry + offset;
    hash_entry = _soc_cancun_hash_find_entry(hash_table_entry, mem, field, app);
    if(hash_entry == NULL) {
        return SOC_E_NOT_FOUND;
    }
    cmh_map = (soc_cancun_cmh_map_t *) hash_entry;

    p = &cmh_map->dest_map_entry;
    if(cmh_map->format == 3) {
        if(cmh_map->dest_mem_num > 1) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "Only 1 destination memory for CMH format 3 for now\n")));
            return SOC_E_INTERNAL;
        }
        dest_entry->dest_index_num = 0;
        dest_entry->dest_mems[0] = *p++;
        dest_entry->dest_field_num = *p++;
        if (dest_entry->dest_field_num > CANCUN_DEST_FIELD_NUM_MAX) {
            return SOC_E_INTERNAL;
        }
        for(i = 0; i < dest_entry->dest_field_num; i++) {
            dest_entry->dest_fields[i] = *p++;
            dest_entry->dest_values[i] = *p++;
        }
    } else if(cmh_map->format == 4) {
        if(cmh_map->dest_mem_num > CANCUN_DEST_MEM_NUM_MAX) {
            return SOC_E_INTERNAL;
        }
        dest_entry->dest_index_num = cmh_map->dest_mem_num;
        dest_entry->dest_field_num = 0;
        for(i = 0; i < dest_entry->dest_index_num; i++) {
            dest_entry->dest_mems[i] = *p++;
            dest_entry->dest_values[i] = *p++;
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CMH format %d is not supported yet\n"), cmh_map->format));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cancun_cch_list
 * Purpose:
 *      Return the memory and field list of CCH coverage
 * Parameters:
 *      unit        - (IN) Unit ID
 *      buf         - (INOUT) buffer to load CCH coverage list
 *      buf_len     - (IN) the length of passing buffer
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
 *
 * Note: Output format of CANCUN coverage list
 *      +-------------------+-----------------+--------------+--------------+
 *      | mem_num           | mem_1           | field_num    | field_1      |
 *      +-------------------+-----------------+--------------+--------------+
 *      | field_2           | ...             | mem_n        | field_num_n  |
 *      +-------------------+-----------------+--------------+--------------+
 *      | field_n1          | field_n2        | ...          |              |
 *      +-------------------+-----------------+--------------+--------------+
*/
int soc_cancun_cch_list (int unit, uint32* buf, uint32 buf_len) {
    soc_cancun_t *pcc = soc_cancun_info[unit];
    soc_cancun_cch_t *cch;
    soc_cancun_hash_table_t *hash_table;
    soc_cancun_list_t *p_mem, *p_fld;
    uint32 *p1, *p2, *hash_table_entry, *buf_end;
    uint32 i, j, key = 0;
    int rv;

    if(pcc == NULL) {
        return SOC_E_UNIT;
    } else if (!(pcc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    } else if (buf == NULL) {
            return SOC_E_PARAM;
    }

    buf_end = buf + buf_len - 1;
    cch = pcc->cch;
    if(cch != NULL) {
        hash_table = (soc_cancun_hash_table_t*) cch->cch_table;
        hash_table_entry = &hash_table->table_entry;

        /* mem list is always at key 0 */
        p1 = hash_table_entry + *(hash_table_entry + 0);
        p1 = _soc_cancun_hash_find_entry(p1, 0, INVALIDf, CANCUN_APP_INVALID);
        if(p1 == NULL) {
            return SOC_E_INTERNAL;
        }

        p_mem = (soc_cancun_list_t*) p1;
        if(buf > buf_end) {
            return SOC_E_MEMORY;
        }
        *buf++ = p_mem->member_num;
        p1 = &p_mem->members;
        for(i = 0; i < p_mem->member_num; i++) {
            if(buf > buf_end) {
                return SOC_E_MEMORY;
            }
            *buf++ = *p1;
            rv = _soc_cancun_hash(hash_table, *p1, 0, &key);
            if(rv != SOC_E_NONE) {
                return rv;
            }
            p2 = hash_table_entry + *(hash_table_entry + key);
            p2 = _soc_cancun_hash_find_entry(p2, *p1, SOC_CANCUN_FIELD_LISTf, 
                                             CANCUN_APP_INVALID);
            if(p2 == NULL) {
                return SOC_E_INTERNAL;
            }

            p_fld = (soc_cancun_list_t*) p2;
            *buf++ = p_fld->member_num;
            p2 = &p_fld->members;
            for(j = 0; j < p_fld->member_num; j++) {
                if(buf > buf_end) {
                    return SOC_E_MEMORY;
                }
                *buf++ = *p2++;
            }
            p1++;
        }

        return SOC_E_NONE;
    }
    return SOC_E_INIT;
}

int soc_cancun_cch_reg_set(int unit, soc_reg_t reg, int index, uint64 data) {

    soc_cancun_t *cc = NULL;
    soc_cancun_cch_t *cch = NULL;
    soc_cancun_cch_map_t *cch_map = NULL;
    soc_cancun_hash_table_t *hash_table_header = NULL;
    soc_cancun_list_t *field_list = NULL;
    uint32 *hash_table_entry = NULL, *p_field = NULL;
    uint32 *hash_entry = NULL, *p = NULL;
    uint32  l, key = 0, offset, val_idx = 0;
    uint64 rval, orval, odata64;
    uint64 p64;
    int rv;

    cc = soc_cancun_info[unit];
    if (cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_CCH_LOADED)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CCH file is not loaded\n")));
        return SOC_E_INIT;
    }

    /* dealing with array register */
    if (SOC_REG_ARRAY(unit, reg)) {
        assert(index >= 0 && index < 2 * SOC_REG_NUMELS(unit, reg));
        reg += (index + 1);
    }

    cch = cc->cch;
    hash_table_header = (soc_cancun_hash_table_t*) cch->cch_table;
    hash_table_entry = &hash_table_header->table_entry;
    rv = _soc_cancun_hash(hash_table_header, reg, 0, &key);
    if (rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    hash_table_entry = &hash_table_header->table_entry + offset;

    /* Obtain the source field list to write */
    field_list = (soc_cancun_list_t *) \
                 _soc_cancun_hash_find_entry(hash_table_entry, reg,
                     SOC_CANCUN_FIELD_LISTf, CANCUN_APP_INVALID);
    if (field_list == NULL) {
        LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "can't find field list for %s\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_NOT_FOUND;
    }

    /* Go through CCH covered field list for this register */
    if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
        rv = soc_cancun_pseudo_reg_get(unit, reg, &odata64);
    } else {
        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &odata64);
    }
    if (rv != SOC_E_NONE) {
        return rv;
    }

    p_field = &field_list->members;
    for (l = 0; l < field_list->member_num; l++, p_field++) {
        hash_table_entry = &hash_table_header->table_entry;
        rv = _soc_cancun_hash(hash_table_header, reg, *p_field, &key);
        if(rv != SOC_E_NONE) {
            return rv;
        }
        offset = *(hash_table_entry + key);
        if (offset == 0) {
            return SOC_E_NOT_FOUND;
        }
        hash_table_entry = &hash_table_header->table_entry + offset;
        hash_entry = _soc_cancun_hash_find_entry(hash_table_entry, reg,
                                                 *p_field, CANCUN_APP_INVALID);
        if (hash_entry == NULL) {
            return SOC_E_NOT_FOUND;
        }
        cch_map = (soc_cancun_cch_map_t *) hash_entry;

        /* Get CCH map entry for current source field */
        p = &cch_map->dest_map_entry;
        orval = soc_reg64_field_get(unit, reg, odata64, *p_field);
        rval = soc_reg64_field_get(unit, reg, data, *p_field);
        if (COMPILER_64_EQ(orval, rval)) {
            /* No value change on this field. Skip */
            continue;
        }
        if (cch_map->src_value_num != 0) {
            for (val_idx = 0; val_idx < cch_map->src_value_num; val_idx++) {
                COMPILER_64_SET(p64, 0, *p++);
                if (COMPILER_64_EQ(rval, p64)) {
                    break;
                }
            }
            if (val_idx >= cch_map->src_value_num) {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "can't find source value %0X_%0X for (%s, %s)\n"),
                        COMPILER_64_HI(data),
                        COMPILER_64_LO(data),
                        SOC_REG_NAME(unit, reg),
                        SOC_FIELD_NAME(unit, *p_field)));
                return SOC_E_NOT_FOUND;
            }
        }

        if (cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "For reg %s, field %s, value 0x%08X_%08X, CCH writes:\n"),
                 SOC_REG_NAME(unit, reg),
                 SOC_FIELD_NAME(unit, *p_field),
                 COMPILER_64_HI(rval),
                 COMPILER_64_LO(rval)));
        }

        SOC_IF_ERROR_RETURN(_soc_cancun_cch_dest_set_with_idx (unit, cch_map, rval, val_idx));
     }

    return SOC_E_NONE;
}

int soc_cancun_pseudo_reg_set(int unit, soc_reg_t reg, uint64 data) {

    soc_cancun_t* cc = soc_cancun_info[unit];
    soc_cancun_cch_t *cch;
    uint64 *preg, *flags;
    uint64 flags_read64;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    } else {
        cch = cc->cch;
    }

    if(cch->pseudo_regs == NULL) {
        return SOC_E_INIT;
    }

    if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
        
        flags = cch->pseudo_regs + (SOC_REG_INFO(unit, reg).offset & 0xFFFF)/
                (sizeof(uint64)/sizeof(uint8));
        preg = flags + 1;
        COMPILER_64_SET(flags_read64, 0, SOC_CANCUN_PSEUDO_REGS_FLAGS_VALID_READ);
        COMPILER_64_OR(*flags, flags_read64);
        *preg = data;
        return SOC_E_NONE;
    } else {
        return SOC_E_PARAM;
    }
}

int soc_cancun_pseudo_reg_get(int unit, soc_reg_t reg, uint64 *data) {

    soc_cancun_t* cc = soc_cancun_info[unit];
    soc_cancun_cch_t *cch;
    uint64 *preg, *flags;
    uint64 rval64, flags_valid_read64;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    } else {
        cch = cc->cch;
    }


    if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
        
        flags = cch->pseudo_regs + (SOC_REG_INFO(unit, reg).offset & 0xFFFF)/
                (sizeof(uint64)/sizeof(uint8));
        preg = flags + 1;
        COMPILER_64_SET(flags_valid_read64, 0, SOC_CANCUN_PSEUDO_REGS_FLAGS_VALID_READ);
        COMPILER_64_AND(*flags, flags_valid_read64);
        /* Load RESET value at first read */
        
        if (COMPILER_64_IS_ZERO(*flags)) {
            COMPILER_64_SET(rval64, SOC_REG_INFO(unit, reg).rst_val_hi,
                                    SOC_REG_INFO(unit, reg).rst_val_lo);
            COMPILER_64_OR(*flags, flags_valid_read64);
            *preg = rval64;
        }
        *data = *preg;
        return SOC_E_NONE;
    } else {
        return SOC_E_PARAM;
    }
}

int _soc_cancun_cch_dest_set_with_idx (int unit,
        soc_cancun_cch_map_t  *cch_map, uint64 data, uint32 val_idx) {

    soc_cancun_t *cc = soc_cancun_info[unit];
    uint32 *p = NULL, *p_val = NULL;
    uint32 i, j, k, bit_offset;
    uint32 d_ind_num, d_fld_num, d_mem, d_ind, d_fld;
    uint32 fld_max, fld_min, fld_bits, fld_words;
    uint32 entbuf[SOC_MAX_MEM_WORDS] = {0};
    uint32 fldbuf[SOC_MAX_MEM_WORDS] = {0};
    uint64 rval, rdata64, fdata64;
    int cch_d_mem_is_reg;
    soc_field_info_t *finfop = NULL;
    soc_mem_info_t *meminfo = NULL;
    uint32 rvalbuf[2];
    uint32 p_shift = 0;
    uint32 idx_fld_num;

    if (cch_map == NULL) {
        return SOC_E_PARAM;
    }

    COMPILER_64_COPY(rval, data);

    p = &cch_map->dest_map_entry + cch_map->src_value_num;
    for (i = 0; i < cch_map->dest_mem_num; i++) {
        d_mem = *p++;
        d_ind_num = *p++;
        d_fld_num = *p++;
        for (j = 0; j < d_ind_num; j++) {
            d_ind = *p++;
            idx_fld_num = d_fld_num;
            if (d_mem & SOC_CANCUN_FLAG_REG_ENUM) {
                cch_d_mem_is_reg = 1;
                d_mem &= ~SOC_CANCUN_FLAG_REG_ENUM;
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, d_mem, REG_PORT_ANY, 0, &rdata64));
            } else {
                if (cch_map->format == 2) {
                    /* retrieve number of fields per index */
                    if (d_ind >> 16) {
                        idx_fld_num = d_ind >> 16;
                        d_ind &= 0xffff;
                    }
                }
                cch_d_mem_is_reg = 0;
                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, d_mem, MEM_BLOCK_ANY, d_ind, entbuf));
            }

            for (k = 0; k < idx_fld_num; k++) {
                d_fld = *p++;
                fld_max = *p >> 16 & 0xFFFF;
                fld_min = *p++ & 0xFFFF;
                fld_bits = fld_max - fld_min + 1;
                fld_words = BITS2WORDS(fld_bits);
                p_shift = 2;
                if (cch_map->src_value_num != 0) {
                    p_val = p + (val_idx * fld_words);
                    p += cch_map->src_value_num * fld_words;
                    COMPILER_64_SET(fdata64, 0, *p_val);
                    p_shift += cch_map->src_value_num * fld_words;
                } else {
                    rvalbuf[0] = COMPILER_64_LO(rval);
                    rvalbuf[1] = COMPILER_64_HI(rval);
                    p_val = &rvalbuf[0];
                    COMPILER_64_COPY(fdata64, rval);
                }

                if (cch_d_mem_is_reg == 1) {
                    SOC_FIND_FIELD(d_fld,
                                   SOC_REG_INFO(unit, d_mem).fields,
                                   SOC_REG_INFO(unit, d_mem).nFields,
                                   finfop);
                    if (finfop == NULL) {
                        return SOC_E_INTERNAL;
                    }
                    if (finfop->len != fld_bits) {
                        uint64 rfdata64;
                        uint64 fmask;

                        rfdata64 = soc_reg64_field_get(unit, d_mem, rdata64,
                                        d_fld);
                        bit_offset = fld_min - finfop->bp;
                        COMPILER_64_MASK_CREATE(fmask, fld_bits, 0);
                        COMPILER_64_AND(fdata64, fmask);
                        COMPILER_64_SHL(fdata64, bit_offset);
                        COMPILER_64_SHL(fmask, bit_offset);
                        COMPILER_64_NOT(fmask);
                        COMPILER_64_AND(rfdata64, fmask);
                        COMPILER_64_OR(fdata64, rfdata64);
                    }
                    soc_reg64_field_set(unit, d_mem, &rdata64, d_fld, fdata64);

                    if (cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                        LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                            "\t%s.%s = 0x%08X_%08X\n"),
                             SOC_REG_NAME(unit, d_mem),
                             SOC_FIELD_NAME(unit, d_fld),
                             COMPILER_64_HI(fdata64),
                             COMPILER_64_LO(fdata64)));
                    }
                } else {
                    /* Bit shift if needed */
                    
                    meminfo = &SOC_MEM_INFO(unit, d_mem);
                    SOC_FIND_FIELD(d_fld,
                                   meminfo->fields,
                                   meminfo->nFields,
                                   finfop);
                    if (finfop == NULL) {
                        LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                            "\t special field num detected: %d,%d \n"), d_fld_num, k));
                        p -= p_shift;

                        break;
                    }

                    soc_mem_field_get(unit, d_mem, entbuf, d_fld, fldbuf);

                    if (finfop->len != fld_bits) {
                        bit_offset = fld_min - finfop->bp;
                        SHR_BITCOPY_RANGE(fldbuf, bit_offset, p_val, 0, fld_bits);
                    } else {
                        SHR_BITCOPY_RANGE(fldbuf, 0, p_val, 0, fld_bits);
                    }

                    soc_mem_field_set(unit, d_mem, entbuf, d_fld, fldbuf);

                    if (cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                        char *fldbuf_str = sal_alloc(sizeof(char) *
                                 ((SOC_MAX_MEM_FIELD_WORDS * 8) + 3),
                                 "soc_cancun_debug_cch_str");
                        if (fldbuf_str != NULL) {
                            _shr_format_long_integer(fldbuf_str, fldbuf,
                                                     BITS2BYTES(finfop->len));

                            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                                "\t%s[%d].%s = %s\n"),
                                 SOC_MEM_NAME(unit, d_mem),
                                 d_ind,
                                 SOC_FIELD_NAME(unit, d_fld),
                                 fldbuf_str));
                            sal_free(fldbuf_str);
                        }
                    }
                }
            }

            if (cch_d_mem_is_reg == 1) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, d_mem,
                    REG_PORT_ANY, 0, rdata64));
            } else {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, d_mem,
                    MEM_BLOCK_ALL, d_ind, entbuf));
            }
        }
    }

    return SOC_E_NONE;

}

int _soc_cancun_cch_dest_set (int unit,
        soc_cancun_cch_map_t *cch_map, uint64 data) {
        return _soc_cancun_cch_dest_set_with_idx (unit, cch_map, data, 0x0);
}

/*
 * Function:
 *      soc_cancun_cch_app_set
 * Purpose:
 *      Configure memory field values according to application
 * Parameters:
 *      unit  - (IN) Unit ID
 *      src_app   - (IN) Source application ID
 *      data - (IN) Memory field value
 *
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
 *      SOC_E_INTERNAL  - CCH tables are not loaded
 *      SOC_E_PARAM     - Input parameters are invalid
 *      SOC_E_NOT_FOUND - Input mem/reg/field are not found in CCH
 *
 * Note:
 *      CCH entry format 4 is shown as following. *
 *      +----------+-----------------+-------------------+--------------+
 *      | size     | format          | src_mem           | src_field    |
 *      +----------+-----------------+-------------------+--------------+
 *      | src_app  | val_num         | dest_tab_num      | ...          |
 *      +----------+-----------------+-------------------+--------------+
 *      | dest_tab | dest_index_num  | dest_field_num    | ...          |
 *      +----------+-----------------+-------------------+--------------+
 *      | index    | dest_field      | field_max_min_bit | ...          |
 *      +----------+-----------------+-------------------+--------------+
 */
int soc_cancun_cch_app_set (int unit, uint32 src_app, uint64 data) {

    soc_cancun_t *cc = NULL;
    soc_cancun_cch_t *cch = NULL;
    soc_cancun_cch_map_t *cch_map = NULL;
    soc_cancun_hash_table_t *hash_table_header = NULL;
    uint32 *hash_table_entry = NULL;
    uint32 *hash_entry = NULL;
    uint32 offset = 0;
    uint32 key = 0;
    int rv = 0;

    cc = soc_cancun_info[unit];
    if (cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_CCH_LOADED)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CCH file is not loaded\n")));
        return SOC_E_INIT;
    }

    cch = cc->cch;
    hash_table_header = (soc_cancun_hash_table_t*) cch->cch_table;
    hash_table_entry = &hash_table_header->table_entry;
    rv = _soc_cancun_hash (hash_table_header, 0, src_app, &key);
    if (rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    hash_table_entry = &hash_table_header->table_entry + offset;
    hash_entry = _soc_cancun_hash_find_entry(hash_table_entry, INVALIDm,
                                             INVALIDf, src_app);
    if (hash_entry == NULL) {
        return SOC_E_NOT_FOUND;
    }

    cch_map = (soc_cancun_cch_map_t *) hash_entry;
    SOC_IF_ERROR_RETURN(_soc_cancun_cch_dest_set (unit, cch_map, data));

    return SOC_E_NONE;
}


int _soc_cancun_load_status_clear(int unit, soc_cancun_file_type_e file_type) {
    soc_cancun_t *cc = soc_cancun_info[unit];
    uint32 *status;

    if(cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else {
        status = NULL;
        switch (file_type) {
            case CANCUN_SOC_FILE_TYPE_CIH:
                status = &cc->cih->status;
                break;
            case CANCUN_SOC_FILE_TYPE_CMH:
                status = &cc->cmh->status;
                break;
            case CANCUN_SOC_FILE_TYPE_CCH:
                status = &cc->cch->status;
                break;
            default:
                return SOC_E_PARAM;
        }
    }

    if(status == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN %s is not loaded\n"),
            soc_chip_type_names[file_type]));
        return SOC_E_INIT;
    }

    *status = SOC_CANCUN_LOAD_STATUS_NOT_LOADED;

    return SOC_E_NONE;
}

int soc_cancun_access_scan(int unit, soc_cancun_file_type_e file_type) {
    soc_cancun_t *cc = soc_cancun_info[unit];
    soc_cancun_cmh_t *cmh = NULL;
    soc_cancun_cch_t *cch = NULL;
    soc_cancun_cmh_map_t *cmh_map = NULL;
    soc_cancun_cch_map_t *cch_map = NULL;
    soc_cancun_hash_table_t *hash_table_header;
    uint32 *hash_table_entry, *p;
    uint32 i, j, k, l, offset, entry_num, field_num, index_num, dest_mem, pass = 1;

    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    }

    if(file_type == CANCUN_SOC_FILE_TYPE_CMH) {
        cmh = cc->cmh;
        hash_table_header = (soc_cancun_hash_table_t*) cmh->cmh_table;
        for(i = 0; i < hash_table_header->pd; i++) {
            hash_table_entry = &hash_table_header->table_entry;
            offset = *(hash_table_entry + i);
            if(offset == 0) {
                continue;
            }
            hash_table_entry = &hash_table_header->table_entry + offset;
            entry_num = *hash_table_entry++;
            cmh_map = (soc_cancun_cmh_map_t *) hash_table_entry;
            for(j = 0; j < entry_num; j++) {
                if (cmh_map->format == 1) {
                    pass &= _soc_cancun_enum_check(unit, 0, cmh_map->src_mem,
                                                   SOC_CANCUN_ENUM_TYPE_MEM);
                    pass &= _soc_cancun_enum_check(unit, cmh_map->src_mem,
                                               cmh_map->src_field, 
                                               SOC_CANCUN_ENUM_TYPE_MEM_FIELD);
                    p = &cmh_map->dest_map_entry + cmh_map->src_value_num;
                    for(k = 0; k < cmh_map->dest_mem_num; k++) {
                        dest_mem = *p++;
                        pass &= _soc_cancun_enum_check(unit, 0, dest_mem,
                                               SOC_CANCUN_ENUM_TYPE_MEM);
                        field_num = *p++;
                        for(l = 0; l < field_num; l++) {
                            pass &= _soc_cancun_enum_check(unit, dest_mem, *p++,
                                                SOC_CANCUN_ENUM_TYPE_MEM_FIELD);
                        }
                        p += field_num * cmh_map->src_value_num;
                    }
                /* TO-DO: Add format 2~4 */
                } else {
                    LOG_DEBUG(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, 
                              "Scan for CMH format %d is not yet supported\n"),
                              cmh_map->format));
                    continue;
                }
            }
        }
    } else if(file_type == CANCUN_SOC_FILE_TYPE_CCH) {
        cch = cc->cch;
        hash_table_header = (soc_cancun_hash_table_t*) cch->cch_table;
        for(i = 0; i < hash_table_header->pd; i++) {
            hash_table_entry = &hash_table_header->table_entry;
            offset = *(hash_table_entry + i);
            if(offset == 0) {
                continue;
            }
            hash_table_entry = &hash_table_header->table_entry + offset;
            entry_num = *hash_table_entry++;
            cch_map = (soc_cancun_cch_map_t *) hash_table_entry;
            for(j = 0; j < entry_num; j++) {
                if(cch_map->src_mem != 0 && cch_map->src_field != 0) {
                    pass &= _soc_cancun_enum_check(unit, 0, cch_map->src_mem,
                            SOC_CANCUN_ENUM_TYPE_REG);
                    if (cch_map->format == 1 ||
                        cch_map->format == 2 ||
                        cch_map->format == 3) {
                        pass &= _soc_cancun_enum_check(unit, cch_map->src_mem,
                                               cch_map->src_field,
                                               SOC_CANCUN_ENUM_TYPE_REG_FIELD);
                        p = &cch_map->dest_map_entry + cch_map->src_value_num;
                        for(k = 0; k < cch_map->dest_mem_num; k++) {
                            dest_mem = *p++;
                            pass &= _soc_cancun_enum_check(unit, 0, dest_mem,
                                                   SOC_CANCUN_ENUM_TYPE_MEM);
                            index_num = *p++;
                            field_num = *p;
                            p += 2;
                            for(l = 0; l < field_num; l++) {
                                pass &= _soc_cancun_enum_check(unit, dest_mem, *p,
                                                    SOC_CANCUN_ENUM_TYPE_MEM_FIELD);
                                p += (cch_map->src_value_num + 2);
                            }
                            if(index_num > 1) {
                                p += (index_num - 1) * field_num *
                                     (2 + cch_map->src_value_num) + 1;
                            }
                        }
                    } else {
                        LOG_DEBUG(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                                  "Scan for CCH format %d is not yet supported\n"),
                                  cch_map->format));
                        continue;
                    }
                }
                hash_table_entry += cch_map->entry_size;
                cch_map = (soc_cancun_cch_map_t *) hash_table_entry;
            }
        }
    } else {
        return SOC_E_PARAM;
    }

    if(pass) {
        return SOC_E_NONE;
    } else {
        return SOC_E_FAIL;
    }
}

/*
 * Internal Functions
 */
static int _soc_cancun_alloc(soc_cancun_t** cancun) {
    soc_cancun_t* cc;

    cc = sal_alloc(sizeof(soc_cancun_t), "soc_cancun_info");
    if(cc == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc, 0, sizeof(soc_cancun_t));

    cc->cih = sal_alloc(sizeof(soc_cancun_cih_t), "soc_cancun_cih");
    if(cc->cih == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc->cih, 0, sizeof(soc_cancun_cih_t));

    cc->cmh = sal_alloc(sizeof(soc_cancun_cmh_t), "soc_cancun_cmh");
    if(cc->cmh == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc->cmh, 0, sizeof(soc_cancun_cmh_t));

    cc->cch = sal_alloc(sizeof(soc_cancun_cch_t), "soc_cancun_cch");
    if(cc->cch == NULL) {
        goto _soc_cancun_alloc_error;
    }

    sal_memset(cc->cch, 0, sizeof(soc_cancun_cch_t));
    cc->flow_db = sal_alloc(sizeof(soc_flow_db_t), "soc_flow_db");
    if(cc->flow_db == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc->flow_db, 0, sizeof(soc_flow_db_t));

    cc->ceh = sal_alloc(sizeof(soc_cancun_ceh_t), "soc_cancun_ceh");
    if(cc->ceh == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc->ceh, 0, sizeof(soc_cancun_ceh_t));

    cc->ispf = sal_alloc(sizeof(soc_cancun_ispf_t), "soc_cancun_ispf");
    if(cc->cih == NULL) {
        goto _soc_cancun_alloc_error;
    }
    sal_memset(cc->ispf, 0, sizeof(soc_cancun_ispf_t));

    *cancun = cc;

    return 0;

_soc_cancun_alloc_error:
    if(cc) {
        if(cc->cih) {
            sal_free(cc->cih);
        }
        if(cc->cmh) {
            sal_free(cc->cmh);
        }
        if(cc->cch) {
            sal_free(cc->cch);
        }
        if (cc->flow_db) {
            sal_free(cc->flow_db);
        }
        if(cc->ceh) {
            sal_free(cc->ceh);
        }
        if(cc->ispf) {
            sal_free(cc->ispf);
        }
        sal_free(cc);
    }

    return -1;
}

static int _soc_cancun_file_pio_load(int unit, uint8* buf, int buf_words) {
    long schan_msg_len_words, msg_num, i;
    schan_msg_t* msg;
    int index;

    schan_msg_len_words = CMIC_SCHAN_WORDS(unit);
    msg_num = buf_words / schan_msg_len_words;
    msg = (schan_msg_t*) buf;

    for(i = 0; i < msg_num; i++) {
#if _PCID_TEST
        {
            int j;
            for(j = 0; j< schan_msg_len_words; j++) {
                msg->dwords[j] = soc_htonl(msg->dwords[j]);
                cli_out("0x%x ", msg->dwords[j]);
            }
            cli_out("\n");
            index = 0;
        }
#else
        index = soc_schan_op(unit, msg, schan_msg_len_words,
                             schan_msg_len_words, 0);
#endif
        if (index < 0) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "S-Channel operation failed: %s\n"), soc_errmsg(index)));
            return SOC_E_FAIL;
        }
        msg++;
    }

    return SOC_E_NONE;
}

static int _soc_cancun_file_cmh_load(int unit, uint8* buf, int buf_words) {
    soc_cancun_cmh_t *cmh = soc_cancun_info[unit]->cmh;

    if(cmh == NULL) {
        return SOC_E_INIT;
    } else if (buf_words <= 0) {
        return SOC_E_PARAM;
    }

    /* NOTE: We do not have sal_realloc in our libc so need to free and then
     *       re-alloc here for a new CMH load*/
    if(cmh->cmh_table) {
        sal_free(cmh->cmh_table);
    }
    cmh->cmh_table = sal_alloc((BYTES_PER_UINT32 * buf_words),
                               "soc_cancun_cmh_table");
    if(cmh->cmh_table == NULL) {
        return SOC_E_MEMORY;
    }

    _soc_cancun_memcpy_letohl((uint32 *)cmh->cmh_table, (uint32 *)buf, buf_words);

    return _soc_cancun_cmh_list_update(cmh);

}

static int _soc_cancun_cmh_list_update(soc_cancun_cmh_t *cmh) {
    soc_cancun_hash_table_t *hash_table;
    soc_cancun_cmh_map_t *cmh_map_entry;
    uint32 *p_hask_key, *p_entry, *p_list;
    uint32 entry_num, list_element_count = 0;
    int i;

    hash_table = (soc_cancun_hash_table_t*) cmh->cmh_table;

    if(cmh->cmh_list) {
        sal_free(cmh->cmh_list);
    }
    cmh->cmh_list = sal_alloc((BYTES_PER_UINT32 *
                              (hash_table->entry_num * 3 + 1)),
                              "soc_cancun_cmh_list");
    if(cmh->cmh_list == NULL) {
        return SOC_E_MEMORY;
    }

    p_hask_key = &hash_table->table_entry;
    p_list = cmh->cmh_list + 1;
    for(i = 0; i < hash_table->pd; i++) {
        if(p_hask_key[i] != 0) {
            p_entry = p_hask_key + p_hask_key[i];
            entry_num = *p_entry++;
            cmh_map_entry = (soc_cancun_cmh_map_t*) p_entry;
            while(entry_num-- > 0) {
                *p_list++ = cmh_map_entry->src_mem;
                *p_list++ = cmh_map_entry->src_field;
                *p_list++ = cmh_map_entry->src_app;
                list_element_count++;
                if(entry_num > 0) {
                    p_entry += cmh_map_entry->entry_size;
                    cmh_map_entry = (soc_cancun_cmh_map_t*) p_entry;
                }
            }
        }
    }
    *cmh->cmh_list = list_element_count;

    return SOC_E_NONE;
}

static int _soc_cancun_file_cch_load(int unit, uint8* buf, int buf_words) {
    soc_cancun_cch_t *cch = soc_cancun_info[unit]->cch;

    if(cch == NULL) {
        return SOC_E_INIT;
    } else if (buf_words <= 0) {
        return SOC_E_PARAM;
    }

    /* NOTE: We do not have sal_realloc in our libc so need to free and then
     *       re-alloc here for a new CCH load*/
    if(cch->cch_table) {
        sal_free(cch->cch_table);
    }
    cch->cch_table = sal_alloc((BYTES_PER_UINT32 * buf_words),
                               "soc_cancun_cch_table");
    if(cch->cch_table == NULL) {
        return SOC_E_MEMORY;
    }

    if(cch->pseudo_regs) {
        sal_free(cch->pseudo_regs);
    }
    cch->pseudo_regs = (uint64*) sal_alloc(SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE,
                                           "soc_cancun_cch_pseudo_regs");
    if(cch->pseudo_regs == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(cch->pseudo_regs, 0, SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        (void)soc_cancun_scache_recovery(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    _soc_cancun_memcpy_letohl((uint32 *)cch->cch_table, (uint32 *)buf, buf_words);

    return SOC_E_NONE;

}

static int _soc_cancun_file_ceh_load(int unit, uint8* buf, int buf_words)
{
    uint8 *cur_dst;
    uint8 *cur_src;
    int cur_len;
    int str_tbl_len;
    int rem_len;
    soc_cancun_ceh_t *ceh = soc_cancun_info[unit]->ceh;

    if(ceh == NULL) {
        return SOC_E_INIT;
    } else if (buf_words <= 0) {
        return SOC_E_PARAM;
    }

    /* NOTE: We do not have sal_realloc in our libc so need to free and then
     *       re-alloc here for a new CEH load*/
    if(ceh->ceh_table) {
        sal_free(ceh->ceh_table);
    }
    ceh->ceh_table = sal_alloc((BYTES_PER_UINT32 * buf_words),
                               "soc_cancun_ceh_table");
    if(ceh->ceh_table == NULL) {
        return SOC_E_MEMORY;
    }

    cur_dst = (uint8 *)ceh->ceh_table;
    cur_src = buf;
    cur_len = SOC_CANCUN_CEH_BLOCK_HASH_HEADER_LEN * BYTES_PER_UINT32;
    rem_len = buf_words * BYTES_PER_UINT32;
    
    /* load block and hash header */
    sal_memcpy(cur_dst, cur_src,cur_len);
    soc_cancun_buf_swap32(cur_dst, cur_len);

    /* next, string table len */
    cur_dst += cur_len;
    cur_src += cur_len; 
    rem_len -= cur_len;
    cur_len = BYTES_PER_UINT32;
    sal_memcpy(cur_dst, cur_src,cur_len);
    soc_cancun_buf_swap32(cur_dst, cur_len);

    /* no endianness to convert for string table. Just copy */
    str_tbl_len = *(uint32 *)cur_dst; 
    cur_dst += cur_len;
    cur_src += cur_len; 
    rem_len -= cur_len;
    cur_len = str_tbl_len - BYTES_PER_UINT32;
    sal_memcpy(cur_dst,cur_src, cur_len);

    /* convert rest of buffer */   
    cur_dst += cur_len;
    cur_src += cur_len; 
    rem_len -= cur_len;
    sal_memcpy(cur_dst, cur_src,rem_len);
    soc_cancun_buf_swap32(cur_dst, rem_len);

    return SOC_E_NONE;
}

static int _soc_cancun_hash(soc_cancun_hash_table_t* t, uint32 mem,
                            soc_field_t field, uint32* key) {

    if((t == NULL) || (key == NULL)) {
        return SOC_E_PARAM;
    }

    /* generalized hash function */
    *key = (t->pa * field + t->pa * mem + t->pc) % t->pd;

    return SOC_E_NONE;
}

static int _soc_cancun_cih_load(int unit, uint8* buf, int num_data_blobs) {
    int i;
    uint32 length, flags;

    for (i = 0; i < num_data_blobs; i++) {
        /* Length and flags fields are offset by 8 and 12 bytes from start of
         * each data blob */
        length = *(uint32 *)(buf + SOC_CANCUN_CIH_LENGTH_OFFSET);
        flags = *(uint32 *)(buf + SOC_CANCUN_CIH_FLAG_OFFSET);

        /* Blob data format */
        switch (flags & SOC_CANCUN_BLOB_FORMAT_MASK) {
            case SOC_CANCUN_BLOB_FORMAT_PIO:
                SOC_IF_ERROR_RETURN
                    (_soc_cancun_cih_pio_load(unit, buf, length, flags));
                buf += (sizeof(uint32) * SOC_CANCUN_CIH_PIO_DATA_BLOB_SIZE);
                break;
            case SOC_CANCUN_BLOB_FORMAT_DMA:
                break;
            case SOC_CANCUN_BLOB_FORMAT_FIFO:
                break;
            case SOC_CANCUN_BLOB_FORMAT_RSVD:
                break;
            default:
                return SOC_E_PARAM;
        }
    }
    return SOC_E_NONE;
}

static int _soc_cancun_cih_pio_load(int unit, uint8* buf, int length,
                                    uint32 flags) {
    schan_msg_t msg;
    int i, index;
    uint32 *p = (uint32 *) buf;
    int rv;

    sal_memset(&msg, 0, sizeof(schan_msg_t));

    if (flags & SOC_CANCUN_BLOB_FLAG_MEM_ID_PRESENT) {
        return _soc_cancun_cih_mem_load(unit, buf);
    }

    /* Special case of TCAM memory loading */
    if (flags & SOC_CANCUN_BLOB_FLAG_TCAM) {
        rv = _soc_cancun_cih_tcam_write(unit, buf);
        return rv;
    }

    /* Copy destination address to schan msg structure */
    msg.dwords[1] = *p++;

    /* Copy opcode to schan msg structure */
    msg.dwords[0] = *p++;

    /* Copy data to schan msg structure */
    p += 2; /* skip length and flag */
    for(i = 0; i < length ; i++) {
        msg.dwords[2+i] = *p++;
    }

    for(i = 0; i < length + 2; i++) {
        LOG_VERBOSE(BSL_LS_SOC_CANCUN,
                    (BSL_META_U(unit, "0x%x "), msg.dwords[i]));
    }
    LOG_VERBOSE(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "\n")));

#if _PCID_TEST
    {
        int j;
        for(j = 0; j < length+2; j++) {
            msg.dwords[j] = soc_htonl(msg.dwords[j]);
            cli_out("0x%x ", msg.dwords[j]);
        }
        cli_out("\n");
        index = 0;
    }
#else
    index = soc_schan_op(unit, &msg, length+2, length+2, 0);
#endif

    if (index < 0) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "S-Channel operation failed: %s\n"), soc_errmsg(index)));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;

}

static int _soc_cancun_cih_tcam_write(int unit, uint8 *buf) {
#define SOC_SBUS_V4_BLOCK_ID_BIT_OFFSET     (19)
#define SOC_SBUS_V4_BLOCK_ID_BIT_MASK       (0x7F)
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 index;
    uint32 offset;
    uint32 block = -1;
    uint32 len;
    uint32 *p = (uint32*) buf;

    if(!buf) {
        return SOC_E_INTERNAL;
    }

    if (soc_feature(unit, soc_feature_xy_tcam)) {
        if (soc_feature(unit, soc_feature_sbus_format_v4)) {
            block = (*(p + SOC_CANCUN_BLOB_OPCODE_OFFSET) >>
                     SOC_SBUS_V4_BLOCK_ID_BIT_OFFSET)
                    & SOC_SBUS_V4_BLOCK_ID_BIT_MASK;
        } else {
            return SOC_E_UNAVAIL;
        }

        mem = soc_addr_to_mem_extended(unit, block, -1, *p);
        if(mem == INVALIDm) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "can't find TCAM memory for addr = 0x%x\n"), *p));
            return SOC_E_INTERNAL;
        }

        index = SOC_MEM_INFO(unit, mem).index_min +
                ((*p - SOC_MEM_INFO(unit, mem).base) /
                  SOC_MEM_INFO(unit, mem).gran);
        if(index > SOC_MEM_INFO(unit, mem).index_max) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "ERROR: index = %d exceeds max of %s (%d)\n"), index,
                    SOC_MEM_NAME(unit, mem),
                    SOC_MEM_INFO(unit, mem).index_max));
            return SOC_E_INTERNAL;
        }

        offset = *p - (SOC_MEM_INFO(unit, mem).base +
                       (index * SOC_MEM_INFO(unit, mem).gran));

        len = *(p + SOC_CANCUN_BLOB_LEN_OFFSET);

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

        _soc_cancun_memcpy_letohl((entry + offset), (p + SOC_CANCUN_BLOB_DATA_OFFSET), len);

        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

        return SOC_E_NONE;

    } else {
        return SOC_E_UNAVAIL;
    }
}

static int _soc_cancun_cih_mem_load(int unit, uint8 *buf) {
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 addr;
    soc_mem_t mem;
    uint32 index;
    uint32 len;
    uint32 *p = (uint32*)buf;

    addr = *p;
    mem = addr >> SOC_CANCUN_BLOB_ADDR_MEM_ID_SHIFT;
    index = addr & SOC_CANCUN_BLOB_ADDR_MEM_IDX_MASK;

    len = *(p + SOC_CANCUN_BLOB_LEN_OFFSET);
    sal_memset(entry, 0, sizeof(entry));
    sal_memcpy(entry, (p + SOC_CANCUN_BLOB_DATA_OFFSET), len*4);

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry);
}

static uint32* _soc_cancun_hash_find_entry(uint32 *hash_entry, uint32 src_mem,
                                           uint32 src_field, int src_app) {

    soc_cancun_entry_hdr_t *p_hdr;
    uint32 entry_num, i;
    uint32 *p;

    if(hash_entry == NULL) {
        return NULL;
    }

    entry_num = *hash_entry;
    p = hash_entry + 1;
    p_hdr = (soc_cancun_entry_hdr_t*) p;

    for(i = 0; i < entry_num; i++) {
        if(p_hdr->src_mem == src_mem) {
        
            if(src_field == INVALIDf ||
               p_hdr->src_field == src_field) {
               
                if(src_app == CANCUN_APP_INVALID ||
                   p_hdr->src_app == src_app) {
                    return p;
                }
            }
        }

        p = p + p_hdr->entry_size;
        p_hdr = (soc_cancun_entry_hdr_t*) p;

    }

    return NULL;
}

static int _soc_cancun_memcpy_letohl(uint32 *des, uint32 *src, uint32 word_len) {
    uint32 i;

    if (des == NULL || src == NULL) {
        return SOC_E_INTERNAL;
    }

    for (i = 0; i < word_len; i++) {
        *des++ = *src++;
    }

    return SOC_E_NONE;
}

static int _soc_cancun_enum_check(int unit, int regmem, int enum_val, int enum_type) {
    if(enum_type == SOC_CANCUN_ENUM_TYPE_APP) {
        if(enum_val >= CANCUN_APP_NUM) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "app: %d\n"), 
                      enum_val));
            return 0;
        }
        return 1;
    }

    if(enum_type == SOC_CANCUN_ENUM_TYPE_MEM) {
        if(!SOC_MEM_IS_VALID(unit, enum_val)) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "%sm\n"), 
                      SOC_MEM_NAME(unit, enum_val)));
            return 0;
        }
        return 1;
    }

    if(enum_type == SOC_CANCUN_ENUM_TYPE_REG) {
        if(!SOC_REG_IS_VALID(unit, enum_val)) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "%sr\n"), 
                      SOC_REG_NAME(unit, enum_val)));
            return 0;
        }
        return 1;
    }

    if(enum_type == SOC_CANCUN_ENUM_TYPE_MEM_FIELD) {
        if(!SOC_MEM_FIELD_VALID(unit, regmem, enum_val)) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "%sm.%sf\n"), 
                SOC_MEM_NAME(unit, regmem), SOC_FIELD_NAME(unit, enum_val)));
            return 0;
        }
        return 1;
    }
    if(enum_type == SOC_CANCUN_ENUM_TYPE_REG_FIELD) {
        if(!SOC_REG_FIELD_VALID(unit, regmem, enum_val)) {
            LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "%sr.%sf\n"), 
                SOC_REG_NAME(unit, regmem), SOC_FIELD_NAME(unit, enum_val)));
            return 0;
        }
        return 1;
    }

    LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "enum type %d not valid\n"), 
                                             enum_type));
    return 0;
}

/* ZLIB CRC-32 routine */
uint32 soc_cancun_crc32(uint32 crc, uint8 *buf, int len_bytes) {

#define DO1(buf) crc = soc_crc32_tab[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

    if (buf == NULL) return 0L;
    crc = crc ^ 0xffffffffL;

    while (len_bytes >= 8)
    {
        DO8(buf);
        len_bytes -= 8;
    }

    if (len_bytes) do {
        DO1(buf);
    } while (--len_bytes);

    return crc ^ 0xffffffffL;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* Version_1_0:
 * 60 registers * 256 bytes each
 * Version_1_1:
 * Newly added: 100 registers * 256 bytes each
 */
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

int
soc_cancun_scache_alloc(int unit)
{
    int stable_size;
    int rv = SOC_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *cancun_scache_ptr;
    uint32 alloc_size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return SOC_E_NONE;
    }

    alloc_size = SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_CANCUN_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_size,
                                          &cancun_scache_ptr,
                                          BCM_WB_DEFAULT_VERSION,
                                          NULL);

    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    return SOC_E_NONE;
}

int
soc_cancun_scache_sync(int unit)
{
    int stable_size;
    int rv = SOC_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *cancun_scache_ptr;
    uint32 alloc_size;
    soc_cancun_cch_t *cch = soc_cancun_info[unit]->cch;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return SOC_E_NONE;
    }

    alloc_size = SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_CANCUN_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          FALSE, &alloc_size,
                                          &cancun_scache_ptr,
                                          BCM_WB_DEFAULT_VERSION,
                                          NULL);

    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    if((cch != NULL) && (cch->pseudo_regs != NULL)) {
        sal_memcpy(cancun_scache_ptr, cch->pseudo_regs, alloc_size);
    }

    return SOC_E_NONE;
}

int
soc_cancun_scache_recovery(int unit)
{
    int stable_size;
    int rv = SOC_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *cancun_scache_ptr;
    uint32 alloc_size;
    uint16  recovered_ver = 0;
    soc_cancun_cch_t *cch = soc_cancun_info[unit]->cch;
    uint32 tmp_size = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return SOC_E_NONE;
    }

    alloc_size = SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_CANCUN_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          FALSE, &tmp_size,
                                          &cancun_scache_ptr,
                                          BCM_WB_DEFAULT_VERSION,
                                          &recovered_ver);

    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    } else if (rv == SOC_E_NOT_FOUND) {
        return soc_cancun_scache_alloc(unit);
    } else {
        if (tmp_size < alloc_size ) {
            SOC_IF_ERROR_RETURN(soc_cancun_scache_alloc(unit));
        }
    }
    if (cancun_scache_ptr != NULL) {
        if ((cch != NULL) && (cch->pseudo_regs != NULL)) {
            if (recovered_ver >= BCM_WB_VERSION_1_1) {
                tmp_size = SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE;
            } else if (recovered_ver >= BCM_WB_VERSION_1_0) {
                tmp_size = SOC_CANCUN_PSEUDO_REGS_BLOCK_BYTE_SIZE_V0;
            }
            sal_memcpy(cch->pseudo_regs, cancun_scache_ptr, tmp_size);
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int soc_cancun_branch_id_get(int unit, uint32 cancun_type,
        soc_cancun_file_branch_id_e *branch_id) {
    int rv = SOC_E_NONE;

    if ((cancun_type < CANCUN_SOC_FILE_TYPE_CIH) ||
        (cancun_type > CANCUN_SOC_FILE_TYPE_ISPF)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "Unsupported cancun type\n")));
        return SOC_E_PARAM;
    }

#ifndef NO_FILEIO
    {
        soc_cancun_t *cc = NULL;
        uint32 version = 0;

        rv = soc_cancun_status_get(unit, &cc);
        if(rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "can't get CANCUN status. (%s)\n"), soc_errmsg(rv)));
            return rv;
        }
        if (cancun_type == CANCUN_SOC_FILE_TYPE_CCH) {
            version = cc->cch->version;
        } else if (cancun_type == CANCUN_SOC_FILE_TYPE_CMH) {
            version = cc->cmh->version;
        } else if (cancun_type == CANCUN_SOC_FILE_TYPE_CEH) {
            version = cc->ceh->version;
        } else if (cancun_type == CANCUN_SOC_FILE_TYPE_CFH) {
            version = cc->flow_db->version;
        } else if (cancun_type == CANCUN_SOC_FILE_TYPE_ISPF) {
            version = cc->ispf->version;
        } else {
            /* return CIH version by default */
            version = cc->cih->version;
        }

        *branch_id = (version >> CANCUN_VERSION_OFFSET_BRANCH_ID) & 0xF;
    }
#else
    {
    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));

    return SOC_E_UNAVAIL;
    }
#endif

    return rv;
}


/*
 * Function:
 *      soc_cancun_udf_abstr_type_info_get
 * Purpose:
 *      Get udf abstact packet type info
 * Parameters:
 *      unit  - (IN) Unit ID
 *      abstract_type   - (IN) abstract type
 *      parser   - (IN) parser identifier
 *      max_stages - (IN) size of stage info array
 *      hfe_prof_ptr_arr - (IN) HFE profile pointer array associated to
 *                              each stage for a given abstract type.
 *      stage_info_arr - (OUT) Array of Stage Info structures
 *      actual_stage_num - (OUT) num of stages associated given abstract type.
 *      hfe_prof_ptr_arr_len - (OUT) HFE prof pointer array len.
 *
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_FAIL
 *      SOC_E_INTERNAL  - UDF DB is not loaded
 *      SOC_E_PARAM     - Input parameters are invalid
 *      SOC_E_NOT_FOUND - stage info is not found
 *
 * Note:
 *      CCH entry format 5 (UDF) is shown as following. *
 *      +------------+-----------------+-------------------+-------------------+
 *      | size       | format          | src_mem(INVALID)  | src_field(INVALID)|
 *      +------------+-----------------+-------------------+-------------------+
 *      | src_app    | stage_info_num  |   size            | policy_mem        |
 *      +------------+-----------------+-------------------+-------------------+
 *      |hfe_pro_ptr | start_pos       |    flags          | size              |
 *      +------------+-----------------+-------------------+-------------------+
 *      | policy_mem | hfe_pro_ptr     |   start_pos       | flags             |
 *      +------------+-----------------+-------------------+-------------------+
 */
int soc_cancun_udf_abstr_type_info_get(int unit,
        uint32 abstract_type,
        uint32 parser,
        uint32 max_stages,
        uint32 **hfe_prof_ptr_arr,
        soc_cancun_udf_stage_info_t *stage_info_arr,
        uint32 *actual_stage_num,
        uint32 *hfe_prof_ptr_arr_len) {

    soc_cancun_t *cc = NULL;
    soc_cancun_cch_t *cch = NULL;
    soc_cancun_hash_table_t *hash_table_header = NULL;
    soc_cancun_cch_map_t *cch_map = NULL;
    uint32 *hash_table_entry = NULL;
    uint32 *hash_entry = NULL;
    uint32 offset = 0;
    uint32 key = 0;
    uint8 stg;
    int rv = 0;
    uint32 src_app = 0;
    uint32 stage_info_num = 0;

    cc = soc_cancun_info[unit];
    if (cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CANCUN is not initialized\n")));
        return SOC_E_INIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_CCH_LOADED)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "CCH file is not loaded\n")));
        return SOC_E_INIT;
    }

    cch = cc->cch;
    hash_table_header = (soc_cancun_hash_table_t*) cch->cch_table;
    hash_table_entry = &hash_table_header->table_entry;
    /* combine parse and abstract type as "srcapp" */
    src_app = (parser << 16) + abstract_type;
    rv = _soc_cancun_hash (hash_table_header, 0, src_app, &key);
    if (rv != SOC_E_NONE) {
        return rv;
    }
    offset = *(hash_table_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    hash_table_entry = &hash_table_header->table_entry + offset;
    hash_entry = _soc_cancun_hash_find_entry(hash_table_entry, INVALIDm,
                                             INVALIDf, src_app);
    if (hash_entry == NULL) {
        return SOC_E_NOT_FOUND;
    }

    cch_map = (soc_cancun_cch_map_t *) hash_entry;

    stage_info_num = cch_map->src_value_num;
    if (actual_stage_num != NULL) {
        *actual_stage_num = stage_info_num;
    }

    if (max_stages > stage_info_num) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "stage_info_num (%d) less than expected (%d)\n"),
            stage_info_num, max_stages));
        return SOC_E_PARAM;
    }

    if (hfe_prof_ptr_arr_len != NULL) {
        uint32 *info_ptr;
        soc_cancun_udf_stage_info_t *abstr_type_info;

        info_ptr = &(cch_map->dest_mem_num);
        for (stg = 0; stg < stage_info_num; stg++) {
            abstr_type_info = (soc_cancun_udf_stage_info_t *)info_ptr;
            hfe_prof_ptr_arr_len[stg] = abstr_type_info->hfe_prof_ptr_arr_len;
            info_ptr += 6;    /* increment till hfe_prof_ptr */
            info_ptr += hfe_prof_ptr_arr_len[stg];
        }
    }

    if (stage_info_arr != NULL) {
        uint8 ct;
        uint32 *info_ptr;
        soc_cancun_udf_stage_info_t *info_arr = stage_info_arr;

        info_ptr = &(cch_map->dest_mem_num);
        for (stg = 0; stg < max_stages; stg++) {
            info_arr->size = *info_ptr;
            info_ptr++;
            info_arr->policy_mem = *info_ptr;
            info_ptr++;
            info_arr->start_pos = *info_ptr;
            info_ptr++;
            info_arr->unavail_contbmap = *info_ptr;
            info_ptr++;
            info_arr->flags = *info_ptr;
            info_ptr++;
            info_arr->hfe_prof_ptr_arr_len = *info_ptr;
            info_ptr++;

            if (hfe_prof_ptr_arr[stg] == NULL) {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                   "NULL hfe_prof_ptr for stage:%d\n"), stg));
                return SOC_E_PARAM;
            }

            info_arr->hfe_profile_ptr = hfe_prof_ptr_arr[stg];
            for (ct = 0; ct < info_arr->hfe_prof_ptr_arr_len; ct++) {
                info_arr->hfe_profile_ptr[ct] = *info_ptr;
                info_ptr++;
            }
            info_arr++;
        }
    }

    return SOC_E_NONE;
}

int soc_cancun_version_get(int unit, uint32* cancun_ver) {
    int rv = SOC_E_NONE;

#ifndef NO_FILEIO
    {
        soc_cancun_t *cc = NULL;

        rv = soc_cancun_status_get(unit, &cc);
        if(rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "can't get CANCUN status. (%s)\n"), soc_errmsg(rv)));
            return rv;
        }
        *cancun_ver = (cc->cih->version & (~CANCUN_VERSION_REVISION_MASK));
    }
#else
    {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));

        return SOC_E_UNAVAIL;
    }
#endif

    return rv;
}

int soc_cancun_revision_get(int unit, uint32* revision) {
    int rv = SOC_E_NONE;

#ifndef NO_FILEIO
    {
        soc_cancun_t *cc = NULL;

        rv = soc_cancun_status_get(unit, &cc);
        if(rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "can't get CANCUN status. (%s)\n"), soc_errmsg(rv)));
            return rv;
        }
        *revision = (cc->cih->version >> CANCUN_VERSION_OFFSET_REVISION) & 0xF;
    }
#else
    {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));

        return SOC_E_UNAVAIL;
    }
#endif

    return rv;
}

/* Selectively, based on the version of the Cancun running, either
 * enable/disable the soc_feature
 */
int soc_cancun_version_soc_feature_bind(int unit) {

    int entry = 0;
    uint32 running_cancun_ver, cancun_ver_low, cancun_ver_high;
    soc_feature_cancun_version_map_t map_entry;

    /* Get running cancun version */
    SOC_IF_ERROR_RETURN(soc_cancun_version_get(unit, &running_cancun_ver));

    for (entry = 0; entry < COUNTOF(soc_feature_cancun_version_map); entry++) {

        map_entry = soc_feature_cancun_version_map[entry];
        cancun_ver_low = map_entry.cancun_ver_low;
        cancun_ver_high = map_entry.cancun_ver_high;

        /* If rule not for current chip, just continue */
        if (SOC_INFO(unit).chip_type != map_entry.chip_type) {
            continue;
        }

        /* Sanity checks */

        /* The low version number cannot be invalid */
        if (map_entry.cancun_ver_low == SOC_CANCUN_VERSION_DEF_INVALID) {
            return SOC_E_INTERNAL;
        }

        /* If the low and high versions are different, RANGE
           is the only range check value allowed, since the
           comparison is automatic, between the two versions */
        if (map_entry.cancun_ver_high != SOC_CANCUN_VERSION_DEF_INVALID) {
            if (map_entry.cancun_ver_low != map_entry.cancun_ver_high) {
                if (map_entry.range_check != CANCUN_VER_CHECK_IN_RANGE) {
                    return SOC_E_INTERNAL;
                }
                /* Also, check that the low is smaller than the high rev */
                if (map_entry.cancun_ver_low > map_entry.cancun_ver_high) {
                    return SOC_E_INTERNAL;
                }
            }
        }

        /* Based on the range checks, enable/disable the features */
        switch (map_entry.range_check) {
            case CANCUN_VER_CHECK_IN_RANGE:

                /* The high version number cannot be invalid, if range check is
                   'NONE' */
                if (map_entry.cancun_ver_high == SOC_CANCUN_VERSION_DEF_INVALID) {
                    return SOC_E_INTERNAL;
                }
                if ((running_cancun_ver >= cancun_ver_low)
                    && (running_cancun_ver <= cancun_ver_high)) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            case CANCUN_VER_CHECK_EQUAL_TO:
                if (running_cancun_ver == cancun_ver_low) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            case CANCUN_VER_CHECK_LESS_THAN:
                if (running_cancun_ver < cancun_ver_low) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            case CANCUN_VER_CHECK_LESS_THAN_OR_EQUAL_TO:
                if (running_cancun_ver <= cancun_ver_low) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            case CANCUN_VER_CHECK_GREATER_THAN:
                if (running_cancun_ver > cancun_ver_low) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            case CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO:
                if (running_cancun_ver >= cancun_ver_low) {
                    /* The relevant feature set to enable/disable below */
                } else {
                    continue;
                }
                break;
            default:
                    return SOC_E_INTERNAL;

            }

        /* Depending on what criteria is met with above rules, either
           enable or the disable the desired feature */
        if (map_entry.feature_enable) {
            SOC_FEATURE_SET(unit, map_entry.soc_feature);
        } else {
            SOC_FEATURE_CLEAR(unit, map_entry.soc_feature);
        }

    }

    return SOC_E_NONE;
}
