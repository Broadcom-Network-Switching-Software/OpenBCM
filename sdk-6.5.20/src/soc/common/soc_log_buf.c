/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: soc_log_buf.c
 * Purpose: Thread safe circular buffer for logs
 */

#include <shared/bsl.h>

#include <soc/soc_log_buf.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <sal/appl/sal.h>

typedef struct _soc_log_buf_s {
    int first_entry;
    int last_entry;
    int buf_size;
    sal_mutex_t mutex;  
    int boot_count;
} _soc_log_buf_t;

typedef struct _soc_log_entry_hdr_s {
    int id;
    int size;
} _soc_log_entry_hdr_t;

sal_mutex_t
soc_log_buf_get_mutex(void *location)
{
    _soc_log_buf_t *buf = location;
    return buf->mutex;
}

int 
soc_log_buf_set_mutex(void *location, sal_mutex_t mutex)
{
    _soc_log_buf_t *buf = location;
    if (location == NULL) {
        return SOC_E_PARAM;
    }
    buf->mutex = mutex;
    return SOC_E_NONE;
}

STATIC int 
_soc_log_buf_read(void *location, int offset, void *data, int size)
{
    _soc_log_buf_t *buf_hdr = location;
    char *buf = (char*)location + sizeof(_soc_log_buf_t);
    int fixed_offset = offset % buf_hdr->buf_size;
    char *cur_log = buf + fixed_offset;
    int part_one = buf_hdr->buf_size - fixed_offset;

    if (part_one >= size) { /* There is no overrun */
        memcpy(data, cur_log, size);
    } else { /* The read must be broken into two parts */
        int part_two = size - part_one;
        char *cur_dst = data;
        memcpy(cur_dst, cur_log, part_one);
        cur_dst += part_one;
        cur_log = buf;
        memcpy(cur_dst, cur_log, part_two);
    }

    return SOC_E_NONE;
}

