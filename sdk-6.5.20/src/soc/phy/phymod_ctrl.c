/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phymod_ctrl.c
 * Purpose:     Interface functions for PHYMOD
 */

#ifdef PHYMOD_SUPPORT

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/phy/phymod_ctrl.h>
#include <phymod/phymod_types.h>
#include <phymod/phymod.h>

#define OBJ_ID(obj_) (obj_)->obj_id
#define OBJ_ID_VALID(id_) ((id_) >= 0)

static soc_phy_obj_t *core_list[SOC_MAX_NUM_DEVICES];
static soc_phy_obj_t *phy_list[SOC_MAX_NUM_DEVICES];



STATIC int
soc_phy_obj_exists(soc_phy_obj_t **obj_head, int obj_id, soc_phy_obj_t **f_obj)
{
    soc_phy_obj_t *obj = *obj_head;

    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("obj_exists 0x%x "), obj_id));
    while (obj) {
        LOG_DEBUG(BSL_LS_SOC_PHYMOD,
                  (BSL_META("[0x%x] "), OBJ_ID(obj)));
        if (OBJ_ID(obj) == obj_id) {
            if (f_obj) {
                *f_obj = obj;
            }
            return 1;
        }
        obj = obj->next;
    }
    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("\n")));
    return 0;
}

STATIC int
soc_phy_obj_insert(soc_phy_obj_t **obj_head, soc_phy_obj_t *new_obj)
{
    soc_phy_obj_t *obj = *obj_head;

    if (new_obj == NULL) {
        return -1;
    }

    new_obj->next = NULL;

    while (obj && obj->next) {
        LOG_DEBUG(BSL_LS_SOC_PHYMOD,
                  (BSL_META("[0x%x] "), OBJ_ID(obj)));
        if (OBJ_ID(obj->next) > OBJ_ID(new_obj)) {
            break;
        }
        obj = obj->next;
    }
    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("\n")));

    if (obj) {
        LOG_DEBUG(BSL_LS_SOC_PHYMOD,
                  (BSL_META("end_id 0x%x\n"), OBJ_ID(obj)));
        if (OBJ_ID(obj) > OBJ_ID(new_obj)) {
            new_obj->next = obj;
            *obj_head = new_obj;
        } else {
            new_obj->next = obj->next;
            obj->next = new_obj;
        }
    } else {
        *obj_head = new_obj;
    }

    return 0;
}

STATIC int
soc_phy_obj_delete(soc_phy_obj_t **obj_head, soc_phy_obj_t *del_obj)
{
    soc_phy_obj_t *prev, *obj = *obj_head;

    if (del_obj == NULL) {
        return -1;
    }

    prev = NULL;
    while (obj) {
        if (OBJ_ID(obj) == OBJ_ID(del_obj)) {
            LOG_DEBUG(BSL_LS_SOC_PHYMOD,
                      (BSL_META("delete 0x%x\n"), OBJ_ID(obj)));
            if (prev) {
                prev->next = obj->next;
            } else {
                *obj_head = (*obj_head)->next;
            }
            break;
        }
        prev = obj;
        obj = obj->next;
    }

    return 0;
}

STATIC int
soc_phymod_free_core_id_get(int unit)
{
    soc_phy_obj_t *core = core_list[unit];
    int new_id = 1;

    while (core && core->next) {
        new_id = OBJ_ID(core) + 1;
        if (new_id != OBJ_ID(core->next)) {
            return new_id;
        }
        core = core->next;
    }

    if (core) {
        new_id = OBJ_ID(core) + 1;
    }

    return 0;
}

STATIC int
soc_phymod_free_phy_id_get(int unit)
{
    soc_phy_obj_t *phy = phy_list[unit];
    int new_id = 1;

    while (phy && phy->next) {
        new_id = OBJ_ID(phy) + 1;
        if (new_id != OBJ_ID(phy->next)) {
            return new_id;
        }
        phy = phy->next;
    }

    if (phy) {
        new_id = OBJ_ID(phy) + 1;
    }

    return new_id;
}

int
soc_phymod_miim_bus_read(int unit, uint32_t addr, uint32_t reg, uint32_t *data)
{
    int rv;
    uint16 data16;

    rv = soc_miim_read(unit, addr, reg, &data16);
    *data = data16;

    return rv;
}

int
soc_phymod_miim_bus_write(int unit, uint32_t addr, uint32_t reg, uint32_t data)
{
    return soc_miim_write(unit, addr, reg, data);
}

