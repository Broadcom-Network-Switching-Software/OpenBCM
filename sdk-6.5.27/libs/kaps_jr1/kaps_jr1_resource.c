

#include "kaps_jr1_ab.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_resource.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_algo_hw.h"

static const char *table_colors[] = { "green", "aquamarine", "blue", "blueviolet",
    "brown", "burlywood", "cadetblue",
    "chartreuse", "chocolate", "coral", "cornflowerblue",
    "cornsilk", "crimson", "cyan",
    "fuchsia",
    "gold", "goldenrod", "aqua",
    "greenyellow", "honeydew", "hotpink", "indigo",
    "ivory", "khaki", "lavender", "lavenderblush", "lawngreen",
    "lemonchiffon", "lime", "limegreen", "linen", "magenta",
    "maroon", "midnightblue", "mintcream", "mistyrose", "moccasin",
    "navy", "oldlace", "olive", "olivedrab", "orange",
    "orchid", "papayawhip", "peachpuff", "peru",
    "pink", "plum", "powderblue", "purple", "red", "rosybrown",
    "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
    "seashell", "sienna", "silver", "skyblue", "snow", "springgreen",
    "steelblue", "tan", "teal", "thistle", "tomato", "turquoise",
    "violet", "wheat", "yellow", "yellowgreen"
};

#define MAX_PRINT_BUF_SIZE 2000
int
kaps_jr1_print_to_file(
    struct kaps_jr1_device *device,
    FILE * fp,
    const char *fmt,
    ...)
{
    int32_t r;
    va_list ap;
    char buffer[MAX_PRINT_BUF_SIZE];
    int32_t i;
    int32_t pad[] = { 105, 76, 79, 31, 157, 38, 79, 58, 187, 67, 85, 78, 12, 31, 18, 9, 89 };
    int new_val;

    if (!fp)
        return 0;

    if (device->main_dev)
    {
        device = device->main_dev;
    }

    if (device->main_bc_device)
    {
        device = device->main_bc_device;
    }

    if (device->map_print)
    {
        va_start(ap, fmt);
        r = kaps_jr1_vsnprintf(buffer, MAX_PRINT_BUF_SIZE, fmt, ap);
        if (r < 1)
            kaps_jr1_sassert(0);
        va_end(ap);
        for (i = 0; i < r; i++)
        {
            new_val = buffer[i];
            new_val = (new_val + pad[device->map_print_offset] + 105) % 255;
            kaps_jr1_fprintf(fp, "%03d", new_val);
            device->map_print_offset = (device->map_print_offset + 1) % (sizeof(pad) / sizeof(pad[0]));
        }
    }
    else
    {
        va_start(ap, fmt);
        r = kaps_jr1_vfprintf(fp, fmt, ap);
        va_end(ap);
    }

    return r;
}

struct memory_map *
resource_kaps_jr1_get_memory_map(
    struct kaps_jr1_device *device)
{
    int32_t dev_num = device->device_no;

    if (device->main_dev)
        device = device->main_dev;

    return &device->map[dev_num];
}

enum kaps_jr1_ad_type
kaps_jr1_resource_get_ad_type(
    struct kaps_jr1_db *db)
{
    return db->hw_res.db_res->ad_type;
}

void
kaps_jr1_resource_set_ad_type(
    struct kaps_jr1_db *db,
    enum kaps_jr1_ad_type ad_type)
{
    db->hw_res.db_res->ad_type = ad_type;
}

struct kaps_jr1_c_list *
kaps_jr1_resource_get_ab_list(
    struct kaps_jr1_db *db)
{
    if (db->common_info->is_cascaded && db->device->core_id == 1)
        return &db->hw_res.db_res->cas_ab_list;

    return &db->hw_res.db_res->ab_list;
}