/* write from the buffer accouting for splits */
STATIC int 
_soc_log_buf_write(void *location, int offset, void *data, int size)
{
    _soc_log_buf_t *buf_hdr = location;
    char *buf = (char*)location + sizeof(_soc_log_buf_t);
    int fixed_offset = offset % buf_hdr->buf_size;
    char *cur_log = buf + fixed_offset;
    int part_one = buf_hdr->buf_size - fixed_offset;

    if (part_one >= size) { /* There is no overrun */
        memcpy(cur_log, data, size);
    } else { /* The write must be split into two parts */
        int part_two = size - part_one;
        char *cur_dst = data;
        memcpy(cur_log, cur_dst, part_one);
        cur_dst += part_one;
        cur_log = buf;
        memcpy(cur_log, cur_dst, part_two);
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_log_buf_clear(void *location, int offset, int size)
{
    _soc_log_buf_t *buf_hdr = location;
    char *buf = (char*)location + sizeof(_soc_log_buf_t);
    int fixed_offset = offset % buf_hdr->buf_size; /* Incase incoming offset is beyond buffer */
    char *cur_log = buf + fixed_offset;
    int part_one = buf_hdr->buf_size - fixed_offset;

    if (part_one >= size) { /* There is no overrun */
        sal_memset(cur_log, 0, size);
    } else { /* The write must be split into two parts */
        int part_two = size - part_one;
        sal_memset(cur_log, 0, part_one);
        cur_log = buf;
        sal_memset(cur_log, 0, part_two);
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_log_buf_entry_next(void *location, int offset)
{
    _soc_log_entry_hdr_t hdr;
    _soc_log_buf_t *buf_hdr = location;

    _soc_log_buf_read(location, offset, &hdr, sizeof(_soc_log_entry_hdr_t));
    return (hdr.size + sizeof(_soc_log_entry_hdr_t) + offset) % buf_hdr->buf_size;
}

STATIC int 
_soc_log_buf_entry_find(void *location, int id)
{
    _soc_log_entry_hdr_t hdr;
    _soc_log_buf_t *buf_hdr = location;
    int cur_entry = buf_hdr->first_entry;

    while (1) {
        _soc_log_buf_read(location, cur_entry, &hdr, sizeof(_soc_log_entry_hdr_t));
        if (hdr.id == id) {
            return cur_entry;
        }
        if (cur_entry == buf_hdr->last_entry) {
            return SOC_E_NOT_FOUND;
        }
        cur_entry = _soc_log_buf_entry_next(location, cur_entry);
    }
}

int
soc_log_buf_init(void *location, int size, sal_mutex_t mutex) 
{
    _soc_log_buf_t *buf = location;
    _soc_log_entry_hdr_t *entry = (_soc_log_entry_hdr_t*)((char*)location + sizeof(_soc_log_buf_t));

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(mutex, sal_mutex_FOREVER);

    if (size <= (sizeof(_soc_log_buf_t) + sizeof(_soc_log_entry_hdr_t))) {
        sal_mutex_give(mutex);
        return SOC_E_PARAM;
    }

    buf->first_entry = 0;
    buf->last_entry = 0;
    buf->buf_size = size - sizeof(_soc_log_buf_t);
    buf->boot_count = 0;
    buf->mutex = mutex;
    
    entry->id = 0;
    entry->size = 0;

    sal_mutex_give(mutex);

    return SOC_E_NONE;
}

int
soc_log_buf_add(void *location, int size)
{
    _soc_log_buf_t *buf_hdr = location;
    _soc_log_entry_hdr_t temp_entry_header;
    int new_entry_location;
    int free_space;
    int amount_to_write = size + sizeof(_soc_log_entry_hdr_t);
    int new_id;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    if (amount_to_write > buf_hdr->buf_size) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM;
    }

    _soc_log_buf_read(location, buf_hdr->last_entry, &temp_entry_header,
        sizeof(_soc_log_entry_hdr_t));

    new_entry_location = _soc_log_buf_entry_next(location, buf_hdr->last_entry);

    if (new_entry_location > buf_hdr->first_entry) {
        free_space = buf_hdr->buf_size - 
            (new_entry_location - buf_hdr->first_entry);
    } else {
        free_space = buf_hdr->first_entry - new_entry_location;
    }

    while ((free_space < amount_to_write) && (free_space != 0)) {
        buf_hdr->first_entry =
            _soc_log_buf_entry_next(location, buf_hdr->first_entry);

        if (new_entry_location > buf_hdr->first_entry) {
            free_space = buf_hdr->buf_size -
                (new_entry_location - buf_hdr->first_entry);
        } else {
            free_space = buf_hdr->first_entry - new_entry_location;
        }
    }

    temp_entry_header.size = size;
    temp_entry_header.id++;
    if (temp_entry_header.id == 0) { /* Zero is reserved */
        temp_entry_header.id++; 
    }
    new_id = temp_entry_header.id;
    _soc_log_buf_write(location, new_entry_location, 
        &temp_entry_header, sizeof(_soc_log_entry_hdr_t));
    buf_hdr->last_entry = new_entry_location;
    _soc_log_buf_clear(location, new_entry_location + 
        sizeof(_soc_log_entry_hdr_t), size); 

    sal_mutex_give(buf_hdr->mutex);

    return new_id;
}

int
soc_log_buf_inc_boot_count(void *location)
{
    _soc_log_buf_t *buf_hdr = location;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);
    buf_hdr->boot_count++;
    sal_mutex_give(buf_hdr->mutex);

    return SOC_E_NONE;
}

int 
soc_log_buf_get_boot_count(void *location)
{
    _soc_log_buf_t *buf_hdr = location;
    int boot_count;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);
    boot_count = buf_hdr->boot_count;
    sal_mutex_give(buf_hdr->mutex);

    return boot_count;
}

/* Get the size of an entries data */
int
soc_log_buf_entry_get_size(void *location, int id, int *size)
{
    _soc_log_buf_t *buf_hdr = location;
    _soc_log_entry_hdr_t hdr;
    int entry_offset;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    entry_offset = _soc_log_buf_entry_find(location, id);
    if (entry_offset < 0) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM; /*invalid entry id */
    }

    _soc_log_buf_read(location, entry_offset, &hdr,
        sizeof(_soc_log_entry_hdr_t));

    sal_mutex_give(buf_hdr->mutex);

    *size = hdr.size;

    return SOC_E_NONE;
}

