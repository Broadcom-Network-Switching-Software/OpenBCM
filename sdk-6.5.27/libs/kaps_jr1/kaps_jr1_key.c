

#include "kaps_jr1_ab.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_device_wb.h"



kaps_jr1_status
kaps_jr1_key_init(
    struct kaps_jr1_device *device,
    struct kaps_jr1_key **key)
{
    struct kaps_jr1_key *new_key;

    KAPS_JR1_TRACE_IN("%p %p\n", device, key);
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!key)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    new_key = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_key));
    if (!new_key)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    new_key->device = device;
    *key = new_key;
    KAPS_JR1_TRACE_OUT("%p\n", *key);

    return KAPS_JR1_OK;
}

void
kaps_jr1_key_destroy_internal(
    struct kaps_jr1_key *key)
{
    struct kaps_jr1_key_field *f;
    if (!key)
        return;

    f = key->first_field;
    while (f)
    {
        struct kaps_jr1_key_field *tmp = f->next;

        key->device->alloc->xfree(key->device->alloc->cookie, f);
        f = tmp;
    }

    f = key->first_overlay_field;
    while (f)
    {
        struct kaps_jr1_key_field *tmp = f->next;

        key->device->alloc->xfree(key->device->alloc->cookie, f);
        f = tmp;
    }

    key->device->alloc->xfree(key->device->alloc->cookie, key);
}

void
kaps_jr1_key_adjust_offsets(
    struct kaps_jr1_key *key)
{
    struct kaps_jr1_key_field *tmp;
    uint32_t cur_offset_1;

    
    cur_offset_1 = 0;
    for (tmp = key->first_field; tmp; tmp = tmp->next)
    {
        tmp->offset_1 = cur_offset_1;
        cur_offset_1 += tmp->width_1;
    }
    key->width_1 = cur_offset_1;
}


void
kaps_jr1_key_move_internal(
    struct kaps_jr1_key *key,
    struct kaps_jr1_key_field *field,
    uint32_t offset_1)
{
    struct kaps_jr1_key_field *tmp;
    uint32_t found = 0;

    kaps_jr1_sassert(field);

    if (field->offset_1 == offset_1)
        return;

    
    if (field->prev)
        field->prev->next = field->next;
    if (field->next)
        field->next->prev = field->prev;

    
    if (key->first_field == field)
        key->first_field = field->next;
    if (key->last_field == field)
        key->last_field = field->prev;

    for (tmp = key->first_field; tmp; tmp = tmp->next)
    {
        if (tmp->offset_1 == offset_1)
        {
            found = 1;
            break;
        }
    }
    kaps_jr1_sassert(found);

    if (offset_1 < field->offset_1)
    {
        
        field->next = tmp;
        field->prev = tmp->prev;
        if (tmp->prev)
            tmp->prev->next = field;
        tmp->prev = field;
        if (offset_1 == 0)
            key->first_field = field;
    }
    else
    {
        
        if (tmp->next)
            tmp->next->prev = field;
        field->next = tmp->next;
        field->prev = tmp;
        tmp->next = field;
        if (key->last_field == tmp)
            key->last_field = field;
    }

    kaps_jr1_key_adjust_offsets(key);
}