kaps_jr1_status
kaps_jr1_resource_init_module(
    struct kaps_jr1_device * device)
{
    device->description = NULL;
    device->next_algo_color = 0;
    kaps_jr1_c_list_reset(&device->db_list);
    kaps_jr1_c_list_reset(&device->inst_list);

    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_resource_normalized_ad_width(
    struct kaps_jr1_device * device,
    uint32_t ad_width_1)
{
    return ad_width_1;
}


kaps_jr1_status
kaps_jr1_resource_add_database(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    enum kaps_jr1_db_type type)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_device *main_dev, *bc_dev;
    struct kaps_jr1_db *p_db = NULL;        

    if (device->main_dev)
        main_dev = device->main_dev;
    else
        main_dev = device;

    bc_dev = main_dev;
    if (main_dev->main_bc_device)
        bc_dev = main_dev->main_bc_device;

    
    kaps_jr1_c_list_iter_init(&main_dev->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        uint32_t type_match = 0;

        if (tmp->type >= KAPS_JR1_DB_AD && type == tmp->type)
            type_match = 1;
        else if (type < KAPS_JR1_DB_AD && tmp->type < KAPS_JR1_DB_AD)
            type_match = 1;
        if (type_match && tmp->tid == id)
        {
            if (db->is_internal)
            {
                kaps_jr1_sassert(type == KAPS_JR1_DB_AD);
                kaps_jr1_sassert(p_db == NULL);
                p_db = tmp;
            }
            else
            {
                return KAPS_JR1_DUPLICATE_DB_ID;
            }
        }
    }

    kaps_jr1_c_list_add_tail(&main_dev->db_list, &db->node);

    db->common_info = main_dev->alloc->xcalloc(main_dev->alloc->cookie, 1, sizeof(struct kaps_jr1_db_common_info));
    if (db->common_info == NULL)
        return KAPS_JR1_OUT_OF_MEMORY;

    db->type = type;
    db->tid = id;
    db->common_info->capacity = capacity;
    db->device = device;
    if (type == KAPS_JR1_DB_AD)
    {
        uint32_t norm_width_1 = kaps_jr1_resource_normalized_ad_width(device, width_1);

        db->hw_res.ad_res =
            db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(struct kaps_jr1_ad_db_hw_resource));
        if (db->hw_res.ad_res == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->common_info);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        db->width.ad_width_1 = norm_width_1;

    }
    else if (type == KAPS_JR1_DB_HB)
    {
        
    }
    else
    {
        
        db->hw_res.db_res =
            db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(struct kaps_jr1_db_hw_resource));
        if (db->hw_res.db_res == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->common_info);
            return KAPS_JR1_OUT_OF_MEMORY;
        }

        kaps_jr1_resource_set_algorithmic(device, db, 1);

        db->hw_res.db_res->start_mcor = db->hw_res.db_res->end_mcor = -1;
    }

    db->common_info->hw_res_alloc = 1;

    

    if (type == KAPS_JR1_DB_LPM || type == KAPS_JR1_DB_EM || type == KAPS_JR1_DB_DMA)
    {
        if (bc_dev->next_algo_color >= (sizeof(table_colors) / sizeof(char *)))
        {
            
            bc_dev->next_algo_color = 0;
        }

        db->hw_res.db_res->color = table_colors[bc_dev->next_algo_color];
        bc_dev->next_algo_color++;
    }

    return KAPS_JR1_OK;
}

void
kaps_jr1_resource_free_database(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    if (db->common_info->hw_res_alloc)
    {
        if (db->type == KAPS_JR1_DB_AD)
        {
            if (db->hw_res.ad_res->hb_res)
            {
                struct kaps_jr1_hb_bank_list *bank_list = db->hw_res.ad_res->hb_res->hb_bank_list;
                struct kaps_jr1_hb_bank_list *list_next;

                while (bank_list)
                {
                    list_next = bank_list->next;
                    device->alloc->xfree(device->alloc->cookie, bank_list);
                    bank_list = list_next;
                }
                device->alloc->xfree(device->alloc->cookie, db->hw_res.ad_res->hb_res);
            }
            if (!device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, db->hw_res.ad_res);
        }
        else
        {
            device->alloc->xfree(device->alloc->cookie, db->hw_res.db_res);
        }
    }

    
    device->alloc->xfree(device->alloc->cookie, db->common_info);
    if (db->description)
        device->alloc->xfree(device->alloc->cookie, db->description);
}

