

#include <kaps_jr1_arena.h>


#define NLM_ALIGN_UP(value, align) (((value) + ((align) - 1)) & ~((align) - 1))

typedef struct kaps_jr1_arena
{
    char *m_arenaStart_p;       
    char *m_arenaEnd_p;         
    uint32_t m_numBytesAvailable;       
    uint32_t m_numAllocedChunks;        
    uint32_t m_curOffset;       
    struct kaps_jr1_arena *m_next_p;        
    kaps_jr1_nlm_allocator *m_alloc_p;      
} kaps_jr1_arena;

typedef struct kaps_jr1_arena_info
{

    kaps_jr1_arena *g_arenaHead_p;  
    uint32_t g_allowAllocFromArena;     
    uint32_t g_numAllocedChunksFromAllArenas;   
    uint32_t g_numCompletelyFreeArena;  
    uint32_t g_arenaSizeInBytes;        

} kaps_jr1_arena_info;


NlmErrNum_t
kaps_jr1_arena_add_arena(
    kaps_jr1_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason_p)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) alloc_p->m_arena_info;
    kaps_jr1_arena *arena_p = NULL;

    
    arena_p = (kaps_jr1_arena *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, sizeof(kaps_jr1_arena));

    if (!arena_p)
    {
        if (o_reason_p)
            *o_reason_p = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    
    arena_p->m_arenaStart_p =
        (char *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, arena_info_p->g_arenaSizeInBytes);

    if (!arena_p->m_arenaStart_p)
    {
        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, arena_p);
        if (o_reason_p)
            *o_reason_p = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    arena_p->m_arenaEnd_p = arena_p->m_arenaStart_p + arena_info_p->g_arenaSizeInBytes - 1;
    arena_p->m_numBytesAvailable = arena_info_p->g_arenaSizeInBytes;
    arena_p->m_alloc_p = alloc_p;

    if (!arena_info_p->g_arenaHead_p)
    {
        arena_info_p->g_arenaHead_p = arena_p;
    }
    else
    {
        kaps_jr1_arena *curArena_p = arena_info_p->g_arenaHead_p;
        while (curArena_p->m_next_p)
            curArena_p = curArena_p->m_next_p;

        
        curArena_p->m_next_p = arena_p;
    }

    arena_info_p->g_numCompletelyFreeArena++;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_arena_init(
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t arenaSizeInBytes,
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_arena_info *arena_info_p;

    alloc_p->m_arena_info =
        (void *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, sizeof(struct kaps_jr1_arena_info));
    arena_info_p = (struct kaps_jr1_arena_info *) alloc_p->m_arena_info;
    arena_info_p->g_arenaSizeInBytes = KAPS_JR1_DEFAULT_ARENA_SIZE_IN_BYTES;

    if (arenaSizeInBytes > arena_info_p->g_arenaSizeInBytes)
        arena_info_p->g_arenaSizeInBytes = arenaSizeInBytes;

    errNum = kaps_jr1_arena_add_arena(alloc_p, o_reason_p);

    return errNum;
}

NlmErrNum_t
kaps_jr1_arena_activate_arena(
    kaps_jr1_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason_p)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) alloc_p->m_arena_info;
    NlmErrNum_t errNum = NLMERR_OK;

    if (arena_info_p->g_numCompletelyFreeArena)
    {
        
        arena_info_p->g_allowAllocFromArena = 1;
        return NLMERR_OK;
    }

    
    errNum = kaps_jr1_arena_add_arena(alloc_p, o_reason_p);

    
    if (arena_info_p->g_numCompletelyFreeArena)
        arena_info_p->g_allowAllocFromArena = 1;

    return errNum;
}

