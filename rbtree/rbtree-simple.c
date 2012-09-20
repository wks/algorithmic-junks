#include <stdio.h>
#include <stdlib.h>

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

/*
 * Return value:
 * 0: normal
 * -1: node value already exist
 */
int insert_node(rbtree_t *tree, rbnode_t *new_node) {
    if (tree->root == NULL) {
        tree->root = new_node;
        new_node->color = BLACK;
        new_node->parent = NULL;
        new_node->left = new_node->right = NULL;
        return 0;
    } else {
        rbnode_t *old_node = tree->root;

        while (1) {
            if (new_node->value == old_node->value) {
                return -1;
            } else if (new_node->value < old_node->value) {
                if (old_node->left == NULL) {
                    old_node->left = new_node;
                    new_node->color = RED;
                    new_node->parent = old_node;
                    new_node->left = new_node->right = NULL;
                    break;
                } else {
                    old_node = old_node->left;
                }
            } else {
                if (old_node->right == NULL) {
                    old_node->right = new_node;
                    new_node->color = RED;
                    new_node->parent = old_node;
                    new_node->left = new_node->right = NULL;
                    break;
                } else {
                    old_node = old_node->right;
                }
            }
        }

        printf("****TEST****: Just inserted:\n");
        print_tree(tree);
        printf("****/TEST****\n");


        rbnode_t *cur_node = new_node;

        // rotating
        while(1) {
            printf("rotating...\n");
            rbnode_t *parent = cur_node->parent;
            // NOTE: cur_node is always RED in the beginning of each iter
            if (parent == NULL) {
                // root needs rotation
                cur_node->color = BLACK;
            } else if (parent->color == RED) {
                // A red parent always implies a black grandpa.
                rbnode_t *grandpa = cur_node->parent->parent; 
                rbnode_t *uncle;
                if (grandpa->left == parent) {
                    uncle = grandpa->right;
                } else {
                    uncle = grandpa->left;
                }

                if (uncle == NULL || uncle->color == BLACK) {
                    rbnode_t *left, *mid, *right, *great_grandpa;
                    great_grandpa = grandpa->parent;
                    printf("great_grandpa = %p\n", great_grandpa);
                    if (great_grandpa != NULL) {
                        printf("great_grandpa->value = %d\n", great_grandpa->value);
                    }
                    printf("grandpa = %p : %d\n", grandpa, grandpa->value);
                    printf("parent = %p : %d\n", parent, parent->value);
                    printf("cur_node = %p : %d\n", cur_node, cur_node->value);
                    if (parent == grandpa->left) {
                        right = grandpa;
                        if (cur_node == parent->left) {
                            left = cur_node;
                            mid = parent;
                        } else {
                            left = parent;
                            mid = cur_node;
                            left->right = mid->left;
                            mid->left->parent = left;
                        }
                        right->left = mid->right;
                    } else {
                        left = grandpa;
                        if (cur_node == parent->right) {
                            right = cur_node;
                            mid = parent;
                        } else {
                            right = parent;
                            mid = cur_node;
                            right->left = mid->right;
                        }
                        left->right = mid->left;
                    }

                    left->parent = right->parent = mid;
                    mid->left = left;
                    mid->right = right;
                    mid->parent = great_grandpa;

                    if (left->right != NULL) {
                        left->right->parent = left;
                    }
                    if (right->left != NULL) {
                        right->left->parent = right;
                    }

                    left->color = right->color = RED;
                    mid->color = BLACK;

                    if (great_grandpa == NULL) {
                        tree->root = mid;
                    } else if (great_grandpa->left == grandpa) {
                        great_grandpa->left = mid;
                    } else {
                        great_grandpa->right = mid;
                    }

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
                    continue;
                }
            } else {
                // A black parent is okay to have a red child.
            }

            break;
        }
    }
}

#define SZ 10

int main() {
    rbtree_t t;
    rbnode_t nodes[SZ];

    int i,j;

    t.root = NULL;

    for (i=0; i<SZ; i++) {
        nodes[i].value = i;
        printf("-----inserting %d...-----\n", i);
        insert_node(&t, &nodes[i]);
        printf("-----pre-check-----\n");
        node_check(t.root);
        printf("-----tree-----\n");
        print_tree(&t);
        printf("-----check-----\n");
        for (j=0; j<=i; j++) {
            if (find_node(&t, j) == NULL) {
                printf("Value %d not found!\n", j);
            }
        }
        printf("============\n");
    }

    return 0;
}