kaps_jr1_status
kaps_jr1_key_add_field_internal(
    struct kaps_jr1_key *key,
    char *name,
    uint32_t width_1,
    enum kaps_jr1_key_field_type type,
    uint32_t is_user_bmr)
{
    struct kaps_jr1_key_field *field;
    uint32_t size8;

    if (!name || !key)
        return KAPS_JR1_INVALID_ARGUMENT;

    if ((width_1 == 0) || (width_1 > KAPS_JR1_HW_MAX_DBA_WIDTH_1))
        return KAPS_JR1_INVALID_KEY_WIDTH;

    if ((key->device->type != KAPS_JR1_DEVICE) && (width_1 % 8 != 0))
        return KAPS_JR1_INVALID_KEY_WIDTH;

    if (type >= KAPS_JR1_KEY_FIELD_INVALID)
        return KAPS_JR1_INVALID_KEY_TYPE;

    if (key->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE)
            continue;
        if (strcmp(field->name, name) == 0)
            return KAPS_JR1_DUPLICATE_KEY_FIELD;
    }

    if (type == KAPS_JR1_KEY_FIELD_HOLE && is_user_bmr)
        key->has_user_bmr = 1;

    size8 = sizeof(*field) + strlen(name) + 1;
    field = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, size8);
    if (!field)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    field->name = (char *) field + sizeof(*field);
    strcpy(field->name, name);
    field->width_1 = width_1;
    field->type = type;
    field->next = NULL;
    field->prev = NULL;
    key->width_1 += width_1;
    field->rinfo = NULL;
    field->do_not_bmr = 0;
    field->is_usr_bmr = 0;
    field->is_padding_field = 0;
    if (type == KAPS_JR1_KEY_FIELD_HOLE && is_user_bmr)
        field->is_usr_bmr = 1;

    if (!key->first_field)
    {
        
        field->offset_1 = 0;
        key->first_field = field;
    }
    else
    {
        
        field->offset_1 = key->last_field->offset_1 + key->last_field->width_1;
        key->last_field->next = field;
        field->prev = key->last_field;
    }

    field->orig_offset_1 = field->offset_1;
    if (type == KAPS_JR1_KEY_FIELD_RANGE)
        key->nranges++;
    if (type == KAPS_JR1_KEY_FIELD_DUMMY_FILL)
        key->has_dummy_fill_field = 1;
    key->last_field = field;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_add_field(
    struct kaps_jr1_key * key,
    char *name,
    uint32_t width_1,
    enum kaps_jr1_key_field_type type)
{
    uint32_t is_user_bmr = 0;

    KAPS_JR1_TRACE_IN("%p %s %u %d\n", key, name, width_1, type);
    if (type == KAPS_JR1_KEY_FIELD_HOLE)
        is_user_bmr = 1;

    return kaps_jr1_key_add_field_internal(key, name, width_1, type, is_user_bmr);
}

