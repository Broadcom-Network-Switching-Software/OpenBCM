/*
 * $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flow_db_core.c
 * Purpose:     Flex flow Database core functions.
 *              This module implements core functions
 *              for database loading and accessing.
 * Note:        Utility functions defining the core implementation
 *              in this file are not expected to be called
 *              directly and flow db utilities in flow_db.h
 *              should be used instead by all other modules
 *              since the wrapper utlities does the necessary
 *              validations before calling the core implementation.
 */

#include <soc/esw/flow_db_core.h>
#include <soc/esw/flow_db_enum.h>
#include <soc/esw/cancun.h>
#include <shared/bsl.h>

/* Local defines */
#define BYTES_PER_UINT32    (sizeof(uint32))

#define SOC_FLOW_DB_MAX_LEGACY_FLOWS   \
        (sizeof(legacy_flow_map) / sizeof(legacy_flow_map[0]))

STATIC soc_flow_db_legacy_flow_rec_t legacy_flow_map[] =
{
    {"CLASSIC_VXLAN", SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN},
    {"CLASSIC_L2GRE", SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE},
};

soc_flow_db_flow_map_t *soc_flow_db_flow_map[BCM_MAX_NUM_UNITS];

STATIC int _soc_flow_db_memcpy_letohl(uint32 *des, uint32 *src, uint32 word_len) {
    uint32 i;

    if (des == NULL || src == NULL) {
        return SOC_E_INTERNAL;
    }

    for(i = 0; i < word_len; i++) {
        des[i] = soc_letohl_load(&src[i]);
    }

    return SOC_E_NONE;
}

STATIC int _soc_flow_db_ffo_tpl_hash_key_get(int unit,
                                      soc_flow_db_tbl_map_t *map,
                                      uint32 flow_handle,
                                      uint32 function_id,
                                      uint32 option_id,
                                      uint32 *key)
{
    if ((map == NULL) ||
        (key == NULL)) {
        return SOC_E_PARAM;
    }
    /* hash function */
    *key = ((map->pa * flow_handle) +
            (map->pb * function_id) +
            (map->pc * option_id)) % map->pe;

    return SOC_E_NONE;
}

STATIC int _soc_flow_db_view_hash_key_get(int unit,
                                   soc_flow_db_tbl_map_t *map,
                                   uint32 view_id,
                                   uint32 *key)
{
    if ((map == NULL) ||
        (key == NULL)) {
        return SOC_E_PARAM;
    }
    *key = (map->pa *  view_id);

    return SOC_E_NONE;
}


STATIC int _soc_flow_db_flow_name_hash_key_get(int unit,
                                      soc_flow_db_tbl_map_t *map,
                                      uint32 flow_handle,
                                      const char *name,
                                      uint32 *key)
{
    const char *cp;
    uint32 len = 0;
    uint32 hash = 0;

    if ((map == NULL) ||
        (key == NULL)) {
        return SOC_E_PARAM;
    }
    /* hash function */
    cp = name;
    len = sal_strlen(name);
    if (len == 0) {
        return SOC_E_PARAM;
    }
    do {
        hash += *cp++;
    }while (*cp != '\0');

    *key = ((map->pa * flow_handle) +
            (map->pb * hash)) % map->pe;

    return SOC_E_NONE;

}

void soc_flow_db_view_ffo_list_free(int unit)
{
    soc_flow_db_flow_map_t *fm = NULL;
    soc_flow_db_view_ffo_tuple_t *view_ffo_entry = NULL;
    soc_flow_db_ffo_tuple_t *ffo_tpl = NULL;
    soc_flow_db_ffo_tuple_t *tmp = NULL;
    uint32 num_view_entries = 0;
    int i = 0;

    num_view_entries = SOC_FLOW_DB_NUM_VIEW(unit);
    fm = soc_flow_db_flow_map[unit];
    if (fm->view_ffo_list != NULL) {

        for (i = 0; i < num_view_entries; i++) {
            view_ffo_entry = (fm->view_ffo_list + i);
            if(view_ffo_entry != NULL) {
                ffo_tpl = (soc_flow_db_ffo_tuple_t *) view_ffo_entry->ffo_tuple;
                if (ffo_tpl != NULL) {
                    do {
                        tmp = ffo_tpl;
                        ffo_tpl = (soc_flow_db_ffo_tuple_t *) ffo_tpl->next;
                        sal_free(tmp);
                    } while(ffo_tpl != NULL);
                }
            }

        }
        sal_free(fm->view_ffo_list);
        fm->view_ffo_list = NULL;
    }

}