kaps_jr1_status
kaps_jr1_resource_db_set_key(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    struct kaps_jr1_key *key)
{
    db->key = key;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_resource_db_add_ad(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    struct kaps_jr1_db * ad)
{
   
    
    if (db->common_info->ad_info.ad)
        return KAPS_JR1_TOO_MANY_AD;

    
    if (ad->common_info->ad_info.db)
        return KAPS_JR1_ONE_ADDB_PER_DB;

    if (ad->common_info->capacity == 0)
        ad->common_info->capacity = db->common_info->capacity;


    ad->hw_res.ad_res->ad_type = KAPS_JR1_AD_TYPE_INPLACE;
    db->hw_res.db_res->ad_type = KAPS_JR1_AD_TYPE_INPLACE;

    
    if (!db->common_info->ad_info.ad)
    {
        db->common_info->ad_info.ad = ad;
        ad->common_info->ad_info.db = db;
    }
    else 
    {
        struct kaps_jr1_ad_db *tmp, *ad_db;

        
        tmp = (struct kaps_jr1_ad_db *) (db->common_info->ad_info.ad);
        ad_db = (struct kaps_jr1_ad_db *) ad;

        KAPS_JR1_WB_HANDLE_TABLE_DESTROY(&(ad_db->db_info));
        ad->common_info->user_hdl_table_size = tmp->db_info.common_info->user_hdl_table_size;
        ad->common_info->user_hdl_to_entry = tmp->db_info.common_info->user_hdl_to_entry;

        
        while (tmp->next)
            tmp = tmp->next;

        
        tmp->next = ad_db;
        ad->common_info->ad_info.db = db;
    }

    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_resource_db_add_hb(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    struct kaps_jr1_db * hb_db)
{
    uint32_t is_supported;

    is_supported = 0;
    if (device->type == KAPS_JR1_DEVICE && device->num_hb_blocks > 0)
    {
        is_supported = 1;
    }

    if (!is_supported)
        return KAPS_JR1_UNSUPPORTED;

    if ((db->type != KAPS_JR1_DB_LPM))
        return KAPS_JR1_UNSUPPORTED;

    if (db->common_info->hb_info.hb)
        return KAPS_JR1_TOO_MANY_HB;

    if (hb_db->common_info->hb_info.db)
        return KAPS_JR1_ONE_HB_DB_PER_DB;

    if (!db->common_info->ad_info.ad)
        return KAPS_JR1_HB_NO_AD;

    db->common_info->hb_info.hb = hb_db;
    hb_db->common_info->hb_info.db = db;

    if (device->type == KAPS_JR1_DEVICE)
        return KAPS_JR1_OK;

    return KAPS_JR1_OK;
}







void
kaps_jr1_resource_print_db_capacity(
    FILE * fp,
    struct kaps_jr1_db *db)
{
    kaps_jr1_print_to_file(db->device, fp, " d%d c%u", db->tid, db->common_info->cur_capacity);
}

void
kaps_jr1_resource_print_ab_release_info(
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info *ab)
{
    FILE *fp = db->device->dynamic_alloc_fp;

    if (!fp)
        return;

    kaps_jr1_print_to_file(db->device, fp, "D_1:");
    kaps_jr1_resource_print_db_capacity(fp, db);
    kaps_jr1_print_to_file(db->device, fp, " a%d\n", ab->ab_num);
}






uint32_t
kaps_jr1_resource_get_ab_usage(
    struct kaps_jr1_device * this_device,
    struct kaps_jr1_db * db)
{
    struct memory_map *mem_map = resource_kaps_jr1_get_memory_map(this_device);
    uint32_t i, num_ab;

    if (!this_device->is_config_locked)
    {
        return db->common_info->num_ab;
    }

    num_ab = 0;
    for (i = 0; i < this_device->num_ab; ++i)
    {
        if (mem_map->ab_memory[i].db == db)
        {
            num_ab += 2;
        }
    }

    return (num_ab + 1) / 2;
}



kaps_jr1_status
kaps_jr1_standalone_resource_assign_rpbs(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    int32_t i, j, count;
    struct kaps_jr1_db *cur_db;
    struct kaps_jr1_ab_info *prev_ab_info;

    if (device->issu_in_progress)
    {
        for (i = 0; i < device->num_ab; i++)
        {
            if (device->map->ab_memory[i].db != NULL)
            {
                cur_db = device->map->ab_memory[i].db;
               
                if (!device->map->ab_memory[i].is_dup_ab)
                    kaps_jr1_c_list_add_tail(kaps_jr1_resource_get_ab_list(cur_db), &device->map->ab_memory[i].ab_node);

                cur_db->common_info->num_ab = 1;
            }
        }
        return KAPS_JR1_OK;
    }

    
    for (i = 0; i < device->num_ab; i++)
    {
        struct kaps_jr1_ab_info *ab = &device->map->ab_memory[i];
        ab->conf = KAPS_JR1_NORMAL_160;
        ab->num_slots = ab->device->hw_res->num_rows_in_rpb[i];
    }

    
    for (i = 0; i < device->max_num_searches; i++)
    {

        
        cur_db = NULL;
        kaps_jr1_c_list_iter_init(&device->inst_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {

            struct kaps_jr1_instruction *instr = KAPS_JR1_INSTLIST_TO_ENTRY(el);

            if (instr->desc[i].db == NULL)
                continue;

            if (cur_db == NULL)
            {
                cur_db = kaps_jr1_db_get_main_db(instr->desc[i].db);
                continue;
            }

            if (cur_db != kaps_jr1_db_get_main_db(instr->desc[i].db))
                return KAPS_JR1_CONFLICTING_SEARCH_PORTS;
        }

        if (cur_db)
        {
            
            
            if (kaps_jr1_c_list_count(&cur_db->hw_res.db_res->ab_list) == 0)
                kaps_jr1_c_list_add_tail(kaps_jr1_resource_get_ab_list(cur_db), &device->map->ab_memory[i].ab_node);
        

            device->map->ab_memory[i].db = cur_db;
            cur_db->common_info->num_ab = 1;

            cur_db->rpb_id = i;
            
        }
    }

    for (i = 0; i < device->max_num_searches; i++)
    {
        cur_db = device->map->ab_memory[i].db;
        if (!cur_db)
            continue;

        count = 1;

        for (j = 0; j < device->max_num_searches; j++)
        {
            if (!device->map->ab_memory[j].db)
                continue;

            if (i == j)
                continue;

            if (device->map->ab_memory[j].db == cur_db)
                count++;

            if (count > device->max_num_clone_parallel_lookups)
                return KAPS_JR1_EXCEEDED_MAX_PARALLEL_SEARCHES;
        }
    }

    
    for (i = 0; i < device->max_num_searches; i++)
    {

        cur_db = device->map->ab_memory[i].db;
        if (!cur_db)
            continue;

        prev_ab_info = &device->map->ab_memory[i];

        for (j = i + 1; j < device->max_num_searches; j++)
        {
            if (!device->map->ab_memory[j].db)
                continue;

            if (cur_db == device->map->ab_memory[j].db)
            {
                if (!prev_ab_info->dup_ab)
                    prev_ab_info->dup_ab = &device->map->ab_memory[j];

                device->map->ab_memory[j].is_dup_ab = 1;
                cur_db->has_dup_ad = 1;
                device->map->ab_memory[j].db->has_dup_ad = 1;

                prev_ab_info = &device->map->ab_memory[j];
            }
        }
    }

    return KAPS_JR1_OK;
}