NlmErrNum_t
kaps_jr1_arena_deactivate_arena(
    kaps_jr1_nlm_allocator * api_alloc_p)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) api_alloc_p->m_arena_info;
    
    arena_info_p->g_allowAllocFromArena = 0;

    
    if (arena_info_p->g_numCompletelyFreeArena <= 1)
        return NLMERR_OK;

    {
        kaps_jr1_arena *curArena_p = arena_info_p->g_arenaHead_p;
        kaps_jr1_arena *prevArena_p = NULL;
        kaps_jr1_arena *temp;
        kaps_jr1_nlm_allocator *alloc_p;
        int32_t isFirstFullyFreeArenaFound = 0;

        
        arena_info_p->g_arenaHead_p = NULL;

        while (curArena_p)
        {
            temp = curArena_p->m_next_p;

            if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes && isFirstFullyFreeArenaFound)
            {
                if (prevArena_p)
                    prevArena_p->m_next_p = curArena_p->m_next_p;

                alloc_p = curArena_p->m_alloc_p;

                alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p->m_arenaStart_p);
                alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p);

                --arena_info_p->g_numCompletelyFreeArena;

            }
            else
            {
                if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes)
                    isFirstFullyFreeArenaFound = 1;

                if (!arena_info_p->g_arenaHead_p)
                    arena_info_p->g_arenaHead_p = curArena_p;

                prevArena_p = curArena_p;
            }

            curArena_p = temp;
        }

    }

    return NLMERR_OK;
}

void *
kaps_jr1_arena_allocate(
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t size)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) alloc_p->m_arena_info;
    
    if (!arena_info_p || !arena_info_p->g_allowAllocFromArena || size == 0)
        return NULL;

    {
        uint32_t actual_size = NLM_ALIGN_UP(size, sizeof(char *));      
        kaps_jr1_arena *curArena_p = arena_info_p->g_arenaHead_p;
        void *result = NULL;

        
        while (curArena_p)
        {
            if (curArena_p->m_numBytesAvailable >= actual_size)
            {
                result = &curArena_p->m_arenaStart_p[curArena_p->m_curOffset];

                if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes)
                    arena_info_p->g_numCompletelyFreeArena--;
                curArena_p->m_numBytesAvailable -= actual_size;
                curArena_p->m_curOffset += actual_size;
                curArena_p->m_numAllocedChunks++;

                arena_info_p->g_numAllocedChunksFromAllArenas++;

                break;
            }
            curArena_p = curArena_p->m_next_p;
        }

        return result;
    }

}

int32_t
kaps_jr1_arena_free_if_arena_memory(
    kaps_jr1_nlm_allocator * alloc_p,
    void *ptr)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) alloc_p->m_arena_info;
    
    if (!arena_info_p || !arena_info_p->g_numAllocedChunksFromAllArenas || !ptr)
        return 0;

    {
        kaps_jr1_arena *curArena_p = arena_info_p->g_arenaHead_p;
        int32_t result = 0;

        while (curArena_p)
        {
            if (ptr >= (void *) curArena_p->m_arenaStart_p && ptr <= (void *) curArena_p->m_arenaEnd_p)
            {
                
                result = 1;

                if (curArena_p->m_numAllocedChunks)
                    curArena_p->m_numAllocedChunks--;

                if (arena_info_p->g_numAllocedChunksFromAllArenas)
                    arena_info_p->g_numAllocedChunksFromAllArenas--;

                if (curArena_p->m_numAllocedChunks == 0)
                {
                    
                    curArena_p->m_curOffset = 0;
                    curArena_p->m_numBytesAvailable = arena_info_p->g_arenaSizeInBytes;
                    arena_info_p->g_numCompletelyFreeArena++;
                }
                break;
            }
            curArena_p = curArena_p->m_next_p;
        }

        return result;
    }
}

void
kaps_jr1_arena_destroy(
    kaps_jr1_nlm_allocator * alloc)
{
    struct kaps_jr1_arena_info *arena_info_p = (struct kaps_jr1_arena_info *) alloc->m_arena_info;
    kaps_jr1_arena *curArena_p = arena_info_p->g_arenaHead_p;
    kaps_jr1_arena *temp;
    kaps_jr1_nlm_allocator *alloc_p = alloc;

    
    while (curArena_p)
    {
        temp = curArena_p->m_next_p;
        alloc_p = curArena_p->m_alloc_p;

        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p->m_arenaStart_p);
        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p);

        curArena_p = temp;
    }
    alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, alloc_p->m_arena_info);
    alloc_p->m_arena_info = NULL;
}