STATIC int _soc_flow_db_view_ffo_list_update(int unit)
{
    soc_flow_db_flow_map_t *fm = NULL;
    soc_flow_db_tbl_map_t *ffo_tpl_tbl_map = NULL;
    soc_flow_db_ffo_tuple_rec_t *entry = NULL;
    soc_flow_db_view_ffo_tuple_t *view_ffo_entry = NULL;
    soc_flow_db_ffo_tuple_t *ffo_tpl_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    uint32 num_view_entries = 0;
    uint32 id = 0;

    fm = soc_flow_db_flow_map[unit];
    if (fm == NULL) {
       return SOC_E_INTERNAL;
    }

    ffo_tpl_tbl_map = SOC_FLOW_DB_FFO_TPL_TBL_MAP(unit);
    if (ffo_tpl_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    if (fm->view_ffo_list) {
        soc_flow_db_view_ffo_list_free(unit);
    }
    num_view_entries = SOC_FLOW_DB_NUM_VIEW(unit);
    if (num_view_entries == 0) {
        return SOC_E_INTERNAL;
    }
    fm->view_ffo_list = sal_alloc(
                                (num_view_entries *
                                 sizeof(soc_flow_db_view_ffo_tuple_t)),
                                 "view_ffo_list");
    sal_memset(fm->view_ffo_list, 0,(num_view_entries *
                         sizeof(soc_flow_db_view_ffo_tuple_t)));

    num_hash_entries = ffo_tpl_tbl_map->pe;
    hash_entry = &ffo_tpl_tbl_map->tbl_entry;

    for (i = 0; i < num_hash_entries; i++) {
        offset = *(hash_entry + i);
        if (offset == 0) {
            continue;
        }
        p = hash_entry + offset;
        entry_num = *p++;
        entry = (soc_flow_db_ffo_tuple_rec_t *) p;
        if (entry_num !=0) {
            for (j = 0; (j < entry_num) && (entry != NULL);
                 j++, entry++) {
                id = SOC_FLOW_DB_VIEW_TO_ID(entry->view_id);
                view_ffo_entry = (fm->view_ffo_list + id);
                view_ffo_entry->view_id = entry->view_id;
                view_ffo_entry->nffos++;
                ffo_tpl_entry = sal_alloc(sizeof(soc_flow_db_ffo_tuple_t),
                                 "soc_flow_db_ffo_tuple_t");
                if (ffo_tpl_entry == NULL) {
                    return SOC_E_MEMORY;
                }
                ffo_tpl_entry->flow_handle = entry->flow_handle;
                ffo_tpl_entry->function_id = entry->function_id;
                ffo_tpl_entry->option_id = entry->option_id;
                ffo_tpl_entry->next = (uint32 *) view_ffo_entry->ffo_tuple;
                view_ffo_entry->ffo_tuple = (uint32 *) ffo_tpl_entry;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int soc_flow_db_alloc(soc_flow_db_flow_map_t **flow_map) {
    soc_flow_db_flow_map_t *fm = NULL;

    fm = sal_alloc(sizeof(soc_flow_db_flow_map_t),
                   "soc_flow_db_flow_map_t");
    if (fm == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(fm, 0, sizeof(soc_flow_db_flow_map_t));
    *flow_map = fm;
    return SOC_E_NONE;
}

STATIC
int _soc_flow_db_flow_map_load(int unit)
{
    soc_cancun_t             *cc = NULL;
    soc_flow_db_t            *flow_db = NULL;
    soc_flow_db_flow_map_t   *fm = NULL;
    soc_flow_db_flow_map_t   *flow_map = NULL;
    soc_flow_db_tbl_map_t    *flow_tbl_map =  NULL;
    soc_flow_db_tbl_map_t    *flow_op_tbl_map =  NULL;
    soc_flow_db_tbl_map_t    *ffo_tuple_tbl_map =  NULL;
    soc_flow_db_tbl_map_t    *view_tbl_map =  NULL;
    soc_flow_db_tbl_map_t    *lg_fl_tbl_map =  NULL;
#if 0
    uint32 crc = 0;
    uint32 block_crc = 0;
#endif
    uint32 *p = NULL;
    uint32 block_size = 0;

    SOC_IF_ERROR_RETURN(soc_cancun_info_get(unit, &cc));

    flow_db = cc->flow_db;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Loading Flow DB Map")));

    if (flow_db == NULL) {
        return SOC_E_INIT;
    }

    flow_map = flow_db->flow_map;

    if (flow_map == NULL) {
        return SOC_E_INIT;
    }
    fm = soc_flow_db_flow_map[unit];
    if (fm == NULL) {
        return SOC_E_INIT;
    }

    /* get the string table */
    fm->str_tbl = flow_db->str_tbl;
 
    /*****************************
     Load the Flow table
 */
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Start Flow Table")));

    p = (uint32 *)flow_map;

    fm->flow_tbl_lyt = (soc_flow_db_tbl_map_t *)p;
    flow_tbl_map = (soc_flow_db_tbl_map_t *)p;

    if (flow_tbl_map->tbl_start != SOC_FLOW_DB_START_FL_TBL_CHUNK) {
        /* Corruption detected */
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(0,
               "\n ERROR: Memory corruption detected in Flow DB \
                    Start of Flow table not detected")));
        return SOC_E_INTERNAL;
    }

    block_size = flow_tbl_map->block_size;
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Flow Table - Size %d"), block_size));

    /* CRC validation */
#if 0
    crc = soc_cancun_crc32(0, (uint8 *)p, WORDS2BYTES(block_size));
    block_crc = flow_tbl_map->crc;
    if (crc != block_crc) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                "ERROR: Flow Table Block CRC check failed. Abort\n")));
        return SOC_E_INTERNAL;

    }
#endif
    p = p + block_size;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n End of loading Flow Table")));
    /*****************************
     Load the Flow Option table
 */
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Start Flow Option Table")));


    fm->flow_option_tbl_lyt = (soc_flow_db_tbl_map_t *)p;
    flow_op_tbl_map = (soc_flow_db_tbl_map_t *) p;

    if (flow_op_tbl_map->tbl_start
            != SOC_FLOW_DB_START_FL_OP_TBL_CHUNK) {
        /* Corruption detected */
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(0,
               "\n ERROR: Memory corruption detected in Flow DB \
                    Start of Flow Option table not detected")));
        return SOC_E_INTERNAL;
    }

    block_size = flow_op_tbl_map->block_size;
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Flow option Table - Size %d"), block_size));
#if 0
    /* CRC validation */
    crc = soc_cancun_crc32(0, (uint8 *)p, WORDS2BYTES(block_size));
    block_crc = flow_op_tbl_map->crc;
    if (crc != block_crc) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                "ERROR: Flow Option Table Block CRC check failed. Abort\n")));
        return SOC_E_INTERNAL;

    }

