/*
 * Input file reading logic
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */

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
    scanf("%d %d\n", &context->num_vertices, &num_line_below);

    context->house_bitmap.resize(context->num_vertices + 1, 0);
    for (int i = 0; i < context->houses.size(); i++) {
        int house = context->houses[i];
        context->house_bitmap[house] = 1;
        context->house_index_map[house] = i;
    }
    
    context->adjlist.resize(context->num_vertices + 1);
    for (int i = 1; i <= num_line_below; ++i) {
        int v_i, num_neighbors;
        scanf("%d %d ", &v_i, &num_neighbors);
        
        for (int j = 0; j < num_neighbors; ++j) {
            int v_ij;
            scanf("%d ", &v_ij);
            context->adjlist[v_i].PushBack(v_ij);
            context->adjlist[v_ij].PushBack(v_i);
        }
    }
}