/* Copy a section of an entry to a buffer */
int 
soc_log_buf_entry_read(void *location,
                        int id, 
                        int offset, 
                        int size, 
                        void *data)
{
    _soc_log_buf_t *buf_hdr = location;
    _soc_log_entry_hdr_t hdr;
    int entry_offset;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    entry_offset = _soc_log_buf_entry_find(location, id);
    if (entry_offset < 0) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM;
    }

    _soc_log_buf_read(location, entry_offset, &hdr,
        sizeof(_soc_log_entry_hdr_t));

    if ((offset + size) > hdr.size) { 
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM;
    }

    _soc_log_buf_read(location, 
        entry_offset + offset + sizeof(_soc_log_entry_hdr_t), data, size);
    
    sal_mutex_give(buf_hdr->mutex);
    return SOC_E_NONE;
}

/* write to a section of an entry */
int 
soc_log_buf_entry_write(void *location,
                        int id, 
                        int offset, 
                        int size, 
                        void *data)
{
    _soc_log_entry_hdr_t hdr;
    _soc_log_buf_t *buf_hdr = location;
    int entry_offset;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    entry_offset = _soc_log_buf_entry_find(location, id);
    if (entry_offset < 0) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM;
    }

    _soc_log_buf_read(location, entry_offset, &hdr,
        sizeof(_soc_log_entry_hdr_t));

    if ((offset + size) > hdr.size) { 
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM;
    }

    _soc_log_buf_write(location,
        entry_offset + offset + sizeof(_soc_log_entry_hdr_t), data, size);
    
    sal_mutex_give(buf_hdr->mutex);
    return SOC_E_NONE;
}

int 
soc_log_buf_entry_valid(void *location, int id)
{
    _soc_log_entry_hdr_t hdr;
    _soc_log_buf_t *buf_hdr = location;
    int entry_offset;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    entry_offset = _soc_log_buf_entry_find(location, id);
    if (entry_offset < 0) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_PARAM; /*invalid entry id */
    }

    _soc_log_buf_read(location,
        entry_offset, &hdr, sizeof(_soc_log_entry_hdr_t));
    if ((hdr.size < 0) || (hdr.size > buf_hdr->buf_size)) {
        sal_mutex_give(buf_hdr->mutex);
        return SOC_E_INTERNAL;
    }

    sal_mutex_give(buf_hdr->mutex);
    return SOC_E_NONE;
}

STATIC int
_soc_log_buf_print_hdr(void *location)
{
    _soc_log_buf_t *buf_hdr = location;

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("Log Buffer Header\n")));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tfirst_entry: %d\n"), buf_hdr->first_entry));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tlast_entry: %d\n"), buf_hdr->last_entry));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tbuf_size: %d\n"), buf_hdr->buf_size));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tmutex: %p\n"), (char*)(buf_hdr->mutex)));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tboot_count: %d\n"), buf_hdr->boot_count));

    return SOC_E_NONE;
}

