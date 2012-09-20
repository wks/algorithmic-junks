#include <stdio.h>
#include <stdlib.h>

int min(int a, int b) {
    return a<b?a:b;
}

void swap(int *a, int *b) {
    int tmp;
    tmp = *a; *a = *b; *b = tmp;
}


void median_of_medians(int *ar, int sz) {
    int h; // Hop

    for (h=1; h<sz; h*=5) {
        int g,ng; // Group, Next group
        for (g=0, ng=g+h*5; g<sz; g=ng, ng+=h*5) {
            int g_limit, g_last;
            int j;

            g_limit = min(ng, sz);
            g_last = g;

            for (j=g+h; j<g_limit; j+=h) {
                int tmp;
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

int main() {
    int sz;
    int *ar;
    int i;

    scanf("%d", &sz);

    ar = (int*)malloc(sz*(sizeof(int)));
    if (ar == NULL) {
        perror("malloc");
        exit(1);
    }

    for (i=0; i<sz; i++) {
        scanf("%d", &ar[i]);
    }

    median_of_medians(ar, sz);

    printf("%d\n", ar[0]);

    free(ar);

    return 0;
}