int
soc_phymod_core_create(int unit, int core_id, soc_phymod_core_t **core)
{
    soc_phymod_core_t *new_core;
    soc_phy_obj_t *core_obj;

    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("core_create 0x%x\n"), core_id));
    if (OBJ_ID_VALID(core_id) &&
        soc_phy_obj_exists(&core_list[unit], core_id, NULL)) {
        return SOC_E_EXISTS;
    }

    new_core = sal_alloc(sizeof(soc_phymod_core_t), "soc_phymod_core");
    if (new_core == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(new_core, 0 ,sizeof(soc_phymod_core_t));
    core_obj = &new_core->obj;
    core_obj->owner = (void *)new_core;

    if (OBJ_ID_VALID(core_id)) {
        OBJ_ID(core_obj) = core_id;
    } else {
        OBJ_ID(core_obj) = soc_phymod_free_core_id_get(unit);
    }

    soc_phy_obj_insert(&core_list[unit], core_obj);

    new_core->device_aux_modes = NULL ;

    *core = new_core;

    return SOC_E_NONE;    
}

int
soc_phymod_core_destroy(int unit, soc_phymod_core_t *core)
{
    if (core == NULL) {
        return SOC_E_PARAM;
    }

    soc_phy_obj_delete(&core_list[unit], &core->obj);

    sal_free(core);

    return SOC_E_NONE;    
}

int
soc_phymod_core_find_by_id(int unit, int core_id, soc_phymod_core_t **core)
{
    soc_phy_obj_t *core_obj;

    if (core == NULL) {
        return SOC_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("core_find 0x%x\n"), core_id));
    if (OBJ_ID_VALID(core_id) &&
        soc_phy_obj_exists(&core_list[unit], core_id, &core_obj)) {
        *core = (soc_phymod_core_t *)core_obj->owner;
        return SOC_E_NONE;
    }

    return SOC_E_NOT_FOUND;    
}

int
soc_phymod_phy_create(int unit, int phy_id, soc_phymod_phy_t **phy)
{
    soc_phymod_phy_t *new_phy;
    soc_phy_obj_t *phy_obj;

    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("phy_create 0x%x\n"), phy_id));
    if (OBJ_ID_VALID(phy_id) &&
        soc_phy_obj_exists(&phy_list[unit], phy_id, NULL)) {
        return SOC_E_EXISTS;
    }

    new_phy = sal_alloc(sizeof(soc_phymod_phy_t), "soc_phymod_phy");
    if (new_phy == NULL) {
        return SOC_E_MEMORY;
    }
    phy_obj = &new_phy->obj;
    phy_obj->owner = (void *)new_phy;

    if (OBJ_ID_VALID(phy_id)) {
        OBJ_ID(phy_obj) = phy_id;
    } else {
        OBJ_ID(phy_obj) = soc_phymod_free_phy_id_get(unit);
    }

    soc_phy_obj_insert(&phy_list[unit], phy_obj);

    *phy = new_phy;

    return SOC_E_NONE;    
}

int
soc_phymod_phy_destroy(int unit, soc_phymod_phy_t *phy)
{
    if (phy == NULL) {
        return SOC_E_PARAM;
    }

    soc_phy_obj_delete(&phy_list[unit], &phy->obj);

    sal_free(phy);

    return SOC_E_NONE;    
}

int
soc_phymod_phy_find_by_id(int unit, int phy_id, soc_phymod_phy_t **phy)
{
    soc_phy_obj_t *phy_obj;

    if (phy == NULL) {
        return SOC_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_SOC_PHYMOD,
              (BSL_META("phy_find 0x%x\n"), phy_id));
    if (OBJ_ID_VALID(phy_id) &&
        soc_phy_obj_exists(&phy_list[unit], phy_id, &phy_obj)) {
        *phy = (soc_phymod_phy_t *)phy_obj->owner;
        return SOC_E_NONE;
    }

    return SOC_E_NOT_FOUND;    
}

void
soc_phymod_usleep(uint32_t usecs)
{
    sal_usleep(usecs);
}

void
soc_phymod_sleep(int secs)
{
    sal_sleep(secs);
}

int
soc_phymod_strcmp(const char *str1, const char *str2)
{
    return sal_strcmp(str1, str2);
}

void *
soc_phymod_memcpy(void *dst, const void *src, size_t n)
{
    return sal_memcpy(dst, src, n);
}

void *
soc_phymod_memset(void *dst, int c, size_t n)
{
    return sal_memset(dst, c, n);
}

void *
soc_phymod_alloc(size_t size, char *descr)
{
    return sal_alloc(size, descr);
}

void
soc_phymod_free(void *ptr)
{
    sal_free(ptr);
}

#else
typedef int _phymod_ctrl_not_empty; /* Make ISO compilers happy. */
#endif