#endif
    p = p + block_size;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n End of loading Flow Option Table")));
    /*****************************
     Load the ffo_tuple table
 */
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Start ffo_tuple map Table")));


    fm->ffo_tuple_tbl_lyt = (soc_flow_db_tbl_map_t *) p;
    ffo_tuple_tbl_map = (soc_flow_db_tbl_map_t *) p;

    if (ffo_tuple_tbl_map->tbl_start
          != SOC_FLOW_DB_START_FFO_TPL_TBL_CHUNK) {
        /* Corruption detected */
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(0,
               "\n ERROR: Memory corruption detected in Flow DB \
                    Start of FFO TUPLE table not detected")));
        return SOC_E_INTERNAL;
    }

    block_size = ffo_tuple_tbl_map->block_size;
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n FFO TUPLE Table - Size %d"), block_size));

#if 0
    /* CRC validation */
    crc = soc_cancun_crc32(0, (uint8 *)p, WORDS2BYTES(block_size));
    block_crc = ffo_tuple_tbl_map->crc;
    if (crc != block_crc) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                "ERROR: Flow tuple Table Block CRC check failed. Abort\n")));
        return SOC_E_INTERNAL;

    }
#endif

    p = p + block_size;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n End of loading FFO TUPLE Table")));

    /*************************************
     * Load the View table
 */
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Start view table")));

    fm->view_tbl_lyt = (soc_flow_db_tbl_map_t *) p;
    view_tbl_map = (soc_flow_db_tbl_map_t *) p;

    if (view_tbl_map->tbl_start
        != SOC_FLOW_DB_START_VW_TBL_CHUNK) {
        /* Corruption detected */
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(0,
               "\n ERROR: Memory corruption detected in Flow DB \
                    Start of view table not detected")));
        return SOC_E_INTERNAL;
    }

    block_size = view_tbl_map->block_size;
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n View Table - Size %d"), block_size));
#if 0
    /* CRC validation */
    crc = soc_cancun_crc32(0,(uint8 *)p, WORDS2BYTES(block_size));
    block_crc = view_tbl_map->crc;
    if (crc != block_crc) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                "ERROR: View Table Block CRC check failed. Abort\n")));
        return SOC_E_INTERNAL;

    }
#endif

    p = p + block_size;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n End of loading View Table")));

    /*************************************
     * Load the Logical field map table
 */
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Start logical field map table")));

    fm->lg_field_tbl_lyt = (soc_flow_db_tbl_map_t *) p;
    lg_fl_tbl_map = (soc_flow_db_tbl_map_t *) p;

    if (lg_fl_tbl_map->tbl_start
        != SOC_FLOW_DB_START_LG_FL_MAP_TBL_CHUNK) {
        /* Corruption detected */
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(0,
               "\n ERROR: Memory corruption detected in Flow DB \
                    Start of logical field map table not detected")));
        return SOC_E_INTERNAL;
    }

    block_size = lg_fl_tbl_map->block_size;
    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n Logical field map Table - Size %d"), block_size));

#if 0
    /* CRC validation */
    crc = soc_cancun_crc32(0, (uint8 *)p, WORDS2BYTES(block_size));
    block_crc = lg_fl_tbl_map->crc;
    if (crc != block_crc) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                "ERROR: Logical field Table Block "\
                "CRC check failed. Abort\n")));
        return SOC_E_INTERNAL;

    }
#endif

    p = p + block_size;

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
      (BSL_META_U(0,
       "\n End of loading logical field map Table")));
   return SOC_E_NONE;
}