kaps_jr1_status
kaps_jr1_key_set_critical_field(
    struct kaps_jr1_key * key,
    char *name)
{
    struct kaps_jr1_key_field *field;

    KAPS_JR1_TRACE_IN("%p %s\n", key, name);
    if (!name || !key)
        return KAPS_JR1_INVALID_ARGUMENT;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE)
            continue;
        if (field->type == KAPS_JR1_KEY_FIELD_RANGE)
            continue;
        if (field->type == KAPS_JR1_KEY_FIELD_DUMMY_FILL)
            continue;
        if (strcmp(field->name, name) == 0)
        {
            break;
        }
    }

    if (!field)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (field->field_prio)
        return KAPS_JR1_INVALID_ARGUMENT;

    key->critical_field_count++;
    field->field_prio = key->critical_field_count;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_overlay_field(
    struct kaps_jr1_key * key,
    char *name,
    uint32_t width_1,
    enum kaps_jr1_key_field_type type,
    uint32_t offset_1)
{
    struct kaps_jr1_key_field *field;
    uint32_t size8;

    KAPS_JR1_TRACE_IN("%p %s %u %d %u\n", key, name, width_1, type, offset_1);
    if (!name || !key)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (key->device->type == KAPS_JR1_DEVICE)
        return KAPS_JR1_UNSUPPORTED;

    if (width_1 % 8 != 0)
        return KAPS_JR1_INVALID_KEY_WIDTH;

    if ((width_1 == 0) || (width_1 >= key->width_1))
        return KAPS_JR1_INVALID_KEY_WIDTH;

    if (offset_1 % 8 != 0)
        return KAPS_JR1_INVALID_ARGUMENT;

    if ((offset_1 + width_1) > key->width_1)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (type >= KAPS_JR1_KEY_FIELD_INVALID)
        return KAPS_JR1_INVALID_KEY_TYPE;

    if (key->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (type == KAPS_JR1_KEY_FIELD_HOLE)
        return KAPS_JR1_INVALID_ARGUMENT;

    for (field = key->first_overlay_field; field; field = field->next)
    {
        if (strcmp(field->name, name) == 0)
            return KAPS_JR1_DUPLICATE_KEY_FIELD;
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE)
            continue;
        if (strcmp(field->name, name) == 0)
            return KAPS_JR1_DUPLICATE_KEY_FIELD;
    }

    size8 = sizeof(*field) + strlen(name) + 1;
    field = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, size8);
    if (!field)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    field->name = (char *) field + sizeof(*field);
    strcpy(field->name, name);
    field->width_1 = width_1;
    field->type = type;
    field->next = NULL;
    field->prev = NULL;

    field->rinfo = NULL;
    field->do_not_bmr = 0;
    field->is_usr_bmr = 0;
    field->is_padding_field = 0;

    field->offset_1 = offset_1;

    if (!key->first_overlay_field)
    {
        key->first_overlay_field = field;
    }
    else
    {
        key->last_overlay_field->next = field;
        field->prev = key->last_overlay_field;
    }

    key->last_overlay_field = field;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_verify(
    struct kaps_jr1_key * master_key,
    struct kaps_jr1_key * db_key,
    char **error_field_name)
{
    int32_t start = -1, end = -1;
    struct kaps_jr1_key_field *db_key_field;

    KAPS_JR1_TRACE_IN("%p %p %p\n", master_key, db_key, error_field_name);
    if (!db_key)
        return KAPS_JR1_INVALID_KEY;
    if (!master_key)
        return KAPS_JR1_INVALID_MASTER_KEY;
    if (master_key->device->type == KAPS_JR1_DEVICE)
        return KAPS_JR1_OK;

    db_key_field = db_key->first_field;

    while (db_key_field)
    {
        struct kaps_jr1_key_field *master_key_field = master_key->first_field;

        if (db_key_field->type == KAPS_JR1_KEY_FIELD_HOLE)
        {
            db_key_field = db_key_field->next;
            continue;
        }
        while (master_key_field)
        {
            if (strcmp(master_key_field->name, db_key_field->name) == 0)
            {
                if (master_key_field->width_1 == db_key_field->width_1)
                {
                    if (master_key_field->type != KAPS_JR1_KEY_FIELD_DUMMY_FILL)
                    {
                        if (master_key_field->type == db_key_field->type)
                            break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            master_key_field = master_key_field->next;
        }

        if (master_key_field == NULL && master_key->first_overlay_field)
        {
            master_key_field = master_key->first_overlay_field;
            while (master_key_field)
            {
                if (strcmp(master_key_field->name, db_key_field->name) == 0)
                {
                    if (master_key_field->width_1 == db_key_field->width_1)
                    {
                        if (master_key_field->type != KAPS_JR1_KEY_FIELD_DUMMY_FILL)
                        {
                            if (master_key_field->type == db_key_field->type)
                                break;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                master_key_field = master_key_field->next;
            }
        }

        if (master_key_field == NULL)
        {

            if (error_field_name)
            {
                *error_field_name = db_key_field->name;
                KAPS_JR1_TRACE_OUT("%s\n", *error_field_name);
            }
            return KAPS_JR1_KEY_FIELD_MISSING;
        }

        if (start == -1)
        {
            kaps_jr1_sassert(end == -1);
            start = master_key_field->offset_1;
            end = start + master_key_field->width_1;
        }
        else if (master_key_field->offset_1 == end)
        {
            end = end + master_key_field->width_1;
        }
        else
        {
            if (start & 0x7 || end & 0x7)
            {
                return KAPS_JR1_KEY_GRAN_ERROR;
            }
            start = master_key_field->offset_1;
            end = start + master_key_field->width_1;
        }

        db_key_field = db_key_field->next;
    }
    if (start != -1)
    {
        if (start & 0x7)
        {
            return KAPS_JR1_KEY_GRAN_ERROR;
        }
    }
    return KAPS_JR1_OK;
}

char *
kaps_jr1_key_get_type_internal(
    enum kaps_jr1_key_field_type type)
{
    if (type == KAPS_JR1_KEY_FIELD_TERNARY)
        return "ternary";
    else if (type == KAPS_JR1_KEY_FIELD_PREFIX)
        return "prefix";
    else if (type == KAPS_JR1_KEY_FIELD_EM)
        return "em";
    else if (type == KAPS_JR1_KEY_FIELD_RANGE)
        return "range";
    else if (type == KAPS_JR1_KEY_FIELD_TABLE_ID)
        return "tid";
    else if (type == KAPS_JR1_KEY_FIELD_DUMMY_FILL)
        return "dummyfill";
    else
        return "hole";
}

kaps_jr1_status
kaps_jr1_key_print(
    struct kaps_jr1_key * key,
    FILE * file)
{
    struct kaps_jr1_key_field *field;
    uint32_t fcount = 0, count = 0, next_offset = 0;

    KAPS_JR1_TRACE_IN("%p %p\n", key, file);
    if (!key || !file)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (key->device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (key->device->issu_status != KAPS_JR1_ISSU_INIT)
        {
            return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE && !field->is_usr_bmr)
            continue;
        fcount++;
    }
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;
            kaps_jr1_fprintf(file, "(%s,%d,%s) ", field->name, field->width_1, kaps_jr1_key_get_type_internal(field->type));
            break;
        }
    }
    while (count < fcount);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_print_internal(
    struct kaps_jr1_key * key,
    FILE * file)
{
    struct kaps_jr1_key_field *field;

    if (!key || !file)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (key->device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (key->device->issu_status != KAPS_JR1_ISSU_INIT)
        {
            return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    for (field = key->first_field; field; field = field->next)
    {
        kaps_jr1_fprintf(file, "(%s,%d,%s)", field->name, field->width_1, kaps_jr1_key_get_type_internal(field->type));
    }
    kaps_jr1_fprintf(file, "\n");
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_clone(
    struct kaps_jr1_device * device,
    struct kaps_jr1_key * key,
    struct kaps_jr1_key ** result)
{
    struct kaps_jr1_key *new_key;
    struct kaps_jr1_key_field *field;
    int32_t count, fcount;
    uint32_t next_offset;
    kaps_jr1_status status;

    if (device == NULL)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (key == NULL || result == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    status = kaps_jr1_key_init(device, &new_key);
    if (status != KAPS_JR1_OK)
        return status;

    

    count = fcount = 0;
    next_offset = 0;
    for (field = key->first_field; field; field = field->next)
    {
        fcount++;
    }

    status = KAPS_JR1_OK;
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;
            status = kaps_jr1_key_add_field_internal(new_key, field->name, field->width_1, field->type, field->is_usr_bmr);
            break;
        }
        if (status != KAPS_JR1_OK)
            break;
    }
    while (count < fcount);

    for (field = key->first_overlay_field; field; field = field->next)
    {
        status = kaps_jr1_key_overlay_field(new_key, field->name, field->width_1, field->type, field->offset_1);
        if (status != KAPS_JR1_OK)
            break;
    }

    if (status != KAPS_JR1_OK)
    {
        kaps_jr1_key_destroy_internal(new_key);
        new_key = NULL;
    }

    *result = new_key;
    return status;
}

kaps_jr1_status
kaps_jr1_key_validate_internal(
    struct kaps_jr1_db * db,
    struct kaps_jr1_key * key)
{
    if (!db || !key)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (db->is_clone)
    {
        

        struct kaps_jr1_key_field *f1, *f2;

        if (db->clone_of->key == NULL)
            return KAPS_JR1_CLONE_PARENT_KEY_MISSING;

        f1 = key->first_field;
        f2 = db->clone_of->key->first_field;
        for (; f1 && f2; f2 = f2->next, f1 = f1->next)
        {
            
            if (f1->type == KAPS_JR1_KEY_FIELD_HOLE && db->type == KAPS_JR1_DB_LPM && !db->common_info->mapped_to_acl)
                return KAPS_JR1_LPM_KEY_WITH_HOLE_UNSUPPORTED;

            
            if (f1->type == KAPS_JR1_KEY_FIELD_HOLE && f2->type == KAPS_JR1_KEY_FIELD_RANGE)
                return KAPS_JR1_RANGE_FIELDS_AS_HOLE_UNSUPPORTED;

            
            if (f1->type == KAPS_JR1_KEY_FIELD_HOLE && f2->type == KAPS_JR1_KEY_FIELD_TABLE_ID)
                return KAPS_JR1_TABLE_ID_AS_HOLE_UNSUPPORTED;

            if ((f1->width_1 != f2->width_1 || f1->type != f2->type)
                && (f1->type != KAPS_JR1_KEY_FIELD_HOLE && f2->type != KAPS_JR1_KEY_FIELD_HOLE))
                return KAPS_JR1_CLONE_KEY_FORMAT_MISMATCH;

            f1->p_field = f2;
        }
        if (f1 != NULL || f2 != NULL)
            return KAPS_JR1_CLONE_KEY_FORMAT_MISMATCH;
    }
    else if (db->parent || db->has_tables)
    {
        
        struct kaps_jr1_db *cur_db;
        int32_t table_id_offset = -1, tid_width = -1;

        if (db->parent)
            cur_db = db->parent;
        else
            cur_db = db;

        for (; cur_db; cur_db = cur_db->next_tab)
        {
            struct kaps_jr1_key *cur_db_key = cur_db->key;
            struct kaps_jr1_key_field *f;
            int32_t num_tid = 0;

            if ((struct kaps_jr1_db *) cur_db == db)
                cur_db_key = key;

            if (!cur_db_key || cur_db->is_clone)
                continue;

            for (f = cur_db_key->first_field; f; f = f->next)
            {
                if (f->type == KAPS_JR1_KEY_FIELD_TABLE_ID)
                {
                    num_tid++;

                    if (table_id_offset == -1)
                    {
                        table_id_offset = f->offset_1;
                        tid_width = f->width_1;
                    }
                    else if (f->offset_1 != table_id_offset)
                    {
                        return KAPS_JR1_INCONSISTENT_TABLE_ID_OFFSET;
                    }
                    else if (f->width_1 != tid_width)
                    {
                        return KAPS_JR1_INCONSISTENT_TABLE_ID_WIDTH;
                    }
                }
            }

            if (num_tid != 1)
                return KAPS_JR1_MISSING_TABLE_ID_FIELD;
        }
    }

    return KAPS_JR1_OK;
}


struct kaps_jr1_key_wb_info
{
    uint32_t fcount;      
    uint32_t f_name_len;  
    uint32_t ftype;       
    uint32_t width_1;     
    uint8_t fname[];     
};

struct kaps_jr1_key_overlay_wb_info
{
    uint32_t f_name_len;  
    uint32_t ftype;       
    uint32_t width_1;     
    uint32_t offset_1;    
    uint8_t fname[];     
};

kaps_jr1_status
kaps_jr1_key_wb_save(
    struct kaps_jr1_key *key,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    int32_t count, fcount = 0;
    uint32_t next_offset;
    uint32_t overlay_field_cnt = 0;
    struct kaps_jr1_key_field *field;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE && !field->is_usr_bmr && !field->is_padding_field)
            continue;
        fcount++;
    }

    

    count = 0;
    next_offset = 0;

    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            struct kaps_jr1_key_wb_info *wb_info_ptr;
            uint32_t total_size;

            if (next_offset != field->orig_offset_1)
                continue;

            count++;
            next_offset = next_offset + field->width_1;
            total_size = sizeof(*wb_info_ptr) + strlen(field->name) + 1;
            if (wb_fun->nv_ptr == NULL)
            {
                wb_info_ptr = key->device->alloc->xmalloc(key->device->alloc->cookie, total_size);
                if (wb_info_ptr == NULL)
                    return KAPS_JR1_OUT_OF_MEMORY;
            }
            else
            {
                wb_info_ptr = (struct kaps_jr1_key_wb_info *) wb_fun->nv_ptr;
            }

            kaps_jr1_memset(wb_info_ptr, 0, total_size);
            wb_info_ptr->fcount = fcount;
            wb_info_ptr->f_name_len = strlen(field->name) + 1;
            wb_info_ptr->ftype = field->type;
            wb_info_ptr->width_1 = field->width_1;
            kaps_jr1_memcpy(wb_info_ptr->fname, field->name, wb_info_ptr->f_name_len);

            if (wb_fun->write_fn)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
            if (wb_fun->nv_ptr)
            {
                wb_fun->nv_ptr += total_size;
            }
            else
            {
                key->device->alloc->xfree(key->device->alloc->cookie, wb_info_ptr);
            }
            break;
        }
    }
    while (count < fcount);

    
    overlay_field_cnt = 0;
    for (field = key->first_overlay_field; field; field = field->next)
    {
        overlay_field_cnt++;
    }

    if (wb_fun->write_fn)
    {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & overlay_field_cnt,
                                  sizeof(overlay_field_cnt), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    else
    {
        *((uint32_t *) wb_fun->nv_ptr) = overlay_field_cnt;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(overlay_field_cnt);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(overlay_field_cnt);

    for (field = key->first_overlay_field; field; field = field->next)
    {
        uint32_t total_size;
        struct kaps_jr1_key_overlay_wb_info *wb_info_ptr;

        total_size = sizeof(*wb_info_ptr) + strlen(field->name) + 1;
        if (wb_fun->nv_ptr == NULL)
        {
            wb_info_ptr = key->device->alloc->xmalloc(key->device->alloc->cookie, total_size);
            if (wb_info_ptr == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
        }
        else
        {
            wb_info_ptr = (struct kaps_jr1_key_overlay_wb_info *) wb_fun->nv_ptr;
        }

        kaps_jr1_memset(wb_info_ptr, 0, total_size);
        wb_info_ptr->f_name_len = strlen(field->name) + 1;
        wb_info_ptr->ftype = field->type;
        wb_info_ptr->width_1 = field->width_1;
        wb_info_ptr->offset_1 = field->offset_1;
        kaps_jr1_memcpy(wb_info_ptr->fname, field->name, wb_info_ptr->f_name_len);

        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
        {
            wb_fun->nv_ptr += total_size;
        }
        else
        {
            key->device->alloc->xfree(key->device->alloc->cookie, wb_info_ptr);
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_key_wb_restore(
    struct kaps_jr1_key * key,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    int32_t f_no, fcount = 0;
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_key_wb_info wb_info = {0};
    struct kaps_jr1_key_wb_info *wb_info_ptr;
    uint32_t overlay_field_cnt = 0;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = &wb_info;
    }
    else
    {
        wb_info_ptr = (struct kaps_jr1_key_wb_info *) wb_fun->nv_ptr;
    }

    
    if (wb_fun->read_fn != NULL)
    {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    fcount = wb_info_ptr->fcount;
    for (f_no = 0; f_no < fcount; f_no++)
    {
        struct kaps_jr1_key_wb_info *act_info = NULL;
        uint32_t total_size;

        if (wb_fun->nv_ptr)
            wb_info_ptr = (struct kaps_jr1_key_wb_info *) wb_fun->nv_ptr;

        
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }

        total_size = wb_info_ptr->f_name_len + sizeof(*wb_info_ptr);

        if (wb_fun->read_fn != NULL)
        {
            act_info = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, total_size);
            if (act_info == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) act_info, total_size, *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            act_info = (struct kaps_jr1_key_wb_info *) wb_fun->nv_ptr;
        }

        kaps_jr1_sassert(act_info);
        status = kaps_jr1_key_add_field(key, (char *) act_info->fname, act_info->width_1, act_info->ftype);

        if (wb_fun->read_fn != NULL)
        {
            key->device->alloc->xfree(key->device->alloc->cookie, act_info);
        }

        if (status != KAPS_JR1_OK)
            return status;
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += total_size;
    }

    

    if (wb_fun->read_fn)
    {
        if (0 !=
            wb_fun->read_fn(wb_fun->handle, (uint8_t *) & overlay_field_cnt, sizeof(overlay_field_cnt),
                            *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    else
    {
        overlay_field_cnt = *((uint32_t *) wb_fun->nv_ptr);
        wb_fun->nv_ptr += sizeof(overlay_field_cnt);
    }
    *wb_fun->nv_offset += sizeof(overlay_field_cnt);

    for (f_no = 0; f_no < overlay_field_cnt; f_no++)
    {
        struct kaps_jr1_key_overlay_wb_info *act_info = NULL;
        struct kaps_jr1_key_overlay_wb_info wb_info, *wb_info_ptr;
        uint32_t total_size;

        if (wb_fun->nv_ptr)
            wb_info_ptr = (struct kaps_jr1_key_overlay_wb_info *) wb_fun->nv_ptr;
        else
            wb_info_ptr = &wb_info;

        
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }

        total_size = wb_info_ptr->f_name_len + sizeof(*wb_info_ptr);

        if (wb_fun->read_fn != NULL)
        {
            act_info = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, total_size);
            if (act_info == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) act_info, total_size, *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            act_info = (struct kaps_jr1_key_overlay_wb_info *) wb_fun->nv_ptr;
        }

        kaps_jr1_sassert(act_info);
        status = kaps_jr1_key_overlay_field(key, (char *) act_info->fname, act_info->width_1,
                                        act_info->ftype, act_info->offset_1);

        if (wb_fun->read_fn != NULL)
        {
            key->device->alloc->xfree(key->device->alloc->cookie, act_info);
        }

        if (status != KAPS_JR1_OK)
            return status;
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += total_size;
    }

    return KAPS_JR1_OK;
}


void
kaps_jr1_key_wb_print(
    struct kaps_jr1_print_key *key,
    FILE * txt_fp)
{
    if (!key)
        return;

    kaps_jr1_fprintf(txt_fp, " (%s,%d,%s) ", key->fname, key->width, kaps_jr1_key_get_type_internal(key->type));
    kaps_jr1_key_wb_print(key->next, txt_fp);
}

void
kaps_jr1_key_wb_free(
    struct kaps_jr1_print_key *key)
{
    if (!key)
        return;
    kaps_jr1_key_wb_free(key->next);
    kaps_jr1_sysfree(key->fname);
    kaps_jr1_sysfree(key);
}
