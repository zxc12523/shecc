/*
 * shecc - Self-Hosting and Educational C Compiler.
 *
 * shecc is freely redistributable under the BSD 2 clause license. See the
 * file "LICENSE" for information on usage and redistribution of this file.
 */
#include<defs.h>


struct dag_node
{
    /* data */
    int is_exit;
    ph2_ir_t* ir;
    struct dag_node* child[64];
    int child_size;
    struct dag_node* parent[64];
    int parent_size;
};

typedef struct dag_node dag_node_t;

int ir_nums;
dag_node_t dag_node_arr[64];

void initDagNodes(basic_block_t *bb) {

    int i = 0;
    ph2_ir_t* ph2_ir;

    for(i = 0, ph2_ir = bb->ph2_ir_list.head; ph2_ir ;i++, ph2_ir = ph2_ir->next) {
        dag_node_arr[i].ir = ph2_ir;

        if (ph2_ir->op == OP_branch) {
            dag_node_arr[i].is_exit = 1;
        }
    }

    ir_nums = i;
}

int checkRegister(ph2_ir_t *fisrt, ph2_ir_t *second) {
    if (fisrt->dest == second->src0 || /* data dependency */ 
        fisrt->dest == second->src1 || /* data dependency */ 
        fisrt->dest == second->dest || /* output dependency */ 
        fisrt->src0 == second->dest || /* anti dependency */ 
        fisrt->src1 == second->dest    /* anti dependency */)
        return 1;

    return 0;
}

void constructDependency() {
    for(int i=0 ; i < ir_nums ; i++) {
        for(int j = i + 1 ; j < ir_nums ; j++) {
            dag_node_t* node1 = &dag_node_arr[i];
            dag_node_t* node2 = &dag_node_arr[j];
            if (checkRegister(node1->ir, node2->ir)) {
                node1->child[node1->child_size++] = node2;
                node2->parent[node1->parent_size++] = node1;
            }
        }
    }
}




void buildSchedGrpah(basic_block_t *bb) {

    initDagNodes(bb);
    constructDependency();

    
}
