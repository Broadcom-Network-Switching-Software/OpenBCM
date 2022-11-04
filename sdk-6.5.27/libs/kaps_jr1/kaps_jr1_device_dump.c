

#include <stdio.h>

#include "kaps_jr1_portable.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_resource.h"

KAPS_JR1_INLINE static void
kaps_jr1_in_tag(
    FILE * fp,
    uint32_t s,
    char *xml_tag)
{
    kaps_jr1_fprintf(fp, "    <%s>%d</%s>\n", xml_tag, s, xml_tag);
}


KAPS_JR1_INLINE static void
kaps_jr1_start_tag(
    FILE * fp,
    char *s)
{
    kaps_jr1_fprintf(fp, "<%s>\n", s);
}

KAPS_JR1_INLINE static void
kaps_jr1_end_tag(
    FILE * fp,
    char *s)
{
    kaps_jr1_fprintf(fp, "</%s>\n", s);
}

KAPS_JR1_INLINE static void
kaps_jr1_print_spaces(
    FILE * fp)
{
    kaps_jr1_fprintf(fp, "    ");
}

struct instruction_info
{
    struct kaps_jr1_db *db;
    uint16_t result_id;
    uint16_t ad_transmit_start_byte;
    uint16_t ad_size_to_transmit;
};



static char *
kaps_jr1_get_db_name(
    struct kaps_jr1_db *db)
{
    switch (db->type)
    {
        case KAPS_JR1_DB_LPM:
            return "lpm";
        case KAPS_JR1_DB_EM:
            return "em";
        default:
            kaps_jr1_sassert(0);
    }

    return NULL;
}

static void
print_key(
    FILE * fp,
    struct kaps_jr1_key *key,
    uint8_t in_xml_format)
{
    struct kaps_jr1_key_field *field = key->first_field;
    uint32_t fcount = 0, count = 0, next_offset = 0;
    char field_name[1000];

    if (in_xml_format)
    {
        kaps_jr1_print_spaces(fp);
        kaps_jr1_start_tag(fp, "key");
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE && !field->is_usr_bmr && !field->is_padding_field)
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

            strncpy(field_name, field->name, 999);
            if (1)
            {
                int32_t i;
                for (i = 0; i < strlen(field_name); i++)
                {
                    if (field_name[i] == ':')
                        field_name[i] = '_';
                }
            }

            if (in_xml_format)
            {
                kaps_jr1_print_spaces(fp);
                kaps_jr1_fprintf(fp, "    <%s type='%s'>%d</%s>\n", field_name, kaps_jr1_key_get_type_internal(field->type),
                             field->width_1, field_name);
            }
            else
            {
                if (count == fcount)
                    kaps_jr1_fprintf(fp, "(%s,%d,%s)", field_name, field->width_1, kaps_jr1_key_get_type_internal(field->type));
                else
                    kaps_jr1_fprintf(fp, "(%s,%d,%s) ", field_name, field->width_1,
                                 kaps_jr1_key_get_type_internal(field->type));
            }
            break;
        }
    }
    while (count < fcount);

    if (in_xml_format)
    {
        for (field = key->first_overlay_field; field; field = field->next)
        {
            kaps_jr1_print_spaces(fp);
            strncpy(field_name, field->name, 999);
            if (1)
            {
                int32_t i;
                for (i = 0; i < strlen(field_name); i++)
                {
                    if (field_name[i] == ':')
                        field_name[i] = '_';
                }
            }
            kaps_jr1_fprintf(fp, "    <%s type='%s' offset='%d'>%d</%s>\n", field_name,
                         kaps_jr1_key_get_type_internal(field->type), field->offset_1, field->width_1, field_name);
        }
        kaps_jr1_print_spaces(fp);
        kaps_jr1_end_tag(fp, "key");
    }
    else
    {
        kaps_jr1_fprintf(fp, "\n");
    }
}