int _soc_flow_db_load(int unit, uint8* buf, int buf_words)
{
    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    int str_tbl_len;
    char *cur_src_buf;
    char *cur_dst_buf;
    int cur_len;

    SOC_IF_ERROR_RETURN(soc_cancun_info_get(unit, &cc));
    if(cc == NULL) {
        return SOC_E_UNIT;
    } else if (!(cc->flags & SOC_CANCUN_FLAG_INITIALIZED)) {
        return SOC_E_INIT;
    } else if (buf == NULL) {
        return SOC_E_PARAM;
    } else if (buf_words <= 0) {
        return SOC_E_PARAM;
    }

    flow_db = cc->flow_db;
    if (flow_db == NULL) {
        return SOC_E_INIT;
    }
    if(flow_db->flow_map) {
        sal_free(flow_db->flow_map);
        flow_db->flow_map = NULL;
    }
    flow_db->flow_map = sal_alloc(((sizeof(uint32)) * buf_words),
                               "soc_flow_db_t");
    if(flow_db->flow_map == NULL) {
        return SOC_E_MEMORY;
    }

    /* The buf starts with the length and the string table. The string
     * table should be skipped for endianness conversion
     */
    str_tbl_len = soc_letohl_load(buf);
    if (flow_db->str_tbl) {
        sal_free(flow_db->str_tbl);
        flow_db->str_tbl = NULL;
    }
    flow_db->str_tbl = sal_alloc(str_tbl_len, "soc_flow_db_t");
    if(flow_db->str_tbl == NULL) {
        return SOC_E_MEMORY;
    }
    
    cur_dst_buf = flow_db->str_tbl;        
    *(uint32 *)cur_dst_buf = str_tbl_len;
    cur_src_buf = (char *)buf + BYTES_PER_UINT32;
    cur_dst_buf += BYTES_PER_UINT32;
    cur_len = str_tbl_len - BYTES_PER_UINT32;
    sal_memcpy(cur_dst_buf,cur_src_buf,cur_len);

    cur_src_buf += cur_len;
    cur_dst_buf = (char *)flow_db->flow_map;
    cur_len = buf_words * BYTES_PER_UINT32 - 
              (cur_len + BYTES_PER_UINT32);
    SOC_IF_ERROR_RETURN(
          _soc_flow_db_memcpy_letohl((uint32 *)cur_dst_buf,
                 (uint32 *)cur_src_buf, cur_len/BYTES_PER_UINT32));

    SOC_IF_ERROR_RETURN(
              _soc_flow_db_flow_map_load (unit));

    SOC_IF_ERROR_RETURN(
            _soc_flow_db_view_ffo_list_update(unit));

    return SOC_E_NONE;
}

void soc_flow_db_deinit(uint32 unit)
{
    soc_flow_db_flow_map_t *fm = NULL;
    fm = soc_flow_db_flow_map[unit];
    soc_flow_db_view_ffo_list_free(unit);
    if (fm != NULL) {
       sal_free(fm);
    }
    soc_flow_db_flow_map[unit] = NULL;
}

int soc_flow_db_init(uint32 unit)
{
    soc_flow_db_flow_map_t *flow_map = NULL;

    flow_map = soc_flow_db_flow_map[unit];

    if (flow_map == NULL) {
        if (soc_flow_db_alloc(&flow_map)) {
            return SOC_E_MEMORY;
        }
        soc_flow_db_flow_map[unit] = flow_map;
    }
    return SOC_E_NONE;
}

int soc_flow_db_status_get(int unit)
{
   soc_cancun_t *cc = NULL;
   soc_flow_db_flow_map_t *flow_map = NULL;

   SOC_IF_ERROR_RETURN(soc_cancun_status_get(unit, &cc));

   if (!(cc->flags & SOC_CANCUN_FLAG_CFH_LOADED))  {
       return SOC_E_INIT;
   }

   flow_map = soc_flow_db_flow_map[unit];
   if (flow_map == NULL) {
       return SOC_E_INIT;
   }

   return SOC_E_NONE;
}

int soc_flow_db_legacy_flow_map_get(int unit,
                                soc_flow_db_legacy_flow_rec_t **fmap,
                                uint32 *fmap_entries)
{
    *fmap = legacy_flow_map;
    *fmap_entries = SOC_FLOW_DB_MAX_LEGACY_FLOWS;
    return SOC_E_NONE;
}

