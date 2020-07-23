/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: soc_ser_log.c
 * Purpose: SER logging using a circular buffer
 */

#include <shared/bsl.h>
#include <shared/alloc.h>

#include <soc/soc_ser_log.h>
#include <soc/soc_log_buf.h>
#include <soc/error.h>
#include <sal/appl/sal.h>
#include <soc/util.h>

static char _soc_ser_log_buffer[SOC_MAX_NUM_DEVICES][_SOC_SER_LOG_BUFFER_SIZE];

static void *_soc_ser_log_buffer_ptr[SOC_MAX_NUM_DEVICES];

typedef struct _soc_ser_log_criteria_s {
    soc_mem_t mem;
    int index;
    sal_usecs_t time;
} _soc_ser_log_criteria_t;

int
soc_ser_log_get_entry_size(int unit, int id, int *size)
{
    int rc = SOC_E_NONE;
    if (id == 0) {
        return SOC_E_PARAM;
    }

    rc = soc_log_buf_entry_get_size(_soc_ser_log_buffer_ptr[unit], id, size);

    return rc;
}

int
soc_ser_log_get_entry(int unit, int id, int size, void *entry)
{
    int entry_size;
    int rc = SOC_E_NONE;

    if (id == 0) {
        return SOC_E_PARAM;
    }

    rc = soc_log_buf_entry_get_size(_soc_ser_log_buffer_ptr[unit],
            id, &entry_size);

    if ((entry_size > size) || (SOC_E_NONE != rc)) {
        return SOC_E_PARAM;
    }

    rc = soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
        id, 0, entry_size, entry);
    
    return rc;
}

static int
_soc_ser_bus_buffer_log_match(void *criteria, void *entry)
{
    soc_ser_log_tlv_hdr_t *tlv_hdr = entry;
    soc_ser_log_tlv_memory_t *tlv_mem;
    soc_ser_log_tlv_memory_t entry_data;
    uint32 *hwbase = criteria;
    int found_mem = 0;

    sal_memset(&entry_data, 0, sizeof(_soc_ser_log_criteria_t));

    while ((tlv_hdr->type != 0) && !found_mem) {
        if (tlv_hdr->type == SOC_SER_LOG_TLV_MEMORY) {
            tlv_mem = (soc_ser_log_tlv_memory_t*)(((char*)tlv_hdr) +
                sizeof(soc_ser_log_tlv_hdr_t));
            entry_data.hwbase = tlv_mem->hwbase;
            found_mem = 1;
        }
        tlv_hdr = (soc_ser_log_tlv_hdr_t*)((char*)tlv_hdr +
            tlv_hdr->length + sizeof(soc_ser_log_tlv_hdr_t));
    }

    if (found_mem && (*hwbase == entry_data.hwbase)) {
        return 1;
    }

    return 0;
}

int
soc_ser_bus_buffer_log_find(int unit, uint32 hwbase)
{
    int rv = SOC_E_NONE;
    char *buffer;

    if (_soc_ser_log_buffer_ptr[unit] == NULL) {
        return SOC_E_PARAM;
    }

    buffer = sal_alloc(_SOC_SER_LOG_BUFFER_SIZE, "ser log info");
    if (NULL == buffer) {
        return (SOC_E_MEMORY);
    }

    rv = soc_log_buf_search(_soc_ser_log_buffer_ptr[unit], buffer,
            _SOC_SER_LOG_BUFFER_SIZE, &hwbase, &_soc_ser_bus_buffer_log_match);

    sal_free(buffer);

    if (rv > 0) {
        /* if rv > 0, it means the rv is the log id, and hwbase is found.
           Overwrite the rv to error type */
        rv = SOC_E_NONE;
    }
    return rv;
}

