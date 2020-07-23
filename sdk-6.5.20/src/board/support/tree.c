/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tree.c
 * Purpose:     board tree based programming
 */


#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>
#include <board/board.h>
#include <board/manager.h>
#include <board_int/support.h>


/* Tree programmer

   Based on a connection list, the Tree Programmer creates a tree of
   devices, with each device represented by a vertex, and each
   connection between devices represented by an edge (actually two
   edges). It then then traverses the tree, calling an 'edge
   programmer' function for each edge in the tree, and a 'vertex
   programmer' function for each vertex (more or less). Typically, an
   edge programmer will create trunks between devices when necessary,
   and a vertex programmer will program modport mapping.

   This is intended to work for both local board programming and stack
   board programming. Although the algorithm is the same, programming
   requirements are somewhat different between the local and stack
   case, so the API programming is abstracted out of this function.
   The only BCM API this subsystem calls is bcm_unit_local().

   Returns BCM_E_CONFIG if the connection list does not constitute a
   mathematical tree.

   It may be a future enhancement to accept a general graph of devices
   when a spanning tree or other loop pruning algorithm is introduced.

   Algorithm:

     Each unit is a vertex in a tree.  Each connection adds two edges,
     one for each direction connecting two vertexes.  Each edge has
     vertex endpoints that are either the same as an existing edge,
     where the edge data populates a trunk, or becomes a new edge of
     the graph.

     Once all the edges are added, the resulting graph is checked to
     see if it is a mathematical tree. If it isn't then return
     failure.  Otherwise, traverse all the vertexes in the tree. If
     the vertex is local, iterate across all edges of the vertex. Call
     the edge programmer for the edge that was found.  Next, traverse
     the subtree from the destination vertex of the edge, calling the
     vertex programmer for each vertex in the subtree being traversed,
     along with the parent vertex of the subtree.
     
     Add all connections
     BCM_E_CONFIG if graph is not a tree
     for each vertex
       if vertex is local
         for each edge in vertex
           program edge
           traverse_subtree(vertex.destination) {
             program vertex
           }
         end
       end
     end

*/

struct vertex_s;

typedef struct edge_s {
    bcm_gport_t src;            /* source port */
    bcm_gport_t dst;            /* destination port */
    struct vertex_s *vertex;    /* parent vertex */
    bcm_trunk_add_info_t trunk; /* trunk */
    struct edge_s *next;        /* next edge */
} edge_t;

typedef struct vertex_s {
    int unit;                   /* unit number */
    int modid;                  /* device modid */
    int visited;                /* visited? for traversals */
    edge_t *edge;               /* edges */
    bcm_trunk_chip_info_t ti;   /* trunk info */
} vertex_t;

typedef struct tree_s {
    board_tree_driver_t *driver;
    int num;                    /* unit number */
    vertex_t *vertex;           /* vertex, indexed by unit */
} tree_t;

typedef int (*vertex_cb)(tree_t *tree, vertex_t *vertex, void *user_data);
typedef int (*tree_cb)(tree_t *tree, vertex_t *vertex, void *user_data);

/* return TRUE is the vertex is valid */
#define VERTEX_VALID(v) (((v) != NULL) && ((v)->unit >= 0))

/* shorthand because it's used a lot */
#define UNIT BCM_GPORT_DEVPORT_DEVID_GET


#define MAX(a,b) (((a)>(b))?(a):(b))