int _soc_flow_db_flow_handle_get(int unit,
                                 const char *name,
                                 uint32 *flow_handle) {
    soc_flow_db_tbl_map_t *flow_tbl_map = NULL;
    soc_flow_db_flow_rec_t *flow_entry = NULL;
    soc_flow_db_legacy_flow_rec_t *legacy_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    char *str_tbl;
    char *name_addr;

    /*****************************
     Legacy flow table if match
 */
    for (i = 0, legacy_entry = legacy_flow_map;
        i < SOC_FLOW_DB_MAX_LEGACY_FLOWS;
        i++, legacy_entry++) {
        if (sal_strncmp(name, legacy_entry->name,
                 sal_strlen(legacy_entry->name))== 0) {
            *flow_handle = legacy_entry->flow_handle;
            return SOC_E_NONE;
        }
    }

    /*****************************
      FLEX DB flow table if match
 */

    flow_tbl_map = SOC_FLOW_DB_FLOW_TBL_MAP(unit);
    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if (flow_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &flow_tbl_map->tbl_entry;

    SOC_IF_ERROR_RETURN(_soc_flow_db_flow_name_hash_key_get(unit,
                                              flow_tbl_map,
                                              0,
                                              name,
                                              &key));
    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    p = hash_entry + offset;
    entry_num = *p++;

    flow_entry = (soc_flow_db_flow_rec_t *) p;
    if (entry_num !=0) {
        for (i = 0; i < entry_num; i++) {
            name_addr = str_tbl + flow_entry->name_offset;
            if (sal_strncmp(name, name_addr,
                 sal_strlen(name_addr))== 0) {
                /* Match found */
                *flow_handle = flow_entry->flow_handle;
                return SOC_E_NONE;
            }
            flow_entry += 1;

        }
    }

    return SOC_E_NOT_FOUND;
}

int _soc_flow_db_flow_option_id_get(int unit,
                           uint32 flow_handle,
                           const char *name,
                           uint32 *option_id)
{

    soc_flow_db_tbl_map_t *flow_op_tbl_map = NULL;
    soc_flow_db_flow_option_rec_t *flow_op_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    char *str_tbl;
    char *name_addr;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    flow_op_tbl_map = SOC_FLOW_DB_FLOW_OP_TBL_MAP(unit);
    if (flow_op_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &flow_op_tbl_map->tbl_entry;

    SOC_IF_ERROR_RETURN(_soc_flow_db_flow_name_hash_key_get(unit,
                                               flow_op_tbl_map,
                                               flow_handle,
                                               name,
                                               &key));

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    p = hash_entry + offset;
    entry_num = *p++;

    flow_op_entry = (soc_flow_db_flow_option_rec_t *) p;
    if (entry_num !=0) {
        for (i = 0; i < entry_num; i++) {
            name_addr = str_tbl + flow_op_entry->name_offset;
            if ((sal_strncmp(name, name_addr,
                 sal_strlen(name_addr))== 0) &&
                 (flow_handle == flow_op_entry->flow_handle)) {
                /* Match found */
                *option_id =  flow_op_entry->option_id;
                return SOC_E_NONE;
            }
            flow_op_entry += 1;
        }
    }

    return SOC_E_NOT_FOUND;

}

int _soc_flow_db_mem_view_field_info_get(int unit,
                               uint32 view_id,
                               uint32 field_id,
                               soc_flow_db_mem_view_field_info_t *field_info)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    soc_flow_db_field_rec_t *fentry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int rv = -1;

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }
    nFields = view_entry->nFields;
    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;

    SOC_FLOW_DB_FIND_FIELD(field_id, field_entry,
                           nFields, fentry);
    if (fentry == NULL) {
        return SOC_E_NOT_FOUND;
    }

    field_info->field_id = fentry->field_id;
    field_info->type     = fentry->type;
    field_info->offset   = fentry->offset;
    field_info->width    = fentry->width;
    field_info->flags    = fentry->flags;
    field_info->value    = fentry->value;

    return SOC_E_NONE;

}

#define OFFSET_STR_MAX    4
int _soc_flow_db_mem_view_split_field_info_get(int unit,
                               uint32 view_id,
                               uint32 field_id,
                               soc_flow_db_mem_view_split_field_info_t *sf_info)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    soc_flow_db_field_rec_t *fentry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int rv = -1;
    char f_name[SOC_FLOW_DB_MAX_FIELD_NAME];
    int len;
    int i, j;
    char *str_tbl;
    char *name_addr;
    int cnt;
    uint16 offset_val;
    int found;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }
    nFields = view_entry->nFields;
    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;

    SOC_FLOW_DB_FIND_FIELD(field_id, field_entry,
                           nFields, fentry);
    if (fentry == NULL) {
        return SOC_E_NOT_FOUND;
    }

    if (!(fentry->flags & SOC_FLOW_DB_FIELD_FLAG_VIRTUAL)) {
        return SOC_E_NOT_FOUND;
    }

    /* virtual field */
    /* form the fragment field name */
    sal_memset(f_name,0, sizeof(f_name));
    name_addr = str_tbl + fentry->name_offset;
    len = sal_strlen(name_addr);

    /* boundary safety check */
    if (len >= (SOC_FLOW_DB_MAX_FIELD_NAME - 8)) {
        return SOC_E_INTERNAL;
    }
    sal_strcpy(f_name,name_addr);
    f_name[len++] = '_';  /* XXX use a different delimiter in Cancun CFH*/

    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;

    cnt = 0;
    for (i = 0; i < nFields; i++) {
        name_addr = str_tbl + field_entry[i].name_offset;
        if ((field_entry[i].flags & SOC_FLOW_DB_FIELD_FLAG_FRAGMENT) &&
            (!sal_strncmp(f_name, name_addr, len))) {

            /* safety check */
            if (cnt >= COUNTOF(sf_info->field)) {
                return SOC_E_INTERNAL;
            }
            sf_info->field[cnt].field_id = field_entry[i].field_id;
            sf_info->field[cnt].offset   = field_entry[i].offset;
            sf_info->field[cnt].type   =   field_entry[i].type;
            sf_info->field[cnt].width  =   field_entry[i].width;
            sf_info->field[cnt].flags  =   field_entry[i].flags;
            sf_info->field[cnt].value  =   field_entry[i].value;
            offset_val = 0;
            found = TRUE;
            for (j = 0; j < OFFSET_STR_MAX; j++) {
                if (*(name_addr + len + j) == '_' ||
                    *(name_addr + len + j) == 0) {
                    break;
                } else if ((*(name_addr + len + j) < '0') ||
                           (*(name_addr + len + j) > '9')) {
                    /* not a valid number, skip this entry */
                    found = FALSE;
                    break;
                }
                offset_val = offset_val * 10 + *(name_addr + len + j) - '0';
            } 
            if (found) {
                sf_info->field[cnt].v_offset   = offset_val;
        
                cnt++;
            }
        }
    }
  
    sf_info->num_fld = cnt;
    sf_info->width  = fentry->width;

    return SOC_E_NONE;
}

