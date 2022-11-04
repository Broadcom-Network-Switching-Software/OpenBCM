

#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_device_wb.h"
#include "kaps_jr1_instruction_internal.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_errors.h"





static kaps_jr1_status
add_to_db_list_internal(
    struct kaps_jr1_instruction *inst,
    struct kaps_jr1_db *db)
{
    int32_t i;
    struct kaps_jr1_instruction **list;

    for (i = 0; i < db->ninstructions; i++)
    {
        if (db->instructions[i] == inst)
            return KAPS_JR1_OK;
    }

    list = inst->device->alloc->xcalloc(inst->device->alloc->cookie, 1,
                                        ((db->ninstructions + 1) * sizeof(struct kaps_jr1_instruction *)));
    if (list == NULL)
        return KAPS_JR1_OUT_OF_MEMORY;
    for (i = 0; i < db->ninstructions; i++)
    {
        list[i] = db->instructions[i];
    }
    list[db->ninstructions] = inst;
    if (db->instructions)
        inst->device->alloc->xfree(inst->device->alloc->cookie, db->instructions);
    db->instructions = list;
    db->ninstructions++;
    return KAPS_JR1_OK;
}

static kaps_jr1_status
add_db(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_db *db,
    uint32_t result)
{
    struct instruction_desc *desc;
    struct kaps_jr1_device *device = instruction->device;
    int32_t i;

    if (db->key == NULL)
        return KAPS_JR1_INVALID_KEY;

    

    if (instruction->device->type == KAPS_JR1_DEVICE)
    {
        struct kaps_jr1_db *outer_parent;
        uint8_t num_parallel_searches = 0;

        if (instruction->num_searches == device->max_num_searches)
            return KAPS_JR1_NUM_SEARCHES_EXCEEDED;

        if (result >= device->max_num_searches)
            return KAPS_JR1_RESULT_NOT_VALID;

        for (i = 0; i < device->max_num_searches; i++)
        {
            if (!instruction->desc[i].db)
                continue;
            if (instruction->desc[i].db->parent)
                outer_parent = instruction->desc[i].db->parent;
            else
                outer_parent = instruction->desc[i].db;

            if (db->parent != NULL && outer_parent == db->parent)
                num_parallel_searches++;
            else if (db->parent == NULL && db == outer_parent)
                num_parallel_searches++;

            if (num_parallel_searches >= device->max_num_clone_parallel_lookups)
                return KAPS_JR1_EXCEEDED_MAX_PARALLEL_SEARCHES;
        }

        for (i = 0; i < device->max_num_searches; i++)
        {
            if (!instruction->desc[i].db)
                continue;

            if (db == instruction->desc[i].db)
                return KAPS_JR1_PARENT_CLONE_PARALLEL_SRCH_NOT_SUPPORTED;
        }

        if (instruction->desc[result].db != NULL)
            return KAPS_JR1_RESULT_NOT_VALID;

        desc = &instruction->desc[result];
        desc->kpu_id = result;
        desc->result_id = result;
        desc->db = db;
        instruction->num_searches++;

        return KAPS_JR1_OK;
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status
instruction_alloc(
    struct kaps_jr1_device *main_dev,
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t ltr,
    struct kaps_jr1_instruction **instruction_pp)
{
    struct kaps_jr1_instruction *instruction;

    instruction = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_instruction));
    if (!instruction)
        return KAPS_JR1_OUT_OF_MEMORY;

    instruction->id = id;
    instruction->ltr = ltr;
    instruction->device = device;

    kaps_jr1_c_list_add_tail(&main_dev->inst_list, &instruction->node);
    *instruction_pp = instruction;
    return KAPS_JR1_OK;
}

static void
kaps_jr1_key_exclude_dummyfill_fields(
    struct kaps_jr1_key *key)
{
    struct kaps_jr1_key_field *tmp;
    uint32_t cur_offset_1 = 0;

    for (tmp = key->first_field; tmp; tmp = tmp->next)
    {

        if (tmp->type == KAPS_JR1_KEY_FIELD_DUMMY_FILL)
            continue;

        tmp->offset_1 = cur_offset_1;
        cur_offset_1 += tmp->width_1;
    }

