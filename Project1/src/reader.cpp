#include "reader.h"

#include <stdio.h>

using namespace std;

void Reader::Read(Context * context) {
    int N;
    scanf("%d", &N);
    for (int i = 0; i < N; i++) {
        int house;
        scanf("%d ", &house);
        context->houses.push_back(house);
    }
    
    int num_line_below;
    scanf("%d %d\n", &num_line_below, &context->num_vertices);

    context->house_bitmap.resize(context->num_vertices + 1, false);
    for (int house : context->houses) {
        context->house_bitmap[house] = true;
    }
    
    context->adjlist.resize(context->num_vertices + 1);
    for (int i = 1; i <= num_line_below; i++) {
        int v_i, num_neighbors;
        scanf("%d %d ", &v_i, &num_neighbors);
        
        for (int j = 0; j < num_neighbors; j++) {
            int v_ij;
            scanf("%d ", &v_ij);
            context->adjlist[v_i].push_back(v_ij);
            context->adjlist[v_ij].push_back(v_i);
        }
    }
}