static void
kaps_jr1_print_mask(
    FILE * fp,
    uint32_t key_width,
    uint32_t width,
    uint32_t len)
{
    uint8_t i = 0, mask[80] = { 0, };

    len = (width * KAPS_JR1_BITS_IN_BYTE) - len;
    for (i = width - ((len + 7) / 8); i < width; i++)
    {
        if (!len)
            mask[i] = 0x00;
        else if (!(len % 8))
            mask[i] = 0xff;
        else
        {
            mask[i] = 0xff >> (8 - (len % 8));
            len -= len % 8;
            continue;
        }
        if (len)
            len -= 8;
    }
    for (i = 0; i < width; i++)
    {
        if (i == width - 1)
            kaps_jr1_fprintf(fp, "%.2x", mask[i]);
        else
            kaps_jr1_fprintf(fp, "%.2x.", mask[i]);
    }
}

static void
kaps_jr1_print_field(
    FILE * fp,
    struct kaps_jr1_db *db,
    struct kaps_jr1_entry *e,
    struct kaps_jr1_key_field *field,
    uint8_t * data,
    uint8_t * mask,
    uint16_t length)
{
    uint32_t width_8 = 0, prev_offset = 0, i = 0;
    uint32_t key_width = db->width.key_width_1 / KAPS_JR1_BITS_IN_BYTE;
    char *bits[] = { "0", "1", "*" };
    int nbits = field->width_1;
    int start = 0;

    if (field->offset_1)
        prev_offset = (field->offset_1 / KAPS_JR1_BITS_IN_BYTE);

    switch (field->type)
    {
        case KAPS_JR1_KEY_FIELD_TABLE_ID:
            for (i = 0; i < field->width_1 / KAPS_JR1_BITS_IN_BYTE; i++)
                kaps_jr1_fprintf(fp, "%.2x", data[prev_offset + i]);
            break;
        case KAPS_JR1_KEY_FIELD_EM:
        case KAPS_JR1_KEY_FIELD_PREFIX:
            width_8 = field->width_1 / KAPS_JR1_BITS_IN_BYTE;
            length -= (prev_offset * KAPS_JR1_BITS_IN_BYTE);

            
            for (i = 0; i < width_8; i++)
            {
                if (i == width_8 - 1)
                    kaps_jr1_fprintf(fp, "%.2x", data[prev_offset + i]);
                else
                    kaps_jr1_fprintf(fp, "%.2x.", data[prev_offset + i]);
            }

            kaps_jr1_fprintf(fp, "/");

            
            if (mask)
            {
                for (i = 0; i < field->width_1 / KAPS_JR1_BITS_IN_BYTE; i++)
                {
                    if (i == width_8 - 1)
                        kaps_jr1_fprintf(fp, "%.2x", mask[prev_offset + i]);
                    else
                        kaps_jr1_fprintf(fp, "%.2x.", mask[prev_offset + i]);
                }
            }
            else
            {
                kaps_jr1_print_mask(fp, key_width, width_8, length);
            }
            break;
        case KAPS_JR1_KEY_FIELD_TERNARY:
            start = prev_offset * KAPS_JR1_BITS_IN_BYTE;
            for (i = start; i < (start + nbits); i++)
            {
                if (!(i % 4))
                    kaps_jr1_fprintf(fp, " ");
                kaps_jr1_fprintf(fp, "%s", bits[kaps_jr1_array_get_bit(data, mask, i)]);
            }
            break;
        default:
            return;
    }
}

static void
kaps_jr1_print_lpm_entry(
    FILE * f,
    struct kaps_jr1_db *db,
    struct kaps_jr1_entry *e)
{
    struct kaps_jr1_key_field *db_f = db->key->first_field;
    struct kaps_jr1_entry_info info;
    uint32_t fcount = 0, count = 0, next_offset = 0;
    kaps_jr1_status status;