static int
_soc_ser_log_match_entry(void *criteria, void *entry)
{
    _soc_ser_log_criteria_t *crit = criteria;
    soc_ser_log_tlv_hdr_t *tlv_hdr = entry;
    soc_ser_log_tlv_memory_t *tlv_mem;
    soc_ser_log_tlv_generic_t *tlv_gen;
    _soc_ser_log_criteria_t entry_data;
    int found_gen = 0;
    int found_mem = 0;

    sal_memset(&entry_data, 0, sizeof(_soc_ser_log_criteria_t));

    while ((tlv_hdr->type != 0) && !(found_gen && found_mem)) {
        if (tlv_hdr->type == SOC_SER_LOG_TLV_GENERIC) {
            tlv_gen = (soc_ser_log_tlv_generic_t*)(((char*)tlv_hdr) +
                sizeof(soc_ser_log_tlv_hdr_t));
            entry_data.time = tlv_gen->time;
            found_gen = 1;
        } else if (tlv_hdr->type == SOC_SER_LOG_TLV_MEMORY) {
            tlv_mem = (soc_ser_log_tlv_memory_t*)(((char*)tlv_hdr) +
                sizeof(soc_ser_log_tlv_hdr_t));
            entry_data.index = tlv_mem->index;
            entry_data.mem = tlv_mem->memory;
            found_mem = 1;
        }
        tlv_hdr = (soc_ser_log_tlv_hdr_t*)((char*)tlv_hdr +
            tlv_hdr->length + sizeof(soc_ser_log_tlv_hdr_t));
    }

    if ((found_gen && found_mem) &&
        (crit->mem == entry_data.mem) &&
        (crit->index == entry_data.index) &&
        (SAL_USECS_SUB(crit->time, entry_data.time) <= 0)) {
        return 1;
    }

    return 0;
}

int 
soc_ser_log_find_recent(int unit, soc_mem_t mem, int index, sal_usecs_t time)
{
    int rv = SOC_E_NONE;
    char *buffer;
    _soc_ser_log_criteria_t criteria;

    if (_soc_ser_log_buffer_ptr[unit] == NULL) {
        return 0;
    }

    buffer = sal_alloc(_SOC_SER_LOG_BUFFER_SIZE, "ser log info");
    if (NULL == buffer) {
        return (SOC_E_MEMORY);
    }

    criteria.mem = mem;
    criteria.index = index;
    /* Check within the last 5 seconds */
    criteria.time = SAL_USECS_SUB(time, SECOND_USEC * 5);
    
    rv = soc_log_buf_search(_soc_ser_log_buffer_ptr[unit], buffer,
            _SOC_SER_LOG_BUFFER_SIZE, &criteria, &_soc_ser_log_match_entry);

    sal_free(buffer);
    return rv;
}

int
soc_ser_log_create_entry(int unit, int size)
{
    /* if someone tries to add an entry to a non-existant log, 
     * return the invalid id 
     */
    if (_soc_ser_log_buffer_ptr[unit] == NULL) {
        return 0;
    }
    return soc_log_buf_add(_soc_ser_log_buffer_ptr[unit], size);
}

/* Add tlv to a log, if it already exists overwrite it */
int
soc_ser_log_add_tlv(int unit, int id, soc_ser_log_tlv_type_t type,
    int size, void * buffer)
{
    int offset = 0;
    int entry_size = 0;
    int rc = SOC_E_NONE;
    soc_ser_log_tlv_hdr_t tlv_hdr;

    soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
        id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    while ((tlv_hdr.type != 0) &&
            (tlv_hdr.type != type)) {
        offset = offset + tlv_hdr.length + sizeof(soc_ser_log_tlv_hdr_t);
        soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit], id, offset,
            sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
        if (tlv_hdr.type == type) {
            break;
        }
    }
    /* If we're adding rather than overwriting, make sure we have room */
    rc = soc_log_buf_entry_get_size(_soc_ser_log_buffer_ptr[unit], id, &entry_size);
    
    if (((tlv_hdr.type != type) &&
        (entry_size < (offset + sizeof(soc_ser_log_tlv_hdr_t) + size)))
        || SOC_E_NONE != rc) {
        return SOC_E_PARAM;
    }
    
    /* If we're trying to overwrite, but with a different size tlv, error */
    if ((tlv_hdr.type == type) && (tlv_hdr.length != size)) {
        return SOC_E_PARAM;
    }

    tlv_hdr.type = type;
    tlv_hdr.length = size;
    soc_log_buf_entry_write(_soc_ser_log_buffer_ptr[unit],
        id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    offset += sizeof(soc_ser_log_tlv_hdr_t);
    soc_log_buf_entry_write(_soc_ser_log_buffer_ptr[unit], 
        id, offset, size, buffer);

    return SOC_E_NONE;
}