int _soc_flow_db_mem_view_info_get(int unit,
                              uint32 view_id,
                              soc_flow_db_mem_view_info_t *view_info)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    int rv = -1;

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }

    view_info->mem_view_id = view_entry->view_id;
    view_info->mem = view_entry->mem;
    view_info->width = view_entry->width;
    view_info->type = view_entry->type;
    return SOC_E_NONE;

}

int _soc_flow_db_mem_view_field_is_valid(int unit,
                               uint32 view_id,
                               uint32 field_id)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    soc_flow_db_field_rec_t *fentry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int rv = -1;

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return FALSE;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return FALSE;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return FALSE;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return FALSE;
    }
    nFields = view_entry->nFields;
    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;

    SOC_FLOW_DB_FIND_FIELD(field_id, field_entry,
                           nFields, fentry);

    if (fentry == NULL) {
        return FALSE;
    }
    if (fentry->flags & SOC_FLOW_DB_FIELD_FLAG_VIRTUAL) {
        return SOC_FLOW_DB_FIELD_VIRTUAL_VALID;
    } else {
        return SOC_FLOW_DB_FIELD_VALID;
    }
}

int _soc_flow_db_mem_view_field_name_get(int unit,
                                     uint32 view_id,
                                     uint32 field_id,
                                     char **name)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    soc_flow_db_field_rec_t *fentry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int rv = -1;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }
    nFields = view_entry->nFields;
    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;

    SOC_FLOW_DB_FIND_FIELD(field_id, field_entry,
                           nFields, fentry);
    if (fentry == NULL) {
        return SOC_E_NOT_FOUND;
    }

    *name = str_tbl + fentry->name_offset;
    return SOC_E_NONE;

}


int _soc_flow_db_mem_view_logical_field_id_get(int unit,
                                    uint32 flow_handle,
                                    const char *name,
                                    uint32  *field_id)
{
    soc_flow_db_tbl_map_t *lg_fl_tbl_map = NULL;
    soc_flow_db_flow_lg_field_rec_t *lg_fl_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    char *str_tbl;
    char *name_addr;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    lg_fl_tbl_map = SOC_FLOW_DB_FLOW_LG_FL_TBL_MAP(unit);
    if (lg_fl_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &lg_fl_tbl_map->tbl_entry;
    SOC_IF_ERROR_RETURN(_soc_flow_db_flow_name_hash_key_get(unit,
                                                  lg_fl_tbl_map,
                                                  flow_handle,
                                                  name,
                                                  &key));

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    p = hash_entry + offset;
    entry_num = *p++;

    lg_fl_entry = (soc_flow_db_flow_lg_field_rec_t *) p;
    if (entry_num != 0) {
        for (i = 0; i < entry_num; i++) {
            name_addr = str_tbl + lg_fl_entry->name_offset;
            if ((sal_strcmp(name, name_addr)== 0) &&
                 (flow_handle == lg_fl_entry->flow_handle)) {
                /* Match found */
                *field_id = lg_fl_entry->field_id;
                return SOC_E_NONE;
            }
            lg_fl_entry += 1;
        }
    }

    return SOC_E_NOT_FOUND;

}

int _soc_flow_db_ffo_to_mem_view_id_get(int    unit,
                             uint32 flow_handle,
                             uint32 function_id,
                             uint32 option_id,
                             uint32 *view_id)
{
    soc_flow_db_tbl_map_t *ffo_tpl_tbl_map = NULL;
    soc_flow_db_ffo_tuple_rec_t *ffo_tpl_rec = NULL;
    uint32 *hash_entry = NULL;
    uint32 *p = NULL;
    uint32 key = 0;
    uint32 offset = 0;
    int i = 0;
    uint32 entry_num = 0;

    ffo_tpl_tbl_map = SOC_FLOW_DB_FFO_TPL_TBL_MAP(unit);

    if (ffo_tpl_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
           _soc_flow_db_ffo_tpl_hash_key_get(unit,
                       ffo_tpl_tbl_map,
                       flow_handle,
                       function_id,
                       option_id,
                       &key));

    hash_entry = &ffo_tpl_tbl_map->tbl_entry;
    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }

    p = hash_entry + offset;
    entry_num = *p++;

    ffo_tpl_rec = (soc_flow_db_ffo_tuple_rec_t *) p;
    if (entry_num !=0) {
        for (i = 0; i < entry_num; i++) {
            if ((flow_handle == ffo_tpl_rec->flow_handle)&&
                (function_id == ffo_tpl_rec->function_id) &&
                (option_id == ffo_tpl_rec->option_id))   {
                /* Match found */
                *view_id = ffo_tpl_rec->view_id;
                return SOC_E_NONE;
            }
            ffo_tpl_rec +=1;
        }
    }

    return SOC_E_NOT_FOUND;
}

