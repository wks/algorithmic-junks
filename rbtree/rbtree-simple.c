#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define _RBTREE_DEBUG_

#ifdef _RBTREE_DEBUG_
#define trace printf
#define debug_only(x) x
#define debug_print_tree print_tree
#define debug_print_subtree print_subtree
#else
#define trace
#define debug_only
#define debug_print_tree
#define debug_print_subtree
#endif

typedef int value_t;

enum rbnode_color { BLACK, RED };

struct rbnode {
    value_t value;
    enum rbnode_color color;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
};

typedef struct rbnode rbnode_t;

struct rbtree {
    struct rbnode *root;
};

typedef struct rbtree rbtree_t;

void print_subtree(rbnode_t *node, int depth) {
    if (node != NULL) {
        print_subtree(node->left, depth+1);
        printf("%*s%c:%d\n", depth*4, "",
                node->color==BLACK?'B':'R',
                node->value);
        print_subtree(node->right, depth+1);
    }
}

void print_tree(rbtree_t *tree) {
    print_subtree(tree->root, 0);
}

rbnode_t *rbtree_find_node(rbtree_t *tree, value_t value) {
    rbnode_t *node = tree->root;

    while (node != NULL && value != node->value) {
        if (value < node->value) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return node;
}

void node_check(rbnode_t *node) {
    if (node != NULL) {
        if (node->left != NULL && node->left->parent != node) {
            printf("Node %d at %p 'left' link broken.\n", node->value, node);
        }
        if (node->right != NULL && node->right->parent != node) {
            printf("Node %d at %p 'right' link broken.\n", node->value, node);
        }
    }
}

rbnode_t *rbtree_rotate_left(rbtree_t *tree, rbnode_t *node) {
    rbnode_t *right = node->right;
    rbnode_t *parent = node->parent;
    assert(right != NULL);

    node->right = right->left;
    right->left = node;

    if (node->right != NULL) {
        node->right->parent = node;
    }

    right->parent = parent;
    if (parent != NULL) {
        if (parent->left == node) {
            parent->left = right;
        } else {
            parent->right = right;
        }
    } else {
        tree->root = right;
    }

    node->parent = right;

    return right;
}

rbnode_t *rbtree_rotate_right(rbtree_t *tree, rbnode_t *node) {
    rbnode_t *left = node->left;
    rbnode_t *parent = node->parent;
    assert(left != NULL);

    node->left = left->right;
    left->right = node;

    if (node->left != NULL) {
        node->left->parent = node;
    }

    left->parent = parent;
    if (parent != NULL) {
        if (parent->left == node) {
            parent->left = left;
        } else {
            parent->right = left;
        }
    } else {
        tree->root = left;
    }

    node->parent = left;

    return left;
}

void rbtree_insert_adjust(rbtree_t *tree, rbnode_t *new_node) {
    rbnode_t *cur_node = new_node;

    // adjusting
    while(1) {
        printf("rotating...\n");
        rbnode_t *parent = cur_node->parent;
        // NOTE: cur_node is always RED in the beginning of each iter
        if (parent == NULL) {
            // Just paint the root black.
            cur_node->color = BLACK;
            break;
        } else if (parent->color == BLACK) {
            // A black parent is okay to have a red child.
            break;
        } else {
            // A red parent always implies a black grandpa.
            rbnode_t *grandpa = cur_node->parent->parent; 
            rbnode_t *uncle;
            if (grandpa->left == parent) {
                uncle = grandpa->right;
            } else {
                uncle = grandpa->left;
            }

            if (uncle == NULL || uncle->color == BLACK) {
                // Rotate to balance the tree
                printf("grandpa = %p : %d\n", grandpa, grandpa->value);
                printf("parent = %p : %d\n", parent, parent->value);
                printf("cur_node = %p : %d\n", cur_node, cur_node->value);
                if (parent == grandpa->left) {
                    if (cur_node == parent->right) {
                        cur_node = parent;
                        parent = rbtree_rotate_left(tree, parent);
                        printf("----rotate-L----\n");
                        print_tree(tree);
                    }
                    rbtree_rotate_right(tree, grandpa);
                    printf("----rotate-R----\n");
                    print_tree(tree);
                } else {
                    if (cur_node == parent->left) {
                        cur_node = parent;
                        parent = rbtree_rotate_right(tree, parent);
                        printf("----rotate-R----\n");
                        print_tree(tree);
                    }
                    rbtree_rotate_left(tree, grandpa);
                    printf("----rotate-L----\n");
                    print_tree(tree);
                }

                cur_node->color = grandpa->color = RED;
                parent->color = BLACK;
                break;
            } else {
                // In the B-tree's terminology, the grandpa is "full".
                // Split the grandpa and push the grandpa upward .
                parent->color = BLACK;
                uncle->color = BLACK;
                grandpa->color = RED;
                cur_node = grandpa;
                printf("continuing...\n");
                printf("****TEST****: Before continuing:\n");
                print_tree(tree);
                printf("****TEST****: checking...:\n");
                node_check(tree->root);
                printf("****/TEST****\n");
            }
        }
    }
}

/*
 * Return value:
 * 0: normal
 * -1: node value already exist
 */
int rbtree_insert(rbtree_t *tree, rbnode_t *new_node) {
    rbnode_t **link = &tree->root;
    rbnode_t *parent = NULL;
    while (*link != NULL) {
        parent = *link;
        if (new_node->value == parent->value) {
            return -1;
        } else if (new_node->value < parent->value) {
            link = &parent->left;
        } else {
            link = &parent->right;
        }
    }

    *link = new_node;
    new_node->parent = parent;
    new_node->color = RED;
    new_node->left = new_node->right = NULL;

    printf("****TEST****: Just inserted:\n");
    print_tree(tree);
    printf("****/TEST****\n");

    rbtree_insert_adjust(tree, new_node);
}

void rbtree_delete_adjust(rbtree_t *tree, rbnode_t *node, rbnode_t *parent) {
    while ((node==NULL || node->color == BLACK) && parent != NULL) {
        trace("Adjusting. node=%p parent=%p\n", node, parent);
        if (parent->left == node) {
            trace("Node on the left.\n");
            rbnode_t *brother = parent->right;

            // Make sure the current node's brother is also black
            if (brother->color == RED) {
                trace("Brother %d@%p is red. Rotating...\n", brother->value, brother);
                brother->color = BLACK;
                parent->color = RED;
                rbtree_rotate_left(tree, parent);
                brother = parent->right;
                trace("New brother %p\n", brother);
                printf("------after rotating brother-----\n");
                print_tree(tree);
            }

            if ((brother->left == NULL || brother->left->color == BLACK) &&
                    (brother->right == NULL || brother->right->color == BLACK)) {
                // Paint the brother red.
                // Then both children of "parent" have one less black node in every
                // path to the leaves. We then adjust the parent instead.
                trace("Both children of brother are black.\n");
                brother->color = RED;
                node = parent;
                parent = node->parent;
                continue;
            } else {
                // Make sure the brother has a red right-child.
                // Note: Both children may be red.
                if (brother->right == NULL || brother->right->color == BLACK) {
                    trace("Brother %d @ %p has a black right child.\n", brother->value, brother);
                    brother->color = RED;
                    brother->left->color = BLACK;
                    brother = rbtree_rotate_right(tree, brother);
                }

                // Rotate to re-balance the subtree of the 'parent' node.
                brother->color = parent->color;
                parent->color = BLACK;
                brother->right->color = BLACK;
                rbtree_rotate_left(tree, parent);
            }
        } else {
            trace("Node on the right.\n");
            rbnode_t *brother = parent->left;

            // Make sure the current node's brother is also black
            if (brother->color == RED) {
                trace("Brother %d@%p is red. Rotating...\n", brother->value, brother);
                brother->color = BLACK;
                parent->color = RED;
                rbtree_rotate_right(tree, parent);
                brother = parent->left;
                trace("New brother %p\n", brother);
                printf("------after rotating brother-----\n");
                print_tree(tree);
            }

            if ((brother->left == NULL || brother->left->color == BLACK) &&
                    (brother->right == NULL || brother->right->color == BLACK)) {
                // Paint the brother red.
                // Then both children of "parent" have one less black node in every
                // path to the leaves. We then adjust the parent instead.
                trace("Both children of brother are black.\n");
                brother->color = RED;
                node = parent;
                parent = node->parent;
                continue;
            } else {
                // Make sure the brother has a red left-child.
                // Note: Both children may be red.
                if (brother->left == NULL || brother->left->color == BLACK) {
                    trace("Brother %d @ %p has a black left child.\n", brother->value, brother);
                    brother->color = RED;
                    brother->right->color = BLACK;
                    brother = rbtree_rotate_left(tree, brother);
                }

                // Rotate to re-balance the subtree of the 'parent' node.
                brother->color = parent->color;
                parent->color = BLACK;
                brother->left->color = BLACK;
                rbtree_rotate_right(tree, parent);
            }
        }

        break;
    }
    if (node != NULL && node->color == RED) {
        trace("Dyeing node %d @ %p to black\n", node->value, node);
        // Repaint this node to black.
        // Then the number of black nodes of all paths returns to normal.
        node->color = BLACK;
    }
}

void rbtree_delete(rbtree_t *tree, rbnode_t *node) {
    if (node->left != NULL && node->right != NULL) {
        // Both children are non-leaf. Replace with the next smallest node.
        rbnode_t *old = node;

        node = node->right;
        while(node->left != NULL) {
            node = node->left;
        }

        // Unlike the kernel, our nodes contain values.
        // We just replace the value, not the node itself.
        old->value = node->value;
    }

    rbnode_t *child = node->left != NULL ? node->left : node->right;
    rbnode_t *parent = node->parent;
    enum rbnode_color color = node->color; 

    if (parent != NULL) {
        if (node == parent->left) {
            parent->left = child;
        } else {
            parent->right = child;
        }
    } else {
        tree->root = child;
    }

    if (child != NULL) {
        child->parent = parent;
    }

    if (color == BLACK) {
        rbtree_delete_adjust(tree, child, parent);
    }
}

#define SZ 10

int valuegen(int i) {
    return 3*i % 10;
}

int valuegen2(int i) {
    return 9-i;
}

int main() {
    rbtree_t t;
    rbnode_t nodes[SZ];

    int i,j;

    t.root = NULL;

    for (i=0; i<SZ; i++) {
        nodes[i].value = valuegen(i);
        printf("-----inserting %d...-----\n", nodes[i].value);
        rbtree_insert(&t, &nodes[i]);
        printf("-----pre-check-----\n");
        node_check(t.root);
        printf("-----tree-----\n");
        print_tree(&t);
        printf("-----check-----\n");
        for (j=0; j<=i; j++) {
            if (rbtree_find_node(&t, valuegen(j)) == NULL) {
                printf("Value %d not found!\n", j);
            }
        }
        printf("============\n");
    }

    for (i=0; i<SZ; i++) {
        rbnode_t *node = rbtree_find_node(&t, valuegen2(i));
        printf("-----deleting %d @ %p...-----\n", node->value, node);
        rbtree_delete(&t, node);
        printf("-----pre-check-----\n");
        node_check(t.root);
        printf("-----tree-----\n");
        print_tree(&t);
        printf("-----check-----\n");
        for (j=0; j<=i; j++) {
            if (rbtree_find_node(&t, valuegen2(j)) != NULL) {
                printf("Value %d still exists!\n", j);
            }
        }
        printf("============\n");
    }

    return 0;
}