/*
 * Function:
 *     _trunk_add
 * Purpose:
 *     Add trunk member to edge trunk
 * Parameters:
 *     e      - (INOUT)
 *     member - (IN)
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_trunk_add(edge_t *e, bcm_gport_t member)
{
    int rv = BCM_E_FULL;

    if (e->trunk.num_ports < BCM_TRUNK_MAX_PORTCNT) {
        e->trunk.tp[e->trunk.num_ports] = member;
        e->trunk.tm[e->trunk.num_ports] = -1;
        e->trunk.num_ports++;
        rv = BCM_E_NONE;
    }

    return rv;
}


/*
 * Function:
 *     _edge_add
 * Purpose:
 *     Add an edge, identified by [src-dst], where src is a port
 *     associated with the vertex, returning the edge in edgep.
 * Parameters:
 *     edgep  - (OUT) edge output
 *     vertex - (IN)  vertex of src 
 *     src    - (IN)  edge src port
 *     dst    - (IN)  edge dst port
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_edge_add(edge_t **edgep, vertex_t *vertex, bcm_gport_t src, bcm_gport_t dst)
{
    int rv;
    edge_t *edge, *prev;

    edge=*edgep;
    prev=NULL;
    for (; edge != NULL; edge=edge->next) {
        if (edge->src == src) {
            return BCM_E_CONFIG;
        }
        if (UNIT(edge->dst) == UNIT(dst)) {
            return _trunk_add(edge, src);
        }
        prev=edge;
    }

    /* create a new edge */
    edge = ALLOC(sizeof(*edge));
    if (edge == NULL) {
        return BCM_E_MEMORY;
    }

    edge->src = src;
    edge->dst = dst;
    edge->vertex = vertex;
    bcm_trunk_add_info_t_init(&edge->trunk);
    edge->next = NULL;
    rv = _trunk_add(edge, src);
    if (BCM_SUCCESS(rv)) {
        if (prev) {
            /* add to edge list */
            prev->next = edge;
        } else {
            /* first edge in vertex */
            *edgep = edge;
        }
    } else {
        FREE(edge);
    }

    return rv;
}

/*
 * Function:
 *     _vertex_add
 * Purpose:
 *     Add a vertex to a tree, calling out to the info function
 * Parameters:
 *     t      - (IN)    tree
 *     vertex - (INOUT) vertex
 *     unit   - (IN)    unit ID for vertex
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_vertex_add(tree_t *t, vertex_t *vertex, int unit)
{
    int rv;

    vertex->unit = unit;
    vertex->visited = FALSE;
    vertex->edge = NULL;
    rv = t->driver->info(unit, &vertex->modid, &vertex->ti,
                         t->driver->user_data);

    return rv;
}

/*
 * Function:
 *     _connection_add
 * Purpose:
 *     Add the given simplex connection to the tree
 * Parameters:
 *     t      - (IN) tree
 *     src    - (IN) connection src port
 *     dst    - (IN) connection dst port
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_connection_add(tree_t *t, bcm_gport_t src, bcm_gport_t dst)
{
    int src_u, rv;
    vertex_t *vertex;
    
    src_u = UNIT(src);
    vertex = &t->vertex[src_u];
    rv = BCM_E_NONE;
    if (!VERTEX_VALID(vertex)) {
        rv = _vertex_add(t, vertex, src_u);
        if (t->num < src_u) {
            t->num = src_u+1;
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = _edge_add(&vertex->edge, vertex, src, dst);
    }


    return rv;
    
}

/*
 * Function:
 *     _visit
 * Purpose:
 *     Visit each vertex of the tree that has not already been visited
 *     breadth first starting with the given vertex
 * Parameters:
 *     t         - (IN) tree
 *     v         - (IN) starting vertex
 *     cb        - (IN) vertex callback
 *     user_data - (IN)
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_visit(tree_t *t, vertex_t *v, vertex_cb cb, void *user_data)
{
    edge_t *edge;
    int vtid;

    v->visited = TRUE;
    BCM_IF_ERROR_RETURN(cb(t,v,user_data));
    for (edge = v->edge; edge != NULL; edge=edge->next) {
        vtid = UNIT(edge->dst);
        if (!t->vertex[vtid].visited) {
            BCM_IF_ERROR_RETURN(_visit(t,&t->vertex[vtid],cb,user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _reset_visited
 * Purpose:
 *     Reset all visited flags in the tree
 * Parameters:
 *     t         - (IN) tree
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC void
_reset_visited(tree_t *t)
{
    int i;

    for (i=0; i<t->num; i++) {
        t->vertex[i].visited = FALSE;
    }
}

/*
 * Function:
 *     _traverse_subtree_from
 * Purpose:
 *     Traverse the subtree rooted at 'from', avoiding traversals
 *     though vertex 'parent'.
 * Parameters:
 *     t         - (IN) tree
 *     parent    - (IN) subtree parent
 *     from      - (IN) subtree root
 *     cb        - (IN) vertex callback
 *     user_data - (IN) vertex data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_traverse_subtree_from(tree_t *t, vertex_t *parent, vertex_t *from, 
                       vertex_cb cb, void *user_data)
{
    _reset_visited(t);
    if (parent) {
        parent->visited = TRUE;
    }
    return _visit(t, from, cb, user_data);
}

/*
 * Function:
 *     _count_edges
 * Purpose:
 *     Returns the number of directed edges in the tree
 * Parameters:
 *     t - (IN) tree
 * Returns:
 *     number of directed edges
 */