int _soc_flow_db_mem_view_field_list_get(int unit,
                                    uint32 view_id,
                                    int field_type,
                                    int size,
                                    uint32  *field_id,
                                    uint32  *field_count)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int rv = -1;
    int count = 0;

    if (!size) {
       return SOC_E_PARAM;
    }
    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }
    nFields = view_entry->nFields;
    pf = &(view_entry->field_entry);
    field_entry = (soc_flow_db_field_rec_t *)pf;
    count = 0;
    for (i = 0; i < nFields; i++) {
        if (field_entry->type == field_type) {
            /* match found */
            /***********************************************
               TODO : sort the field array during generation
                and so a binary search for the field element
 */
            if (count >= size) {
                /* max array size reached */
                return SOC_E_INTERNAL;
            }
            field_id[count++] = field_entry->field_id;
        }
        field_entry += 1;
    }

   *field_count = count;
   return SOC_E_NONE;
}

int
 _soc_flow_db_mem_to_view_id_get(int                   unit,
                             uint32                    mem,
                             int                       key_type,
                             int                       data_type,
                             int                       num_fields,
                             soc_flow_db_ctrl_field_t  *field_list,
                             int                       num_ids,
                             uint32                    *view_id)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    soc_flow_db_field_rec_t *field_entry = NULL;
    soc_flow_db_field_rec_t *key_entry = NULL;
    soc_flow_db_field_rec_t *data_entry = NULL;
    soc_flow_db_field_rec_t *ctrl_entry = NULL;
    uint32 num_views = 0;
    uint32 vid = SOC_FLOW_DB_VIEW_IDX_START;
    uint32 key_typef[] = {KEY_TYPEf,
                         KEY_TYPE0f,
                         KEY_TYPE1f,
                         KEY_TYPE0_LWRf};
    uint32 data_typef[] = {DATA_TYPEf,
                          DATA_TYPE0f,
                          DATA_TYPE1f};
    int j = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    uint32 nFields = 0;
    int8 key_type_match = FALSE;
    int8 data_type_match = FALSE;
    int8 ctrl_field_match = FALSE;
    int view_cnt = 0;

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }
    if (num_ids == 0 || view_id == NULL) {
        return SOC_E_PARAM;
    }

    hash_entry = &view_tbl_map->tbl_entry;
    num_views = SOC_FLOW_DB_NUM_VIEW(unit);
    for (j = 0; j < num_ids; j++) {
        view_id[j] = 0;
    }

    for (vid = SOC_FLOW_DB_VIEW_IDX_START;
         vid < (num_views + SOC_FLOW_DB_VIEW_IDX_START);
        vid++) {
        key_type_match = (key_type == SOC_FLOW_DB_KEY_TYPE_INVALID) ?
                      TRUE :  FALSE;
        data_type_match = (data_type == SOC_FLOW_DB_DATA_TYPE_INVALID) ?
                      TRUE :  FALSE;

        ctrl_field_match = (num_fields == 0) ? TRUE : FALSE;

        id = SOC_FLOW_DB_VIEW_TO_ID(vid);
        SOC_IF_ERROR_RETURN(_soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key));
        offset = *(hash_entry + key);
        if (offset == 0) {
            return SOC_E_NOT_FOUND;
        }

        p = hash_entry + offset;
        entry_num = *p++;

        view_entry = (soc_flow_db_view_rec_t *) p;
        if ((entry_num != 1) ||
            (vid != view_entry->view_id)) {
            /* expecting only one entry for view at hash location */
            return SOC_E_INTERNAL;

        }
        if (view_entry->mem != mem) {
            continue;
        }
        nFields = view_entry->nFields;
        pf = &(view_entry->field_entry);
        field_entry = (soc_flow_db_field_rec_t *)pf;
        if (!key_type_match)  {
            for (j = 0; j < COUNTOF(key_typef); j++) {
                SOC_FLOW_DB_FIND_FIELD(key_typef[j], field_entry,
                                       nFields, key_entry);
                if (key_entry != NULL) {
                    if (key_entry->value == key_type) {
                        key_type_match = TRUE;
                        break;
                    }
                }
            }
        }
        if (!data_type_match) {
            for (j = 0; j < COUNTOF(data_typef); j++) {
                SOC_FLOW_DB_FIND_FIELD(data_typef[j], field_entry,
                                       nFields, data_entry);
                 if (data_entry != NULL) {
                    if (data_entry->value == data_type) {
                        data_type_match = TRUE;
                        break;
                    }
                }
            }
        }

        for (j = 0; j < num_fields; j++) {
            ctrl_field_match = FALSE;
            SOC_FLOW_DB_FIND_FIELD(field_list[j].field_id, field_entry,
                                   nFields, ctrl_entry);
            if (ctrl_entry != NULL) {
                if (ctrl_entry->type != SOC_FLOW_DB_FIELD_TYPE_CONTROL) {
                    return SOC_E_PARAM;
                }
                if (ctrl_entry->value == field_list[j].value) {
                    ctrl_field_match = TRUE;
                }
            } else{
                /* control field not found */
                if (!field_list[j].value) {
                    ctrl_field_match = TRUE;
                }
            }
            if (ctrl_field_match == FALSE) {
                break;
            }
        }
        if (key_type_match && data_type_match && ctrl_field_match) {
            if (view_cnt >= num_ids) {  /* exceed view id array size */
                return SOC_E_NONE;
            }
            view_id[view_cnt] = vid;
            view_cnt++;
        }

    }
    if (view_cnt) {
        return SOC_E_NONE;
    } else {
        return SOC_E_NOT_FOUND;
    }
}

