 /********************
 *  RBTree.c
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#include "HeapAllocation.h"
#include "RedBlackTree.h"

#define isLeafNode(x)    ( ((x)->item.pKey == NULL) )
#define isRootNode(T, x) ( (x)->parent == (T)->nil )
#define isLeftNode(x)    ( (x) == (x)->parent->left )

#define BLACK_NODE 0
#define RED_NODE 1

#define RotateRight(T, x, y) \
    do { \
        if ((x)->right) \
            (x)->right->parent = y; \
\
        (y)->left = (x)->right; \
        (x)->right = y; \
        (x)->parent = (y)->parent; \
\
        if ((y)->parent != (T)->nil) { \
            if ((y) == (y)->parent->left) \
                (y)->parent->left = x; \
            else \
                (y)->parent->right = x; \
        } \
\
        (y)->parent = x; \
    } while (0)

#define RotateLeft(T, x, y) \
    do { \
        if ((x)->left) \
            (x)->left->parent = y; \
\
        (y)->right = (x)->left; \
        (x)->left = y; \
        (x)->parent = (y)->parent; \
\
        if ((y)->parent != (T)->nil) { \
            if (y == (y)->parent->left) \
                (y)->parent->left = x; \
            else \
                (y)->parent->right = x; \
        } \
\
        (y)->parent = x; \
    } while (0)



static void pre_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback);
static void in_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback);
static void post_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback);
static int breadth_firstTraversal(RBTreeNode *rbtRoot, vdsTraverseFunc callback);
static void eulerTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback);

static void rebalance_insertion(RBTree *rbt, RBTreeNode *curr);
static void rebalance_deletion(RBTree *rbt, RBTreeNode *curr);


RBTree *RBTree_init(vdsUserCompareFunc KeyCmp,
                          vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    RBTree *rbt = NULL;

    if (KeyCmp) {
        rbt = VdsMalloc(sizeof(RBTree));

        if (rbt) {
            rbt->nil = VdsMalloc(sizeof(RBTreeNode));

            if (rbt->nil) {
                rbt->root = NULL;
                //create the sentinel node
                //its data isn't important but its color is black
                rbt->nil->color = BLACK_NODE; //this is needed in fixup functions
                rbt->KeyCmp = KeyCmp;
            } else {
                VdsFree(rbt);
                rbt = NULL;
                tmp_err = VDS_MALLOC_FAIL;
            }

        } else
            tmp_err = VDS_MALLOC_FAIL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return rbt;
}

RBTreeNode *RBTree_insert(RBTree *rbt,
                                void *pKey,
                                void *pData,
                                vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    RBTreeNode *new_node = NULL;

    if (rbt && pKey) {

        new_node = VdsMalloc(sizeof(RBTreeNode));

        if (new_node) {

            new_node->item.pData = pData;
            new_node->item.pKey = pKey;

            new_node->right = new_node->left = rbt->nil;

            if (!rbt->root) {
                new_node->parent = rbt->nil;
                new_node->color = BLACK_NODE;
                rbt->root = new_node;
            } else {
                RBTreeNode *curr = rbt->root, *parent = rbt->nil;
                int cmp_res;

                new_node->color = RED_NODE; //first we paint the node red
                while (1) {
                    cmp_res = rbt->KeyCmp(pKey, curr->item.pKey);

                    if (cmp_res > 0) {

                        parent = curr;

                        curr = curr->right;

                        if (curr == rbt->nil) {
                            new_node->parent = parent;
                            parent->right = new_node;
                            break;
                        }

                    } else if (cmp_res < 0) {

                        parent = curr;

                        curr = curr->left;

                        if (curr == rbt->nil) {
                            new_node->parent = parent;
                            parent->left = new_node;
                            break;
                        }

                    } else { //if there's another node with the same key already on the tree
                        VdsFree(new_node); //return without doing anything
                        new_node = NULL;
                        tmp_err = VDS_KEY_EXISTS;
                        break;
                    }

                }

                rebalance_insertion(rbt, new_node);

            }
        } else
            tmp_err = VDS_MALLOC_FAIL;

    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

void rebalance_insertion(RBTree *rbt, RBTreeNode *curr)
{
    if (rbt && curr) {
        RBTreeNode *rbtTmp;

        while (curr->parent->color == RED_NODE) {

            //rbtTmp will point to the uncle node (or nil) after this line
            rbtTmp = (isLeftNode(curr->parent)) ? curr->parent->parent->right : curr->parent->parent->left;

            //1st case: if the uncle is black (nil) then we perform a rotation
            if (rbtTmp->color == BLACK_NODE) {

                //if the uncle is a left node
                if (!isLeftNode(curr->parent)) {

                    if (isLeftNode(curr)) {
                        /******************************
                            (B = curr->parent->parent)
                                  /            \
                             (B = nil)       (R = curr->parent)
                                               /
                                           (R = curr)
                        ***************************************/
                        rbtTmp = curr->parent;
                        RotateRight(rbt, curr, rbtTmp);
                        /******************************
                                (B = curr->parent)
                                  /          \
                             (B = nil)     (R = curr)
                                               \
                                             (R = curr->right)
                        ***************************************/
                    } else {
                        /******************************
                            (B = curr->parent->parent)
                                  /          \
                             (B = nil)     (R = curr->parent)
                                               \
                                             (R = curr)
                        ***************************************/
                        curr = curr->parent;
                        /******************************
                               (B = curr->parent)
                                  /          \
                             (B = nil)     (R = curr)
                                               \
                                             (R = curr->right)
                        ***************************************/
                    }

                    rbtTmp = curr->parent;
                    RotateLeft(rbt, curr, rbtTmp);
                    curr->color = BLACK_NODE;
                    curr->left->color = RED_NODE;

                    if (curr->left == rbt->root)
                        rbt->root = curr;

                    /******************************
                                (B = curr)
                                /        \
                              (R)        (R)
                    *******************************/
                } else {

                    if (!isLeftNode(curr)) {
                        /******************************
                             (B = curr->parent->parent)
                               /                   \
                           (R = curr->parent)    (B = nil)
                               \
                             (R = curr)
                        ***************************************/
                        rbtTmp = curr->parent;
                        RotateLeft(rbt, curr, rbtTmp);
                        /******************************
                              (B = curr->parent)
                               /              \
                           (R = curr)       (B = nil)
                             /
                         (R = curr->left)
                        ***************************************/
                    } else {
                        /******************************
                             (B = curr->parent->parent)
                               /                  \
                           (R = curr->parent)   (B = nil)
                             /
                         (R = curr)
                        ***************************************/
                        curr = curr->parent;
                        /******************************
                              (B = curr->parent)
                               /               \
                           (R = curr)        (B = nil)
                             /
                         (R = curr->left)
                        ***************************************/
                    }

                    rbtTmp = curr->parent;
                    RotateRight(rbt, curr, rbtTmp);
                    curr->color = BLACK_NODE;
                    curr->right->color = RED_NODE;

                    if (curr->right == rbt->root)
                        rbt->root = curr;

                    /******************************
                                (B = curr)
                                /        \
                              (R)        (R)
                    *******************************/
                }

                break;

            //2nd case: if the uncle is red then we perform recoloring and keep going up
            } else {

                /******************************
                    (B = curr->parent->parent)
                       /                 \
                   (R = curr->parent)    (R)
                     /
                 (R = curr)

                           ===or===

                    (B = curr->parent->parent)
                       /                 \
                   (R = curr->parent)    (R)
                       \
                     (R = curr)

                           ===or===

                    (B = curr->parent->parent)
                       /            \
                     (R)          (R = curr->parent)
                                      \
                                    (R = curr)

                           ===or===

                    (B = curr->parent->parent)
                       /            \
                     (R)          (R = curr->parent)
                                    /
                                (R = curr)
                ***************************************/
                curr->parent->color = rbtTmp->color = BLACK_NODE;
                curr->parent->parent->color = RED_NODE;
                curr = curr->parent->parent;

            }

        }

        rbt->root->color = BLACK_NODE;

    }
}

