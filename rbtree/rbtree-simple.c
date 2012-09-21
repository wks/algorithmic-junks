#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

rbnode_t *find_node(rbtree_t *tree, value_t value) {
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

#define SZ 10

int main() {
    rbtree_t t;
    rbnode_t nodes[SZ];

    int i,j;

    t.root = NULL;

    for (i=0; i<SZ; i++) {
        nodes[i].value = 3*i % 10;
        printf("-----inserting %d...-----\n", i);
        rbtree_insert(&t, &nodes[i]);
        printf("-----pre-check-----\n");
        node_check(t.root);
        printf("-----tree-----\n");
        print_tree(&t);
        printf("-----check-----\n");
        for (j=0; j<=i; j++) {
            if (find_node(&t, j*3 % 10) == NULL) {
                printf("Value %d not found!\n", j);
            }
        }
        printf("============\n");
    }

    return 0;
}