int
soc_ser_log_mod_tlv(int unit,
                    int id, 
                    soc_ser_log_tlv_type_t type, 
                    int size, 
                    void * buffer)
{
    int offset = 0;
    soc_ser_log_tlv_hdr_t tlv_hdr;

    soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
        id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    while (tlv_hdr.type != type) {
        if (tlv_hdr.type == 0) {
            return SOC_E_PARAM;
        }
        offset = offset + tlv_hdr.length + sizeof(soc_ser_log_tlv_hdr_t);
        soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit], 
            id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    }
    if (size != tlv_hdr.length) {
        return SOC_E_PARAM;
    }

    soc_log_buf_entry_write(_soc_ser_log_buffer_ptr[unit], id, offset +
        sizeof(soc_ser_log_tlv_hdr_t), size, buffer);
    return SOC_E_NONE;
}

int
soc_ser_log_get_tlv(int unit,
                    int id, 
                    soc_ser_log_tlv_type_t type, 
                    int size, 
                    void * buffer)
{
    int offset = 0;
    soc_ser_log_tlv_hdr_t tlv_hdr;

    soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
        id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    while (tlv_hdr.type != type) {
        if (tlv_hdr.type == 0) {
            return SOC_E_PARAM;
        }
        offset = offset + tlv_hdr.length + sizeof(soc_ser_log_tlv_hdr_t);
        soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
            id, offset, sizeof(soc_ser_log_tlv_hdr_t), &tlv_hdr);
    }

    if (size < tlv_hdr.length) {
        return SOC_E_PARAM;
    }

    soc_log_buf_entry_read(_soc_ser_log_buffer_ptr[unit],
        id, offset+sizeof(soc_ser_log_tlv_hdr_t), tlv_hdr.length, buffer);
    return SOC_E_NONE;
}

/* print an entry */
int
soc_ser_log_print_entry(int unit, void *buffer)
{
    soc_ser_log_tlv_hdr_t *tlv_hdr = buffer;

    while (tlv_hdr->type != 0) {
        soc_ser_log_print_tlv(unit, (char*)tlv_hdr);
        tlv_hdr = (soc_ser_log_tlv_hdr_t*)((char*)tlv_hdr +
            tlv_hdr->length + sizeof(soc_ser_log_tlv_hdr_t));
    }

    return SOC_E_NONE;
}

#define _SOC_SER_LOG_PARITY_TYPE_NAME(pt) ( \
    ((pt) == SOC_PARITY_TYPE_NONE)? "NONE" : \
    ((pt) == SOC_PARITY_TYPE_GENERIC)? "GENERIC" : \
    ((pt) == SOC_PARITY_TYPE_PARITY)? "PARITY" : \
    ((pt) == SOC_PARITY_TYPE_ECC)? "ECC" : \
    ((pt) == SOC_PARITY_TYPE_HASH)? "HASH" : \
    ((pt) == SOC_PARITY_TYPE_EDATABUF)? "EDATABUF" : \
    ((pt) == SOC_PARITY_TYPE_COUNTER)? "COUNTER" : \
    ((pt) == SOC_PARITY_TYPE_MXQPORT)? "MXQPORT" : \
    ((pt) == SOC_PARITY_TYPE_SER)? "SER" : \
    "NONE")

#define _SOC_SER_LOG_CORRECTED_TYPE(ct) ( \
    ((ct) == SOC_SER_UNKNOWN)? "UNKNOWN" : \
    ((ct) == SOC_SER_UNCORRECTED)? "NO" : \
    ((ct) == SOC_SER_CORRECTED)? "YES" : \
    "UNKNOWN")

