/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/*
 * ! \file utilex_rhlist.c All utilities related to double link list (rhlist) 
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

#include <sal/appl/sal.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>

rhlist_t *
utilex_rhlist_create(
    char *name,
    int entry_size,
    int sanity)
{
    rhlist_t *rhlist;

    if (entry_size < sizeof(rhentry_t))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Entry size for list creation is too small:%d\n"), entry_size));
        return NULL;
    }

    rhlist = sal_alloc(sizeof(rhlist_t), name);
    if (rhlist)
    {
        sal_memset(rhlist, 0, sizeof(rhlist_t));
        if (name != NULL)
        {
            if (sal_strlen(name) >= RHNAME_MAX_SIZE)
            {
                name[RHNAME_MAX_SIZE - 1] = 0;
            }
            sal_strncpy_s(RHNAME(rhlist), name, RHNAME_MAX_SIZE);
        }
        rhlist->entry_size = entry_size;
        rhlist->sanity = sanity;
    }

    return rhlist;
}

/*
 * free all entries and list itself 
 */
void
utilex_rhlist_free_all(
    rhlist_t * rhlist)
{
    rhentry_t *next_entry, *entry;

    if (!rhlist)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL list handle\n")));
        return;
    }

    entry = rhlist->top;

    while (entry)
    {
        next_entry = entry->next;
        sal_free(entry);
        entry = next_entry;
    }
    sal_free(rhlist);
    return;
}

/*
 * free all entries, reset the content and leave the list 
 */
void
utilex_rhlist_clean(
    rhlist_t * rhlist)
{
    rhentry_t *next_entry, *entry;

    if (!rhlist)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL list handle\n")));
        return;
    }

    entry = rhlist->top;

    while (entry)
    {
        next_entry = entry->next;
        sal_free(entry);
        entry = next_entry;
    }

    rhlist->top = NULL;
    rhlist->tail = NULL;
    rhlist->num = 0;
    rhlist->max_id = 0;

    return;
}

void
utilex_rhlist_refresh_max_id(
    rhlist_t * rhlist)
{
    rhentry_t *entry;
    int max_id = 0;

    if (!rhlist)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL list handle\n")));
        return;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
        if (max_id < entry->id)
            max_id = entry->id;

    rhlist->max_id = max_id;
    return;
}

/************************************************************************************************
 * Only allocate the entry for specific list, but do not modify the list itself, besides max_id *
 */
shr_error_e
utilex_rhlist_entry_alloc(
    rhlist_t * rhlist,
    char *name,
    int id,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry;

    if (rhlist == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL list handle\n")));
        ret = _SHR_E_PARAM;
        goto out;
    }

    /*
     * Check whether the name exists 
     */
    if (rhlist->sanity && name)
    {
        if (utilex_rhlist_name_exists(rhlist, name, rhentry_p) == _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Sanity check on name:%s failed\n"), name));
            ret = _SHR_E_PARAM;
            goto out;
        }
        else if (sal_strlen(name) >= RHNAME_MAX_SIZE)
        {
            ret = _SHR_E_INTERNAL;
            goto out;
        }
    }

    /*
     * Assign the next number after maximum id 
     */
    if (id == RHID_TO_BE_GENERATED)
        id = ++rhlist->max_id;

    if (id < 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Negative entry id:%d is not allowed\n"), id));
        ret = _SHR_E_PARAM;
        goto out;
    }

    /*
     * Check whether the id exists 
     */
    if (rhlist->sanity && utilex_rhlist_id_exists(rhlist, id, NULL) == _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Sanity check on id:%d failed\n"), id));
        ret = _SHR_E_PARAM;
        goto out;
    }

    if (!(entry = sal_alloc(rhlist->entry_size, RHNAME(rhlist))))
    {
        ret = _SHR_E_MEMORY;
        goto out;
    }

    sal_memset(entry, 0, rhlist->entry_size);

    entry->id = id;

    if (name)
    {
        sal_strncpy_s(RHNAME(entry), name, RHNAME_MAX_SIZE - 1);
    }

    *rhentry_p = entry;

out:
    return ret;
}