    if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        status = kaps_jr1_entry_get_info(db, KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, e->user_handle), &info);
    else
        status = kaps_jr1_entry_get_info(db, e, &info);

    if (status != KAPS_JR1_OK)
        kaps_jr1_assert(0, "kaps_jr1_entry_get_info returned an error \n");

    for (; db_f; db_f = db_f->next)
    {
        if (db_f->type == KAPS_JR1_KEY_FIELD_HOLE)
            continue;
        fcount++;
    }

    do
    {
        for (db_f = db->key->first_field; db_f; db_f = db_f->next)
        {
            if (next_offset != db_f->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + db_f->width_1;
            kaps_jr1_print_field(f, db, e, db_f, info.data, NULL, info.prio_len);
            kaps_jr1_fprintf(f, " ");
            break;
        }
    }
    while (count < fcount);
    kaps_jr1_fprintf(f, "\n");
}

static void
kaps_jr1_print_entry(
    FILE * f,
    struct kaps_jr1_db *db,
    struct kaps_jr1_entry *e)
{
    if (db->type == KAPS_JR1_DB_LPM)
    {
        kaps_jr1_print_lpm_entry(f, db, e);
    }
}

static kaps_jr1_status
kaps_jr1_write_entries(
    struct kaps_jr1_db *db,
    FILE * f)
{
    struct kaps_jr1_entry *e = NULL;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    kaps_jr1_fprintf(f, "+KBP-STANDARD-FORMAT\n");
    print_key(f, db->key, 0);

    kaps_jr1_c_list_iter_init(&db->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(el);
        if (e == NULL)
            break;
        kaps_jr1_print_entry(f, db, e);
        kaps_jr1_fprintf(f, "\n");
    }

    if (kaps_jr1_c_list_count(&db->db_pending_list) == 0)
        return KAPS_JR1_OK;

    kaps_jr1_fprintf(f, "-----------------Pending------------\n");
    kaps_jr1_c_list_iter_init(&db->db_pending_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(el);

        if (e == NULL)
            break;

        kaps_jr1_print_entry(f, db, e);
        kaps_jr1_fprintf(f, "\n");
    }

    return KAPS_JR1_OK;
}

static void
kaps_jr1_print_ad_db_info(
    FILE * fp,
    struct kaps_jr1_ad_db *ad_db)
{
    kaps_jr1_print_spaces(fp);
    kaps_jr1_fprintf(fp, "<ad id='%d'>\n", ad_db->db_info.tid);
    kaps_jr1_print_spaces(fp);
    kaps_jr1_in_tag(fp, ad_db->db_info.common_info->capacity, "capacity");
    kaps_jr1_print_spaces(fp);
    kaps_jr1_in_tag(fp, ad_db->user_width_1, "width");
    kaps_jr1_print_spaces(fp);
    kaps_jr1_end_tag(fp, "ad");
    fflush(fp);
}

static void
kaps_jr1_print_xml_clone(
    FILE * fp,
    struct kaps_jr1_db *db)
{
    kaps_jr1_fprintf(fp, "<clone id='%d'>\n", db->tid);

    if (db->key)
        print_key(fp, db->key, 1);
    kaps_jr1_print_spaces(fp);
    kaps_jr1_end_tag(fp, "clone");
    fflush(fp);
}