#define _SOC_SER_LOG_SER_RESPONSE_TYPE(rf) ( \
    ((rf) & SOC_MEM_FLAG_SER_ENTRY_CLEAR)? "SER_ENTRY_CLEAR" : \
    ((rf) & SOC_MEM_FLAG_SER_CACHE_RESTORE)? "SER_CACHE_RESTORE" : \
    ((rf) & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE)? "SER_WRITE_CACHE_RESTORE" : \
    ((rf) & SOC_MEM_FLAG_SER_ECC_CORRECTABLE)? "SER_ECC_CORRECTABLE" : \
    ((rf) & SOC_MEM_FLAG_SER_SPECIAL)? "SER_SPECIAL" : \
    "UNKNOWN")

#define _SOC_SER_LOG_ACC_TYPE(at) ( \
    ((at) == SOC_SER_LOG_ACC_ANY)? "ACC_ANY" : \
    ((at) == SOC_SER_LOG_ACC_GROUP)? "ACC_GROUP" : \
    ((at) == SOC_SER_LOG_ACC_X)? "ACC_X" : \
    ((at) == SOC_SER_LOG_ACC_Y)? "ACC_Y" : \
    ((at) == SOC_SER_LOG_ACC_SBS)? "ACC_SBS" : \
    "UNKNOWN")