    key->width_1 = cur_offset_1;
}

kaps_jr1_status
kaps_jr1_instruction_init_internal(
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t ltr,
    struct kaps_jr1_instruction **instruction_pp,
    int32_t is_cascaded)
{
    struct kaps_jr1_instruction *instruction;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_device *main_dev;
    uint8_t is_rop_mode = 0;

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!instruction_pp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (device->type == KAPS_JR1_DEVICE && ltr != 0)
        return KAPS_JR1_INVALID_LTR_ID;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT && device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    if (device->main_dev)
        main_dev = device->main_dev;
    else
        main_dev = device;

    if (!is_rop_mode && !is_cascaded)
    {
        if (device->flags & KAPS_JR1_DEVICE_SMT)
        {
            if (ltr >= KAPS_JR1_HW_MAX_LTR_DUAL_PORT)
                return KAPS_JR1_INVALID_LTR_NUMBER;
            if (device->smt_no > 0)
                ltr += KAPS_JR1_HW_MAX_LTR_DUAL_PORT;
        }
        else
        {
            if (ltr >= KAPS_JR1_HW_MAX_LTRS)
                return KAPS_JR1_INVALID_LTR_NUMBER;
        }
    }

    kaps_jr1_c_list_iter_init(&main_dev->inst_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);
        if (!is_rop_mode && instruction->ltr == ltr && device->type != KAPS_JR1_DEVICE)
            return KAPS_JR1_LTR_BUSY;
        if (instruction->id == id)
        {
            int32_t is_ok = 0;

            
            if (instruction->type == INSTR_COMPARE3)
            {
                uint8_t cmp3_ltr = instruction->ltr + 1;

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

                if (ltr == cmp3_ltr)
                    is_ok = 1;
            }
            if (!is_ok)
                return KAPS_JR1_DUPLICATE_INSTRUCTION_ID;
        }
    }

    return instruction_alloc(main_dev, device, id, ltr, instruction_pp);
}

kaps_jr1_status
kaps_jr1_instruction_init(
    struct kaps_jr1_device * device,
    uint32_t id,
    uint32_t ltr,
    struct kaps_jr1_instruction ** instruction_pp)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    KAPS_JR1_TRACE_IN("%p %u %u %p\n", device, id, ltr, instruction_pp);

    status = kaps_jr1_instruction_init_internal(device, id, ltr, instruction_pp, 0);

    KAPS_JR1_TRACE_OUT("%p\n", (instruction_pp ? (*instruction_pp) : NULL));
    return status;
}

kaps_jr1_status
kaps_jr1_instruction_refresh_handle(
    struct kaps_jr1_device * device,
    struct kaps_jr1_instruction * stale_ptr,
    struct kaps_jr1_instruction ** instruction_pp)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;

    KAPS_JR1_TRACE_IN("%p %p %p\n", device, stale_ptr, instruction_pp);
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (device->main_dev)
        device = device->main_dev;

    if (!stale_ptr || !instruction_pp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
        return KAPS_JR1_INVALID_FLAGS;

    if (device->issu_status == KAPS_JR1_ISSU_INIT)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_c_list_iter_init(&device->inst_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *tmp = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

        if (tmp->stale_ptr == (uintptr_t) stale_ptr)
        {
            *instruction_pp = tmp;
            return KAPS_JR1_OK;
        }
    }

    if (device->other_core)
    {
        device = device->other_core;
        kaps_jr1_c_list_iter_init(&device->inst_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *tmp = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

            if (tmp->stale_ptr == (uintptr_t) stale_ptr)
            {
                *instruction_pp = tmp;
                return KAPS_JR1_OK;
            }
        }
    }
    KAPS_JR1_TRACE_OUT("%p\n", *instruction_pp);
    return KAPS_JR1_INVALID_ARGUMENT;
}

static kaps_jr1_status
kaps_jr1_instruction_wb_init(
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t ltr,
    uintptr_t stale_ptr,
    struct kaps_jr1_instruction **instruction_pp)
{
    struct kaps_jr1_instruction *instruction;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_device *main_dev;

