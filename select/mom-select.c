#include <stdio.h>
#include <stdlib.h>

typedef int value_t;

int min(value_t a, value_t b) {
    return a<b?a:b;
}

void swap(value_t *a, value_t *b) {
    value_t tmp;
    tmp = *a; *a = *b; *b = tmp;
}


void median_of_medians(value_t *ar, int sz) {
    int h; // Hop

    for (h=1; h<sz; h*=5) {
        int g,ng; // Group, Next group
        for (g=0, ng=g+h*5; g<sz; g=ng, ng+=h*5) {
            int g_limit, g_last; // Group right bound, Group last member
            int j;

            g_limit = min(ng, sz);
            g_last = g;

            for (j=g+h; j<g_limit; j+=h) { // Insert sort
                value_t tmp;
                int i;
                tmp = ar[j];
                for (i = j - h; i>=g && !(ar[i] < tmp); i-=h) {
                    ar[i+h] = ar[i];
                }
                ar[i+h] = tmp;
                g_last = j;
            }
            swap(&ar[g], &ar[min(g+h*2,g_last)]);
        }
    }
}

void print_slice(int *ar, int sz) {
    int i;
    if (sz>0) {
        printf("%d", ar[0]);
        for(i=1; i<sz; i++) {
            printf(" %d", ar[i]);
        }
    }
    printf("\n");
}

void partition_nth(int nth, value_t *ar, int sz) {
    value_t *iar;
    int isz, ipos, inth;

    iar = ar; isz = sz; inth = nth;

    while (1) {
        value_t pivot;
        int left, right;
        int left_size;

        median_of_medians(iar, isz);
        pivot = iar[0];
        left = 0;
        right = isz-1;

        while(left < right) {
            while (left < right && !(iar[right] < pivot)) {
                right--;
            }

            if (left < right) {
                iar[left] = iar[right];
                left++;
            }

            while (left < right && iar[left] < pivot) {
                left++;
            }

            if (left < right) {
                iar[right] = iar[left];
                right--;
            }
        }

        iar[left] = pivot;

        left_size = left+1;
        if (left_size == inth) {
            break;
        } else if (left_size > inth) {
            // iar = iar
            isz = left_size - 1;
            // inth = inth
        } else {
            iar = iar + left_size;
            isz = isz - left_size;
            inth = inth - left_size;
        }
    }
}

int main() {
    int sz;
    int nth;
    value_t *ar;
    int i;

    scanf("%d%d", &sz, &nth);

    ar = (value_t*)malloc(sz*(sizeof(value_t)));
    if (ar == NULL) {
        perror("malloc");
        exit(1);
    }

    for (i=0; i<sz; i++) {
        scanf("%d", &ar[i]);
    }

    partition_nth(nth, ar, sz);

    print_slice(ar, nth);

    free(ar);

    return 0;
}