static void
kaps_jr1_print_xml_table(
    FILE * fp,
    struct kaps_jr1_db *db,
    uint32_t dump_entries_to_file)
{
    char dbfilename[1024];
    struct kaps_jr1_db *tmp = NULL, *parent = db->parent;

    kaps_jr1_print_spaces(fp);
    kaps_jr1_fprintf(fp, "<table id='%d'>\n", db->tid);
    kaps_jr1_sprintf(dbfilename, "%s-%d-table-%d.txt", kaps_jr1_get_db_name(db), db->parent->tid, db->tid);
    kaps_jr1_fprintf(fp, "\t<input>%s</input>\n", dbfilename);

    if (dump_entries_to_file)
    {
        FILE *f = kaps_jr1_fopen(dbfilename, "w+");

        if (f == NULL)
        {
            kaps_jr1_printf("UNABLE TO OPEN THE FILE %s\n", dbfilename);
            return;
        }

        kaps_jr1_write_entries(db, f);
        kaps_jr1_fclose(f);
    }

    if (db->key)
        print_key(fp, db->key, 1);
    kaps_jr1_print_spaces(fp);
    kaps_jr1_print_spaces(fp);

    for (tmp = parent->next_tab; tmp; tmp = tmp->next_tab)
    {
        if (tmp->is_clone && tmp->clone_of == db)
            kaps_jr1_print_xml_clone(fp, tmp);
    }
    kaps_jr1_end_tag(fp, "table");
}