#define _SOC_LOG_BUF_PRINT_BUFFER_SIZE 16
STATIC int
_soc_log_buf_print_entry(void *location, int offset)
{
    _soc_log_entry_hdr_t hdr;
    int index;
    char temp_buf[_SOC_LOG_BUF_PRINT_BUFFER_SIZE];

    _soc_log_buf_read(location, offset, &hdr, sizeof(_soc_log_entry_hdr_t));

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("Log Entry Header\n")));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\toffset: %d\n"), offset));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tid: %d\n"), hdr.id));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tsize: %d\n"), hdr.size));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("\tdata: \n\t\t")));

    _soc_log_buf_read(location, 
        offset+sizeof(_soc_log_entry_hdr_t),
        &temp_buf, _SOC_LOG_BUF_PRINT_BUFFER_SIZE);
    for (index = 0; index < hdr.size; index++) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("%02x "),
                   temp_buf[index%_SOC_LOG_BUF_PRINT_BUFFER_SIZE]));
        if (((index+1)%_SOC_LOG_BUF_PRINT_BUFFER_SIZE) == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("\n\t\t")));
            _soc_log_buf_read(location, 
                offset+sizeof(_soc_log_entry_hdr_t) + index + 1, 
                &temp_buf, 
                _SOC_LOG_BUF_PRINT_BUFFER_SIZE);
        }
    }
    if ((index%_SOC_LOG_BUF_PRINT_BUFFER_SIZE) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("\n")));
    }

    return SOC_E_NONE;
}

int 
soc_log_buf_print(void *location)
{
    _soc_log_buf_t *buf_hdr = location;
    int offset;
    
    if (location == NULL) {
        return SOC_E_PARAM;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    offset = buf_hdr->first_entry;
    _soc_log_buf_print_hdr(location);
    while (1) {
        _soc_log_buf_print_entry(location, offset);
        if (offset == buf_hdr->last_entry) {
            break;
        }
        offset = _soc_log_buf_entry_next(location, offset);
    }

    sal_mutex_give(buf_hdr->mutex);

    return SOC_E_NONE;
}

int 
soc_log_buf_get_next_id(void *location, int id)
{
    _soc_log_buf_t *buf_hdr = location;
    _soc_log_entry_hdr_t hdr;
    int offset = 0;
    
    if (location == NULL) {
        return 0;
    }

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    if (id == 0) {
        _soc_log_buf_read(location, buf_hdr->first_entry, &hdr, sizeof(_soc_log_entry_hdr_t));
        if (hdr.id != 0) {
            sal_mutex_give(buf_hdr->mutex);
            return hdr.id;
        }
    }

    _soc_log_buf_read(location, buf_hdr->last_entry, &hdr, sizeof(_soc_log_entry_hdr_t));
    if (id == hdr.id) {
        sal_mutex_give(buf_hdr->mutex);
        return 0;
    }

    offset = _soc_log_buf_entry_find(location, id);
    offset = _soc_log_buf_entry_next(location, offset);
    _soc_log_buf_read(location, offset, &hdr, sizeof(_soc_log_entry_hdr_t));
    sal_mutex_give(buf_hdr->mutex);
    return hdr.id;
}

int
soc_log_buf_search(void *location,
                   void *buffer,
                   int buf_size,
                   void *criteria, 
                   int (*match)(void * crit, void * entry))
{
    _soc_log_entry_hdr_t hdr;
    _soc_log_buf_t *buf_hdr = location;
    int cur_entry;

    if (location == NULL) {
        return SOC_E_PARAM;
    }

    cur_entry = buf_hdr->first_entry;

    sal_mutex_take(buf_hdr->mutex, sal_mutex_FOREVER);

    while (1) {
        /* read the current header */
        _soc_log_buf_read(location, cur_entry, &hdr, sizeof(_soc_log_entry_hdr_t));

        if (hdr.size > buf_size) {
            sal_mutex_give(buf_hdr->mutex);
            return SOC_E_PARAM;
        }

        _soc_log_buf_read(location, cur_entry + sizeof(_soc_log_entry_hdr_t),
            buffer, hdr.size);

        /* if match returns true, we've found what we were looking for */
        if ((hdr.id != 0) && (match(criteria, buffer))) {
            sal_mutex_give(buf_hdr->mutex);
            return hdr.id;
        }

        if (cur_entry == buf_hdr->last_entry) {
            sal_mutex_give(buf_hdr->mutex);
            return SOC_E_NOT_FOUND;
        }
        cur_entry = _soc_log_buf_entry_next(location, cur_entry);
    }

    sal_mutex_give(buf_hdr->mutex);

    return SOC_E_NOT_FOUND;
}