int _soc_flow_db_mem_view_to_ffo_get(int  unit,
                                uint32 view_id,
                                uint32 size,
                                soc_flow_db_ffo_t *ffo,
                                uint32 *num_ffo_entries)
{
    soc_flow_db_flow_map_t *fm = NULL;
    soc_flow_db_view_ffo_tuple_t *view_ffo_entry = NULL;
    soc_flow_db_ffo_tuple_t *ffo_tpl_entry = NULL;
    uint32 id = 0;
    uint32 cnt = 0;
    uint32 *p = NULL;

    fm = soc_flow_db_flow_map[unit];
    if (fm == NULL) {
       return SOC_E_INTERNAL;
    }

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    view_ffo_entry = (fm->view_ffo_list + id);

    if ((view_ffo_entry->nffos == 0) ||
        (view_ffo_entry->ffo_tuple == NULL)){
        return SOC_E_INTERNAL;
    }

    p = view_ffo_entry->ffo_tuple;
    do {
        ffo_tpl_entry = (soc_flow_db_ffo_tuple_t *) p;
        ffo[cnt].flow_handle = ffo_tpl_entry->flow_handle;
        ffo[cnt].function_id = ffo_tpl_entry->function_id;
        ffo[cnt++].option_id = ffo_tpl_entry->option_id;
        p = ffo_tpl_entry->next;
    } while ((p != NULL) &&
             ((cnt < size) && (cnt < view_ffo_entry->nffos)));

    *num_ffo_entries = cnt;
    return SOC_E_NONE;
}

int
_soc_flow_db_mem_view_mem_name_get(int unit,
                       uint32 view_id,
                       char **mem_name)
{
    soc_flow_db_tbl_map_t  *view_tbl_map =  NULL;
    soc_flow_db_view_rec_t *view_entry = NULL;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 key = 0;
    uint32 id = 0;
    int rv = -1;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    view_tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);
    if (view_tbl_map == NULL) {
        return SOC_E_INTERNAL;
    }

    hash_entry = &view_tbl_map->tbl_entry;

    id = SOC_FLOW_DB_VIEW_TO_ID(view_id);
    rv = _soc_flow_db_view_hash_key_get(unit,
                                   view_tbl_map,
                                    id, &key);
    if (SOC_FAILURE(rv)) {
       return rv;
    }

    offset = *(hash_entry + key);
    if (offset == 0) {
        return SOC_E_NOT_FOUND;
    }
    p = hash_entry + offset;
    entry_num = *p++;

    view_entry = (soc_flow_db_view_rec_t *) p;
    if ((entry_num != 1) ||
        (view_id != view_entry->view_id)) {
        /* expecting only one entry for view at hash location */
        return SOC_E_NOT_FOUND;
    }

    *mem_name = str_tbl + view_entry->mem_name_offset;
    return SOC_E_NONE;
}

int
_soc_flow_db_mem_view_option_name_get(int unit,
                       uint32 flow_handle,
                       uint32 flow_option_id,
                       char **option_name)
{
    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_flow_option_rec_t *entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
    {
        return SOC_E_PARAM;
    }

    flow_db = cc->flow_db;

    if (flow_db == NULL) {
        return SOC_E_PARAM;
    }
    tbl_map = SOC_FLOW_DB_FLOW_OP_TBL_MAP(unit);

    if (tbl_map == NULL) {
        return SOC_E_PARAM;
    }
    num_hash_entries = tbl_map->pe;
    hash_entry = &tbl_map->tbl_entry;
    for (i = 0; i < num_hash_entries; i++) {
        offset = *(hash_entry + i);
        if (offset == 0) {
            continue;
        }
        p = hash_entry + offset;
        entry_num = *p++;
        entry = (soc_flow_db_flow_option_rec_t *) p;
        if (entry_num !=0) {
            for (j = 0; j < entry_num; j++, entry++) {
                if ((entry->option_id == flow_option_id) &&
                   (entry->flow_handle == flow_handle)) {
                    *option_name = str_tbl + entry->name_offset;
                    return SOC_E_NONE;
                }
            }
        }

    }
    return SOC_E_PARAM;
}