static kaps_jr1_status
kaps_jr1_print_all_db_properties(
    struct kaps_jr1_db *db,
    FILE * fp)
{
    kaps_jr1_in_tag(fp, db->common_info->capacity, "capacity");
    if (db->common_info->capacity < kaps_jr1_c_list_count(&db->db_list))
        kaps_jr1_in_tag(fp, kaps_jr1_c_list_count(&db->db_list), "parse_only");

    kaps_jr1_in_tag(fp, kaps_jr1_db_get_algorithmic(db), "kaps_jr1_prop_algorithmic");

    if (db->common_info->user_specified)
    {
        if (db->type == KAPS_JR1_DB_LPM)
        {
            kaps_jr1_in_tag(fp, db->common_info->num_ab, "dba");
            kaps_jr1_in_tag(fp, db->common_info->uda_mb, "uda");
        }
        else if (db->type == KAPS_JR1_DB_AD)
        {
            kaps_jr1_in_tag(fp, db->common_info->uda_mb, "uda");
        }
    }


    if (db->common_info->defer_deletes_to_install)
        kaps_jr1_in_tag(fp, db->common_info->defer_deletes_to_install, "kaps_jr1_prop_defer_deletes");

    if (db->common_info->meta_priority)
        kaps_jr1_in_tag(fp, db->common_info->meta_priority, "kaps_jr1_db_priority");

    if (db->type == KAPS_JR1_DB_HB)
    {
        struct kaps_jr1_hb_db *hb_db = (struct kaps_jr1_hb_db *) db;
        if (hb_db->age_count)
            kaps_jr1_in_tag(fp, hb_db->age_count, "age_count");
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_print_db_xml_format(
    struct kaps_jr1_db *db,
    uint32_t dump_entries_to_file,
    FILE * fp)
{
    struct kaps_jr1_db *next_tab = NULL;
    struct kaps_jr1_ad_db *ad_db = NULL;
    char dbfilename[512];

    kaps_jr1_fprintf(fp, "<%s id='%d'>\n", kaps_jr1_get_db_name(db), db->tid);

    kaps_jr1_print_all_db_properties(db, fp);

    kaps_jr1_sprintf(dbfilename, "%s-%d.txt", kaps_jr1_get_db_name(db), db->tid);

    kaps_jr1_fprintf(fp, "    <input>%s</input>\n", dbfilename);

    if (dump_entries_to_file)
    {
        FILE *f = kaps_jr1_fopen(dbfilename, "w+");

        if (f == NULL)
        {
            kaps_jr1_printf("UNABLE TO OPEN THE FILE %s\n", dbfilename);
            return KAPS_JR1_OK;
        }

        kaps_jr1_write_entries(db, f);
        kaps_jr1_fclose(f);
    }

    if (db->key)
        print_key(fp, db->key, 1);

    ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
    while (ad_db)
    {
        kaps_jr1_print_ad_db_info(fp, ad_db);
        ad_db = ad_db->next;
    }

    for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
    {
        if (next_tab->is_clone)
            continue;
        kaps_jr1_print_xml_table(fp, next_tab, dump_entries_to_file);
    }

    for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
    {
        if (next_tab->is_clone && next_tab->clone_of == db)
            kaps_jr1_print_xml_clone(fp, next_tab);
    }

    kaps_jr1_fprintf(fp, "    <!-- thread %d -->\n", 2 * db->device->smt_no + db->device->core_id);

    kaps_jr1_end_tag(fp, kaps_jr1_get_db_name(db));
    return KAPS_JR1_OK;
}

static void
kaps_jr1_print_db_name_for_search(
    struct instruction_info inst_info,
    FILE * f)
{
    struct kaps_jr1_db *db = inst_info.db;
    uint32_t result_port = inst_info.result_id;

    if (!db)
    {
        kaps_jr1_fprintf(f, "    <empty/>\n");
        return;
    }
    if (db->parent)
    {
        if (db->is_clone)
        {
            if (!db->clone_of->parent)
                kaps_jr1_fprintf(f, "    <%s id='%d' clone='%d' result='%d'", kaps_jr1_get_db_name(db->parent), db->clone_of->tid,
                             db->tid, result_port);
            else
                kaps_jr1_fprintf(f, "    <%s id='%d' table='%d' clone='%d' result='%d'",
                             kaps_jr1_get_db_name(db->clone_of->parent), db->clone_of->parent->tid,
                             db->clone_of->tid, db->tid, result_port);
        }
        else
        {
            kaps_jr1_fprintf(f, "    <%s id='%d' table='%d' result='%d'", kaps_jr1_get_db_name(db->parent), db->parent->tid,
                         db->tid, result_port);
        }
    }
    else
    {
        kaps_jr1_fprintf(f, "    <%s id='%d' result='%d'", kaps_jr1_get_db_name(db), db->tid, result_port);
    }
    if (inst_info.ad_size_to_transmit)
    {
        kaps_jr1_fprintf(f, " ad_start_byte='%d' ad_num_bytes='%d'/>\n", inst_info.ad_transmit_start_byte,
                     inst_info.ad_size_to_transmit);
    }
    else
    {
        kaps_jr1_fprintf(f, "/>\n");
    }
}

static kaps_jr1_status
kaps_jr1_print_instruction_xml_format(
    struct kaps_jr1_instruction *instruction,
    FILE * fp)
{
    uint32_t i, j, num_searches, cmp3_ltr = 0, count = 0;
    struct instruction_info *desc = NULL;
    struct kaps_jr1_instruction *cmp3_pair = NULL;

    num_searches = instruction->num_searches;

    if (num_searches == 0)
    {
        return KAPS_JR1_OK;
    }

    if (instruction->type == INSTR_COMPARE3)
    {
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *el;

        cmp3_ltr = instruction->ltr + 1;

        if (instruction->device->flags & KAPS_JR1_DEVICE_SMT)
        {
            if (instruction->device->smt_no == 0)
            {
                if (cmp3_ltr >= KAPS_JR1_HW_MAX_LTR_DUAL_PORT)
                    cmp3_ltr -= KAPS_JR1_HW_MAX_LTR_DUAL_PORT;
            }
            else
            {
                if (cmp3_ltr >= KAPS_JR1_HW_MAX_LTRS)
                    cmp3_ltr -= KAPS_JR1_HW_MAX_LTR_DUAL_PORT;
            }
        }
        else
        {
            if (cmp3_ltr >= KAPS_JR1_HW_MAX_LTRS)
                cmp3_ltr -= KAPS_JR1_HW_MAX_LTRS;
        }

        if (instruction->device->smt_no == 1)
        {
            kaps_jr1_c_list_iter_init(&instruction->device->smt->inst_list, &it);
        }
        else
        {
            kaps_jr1_c_list_iter_init(&instruction->device->inst_list, &it);
        }

        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *in = KAPS_JR1_INSTLIST_TO_ENTRY(el);

            if (in->device != instruction->device)
                continue;
            if (in->ltr == cmp3_ltr)
            {
                cmp3_pair = in;
                num_searches += in->num_searches;
            }
        }
    }
    

    kaps_jr1_fprintf(fp, "<instruction id='%d'>\n", instruction->id);

    if (instruction->master_key)
        print_key(fp, instruction->master_key, 1);

    kaps_jr1_in_tag(fp, instruction->ltr, "ltr");

    num_searches = 0;
    count = 0;
    if (cmp3_pair)
    {
        for (i = 0; i < instruction->num_searches; i++)
        {
            for (j = 0; j < cmp3_pair->num_searches; j++)
            {
                if (cmp3_pair->desc[j].db == instruction->desc[i].db)
                {
                    break;
                }
            }
            if (j != cmp3_pair->num_searches)
                count++;
        }
        num_searches = instruction->num_searches + cmp3_pair->num_searches - count;
    }
    else
    {
        num_searches = instruction->num_searches;
    }

    desc =
        instruction->device->alloc->xcalloc(instruction->device->alloc->cookie, num_searches,
                                            sizeof(struct instruction_info));
    if (!desc)
        return KAPS_JR1_OUT_OF_MEMORY;

    if (cmp3_pair)
    {
        for (i = 0; i < cmp3_pair->num_searches; i++)
        {
            desc[i].result_id = cmp3_pair->desc[i].result_id + 2;
            desc[i].db = cmp3_pair->desc[i].db;
            if (instruction->is_prop_set_xml_dump[desc[i].result_id])
            {
                desc[i].ad_transmit_start_byte = instruction->prop_byte_offset[desc[i].result_id];
                desc[i].ad_size_to_transmit = instruction->prop_num_bytes[desc[i].result_id];
            }
        }

        count = cmp3_pair->num_searches;
        for (i = 0; i < instruction->num_searches; i++)
        {
            for (j = 0; j < num_searches; j++)
            {
                if (instruction->desc[i].db == desc[j].db)
                    break;
            }
            if (j == num_searches)
            {
                desc[count].result_id = instruction->desc[i].result_id;
                desc[count].db = instruction->desc[i].db;
                if (instruction->is_prop_set_xml_dump[desc[count].result_id])
                {
                    desc[count].ad_transmit_start_byte = instruction->prop_byte_offset[desc[count].result_id];
                    desc[count].ad_size_to_transmit = instruction->prop_num_bytes[desc[count].result_id];
                }
                count++;
            }
        }
    }
    else
    {

        count = instruction->num_searches;
        if (instruction->device->type == KAPS_JR1_DEVICE)
        {
            uint32_t srch_no = 0;

            for (i = 0; i < instruction->device->max_num_searches; i++)
            {

                if (instruction->desc[i].db)
                {
                    desc[srch_no].result_id = instruction->desc[i].result_id;
                    desc[srch_no].db = instruction->desc[i].db;
                    if (instruction->is_prop_set_xml_dump[desc[srch_no].result_id])
                    {
                        desc[srch_no].ad_transmit_start_byte = instruction->prop_byte_offset[desc[srch_no].result_id];
                        desc[srch_no].ad_size_to_transmit = instruction->prop_num_bytes[desc[srch_no].result_id];
                    }
                    srch_no++;
                }
            }
            kaps_jr1_sassert(srch_no == count);

        }
        else
        {
            for (i = 0; i < instruction->num_searches; i++)
            {

                desc[i].result_id = instruction->desc[i].result_id;
                desc[i].db = instruction->desc[i].db;
                if (instruction->is_prop_set_xml_dump[desc[i].result_id])
                {
                    desc[i].ad_transmit_start_byte = instruction->prop_byte_offset[desc[i].result_id];
                    desc[i].ad_size_to_transmit = instruction->prop_num_bytes[desc[i].result_id];
                }
            }
        }
    }

    for (i = 0; i < num_searches; i++)
    {
        struct instruction_info search_info = desc[i];

        kaps_jr1_print_db_name_for_search(search_info, fp);
    }

    if (instruction->device->flags & KAPS_JR1_DEVICE_SMT)
        kaps_jr1_in_tag(fp, instruction->device->smt_no, "thread");

    kaps_jr1_end_tag(fp, "instruction");
    instruction->device->alloc->xfree(instruction->device->alloc->cookie, desc);
    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_dump_device_details(
    struct kaps_jr1_device *device,
    FILE * fp)
{
    kaps_jr1_fprintf(fp, "\n<!--  Device Info: ");


    kaps_jr1_fprintf(fp, "KAPS_JR1 ID = %d, subtype = %d ",
                device->id, device->silicon_sub_type);

    kaps_jr1_fprintf(fp, " -->\n");

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_print_all_device_properties(
    struct kaps_jr1_device *device,
    FILE * fp)
{
    if (device->nv_ptr)
        kaps_jr1_fprintf(fp, "    <!-- KAPS_JR1_DEVICE_PROP_CRASH_RECOVERY is set -->\n");

    if (device->debug_level)
        kaps_jr1_fprintf(fp, "    <!-- KAPS_JR1_DEVICE_PROP_DEBUG_LEVEL is set as: %d -->\n", device->debug_level);

    if (device->dump_on_assert)
        kaps_jr1_fprintf(fp, "    <!-- KAPS_JR1_DEVICE_PROP_DUMP_ON_ASSERT is set as: %d -->\n", device->dump_on_assert);

    if (device->prop.return_error_on_asserts)
        kaps_jr1_fprintf(fp, "    <!-- KAPS_JR1_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS is set as: %d -->\n",
                     device->prop.return_error_on_asserts);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_dump(
    struct kaps_jr1_device * device,
    uint32_t dump_xml_data_with_entries,
    FILE * fp)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint32_t db_id;

    KAPS_JR1_TRACE_IN("%p %u %p\n", device, dump_xml_data_with_entries, fp);

    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->main_dev)
        device = device->main_dev;

    if (device->main_bc_device)
        device = device->main_bc_device;

    
    if (device->core_id % 2)
        return KAPS_JR1_OK;

    kaps_jr1_fprintf(fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    kaps_jr1_dump_device_details(device, fp);
    kaps_jr1_start_tag(fp, "scenario");

    kaps_jr1_print_all_device_properties(device, fp);

    
    kaps_jr1_fprintf(fp, "\n<!-- Database information -->\n");

    {
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_LPM || db->type == KAPS_JR1_DB_EM)
            {
                kaps_jr1_print_db_xml_format(db, device->dump_xml_data_with_entries, fp);
                
                fflush(fp);
            }
        }
    }

    
    kaps_jr1_fprintf(fp, "\n<!-- Instruction information -->\n");
    kaps_jr1_c_list_iter_init(&device->inst_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);

        if (instruction->is_cmp3_pair)
            continue;

        kaps_jr1_print_instruction_xml_format(instruction, fp);

        fflush(fp);
    }

    if (device->other_core)
    {
        kaps_jr1_c_list_iter_init(&device->other_core->inst_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);

            if (instruction->is_cmp3_pair)
                continue;

            kaps_jr1_print_instruction_xml_format(instruction, fp);

            fflush(fp);
        }
    }

    kaps_jr1_end_tag(fp, "scenario");

    
    if (dump_xml_data_with_entries != 0)
    {
        struct kaps_jr1_c_list_iter it_db;
        struct kaps_jr1_list_node *el_db;
        uint32_t count = 0;
        struct kaps_jr1_device *dev = device;

        kaps_jr1_fprintf(fp, "\n\n<!-- Database entries information -->\n");

        kaps_jr1_fprintf(fp, "\n<!--\n");

        
        do
        {
            kaps_jr1_c_list_iter_init(&dev->db_list, &it_db);
            while ((el_db = kaps_jr1_c_list_iter_next(&it_db)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el_db);
                struct kaps_jr1_db *next_tab;

                if (db->type != KAPS_JR1_DB_LPM && db->type != KAPS_JR1_DB_EM)
                {
                    
                    continue;
                }
                if (db->is_bc_required && !db->is_main_bc_db)
                    continue;   

                db_id = db->tid;
                if (db->has_tables == 0)
                {
                    
                    ;
                }

                
                for (next_tab = db; next_tab; next_tab = next_tab->next_tab)
                {
                    struct kaps_jr1_c_list_iter it_entry;
                    struct kaps_jr1_list_node *el_entry;
                    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) next_tab->common_info->ad_info.ad;

                    if (next_tab->is_clone == 1)
                        continue;

                    
                    if (next_tab->key)
                    {
                        kaps_jr1_fprintf(fp, "\n");
                        switch (next_tab->type)
                        {
                            case KAPS_JR1_DB_EM:
                                kaps_jr1_fprintf(fp, "file_begin: %s-%d.txt <em-entries id='%d' tid='%d'> \n",
                                             kaps_jr1_get_db_name(db), db_id, db_id, next_tab->tid);
                                kaps_jr1_fprintf(fp, "+KBP-STANDARD-FORMAT-1\n");
                                kaps_jr1_fprintf(fp, "#_EM_WITH_MASK\n");
                                break;
                            case KAPS_JR1_DB_LPM:
                                kaps_jr1_fprintf(fp, "file_begin: %s-%d.txt <lpm-entries id='%d' tid='%d'> \n",
                                             kaps_jr1_get_db_name(db), db_id, db_id, next_tab->tid);
                                kaps_jr1_fprintf(fp, "+KBP-STANDARD-FORMAT-1\n");
                                kaps_jr1_fprintf(fp, "#_EM_WITH_MASK\n");
                                break;
                            default:
                                kaps_jr1_printf("Not LPM / EM  ?!\n");
                                return KAPS_JR1_OK;
                        }

                        kaps_jr1_key_print(next_tab->key, fp);

                        if (0 && ad_db != NULL) 
                            
                            kaps_jr1_fprintf(fp, "(AD,%d,ad)\n", ad_db->user_width_1);
                        else
                            kaps_jr1_fprintf(fp, "\n");
                    }

                    kaps_jr1_c_list_iter_init(&next_tab->db_list, &it_entry);
                    while ((el_entry = kaps_jr1_c_list_iter_next(&it_entry)) != NULL)
                    {
                        struct kaps_jr1_entry *entry = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(el_entry);
                        struct kaps_jr1_entry *entry_index;
                        struct kaps_jr1_ad *ad_index;

                        entry_index = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, entry->user_handle);

                        
                        KAPS_JR1_TRY(kaps_jr1_entry_print(next_tab, entry_index, fp));

                        
                        if (0 && entry->ad_handle != NULL)
                        {       
                            ad_index = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_ad, entry->ad_handle->user_handle);
                            kaps_jr1_ad_entry_print(ad_db, ad_index, fp);
                        }
                        kaps_jr1_fprintf(fp, "\n");
                    }

                    kaps_jr1_fprintf(fp, "file_end:\n");

                    if (next_tab->key)
                    {
                        switch (next_tab->type)
                        {
                            case KAPS_JR1_DB_EM:
                                kaps_jr1_end_tag(fp, "em-entries");
                                break;
                            case KAPS_JR1_DB_LPM:
                                kaps_jr1_end_tag(fp, "lpm-entries");
                                break;
                            default:
                                return KAPS_JR1_OK;
                        }
                    }
                }       
            }   

            count++;
            if (count > 1)
            {
                break;
            }
            dev = dev->other_core;
        }
        while (dev);

        kaps_jr1_fprintf(fp, "\n-->\n");

    }   

    return KAPS_JR1_OK;
}