STATIC int
_count_edges(tree_t *t)
{
    int edge_count, i;
    edge_t *edge;
    vertex_t *vertex;

    /* Count the number of edges */
    edge_count = 0;
    for (i=0; i<t->num; i++) {
        vertex = &t->vertex[i];
        if (!VERTEX_VALID(vertex)) {
            continue;
        }
        for (edge = vertex->edge; edge != NULL; edge=edge->next) {
            edge_count++;
        }
    }

    return edge_count;
}

/*
 * Function:
 *     _tree_p
 * Purpose:
 *     Return TRUE if the given graph if a mathematical tree
 * Parameters:
 *     t - (IN) tree
 * Returns:
 *     boolean
 */
STATIC int
_tree_p(tree_t *t)
{
    int edge_count;

    /* Count the number of edges */
    edge_count = _count_edges(t);

    /* Must have an even number of directed edges, and the number of
       undirected edges must be one greater than the numver of
       verticies */
    return (((edge_count & 1) == 0) && (((edge_count/2)+1) == t->num));
}

/*
 * Function:
 *     _tree_connection_add
 * Purpose:
 *     Add duplex connection to tree as two simplex edges
 * Parameters:
 *     t           - (IN) tree
 *     connection  - (IN) connection
 * Returns:
 */
STATIC int
_tree_connection_add(tree_t *t, board_connection_t *connection)
{
    BCM_IF_ERROR_RETURN(_connection_add(t, connection->from, connection->to));
    BCM_IF_ERROR_RETURN(_connection_add(t, connection->to, connection->from));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tree_init
 * Purpose:
 *     Initialize tree
 * Parameters:
 *     t      - (IN) tree
 *     driver - (IN) tree driver
 * Returns:
 *     TRUE - tree was created
 *     BCM_E_NONE - connection does not represent a tree
 *     BCM_E_XXX - failed
 */
STATIC int
_tree_init(tree_t *t, board_tree_driver_t *driver)
{
    int i, internal, size;
    
    /* See if there's anything to connect, and calculate the size of
       the vertex array. */
    internal=0;
    for (i=0; i<driver->num_connection; i++) {
        if (driver->connection[i].from != BCM_GPORT_TYPE_NONE &&
            driver->connection[i].to != BCM_GPORT_TYPE_NONE) {
            internal = MAX(internal,UNIT(driver->connection[i].from)+1);
            internal = MAX(internal,UNIT(driver->connection[i].to)+1);
        }
    }

    if (internal == 0) {
        /* Nothing to connect */
        return BCM_E_EMPTY;
    }

    sal_memset(t, 0, sizeof(*t));
    t->driver = driver;
    
    size = internal * sizeof(vertex_t);
    t->vertex = ALLOC(size);
    if (t->vertex == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(t->vertex, 0, size);


    /* Mark all vertexes invalid */
    for (i=0; i<internal; i++) {
        t->vertex[i].unit=-1;
    }

    /* Add connections */
    for (i=0; i<driver->num_connection; i++) {
        if ((driver->connection[i].from != BCM_GPORT_TYPE_NONE) &&
            (driver->connection[i].to != BCM_GPORT_TYPE_NONE)) {
            _tree_connection_add(t, &driver->connection[i]);
        }
    }

    return _tree_p(t);
}

/*
 * Function:
 *     _tree_free
 * Purpose:
 *     Free allocated memory from tree
 * Parameters:
 *     t - (IN) tree
 * Returns:
 *     void
 */
STATIC void
_tree_free(tree_t *t)
{
    int i;
    edge_t *edge, *next;
    vertex_t *vertex;

    for (i=0; i<t->num; i++) {
        vertex = &t->vertex[i];
        if (!VERTEX_VALID(vertex)) {
            continue;
        }
        
        for (edge = vertex->edge; edge != NULL; edge=next) {
            next=edge->next;
            FREE(edge);
        }
    }
    FREE(t->vertex);
}

/*
 * Function:
 *     _program_edge
 * Purpose:
 *     Call edge programmer for given vertex and edge
 * Parameters:
 *     t      - (IN) tree
 *     vertex - (IN) vertex
 *     edge   - (IN) edge
 * Returns:
 */
STATIC int
_program_edge(tree_t *t, vertex_t *vertex, edge_t *edge)
{
    return t->driver->edge(vertex->unit, &vertex->ti,
                           &edge->trunk, t->driver->user_data);
}

/*
 * Function:
 *     _program_vertex
 * Purpose:
 *     Call vertex programmer for given vertex
 * Parameters:
 *     t         - (IN) tree
 *     vertex    - (IN) vertex
 *     user_data - (IN) vertex callback user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_program_vertex(tree_t *t, vertex_t *dst, void *user_data)
{
    edge_t *edge = (edge_t *)user_data;
    vertex_t *src = edge->vertex;

    return t->driver->vertex(src->unit, dst->modid, edge->src,
                             t->driver->user_data);
}

/*
 * Function:
 *     _board_connect_vertex
 * Purpose:
 *     Tree programmer tree traversal callback
 * Parameters:
 *     t         - (IN) tree
 *     vertex    - (IN) vertex
 *     user_data - (IN) vertex callback user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_connect_vertex(tree_t *t, vertex_t *vertex, void *user_data)
{
    int rv = BCM_E_INTERNAL; /* It's expected that there's at least
                                one edge per vertex. */
    edge_t *edge;
    vertex_t *to;


    /* Cannot use bcm_unit_remote, because the unit numbers will not
       likely be attached for slave systems, and bcm_unit_remote will
       return false. */
    if (!bcm_unit_local(vertex->unit)) {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META("Board programming for remote unit %d not required.\n"),
                     vertex->unit));
        return BCM_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                (BSL_META("Board programming unit %d\n"),
                 vertex->unit));
    for (edge = vertex->edge; edge != NULL; edge = edge->next) {

        /* only local units require edge programming */
        rv = _program_edge(t, vertex, edge);
        if (BCM_FAILURE(rv)) {
            break;
        }

        to = &t->vertex[UNIT(edge->dst)];
        rv = _traverse_subtree_from(t, vertex, to,
                                    _program_vertex, (void *)edge);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META("Board programming unit %d failed (%d)\n"),
                     vertex->unit, rv));
    }
    return rv;
}

/*
 * Function:
 *     _tree_traverse
 * Purpose:
 *     Breadth first tree traversal
 * Parameters:
 *     t         - (IN) tree
 *     cb        - (IN) vertex callback
 *     user_data - (IN) callback user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_tree_traverse(tree_t *tree, tree_cb cb, void *user_data)
{
    int i,rv;
    vertex_t *vertex;

    rv = BCM_E_NONE;

    for (i=0; i<tree->num; i++) {
        vertex = &tree->vertex[i];
        if (!VERTEX_VALID(vertex)) {
            continue;
        }
        rv = cb(tree, vertex, user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_tree_connect
 * Purpose:
 *     Tree based board connection algorithm
 * Parameters:
 *     tree_driver - (IN) connection algorithm
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_tree_connect(board_tree_driver_t *tree_driver)
{
    int rv;
    tree_t tree;
    
    rv = _tree_init(&tree, tree_driver);
    if (BCM_SUCCESS(rv)) {
        _tree_traverse(&tree, _board_connect_vertex, NULL);
        _tree_free(&tree);
    } else if (rv == BCM_E_EMPTY) {
        /* Nothing in the tree, so there's nothing to do */
        rv = BCM_E_NONE;
    }

    return rv;
}