int
soc_ser_log_print_tlv(int unit, void *buffer)
{
    soc_ser_log_tlv_hdr_t *tlv_hdr = buffer;
    char *data = (char*)buffer + sizeof(soc_ser_log_tlv_hdr_t);
    int index;
#ifdef _SOC_SER_ENABLE_CLI_DBG
    soc_mem_t mem;
    soc_reg_t reg;
#endif
    soc_ser_parity_type_t parity_type;
    soc_ser_correct_t ct;
    soc_ser_log_flag_t flags;
    soc_ser_log_acc_type_t at;
    int ser_response_flag;
    
    LOG_CLI((BSL_META_U(unit, "Tlv Header:\n")));
    LOG_CLI((BSL_META_U(unit, "\ttype: %d\n"), (int)(tlv_hdr->type)));
    LOG_CLI((BSL_META_U(unit, "\tlength: %d\n"), (int)(tlv_hdr->length)));
    LOG_CLI((BSL_META_U(unit, "\tvalue: \n")));
    LOG_CLI((BSL_META_U(unit, "Tlv Data:\n")));
    switch (tlv_hdr->type) {
    case SOC_SER_LOG_TLV_MEMORY:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_MEMORY\n")));
#ifdef _SOC_SER_ENABLE_CLI_DBG
        mem = (int)(((soc_ser_log_tlv_memory_t*)(data))->memory);
        LOG_CLI((BSL_META_U(unit, "\t\tmemory: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_memory_t*)(data))->memory),
                   SOC_MEM_NAME(unit, mem)));
#else
        LOG_CLI((BSL_META_U(unit, "\t\tmemory: %d\n"),
                   (int)(((soc_ser_log_tlv_memory_t*)(data))->memory)));
#endif
        LOG_CLI((BSL_META_U(unit, "\t\tindex: %d\n"),
                   (int)(((soc_ser_log_tlv_memory_t*)(data))->index)));
        LOG_CLI((BSL_META_U(unit, "\t\thwbase: %d\n"),
                   (int)(((soc_ser_log_tlv_memory_t*)(data))->hwbase)));
        break;
    case SOC_SER_LOG_TLV_REGISTER:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_REGISTER\n")));
#ifdef _SOC_SER_ENABLE_CLI_DBG
        reg = (int)(((soc_ser_log_tlv_register_t*)(data))->reg);
        LOG_CLI((BSL_META_U(unit, "\t\tregister: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_register_t*)(data))->reg),
                   SOC_REG_NAME(unit, reg)));
#else
        LOG_CLI((BSL_META_U(unit, "\t\tregister: %d\n"),
                   (int)(((soc_ser_log_tlv_register_t*)(data))->reg)));
#endif
        LOG_CLI((BSL_META_U(unit, "\t\tindex: %d\n"),
                   (int)(((soc_ser_log_tlv_register_t*)(data))->index)));
        LOG_CLI((BSL_META_U(unit, "\t\tport: %d\n"),
                   (int)(((soc_ser_log_tlv_register_t*)(data))->port)));
        break;
    case SOC_SER_LOG_TLV_CONTENTS:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_CONTENTS\n")));
        for (index = 0; index < tlv_hdr->length; index++) {
            if ((index%16) == 0) {
                LOG_CLI((BSL_META_U(unit, "\t\t")));
            }
            LOG_CLI((BSL_META_U(unit, "%02x "), *((char*)(data + index))));
            if ((index%16)==15) {
                LOG_CLI((BSL_META_U(unit, "\n")));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        break;
    case SOC_SER_LOG_TLV_SER_FIFO:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_SER_FIFO\n")));
        for (index = 0; index < tlv_hdr->length; index++) {
            LOG_CLI((BSL_META_U(unit, "%02x "), *((char*)(data + index))));
            if ((index%16)==15) {
                LOG_CLI((BSL_META_U(unit, "\n")));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        break;
    case SOC_SER_LOG_TLV_CACHE:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_CACHE\n")));
        for (index = 0; index < tlv_hdr->length; index++) {
            if ((index%16) == 0) {
                LOG_CLI((BSL_META_U(unit, "\t\t")));
            }
            LOG_CLI((BSL_META_U(unit, "%02x "), *((char*)(data + index))));
            if ((index%16)==15) {
                LOG_CLI((BSL_META_U(unit, "\n")));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        break;
    case SOC_SER_LOG_TLV_GENERIC:
        LOG_CLI((BSL_META_U(unit, "\tSOC_SER_LOG_TLV_GENERIC\n")));
        flags = (int)(((soc_ser_log_tlv_generic_t*)(data))->flags);
        LOG_CLI((BSL_META_U(unit, "\t\tflags: %d (%s %s %s)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->flags),
                   (flags & SOC_SER_LOG_FLAG_MULTIBIT)? "MULTIPLE_1" : "MULTIPLE_0",
                   (flags & SOC_SER_LOG_FLAG_DOUBLEBIT)? ", DOUBLE_BIT" : ", SINGLE_BIT",
                   (flags & SOC_SER_LOG_FLAG_ERR_SRC)? ", ERR_SRC_CPU" : " "));
        LOG_CLI((BSL_META_U(unit, "\t\ttime: %u\n"),
                   ((soc_ser_log_tlv_generic_t*)(data))->time));
        LOG_CLI((BSL_META_U(unit, "\t\tboot_count: %d\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->boot_count)));
        LOG_CLI((BSL_META_U(unit, "\t\taddress: %d (0x%8x)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->address),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->address)));
        at = (int)(((soc_ser_log_tlv_generic_t*)(data))->acc_type);
        LOG_CLI((BSL_META_U(unit, "\t\tacc_type: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->acc_type),
                   _SOC_SER_LOG_ACC_TYPE(at)));
        LOG_CLI((BSL_META_U(unit, "\t\tpipe_num: %d\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->pipe_num)));
        LOG_CLI((BSL_META_U(unit, "\t\tinst: %d\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->inst)));
        LOG_CLI((BSL_META_U(unit, "\t\thw_cache: %d\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->hw_cache)));
        LOG_CLI((BSL_META_U(unit, "\t\tblock_type: %d\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->block_type)));
        parity_type = (int)(((soc_ser_log_tlv_generic_t*)(data))->parity_type);
        LOG_CLI((BSL_META_U(unit, "\t\tparity_type: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->parity_type),
                   _SOC_SER_LOG_PARITY_TYPE_NAME(parity_type)));
        ser_response_flag = (int)(((soc_ser_log_tlv_generic_t*)(data))->ser_response_flag);
        LOG_CLI((BSL_META_U(unit, "\t\tser_response_flag: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->ser_response_flag),
                   _SOC_SER_LOG_SER_RESPONSE_TYPE(ser_response_flag)));
        ct = (int)(((soc_ser_log_tlv_generic_t*)(data))->corrected);
        LOG_CLI((BSL_META_U(unit, "\t\tcorrected: %d (%s)\n"),
                   (int)(((soc_ser_log_tlv_generic_t*)(data))->corrected),
                   _SOC_SER_LOG_CORRECTED_TYPE(ct)));
        break;
    default:
        LOG_CLI((BSL_META_U(unit, "\tUnknown type\n")));
    }
    return SOC_E_NONE;
}

/* print everything */
int
soc_ser_log_print_all(int unit)
{
    int id = 0;
    do {
        id = soc_log_buf_get_next_id(_soc_ser_log_buffer_ptr[unit], id);

        if (id == 0) {
            return SOC_E_NONE;
        }

        SOC_IF_ERROR_RETURN(soc_ser_log_print_one(unit, id));
    } while (1);
    return SOC_E_NONE;
}

/* print log for specified id */
int
soc_ser_log_print_one(int unit, int id)
{
    int size; 
    int rc = SOC_E_NONE;
    char *buffer;
    
    if (id == 0) {
        return SOC_E_PARAM;
    }

    buffer = sal_alloc(_SOC_SER_LOG_BUFFER_SIZE, "ser log info");
    if (NULL == buffer) {
        return (SOC_E_MEMORY);
    }

    rc = soc_ser_log_get_entry_size(unit, id, &size);
    if ((size <= 0) || (size > _SOC_SER_LOG_BUFFER_SIZE)
        || (SOC_E_NONE != rc)) {
        sal_free(buffer);
        return SOC_E_INTERNAL;
    }

    soc_ser_log_get_entry(unit, id, size, buffer);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "\nLog Entry ID:%d\n"), id));
    soc_ser_log_print_entry(unit, buffer);

    sal_free(buffer);
    return SOC_E_NONE;
}

int
soc_ser_log_find_tlv(void *buffer, soc_ser_log_tlv_type_t type)
{
    soc_ser_log_tlv_hdr_t *tlv_hdr = buffer;
    int offset = 0;

    while ((tlv_hdr->type != type) && (tlv_hdr->type != 0)) {
        offset += tlv_hdr->length + sizeof(soc_ser_log_tlv_hdr_t);
        tlv_hdr = (soc_ser_log_tlv_hdr_t*)((char*)tlv_hdr +
            tlv_hdr->length + sizeof(soc_ser_log_tlv_hdr_t));
    }

    if (tlv_hdr->type == 0) {
        return SOC_E_PARAM;
    }
    return offset;
}

int
soc_ser_log_invalidate(int unit)
{
    sal_mutex_t mut = soc_log_buf_get_mutex(_soc_ser_log_buffer_ptr[unit]);
    if (mut == 0) {
        return SOC_E_INTERNAL;
    }
    soc_log_buf_set_mutex(_soc_ser_log_buffer_ptr[unit], 0);

    sal_mutex_destroy(mut);
    return SOC_E_NONE;
}

int 
soc_ser_log_init(int unit, void *location, int size) 
{
    int buffer_length;
    sal_mutex_t mut = NULL;
    static int init_done[SOC_MAX_NUM_DEVICES];
    int rv;

    if(unit >= SOC_MAX_NUM_DEVICES)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                  "SER Logging failed to check parameter Unit(%d)\n"), unit));
        return SOC_E_PARAM;
    }
    
    if(1 == init_done[unit])
    {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                   "SER Logging had been initialized\n")));
        return SOC_E_NONE;
    }
    
    mut = sal_mutex_create("SER_LOG_MUTEX");
    if (mut == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "SER Logging failed to create mutex\n")));
        return SOC_E_RESOURCE;
    }
   
    if (location == NULL) {
        _soc_ser_log_buffer_ptr[unit] = _soc_ser_log_buffer[unit];
        buffer_length = _SOC_SER_LOG_BUFFER_SIZE;
    } else {
        _soc_ser_log_buffer_ptr[unit] = location;
        buffer_length = size;
    }
    rv = soc_log_buf_init(_soc_ser_log_buffer_ptr[unit], buffer_length, mut);
    if (SOC_FAILURE(rv)) {
        sal_mutex_destroy(mut);
        return SOC_E_PARAM;
    }

    init_done[unit] = 1;

    return SOC_E_NONE;
}

int
soc_ser_log_get_boot_count(int unit)
{
    return soc_log_buf_get_boot_count(_soc_ser_log_buffer_ptr[unit]);
}