shr_error_e
utilex_rhlist_entry_add_top(
    rhlist_t * rhlist,
    char *name,
    int id,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry;

    if (rhentry_p == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if ((ret = utilex_rhlist_entry_alloc(rhlist, name, id, rhentry_p)) != _SHR_E_NONE)
        goto out;

    entry = *rhentry_p;

    entry->next = rhlist->top;
    if (rhlist->top)
        rhlist->top->prev = entry;
    else
        rhlist->tail = entry;

    rhlist->top = entry;

    rhlist->num++;

    if (id > rhlist->max_id)
        rhlist->max_id = id;

out:
    return ret;
}

shr_error_e
utilex_rhlist_entry_add_tail(
    rhlist_t * rhlist,
    char *name,
    int id,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry;

    if (rhentry_p == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if ((ret = utilex_rhlist_entry_alloc(rhlist, name, id, rhentry_p)) != _SHR_E_NONE)
    {
        goto out;
    }

    entry = *rhentry_p;

    entry->prev = rhlist->tail;
    if (rhlist->tail)
        rhlist->tail->next = entry;
    else
        rhlist->top = entry;

    rhlist->tail = entry;

    rhlist->num++;

    if (id > rhlist->max_id)
        rhlist->max_id = id;

out:
    return ret;
}

shr_error_e
utilex_rhlist_entry_add_sorted(
    rhlist_t * rhlist,
    char *name,
    int id,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_NONE;
    rhhandle_t entry, entry_prev;

    if (rhentry_p == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if ((ret = utilex_rhlist_entry_alloc(rhlist, name, id, rhentry_p)) != _SHR_E_NONE)
    {
        goto out;
    }

    entry = *rhentry_p;

    RHITERATOR(entry_prev, rhlist)
    {
        int status = sal_strcasecmp(name, RHNAME(entry_prev));
        if (status == 0)
        {
            /*
             * No need to continue - we have this node already 
             */
            return _SHR_E_NONE;
        }
        else if (status > 0)
        {
            break;
        }
    }

    ret = utilex_rhlist_entry_insert_before(rhlist, entry, entry_prev);

out:
    return ret;
}

shr_error_e
utilex_rhlist_entry_insert_before(
    rhlist_t * rhlist,
    rhhandle_t rhentry,
    rhhandle_t rhentry_next)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry, *entry_next = (rhentry_t *) rhentry_next;

    if (rhentry == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    entry = (rhentry_t *) rhentry;
    /*
     * actual list management 
     */
    if (entry_next == NULL)
    {
        /*
         * NULL entry_next means we are at the end of list, so we add_tail it 
         */
        entry->prev = rhlist->tail;
        if (rhlist->tail)
            rhlist->tail->next = entry;
        else
            rhlist->top = entry;

        rhlist->tail = entry;
    }
    else if (entry_next->prev == NULL)
    {
        /*
         * NULL prev means we are inserting it at top, so add_top it 
         */
        entry->next = rhlist->top;
        if (rhlist->top)
            rhlist->top->prev = entry;
        else
            rhlist->tail = entry;

        rhlist->top = entry;
    }
    else
    {
        /*
         * Insert it in the middle 
         */
        entry->prev = entry_next->prev;
        entry_next->prev = entry;
        entry->prev->next = entry;
        entry->next = entry_next;
    }
    /*
     * end of list management 
     */
    rhlist->num++;

    if (entry->id > rhlist->max_id)
        rhlist->max_id = entry->id;

out:
    return ret;
}

shr_error_e
utilex_rhlist_entry_append(
    rhlist_t * rhlist,
    rhhandle_t rhentry)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry;

    if (rhlist == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if (rhentry == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    entry = (rhentry_t *) rhentry;
    /*
     * If there is no id - assign one
     */
    if (entry->id < 0)
    {
        entry->id = ++rhlist->max_id;
    }
    /*
     * Check whether the name exists 
     */
    if (rhlist->sanity && (utilex_rhlist_name_exists(rhlist, entry->name, NULL) == _SHR_E_NONE))
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    /*
     * Check whether the ID exists 
     */
    if (rhlist->sanity && (utilex_rhlist_id_exists(rhlist, entry->id, NULL) == _SHR_E_NONE))
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    entry->prev = rhlist->tail;
    entry->next = NULL;
    if (rhlist->tail)
        rhlist->tail->next = entry;
    else
        rhlist->top = entry;

    rhlist->tail = entry;

    rhlist->num++;

out:
    return ret;
}

/*
 * See utilex_rhlist.h
 */
shr_error_e
utilex_rhlist_entry_remove(
    rhlist_t * rhlist,
    rhhandle_t rhentry)
{
    shr_error_e ret = _SHR_E_NONE;
    rhentry_t *entry;

    if (!rhlist)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if (rhentry == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    entry = (rhentry_t *) rhentry;

    if (entry == rhlist->top)
        rhlist->top = entry->next;

    if (entry == rhlist->tail)
        rhlist->tail = entry->prev;

    if (entry->next)
        entry->next->prev = entry->prev;
    if (entry->prev)
        entry->prev->next = entry->next;

    rhlist->num--;

out:
    return ret;
}

/*
 * delete from double linked list and free
 */
shr_error_e
utilex_rhlist_entry_del_free(
    rhlist_t * rhlist,
    rhhandle_t rhentry)
{
    shr_error_e ret;

    if ((ret = utilex_rhlist_entry_remove(rhlist, rhentry)) == _SHR_E_NONE)
    {
        sal_free(rhentry);
    }

    return ret;
}

shr_error_e
utilex_rhlist_entry_exists(
    rhlist_t * rhlist,
    rhhandle_t rhentry)
{
    shr_error_e ret = _SHR_E_NOT_FOUND;
    rhentry_t *entry_tmp;

    if (!rhlist)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    if (rhentry == NULL)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    for (entry_tmp = rhlist->top; entry_tmp; entry_tmp = entry_tmp->next)
    {
        if (entry_tmp == rhentry)
            ret = _SHR_E_NONE;
    }

out:
    return ret;
}

rhhandle_t
utilex_rhlist_entry_get_by_id(
    rhlist_t * rhlist,
    int id)
{
    rhentry_t *entry;

    if (!rhlist)
    {
        return NULL;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
    {
        if (entry->id == id)
            return (rhhandle_t) entry;
    }

    return NULL;
}

rhhandle_t
utilex_rhlist_entry_get_by_name(
    rhlist_t * rhlist,
    char *name)
{
    rhentry_t *entry;

    if (!rhlist)
    {
        return NULL;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
    {
        if (!sal_strcasecmp(name, entry->name))
            return entry;
    }

    return NULL;
}

rhhandle_t
utilex_rhlist_entry_get_last_by_name(
    rhlist_t * rhlist,
    char *name)
{
    rhentry_t *entry;

    if (!rhlist)
    {
        return NULL;
    }

    for (entry = rhlist->tail; entry; entry = entry->prev)
    {
        if (!sal_strcasecmp(name, entry->name))
            return entry;
    }

    return NULL;
}

rhhandle_t
utilex_rhlist_entry_get_by_sub(
    rhlist_t * rhlist,
    char *name)
{
    rhentry_t *entry;

    if (!rhlist)
    {
        return NULL;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
    {
        if (!sal_strncasecmp(name, entry->name, sal_strlen(name)))
            return entry;
    }

    return NULL;
}

rhhandle_t
utilex_rhlist_entry_get_first(
    rhlist_t * rhlist)
{
    if (!rhlist)
    {
        return NULL;
    }

    return rhlist->top;
}

rhhandle_t
utilex_rhlist_entry_get_last(
    rhlist_t * rhlist)
{
    if (!rhlist)
    {
        return NULL;
    }

    return rhlist->tail;
}

rhhandle_t
utilex_rhlist_entry_get_next(
    rhhandle_t rhentry)
{
    if (!rhentry)
    {
        return NULL;
    }

    return ((rhentry_t *) rhentry)->next;
}

rhhandle_t
utilex_rhlist_entry_get_prev(
    rhhandle_t rhentry)
{
    if (!rhentry)
    {
        return NULL;
    }

    return ((rhentry_t *) rhentry)->prev;
}

int
utilex_rhlist_id_get_by_name(
    rhlist_t * rhlist,
    char *name)
{
    rhentry_t *entry;

    for (entry = rhlist->top; entry; entry = entry->next)
    {
        if (!sal_strcmp(name, entry->name))
            return entry->id;
    }

    return _SHR_E_INTERNAL;
}

shr_error_e
utilex_rhlist_name_exists(
    rhlist_t * rhlist,
    char *name,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_INTERNAL;
    rhentry_t *entry;

    if (!rhlist)
    {
        ret = _SHR_E_INTERNAL;
        goto out;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
        if (!sal_strcmp(entry->name, name))
        {
            ret = _SHR_E_NONE;
            if (rhentry_p != NULL)
                *rhentry_p = entry;
            goto out;
        }

out:
    return ret;
}

shr_error_e
utilex_rhlist_id_exists(
    rhlist_t * rhlist,
    int id,
    rhhandle_t * rhentry_p)
{
    shr_error_e ret = _SHR_E_NOT_FOUND;
    rhentry_t *entry;

    if (!rhlist)
    {
        ret = _SHR_E_PARAM;
        goto out;
    }

    for (entry = rhlist->top; entry; entry = entry->next)
        if (entry->id == id)
        {
            ret = _SHR_E_NONE;
            if (rhentry_p != NULL)
                *rhentry_p = entry;
            goto out;
        }

out:
    return ret;
}

char *
utilex_rhlist_name_get_by_id(
    rhlist_t * rhlist,
    int id)
{
    rhentry_t *entry;

    for (entry = rhlist->top; entry; entry = entry->next)
    {
        if (id == entry->id)
            return entry->name;
    }

    return NULL;
}