KVPair RBTree_deleteNode(RBTree *rbt,
                         RBTreeNode *rbtToDelete,
                         vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    KVPair item = { 0 };

    if (rbt && rbt->root && rbtToDelete) {

        //null the key of the sentinel node, to use on conditionals
        rbt->nil->item.pKey = NULL;

        //if the node we want to delete has two children nodes
        //we switch it with the first leftmost leaf node from the right subtree
        if (rbtToDelete->right->item.pKey && rbtToDelete->left->item.pKey) {
            //temporary value to store the data that is being swapped
            KVPair swapped_item;
            RBTreeNode *rbtSuccessor = rbtToDelete->right;

            while (rbtSuccessor->left->item.pKey)
                rbtSuccessor = rbtSuccessor->left;

            //swap the data (key and value) of the two nodes
            swapped_item = rbtToDelete->item;
            rbtToDelete->item = rbtSuccessor->item;
            rbtSuccessor->item = swapped_item;

            rbtToDelete = rbtSuccessor;
        }

        //now the node we want to delete has AT MOST one child node

        //we save the parent and the child of the node that is about to be deleted
        RBTreeNode *parent = rbtToDelete->parent, *child;

        //if rbtToDelete has no children, child will point to nil and will be a black node, after this line
        child = (rbtToDelete->right->item.pKey) ? (rbtToDelete->right) : (rbtToDelete->left);

        //if the node we want to delete ISN'T the root node
        if (parent->item.pKey) {
            //if the node we want to delete is a right node, then the right child
            //of its parent has to point to the valid child of rbtToDelete. If there's
            //no valid child available, then parent will point to nil instead.
            if ( !isLeftNode(rbtToDelete) ) {
                //we change the right node of the parent so
                //that it points to either the right node of the node we
                //want to delete, or the left node, depending on which one
                //is not nil (if both are nil then we point to nil)
                parent->right = child;
            } else { //if the key of the parent is smaller, it's a left node
                parent->left = child;
            }
        //else if the node we want to delete IS the root node
        } else {
            //change the tree root accordingly, so that it points to the new root (or to nil, if it's the only node in the tree)
            rbt->root = (rbtToDelete->right->item.pKey) ? rbtToDelete->right : rbtToDelete->left;
        }

        //don't forget to change the parent of the child node too
        //it doesn't matter at this point, if the child is the nil sentinel, since we'll use
        //the child's parent pointer in the fixup function.
        child->parent = parent;

        /* Balance the RBTree after the deletion */
        //if the child is a red node (and the deleted node was black), then we paint it black.
        //this is the easiest case, since we can't have two consecutive red nodes in the tree
        if (child->color == RED_NODE) {
            //we have to paint black the child node in order to preserve the rule:
            //"every path from the root to a null link has the same number of black links"
            child->color = BLACK_NODE;

        //else if both the child of the deleted node, and the deleted node itself, are black nodes
        } else if (child->color == BLACK_NODE && rbtToDelete->color == BLACK_NODE) {

            //we call the fixup function with the child of the deleted node, as argument.
            //Even if the child is the nil, nil's parent is still set to point to the node above it
            rebalance_deletion(rbt, child);
        }
        /* Balancing is over */

        //store the node's item to return it
        item = rbtToDelete->item;

        if (rbt->nil == rbt->root)
            rbt->root = NULL;

        //delete the node because we don't need it anymore
        //and no other nodes point to it
        VdsFree(rbtToDelete);

    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return item;
}

void rebalance_deletion(RBTree *rbt, RBTreeNode *curr)
{
    //this loop will execute as long as curr is black and isn't root
    //or until the red-black condition is satisfied
    while (curr->color == BLACK_NODE && curr != rbt->root) {
        RBTreeNode *rbtTmp, *p = curr->parent;

        //rbtTmp holds the sibling of our node, after this line
        rbtTmp = (isLeftNode(curr)) ? p->right : p->left;

        //if sibling is black
        if (rbtTmp->color == BLACK_NODE) {


            //if the sibling has two black children
            if (rbtTmp->left->color == BLACK_NODE && rbtTmp->right->color == BLACK_NODE) {

                //if the parent of rbtTmp and curr, is red
                if (p->color == RED_NODE) {
                    //recolor the parent
                    p->color = BLACK_NODE;
                    //and the sibling
                    rbtTmp->color = RED_NODE;
                    //and double black is gone
                    break;
                } else {
                    //else recolor the sibling and keep going up from the parent
                    rbtTmp->color = RED_NODE;
                    curr = p;
                    continue;
                }

            }

            //if the sibling has at least one red child
            //if it's a right node
            if (!isLeftNode(rbtTmp)) {

                if (rbtTmp->right->color == RED_NODE) {
                    p->color = rbtTmp->right->color = BLACK_NODE;
                    rbtTmp->color = RED_NODE;
                    RotateLeft(rbt, rbtTmp, p);
                    if (p == rbt->root) {
                        rbtTmp->color = BLACK_NODE;
                        rbt->root = rbtTmp;
                    }
                } else {
                    //using p to store temporarily the left child, in order to perform the rotations
                    p = rbtTmp->left;
                    p->color = BLACK_NODE;
                    RotateRight(rbt, p, rbtTmp);

                    rbtTmp = p->parent;
                    RotateLeft(rbt, p, rbtTmp);
                    p->color = RED_NODE;
                    p->left->color = BLACK_NODE;
                    p->right->color = BLACK_NODE;
                    if (rbtTmp == rbt->root) {
                        p->color = BLACK_NODE;
                        rbt->root = p;
                    }
                }

            //if it's a left node
            } else {

                if (rbtTmp->left->color == RED_NODE) {
                    p->color = rbtTmp->left->color = BLACK_NODE;
                    rbtTmp->color = RED_NODE;
                    RotateRight(rbt, rbtTmp, p);
                    if (p == rbt->root) {
                        rbtTmp->color = BLACK_NODE;
                        rbt->root = rbtTmp;
                    }
                } else {
                    //using p to store temporarily the left child, in order to perform the rotations
                    p = rbtTmp->right;
                    RotateLeft(rbt, p, rbtTmp);

                    rbtTmp = p->parent;
                    RotateRight(rbt, p, rbtTmp);
                    p->color = RED_NODE;
                    p->left->color = BLACK_NODE;
                    p->right->color = BLACK_NODE;
                    if (rbtTmp == rbt->root) {
                        p->color = BLACK_NODE;
                        rbt->root = p;
                    }
                }
            }

            break;
        //if sibling is red
        } else {
            //flip the sibling's color
            rbtTmp->color = BLACK_NODE;
            p->color = RED_NODE;

            if (!isLeftNode(rbtTmp)) {
                RotateLeft(rbt, rbtTmp, p);
                //rbtTmp = rbtTmp->right;
            } else {
                RotateRight(rbt, rbtTmp, p);
                //rbtTmp = rbtTmp->left;
            }

            if (p == rbt->root) {
                rbt->root = rbtTmp;
            }
        }
    }
}

void *RBTree_deleteByKey(RBTree *rbt,
                         void *pKey,
                         vdsErrCode *err)
{
    RBTreeNode *to_delete = RBTree_findNode(rbt, pKey, err);
    void *deleted = NULL;

    if (to_delete) {
        KVPair tmp = RBTree_deleteNode(rbt, to_delete, err);
        deleted = tmp.pData;
    }

    return deleted;
}

RBTreeNode *RBTree_findNode(RBTree *rbt,
                            void *pKey,
                            vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    RBTreeNode *curr = NULL;

    if (rbt && rbt->root && pKey) {
        int cmp_res;

        rbt->nil->item.pKey = NULL;
        curr = rbt->root;

        while (curr->item.pKey) {
            cmp_res = rbt->KeyCmp(pKey, curr->item.pKey);

            if (!cmp_res)
                break;

            if (cmp_res > 0)
                curr = curr->right;
            else /*if (cmp_res < 0)*/
                curr = curr->left;
        }

        //if curr points to the sentinel, then it means that the
        //search has failed and we have to return NULL
        if (!curr->item.pKey)
            curr = NULL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return curr;
}

void *RBTree_findData(RBTree *rbt,
                      void *pKey,
                      vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    RBTreeNode *curr = NULL;

    if (rbt && rbt->root && pKey) {
        int cmp_res;

        rbt->nil->item.pKey = NULL;
        curr = rbt->root;

        while (curr->item.pKey) {
            cmp_res = rbt->KeyCmp(pKey, curr->item.pKey);

            if (!cmp_res)
                break;

            if (cmp_res > 0)
                curr = curr->right;
            else /*if (cmp_res < 0)*/
                curr = curr->left;
        }

        //if curr points to the sentinel, then it means that the
        //search has failed and we have to return NULL
        if (!curr->item.pKey)
            curr = NULL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return (curr) ? curr->item.pData : NULL;
}
void RBTree_traverse(RBTree *rbt,
                     vdsTreeTraversal traversal,
                     vdsTraverseFunc callback,
                     vdsErrCode *err)

{
    vdsErrCode tmp_err = VDS_SUCCESS;

    if (callback && rbt && rbt->root) {
        //To make recursion work just by passing two arguments on each traversal function, I'll
        //nullify the key of the nil sentinel and check against that on each recursive call.
        rbt->nil->item.pKey = NULL;

        switch (traversal) {
        case VDS_PRE_ORDER:
            pre_orderTraversal(rbt->root, callback);
            break;
        case VDS_IN_ORDER:
            in_orderTraversal(rbt->root, callback);
            break;
        case VDS_POST_ORDER:
            post_orderTraversal(rbt->root, callback);
            break;
        case VDS_BREADTH_FIRST:
            if (!breadth_firstTraversal(rbt->root, callback))
                tmp_err = VDS_MALLOC_FAIL;
            break;
        case VDS_EULER:
            eulerTraversal(rbt->root, callback);
            break;
        default:
            break;
        }
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}

void RBTree_destroy(RBTree **rbt,
                    vdsUserDataFunc freeData,
                    vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;

    if (rbt && *rbt) {

        RBTreeNode *curr = (*rbt)->root, *to_delete;

        (*rbt)->nil->item.pKey = NULL;

        while (curr->item.pKey) {
            if (curr->left->item.pKey) {

                curr = curr->left;

            } else if (curr->right->item.pKey) {

                curr = curr->right;

            } else {

                //if we're here we want to delete AND nil the to_delete node
                to_delete = curr;
                //we make curr the parent
                curr = curr->parent;

                if (freeData)
                    freeData((void *)&to_delete->item);

                if (curr->item.pKey) {

                    if (curr->right == to_delete) {
                        VdsFree(curr->right);
                        curr->right = (*rbt)->nil;
                    } else {
                        VdsFree(curr->left);
                        curr->left = (*rbt)->nil;
                    }

                } else { //if curr is nil, it means that to_delete holds the root node
                    VdsFree(to_delete);
                }
            }
        }

        VdsFree((*rbt)->nil);
        VdsFree(*rbt);
        *rbt = NULL;

    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}


//library internal functions to traverse binary tree data structures
//no error checking required

#include "FIFOqueue.h"

void pre_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback)
{
    if (rbtNode->item.pKey) {
        if (!callback((void *)&rbtNode->item)) return;
        pre_orderTraversal(rbtNode->left, callback);
        pre_orderTraversal(rbtNode->right, callback);
    }
}

void in_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback)
{
    if (rbtNode->item.pKey) {
        in_orderTraversal(rbtNode->left, callback);
        if (!callback((void *)&rbtNode->item)) return;
        in_orderTraversal(rbtNode->right, callback);
    }
}

void post_orderTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback)
{
    if (rbtNode->item.pKey) {
        post_orderTraversal(rbtNode->left, callback);
        post_orderTraversal(rbtNode->right, callback);
        if (!callback((void *)&rbtNode->item)) return;
    }
}

int breadth_firstTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback)
{
    vdsErrCode err;
    RBTreeNode *curr;
    FIFOqueue *levelFIFO = FIFO_init(&err);

    if (levelFIFO) {
        FIFO_enqueue(levelFIFO, (void *)rbtNode, &err);

        if (err == VDS_SUCCESS) {

            while (levelFIFO->total_nodes) {
                curr = (RBTreeNode *)FIFO_dequeue(levelFIFO, NULL);

                if (!callback((void *)&curr->item)) break;

                if (curr->right) {
                    FIFO_enqueue(levelFIFO, curr->right, &err);

                    if (err != VDS_SUCCESS)
                        break;
                }

                if (curr->left) {
                    FIFO_enqueue(levelFIFO, curr->left, &err);

                    if (err != VDS_SUCCESS)
                        break;
                }
            }

            FIFO_destroy(&levelFIFO, NULL, NULL);

        }
    }

    if (err == VDS_SUCCESS)
        return 1;

    return 0;
}

void eulerTraversal(RBTreeNode *rbtNode, vdsTraverseFunc callback)
{
    if (rbtNode->item.pKey) {
        if (!callback((void *)&rbtNode->item)) return;

        eulerTraversal(rbtNode->left, callback);
        if (!callback((void *)&rbtNode->item)) return;
        eulerTraversal(rbtNode->right, callback);
        if (!callback((void *)&rbtNode->item)) return;
    }
}