    KAPS_JR1_TRACE_IN("%p %u %u %p %p\n", device, id, ltr, stale_ptr, instruction_pp);
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!stale_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (device->type == KAPS_JR1_DEVICE && ltr != 0)
        return KAPS_JR1_INVALID_LTR_ID;

    if (device->main_dev)
        main_dev = device->main_dev;
    else
        main_dev = device;

    kaps_jr1_c_list_iter_init(&main_dev->inst_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);
        if (instruction->ltr == ltr && device->type != KAPS_JR1_DEVICE)
            return KAPS_JR1_LTR_BUSY;
        if (instruction->id == id)
        {
            int32_t is_ok = 0;

            
            if (instruction->type == INSTR_COMPARE3)
            {
                uint8_t cmp3_ltr = instruction->ltr + 1;

                if (instruction->device->flags & KAPS_JR1_DEVICE_SMT)
                {
                    if (cmp3_ltr >= KAPS_JR1_HW_MAX_LTR_DUAL_PORT)
                        cmp3_ltr -= KAPS_JR1_HW_MAX_LTR_DUAL_PORT;
                }
                else
                {
                    if (cmp3_ltr >= KAPS_JR1_HW_MAX_LTRS)
                        cmp3_ltr -= KAPS_JR1_HW_MAX_LTRS;
                }

                if (ltr == cmp3_ltr)
                    is_ok = 1;
            }
            if (!is_ok)
                return KAPS_JR1_DUPLICATE_INSTRUCTION_ID;
        }
    }

    KAPS_JR1_TRY(instruction_alloc(main_dev, device, id, ltr, &instruction));
    instruction->stale_ptr = stale_ptr;
    *instruction_pp = instruction;
    KAPS_JR1_TRACE_OUT("%p\n", *instruction_pp);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_destroy_single_device(
    struct kaps_jr1_instruction * instruction)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_device *main_dev;
    int32_t i;

    if (!instruction)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (instruction->device->type != KAPS_JR1_DEVICE)
    {

        
        for (i = 0; i < instruction->num_searches; i++)
        {
            if (instruction->desc[i].db)
                return KAPS_JR1_INSTRUCTION_BUSY;
        }
    }

    

    if (instruction->device->main_dev)
        main_dev = instruction->device->main_dev;
    else
        main_dev = instruction->device;

    kaps_jr1_c_list_iter_init(&main_dev->inst_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *ins = KAPS_JR1_INSTLIST_TO_ENTRY(el);

        if (ins == instruction)
        {
            kaps_jr1_c_list_remove_node(&main_dev->inst_list, el, &it);

            if (instruction->type == INSTR_COMPARE3)
            {
                struct kaps_jr1_c_list_iter it1;
                struct kaps_jr1_list_node *e;
                uint8_t cmp3_ltr = instruction->ltr + 1;

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

                kaps_jr1_c_list_iter_init(&main_dev->inst_list, &it1);
                while ((e = kaps_jr1_c_list_iter_next(&it1)) != NULL)
                {
                    struct kaps_jr1_instruction *ins = KAPS_JR1_INSTLIST_TO_ENTRY(e);

                    if (ins->ltr == cmp3_ltr)
                    {
                        kaps_jr1_c_list_remove_node(&main_dev->inst_list, e, &it1);

                        if (ins->master_key)
                            kaps_jr1_key_destroy_internal(ins->master_key);
                        ins->device->alloc->xfree(ins->device->alloc->cookie, ins);
                    }
                }
            }

            break;
        }
    }

    if (instruction->master_key)
    {
        kaps_jr1_key_destroy_internal(instruction->master_key);
    }

    instruction->device->alloc->xfree(instruction->device->alloc->cookie, instruction);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_destroy(
    struct kaps_jr1_instruction * instruction)
{
    KAPS_JR1_TRACE_IN("%p\n", instruction);
    if (!instruction)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_STRY(kaps_jr1_instruction_destroy_single_device(instruction));
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_add_db(
    struct kaps_jr1_instruction * instruction,
    struct kaps_jr1_db * db,
    uint32_t result)
{
    char *error;
    kaps_jr1_status status;

    KAPS_JR1_TRACE_IN("%p %p %u\n", instruction, db, result);

    if (instruction == NULL || db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!instruction->master_key)
        return KAPS_JR1_INVALID_MASTER_KEY;

    if (instruction->is_installed)
        return KAPS_JR1_INSTRUCTION_ALREADY_INSTALLED;

    if (instruction->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    status = kaps_jr1_key_verify(instruction->master_key, db->key, &error);
    if (status != KAPS_JR1_OK)
        return status;
    return add_db(instruction, db, result);
}

kaps_jr1_status
kaps_jr1_instruction_set_key(
    struct kaps_jr1_instruction * instruction,
    struct kaps_jr1_key * master_key)
{
    struct kaps_jr1_device *main_device;

    KAPS_JR1_TRACE_IN("%p %p\n", instruction, master_key);

    if (!instruction || !master_key)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (instruction->master_key)
        return KAPS_JR1_DUPLICATE_KEY;

    if (instruction->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (master_key->first_field == NULL)
        return KAPS_JR1_MISSING_KEY_FIELD;

    main_device = instruction->device;
    if (instruction->device->main_dev)
        main_device = instruction->device->main_dev;

    if (master_key->ref_count != 0)
    {
        
        struct kaps_jr1_key *new_key;

        kaps_jr1_status status = kaps_jr1_key_clone(instruction->device, master_key, &new_key);
        if (status != KAPS_JR1_OK)
            return status;
        master_key = new_key;
    }

    master_key->ref_count++;

    if (master_key->has_dummy_fill_field)
        kaps_jr1_key_exclude_dummyfill_fields(master_key);

    if (master_key->width_1 > main_device->max_search_key_width_1)
    {
        return kaps_jr1_device_log_error(instruction->device, KAPS_JR1_INVALID_KEY_WIDTH,
                                     "Specified Master Key width %d exceeded the max key width %d",
                                     master_key->width_1, main_device->max_search_key_width_1);
    }

    instruction->master_key = master_key;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_install(
    struct kaps_jr1_instruction * instruction)
{
    
    int32_t i, ad_width_1 = 0;
    int32_t has_index = 1, index_width_1 = 0;
    uint32_t last_port = 0;

    KAPS_JR1_TRACE_IN("%p\n", instruction);
    if (!instruction)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (instruction->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (instruction->is_installed)
        return KAPS_JR1_INSTRUCTION_ALREADY_INSTALLED;

    if (instruction->num_searches == 0)
        return KAPS_JR1_INSTRUCTION_NO_DB;

    KAPS_JR1_TX_ERROR_CHECK(instruction->device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((instruction->device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         instruction->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    instruction->is_installed = 1;
    if (instruction->device->type == KAPS_JR1_DEVICE)
    {

        
        instruction->type = INSTR_CBWLPM;
        
        for (i = 0; i < instruction->device->max_num_searches; i++)
        {
            if (instruction->desc[i].db)
            {
                KAPS_JR1_STRY(add_to_db_list_internal(instruction, instruction->desc[i].db));
                last_port = i;
            }
        }

        
        if (instruction->master_key->width_1 < (last_port + 1) * KAPS_JR1_SEARCH_INTERFACE_WIDTH_1)
            return KAPS_JR1_INVALID_KEY_WIDTH;

        return KAPS_JR1_OK;
    }
    else
    {
        instruction->type = INSTR_COMPARE1;
    }

    

    if (instruction->device->flags & KAPS_JR1_DEVICE_SUPPRESS_INDEX)
        has_index = 0;

    for (i = 0; i < instruction->num_searches; i++)
    {
        struct instruction_desc *desc = NULL;
        struct kaps_jr1_db *outer_parent = NULL, *idb = NULL;

        desc = &instruction->desc[i];

        idb = desc->db;
        if (idb->parent)
            outer_parent = idb->parent;
        else
            outer_parent = idb;

        if (outer_parent->is_bc_required && !outer_parent->is_main_bc_db)
            outer_parent = outer_parent->main_bc_db;

        if (outer_parent->common_info->ad_info.ad)
            ad_width_1 += kaps_jr1_resource_normalized_ad_width(instruction->device,
                                                       outer_parent->common_info->ad_info.ad->width.ad_width_1);

        if (has_index)
            index_width_1 += KAPS_JR1_HW_HIT_INDEX_WIDTH_1;
    }

    
    for (i = 0; i < instruction->num_searches; i++)
    {
        KAPS_JR1_STRY(add_to_db_list_internal(instruction, instruction->desc[i].db));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_print(
    struct kaps_jr1_instruction * instruction,
    FILE * fp)
{
    char *name;
    uint32_t srch_cnt;
    int32_t i;
    uint8_t cmp3_ltr = 0;

    KAPS_JR1_TRACE_IN("%p %p\n", instruction, fp);

    if (!instruction || !fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (instruction->device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (instruction->device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    KAPS_JR1_STRY(kaps_jr1_instruction_name(instruction->device, instruction->type, &name));

    kaps_jr1_fprintf(fp, "Instruction ID:%3d LTR:%3d TYPE:%9s \n Master Key: ", instruction->id, instruction->ltr, name);
    if (instruction->master_key)
        kaps_jr1_key_print(instruction->master_key, fp);

    srch_cnt = instruction->num_searches;
    if (instruction->device->type == KAPS_JR1_DEVICE)
        srch_cnt = instruction->device->max_num_searches;

    kaps_jr1_fprintf(fp, "\n Result Map: \n");

    for (i = 0; i < srch_cnt; i++)
    {
        struct kaps_jr1_db *db = instruction->desc[i].db;
        int32_t res_id = 0, kpu_id = 0, id = 0;
        int32_t cycle = 0;

        res_id = instruction->desc[i].result_id;
        kpu_id = instruction->desc[i].kpu_id;
        if (instruction->type == INSTR_COMPARE2)
        {
            cycle = 1;
        }
        else if ((kpu_id == 2 || kpu_id == 3) && (instruction->type == INSTR_COMPARE3))
        {
            continue;
        }

        if (db)
        {
            id = db->tid;
            if (db->parent)
                id = db->parent->tid;
            kaps_jr1_fprintf(fp, "\t %s_%2d", kaps_jr1_device_db_name(db), id);

            kaps_jr1_fprintf(fp, ": result %d, cycle %d", res_id, cycle);   

            if (db->is_clone)
            {
                if (!db->clone_of->parent)
                    kaps_jr1_fprintf(fp, "   (clone-%2d)", db->tid);
                else
                    kaps_jr1_fprintf(fp, "   (table-%2d clone-%2d)", db->clone_of->parent->tid, db->clone_of->tid);
            }
            else if (db->parent)
                kaps_jr1_fprintf(fp, "   (table-%2d)", db->tid);

            kaps_jr1_fprintf(fp, "\n");
        }
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
                for (i = 0; i < in->num_searches; i++)
                {
                    struct kaps_jr1_db *db = in->desc[i].db;
                    int32_t id = db->tid;

                    if (db->parent)
                        id = db->parent->tid;
                    kaps_jr1_fprintf(fp, "\t %s_%2d", kaps_jr1_device_db_name(db), id);
                    kaps_jr1_fprintf(fp, ": kpu %d, result %d, cycle 1", in->desc[i].kpu_id, in->desc[i].result_id);

                    if (db->is_clone)
                        kaps_jr1_fprintf(fp, "   (clone-%2d)", db->tid);
                    else if (db->parent)
                        kaps_jr1_fprintf(fp, "   (table-%2d)", db->tid);

                    kaps_jr1_fprintf(fp, "\n");
                }
            }
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_master_key_from_db_key(
    struct kaps_jr1_instruction * instruction,
    struct kaps_jr1_db * db,
    uint8_t * db_key,
    uint8_t * search_key)
{
    struct kaps_jr1_key_field *db_key_field = NULL, *mkey_field = NULL;
    uint32_t db_key_offset_8, mkey_offset_8 = 0;

    KAPS_JR1_TRACE_IN("%p %p %p %p\n", instruction, db, db_key, search_key);
    if (!instruction || !db || !db->key || !db_key || !search_key)
        return KAPS_JR1_INVALID_ARGUMENT;

    

    mkey_field = instruction->master_key->first_field;
    mkey_offset_8 = 0;

    while (mkey_field)
    {
        db_key_field = db->key->first_field;
        db_key_offset_8 = 0;

        if (mkey_field->type == KAPS_JR1_KEY_FIELD_HOLE)
        {
            mkey_field = mkey_field->next;
            continue;
        }

        while (db_key_field)
        {
            if (db_key_field->type == KAPS_JR1_KEY_FIELD_HOLE)
            {
                db_key_field = db_key_field->next;
                continue;
            }
            if (strcmp(mkey_field->name, db_key_field->name) == 0)
            {
                kaps_jr1_memcpy(&search_key[mkey_offset_8],
                            &db_key[db_key_offset_8], db_key_field->width_1 / KAPS_JR1_BITS_IN_BYTE);
                break;
            }
            db_key_offset_8 += (db_key_field->width_1 / KAPS_JR1_BITS_IN_BYTE);
            db_key_field = db_key_field->next;
        }
        mkey_offset_8 += mkey_field->width_1 / KAPS_JR1_BITS_IN_BYTE;
        mkey_field = mkey_field->next;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_search(
    struct kaps_jr1_instruction * instruction,
    uint8_t * master_key,
    uint32_t cb_addrs,
    struct kaps_jr1_complete_search_result * result)
{
    int16_t j;

    KAPS_JR1_TRACE_IN("%p %p %u %p\n", instruction, master_key, cb_addrs, result);
    if (!instruction || !master_key || !result)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (cb_addrs == -1 && instruction->device->type == KAPS_JR1_DEVICE)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (instruction->device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (instruction->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
        {
            return KAPS_JR1_UNSUPPORTED;
        }
    }

    if (!instruction->device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (instruction->device->type == KAPS_JR1_DEVICE)
    {
        struct kaps_jr1_search_result kaps_jr1_result;
        int32_t i, is_ad_all_zero;

        if (cb_addrs != 0)
            return KAPS_JR1_INVALID_CB_ADDRESS;

        kaps_jr1_memset(result, 0, sizeof(*result));

        for (j = 0; j < instruction->device->max_num_searches; j++)
        {

            result->result_valid[j] = KAPS_JR1_RESULT_IS_INVALID;
            result->hit_index[j] = -1;

            if (!instruction->desc[j].db)
                continue;

            result->result_valid[j] = KAPS_JR1_RESULT_IS_VALID;

            KAPS_JR1_STRY(kaps_jr1_dm_search(instruction->device, &master_key[j * KAPS_JR1_SEARCH_INTERFACE_WIDTH_8],
                                          j, &kaps_jr1_result));

            is_ad_all_zero = 1;
            for (i = 0; i < KAPS_JR1_MAX_AD_WIDTH_8; ++i)
            {
                if (kaps_jr1_result.ad_value[i] != 0)
                {
                    is_ad_all_zero = 0;
                    break;
                }
            }

            {

                
                if (is_ad_all_zero)
                    result->hit_or_miss[j] = KAPS_JR1_MISS;
                else
                    result->hit_or_miss[j] = KAPS_JR1_HIT;
            }

            kaps_jr1_memcpy(result->assoc_data[j], kaps_jr1_result.ad_value, KAPS_JR1_MAX_AD_WIDTH_8);

        }

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_name(
    struct kaps_jr1_device * device,
    enum instruction_type instr,
    char **name)
{
    *name = "invalid_inst";     

    if (device == NULL)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    switch (instr)
    {
        case INSTR_CBWLPM:
            *name = "cbwlpm";
            break;
        case INSTR_COMPARE1:
            *name = "compare1";
            break;
        case INSTR_COMPARE2:
            *name = "compare2";
            break;
        case INSTR_COMPARE3:
            *name = "compare3";
            break;
        default:
            return KAPS_JR1_INVALID_INSTRUCTION;
    }

    return KAPS_JR1_OK;
}


struct kaps_jr1_instruction_desc_wb
{
    uintptr_t db;                   
    uint32_t result_id;             
    uint32_t user_result_id;        
    uint32_t user_specified;        
    uint8_t sram_dt_num[8];             
};


struct kaps_jr1_instruction_wb
{
    uintptr_t stale_ptr;            
    uintptr_t cascaded_inst_stale_ptr; 
    uint32_t id;                    
    uint16_t ltr;                   
    uint16_t lut_index;
    uint16_t lut_rec_length;
    uint16_t num_searches;          
    uint16_t smt_no;                
    uint16_t c3_num_searches;       
    uint32_t is_cmp3;               
    uint32_t is_cascading_src;      
    uint32_t is_cascading_dst;      
    uint16_t has_resolution;        
    struct kaps_jr1_instruction_desc_wb desc[KAPS_JR1_INSTRUCTION_MAX_RESULTS]; 
    struct kaps_jr1_instruction_desc_wb c3_desc[KAPS_JR1_INSTRUCTION_MAX_RESULTS]; 
};

kaps_jr1_status
kaps_jr1_instruction_wb_save(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    uint8_t i = 0, j = 0;
    struct kaps_jr1_instruction *c3_pair = NULL;
    struct kaps_jr1_device *device;
    struct kaps_jr1_instruction_wb wb_info, *wb_info_ptr;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = &wb_info;
    }
    else
    {
        wb_info_ptr = (struct kaps_jr1_instruction_wb *) wb_fun->nv_ptr;
    }

    kaps_jr1_memset(wb_info_ptr, 0, sizeof(*wb_info_ptr));
    wb_info_ptr->stale_ptr = (uintptr_t) instruction;
    wb_info_ptr->ltr = instruction->ltr;
    wb_info_ptr->num_searches = instruction->num_searches;
    wb_info_ptr->id = instruction->id;
    wb_info_ptr->smt_no = instruction->device->smt_no;
    wb_info_ptr->is_cmp3 = instruction->is_cmp3;
    wb_info_ptr->is_cascading_src = instruction->is_cascading_src;
    wb_info_ptr->is_cascading_dst = instruction->is_cascading_dst;
    wb_info_ptr->has_resolution = instruction->has_resolution;
    wb_info_ptr->cascaded_inst_stale_ptr = (uintptr_t) instruction->cascaded_inst;
    wb_info_ptr->lut_index = instruction->lut_index;
    wb_info_ptr->lut_rec_length = instruction->lut_rec_length;

    for (i = 0, j = 0; i < instruction->num_searches; i++, j++)
    {
        while (!instruction->desc[j].db)
            j++;
        wb_info_ptr->desc[i].db = (uintptr_t) instruction->desc[j].db;
        wb_info_ptr->desc[i].result_id = instruction->desc[j].result_id;
        wb_info_ptr->desc[i].user_result_id = instruction->desc[j].user_result_id;
        wb_info_ptr->desc[i].user_specified = instruction->desc[j].user_specified;
        kaps_jr1_memcpy(wb_info_ptr->desc[i].sram_dt_num, instruction->desc[j].sram_dt_num, 8 * sizeof(uint8_t));
    }

    if (instruction->device->main_dev)
        device = instruction->device->main_dev;
    else
        device = instruction->device;

    
    if (instruction->type == INSTR_COMPARE3)
    {
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *e1;

        kaps_jr1_c_list_iter_init(&device->inst_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *tmp;
            uint32_t ltr = (instruction->ltr + 1);

            if (device->flags & KAPS_JR1_DEVICE_SMT
                && (ltr == KAPS_JR1_HW_MAX_LTR_DUAL_PORT || ltr == (2 * KAPS_JR1_HW_MAX_LTR_DUAL_PORT)))
                
                ltr -= KAPS_JR1_HW_MAX_LTR_DUAL_PORT;
            else
                ltr %= KAPS_JR1_HW_MAX_LTRS;        

            tmp = KAPS_JR1_INSTLIST_TO_ENTRY(e1);
            if ((tmp != instruction) && (tmp->id == instruction->id) && (tmp->ltr == ltr))
                c3_pair = tmp;
        }
        kaps_jr1_sassert(c3_pair != NULL);
    }

    
    if (c3_pair)
    {
        int32_t index = 0;

        for (i = 0; i < c3_pair->num_searches; i++)
        {
            if (c3_pair->desc[i].result_id == 2 || c3_pair->desc[i].result_id == 3)
                continue;
            wb_info_ptr->c3_desc[index].db = (uintptr_t) c3_pair->desc[i].db;
            wb_info_ptr->c3_desc[index].result_id = c3_pair->desc[i].result_id + 2;
            index++;
        }
        wb_info_ptr->c3_num_searches = index;
    }

    
    if (wb_fun->write_fn)
    {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*wb_info_ptr);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(*wb_info_ptr);

    

    KAPS_JR1_STRY(kaps_jr1_key_wb_save(instruction->master_key, wb_fun));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_instruction_wb_restore(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_key *master_key;
    struct kaps_jr1_instruction *instruction = NULL;
    struct kaps_jr1_db *db;
    struct kaps_jr1_device *target_device = device;
    struct kaps_jr1_instruction_wb wb_info = {0};
    struct kaps_jr1_instruction_wb *wb_info_ptr;
    int32_t i;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = &wb_info;
    }
    else
    {
        wb_info_ptr = (struct kaps_jr1_instruction_wb *) wb_fun->nv_ptr;
    }

    if (wb_fun->read_fn != NULL)
    {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*wb_info_ptr);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(*wb_info_ptr);

    if (device->flags & KAPS_JR1_DEVICE_SMT)
    {
        if (wb_info_ptr->smt_no == device->smt_no)
            target_device = device;
        else
            target_device = device->smt;
    }

    KAPS_JR1_STRY(kaps_jr1_instruction_wb_init(target_device, wb_info_ptr->id, wb_info_ptr->ltr,
                                       wb_info_ptr->stale_ptr, &instruction));

    KAPS_JR1_STRY(kaps_jr1_key_init(target_device, &master_key));
    KAPS_JR1_STRY(kaps_jr1_key_wb_restore(master_key, wb_fun));
    KAPS_JR1_STRY(kaps_jr1_instruction_set_key(instruction, master_key));

    for (i = 0; i < wb_info_ptr->num_searches; i++)
    {
        KAPS_JR1_STRY(kaps_jr1_db_refresh_handle(device, (struct kaps_jr1_db *) wb_info_ptr->desc[i].db, &db));
        KAPS_JR1_STRY(kaps_jr1_instruction_add_db(instruction, db, wb_info_ptr->desc[i].result_id));
        instruction->desc[i].user_result_id = wb_info_ptr->desc[i].user_result_id;
        instruction->desc[i].user_specified = wb_info_ptr->desc[i].user_specified;
        kaps_jr1_memcpy(instruction->desc[i].sram_dt_num, wb_info_ptr->desc[i].sram_dt_num, 8 * sizeof(uint8_t));
    }
    for (i = 0; i < wb_info_ptr->c3_num_searches; i++)
    {
        KAPS_JR1_STRY(kaps_jr1_db_refresh_handle(device, (struct kaps_jr1_db *) wb_info_ptr->c3_desc[i].db, &db));
        KAPS_JR1_STRY(kaps_jr1_instruction_add_db(instruction, db, wb_info_ptr->c3_desc[i].result_id));
    }

    instruction->is_cmp3 = wb_info_ptr->is_cmp3;
    instruction->is_cascading_src = wb_info_ptr->is_cascading_src;
    instruction->is_cascading_dst = wb_info_ptr->is_cascading_dst;
    instruction->cascaded_inst_stale_ptr = wb_info_ptr->cascaded_inst_stale_ptr;
    instruction->has_resolution = wb_info_ptr->has_resolution;
    instruction->lut_index = wb_info_ptr->lut_index;
    instruction->lut_rec_length = wb_info_ptr->lut_rec_length;

    KAPS_JR1_STRY(kaps_jr1_instruction_install(instruction));

    return KAPS_JR1_OK;
}

