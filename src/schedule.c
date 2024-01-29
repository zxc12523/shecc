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
    int latency;
    ph2_ir_t* ir;
    struct dag_node* child[64];
    int child_size;
    struct dag_node* parent[64];
    int parent_size;
};

typedef struct dag_node dag_node_t;

int ir_nums;
dag_node_t dag_node_arr[64];

void init_dag_nodes(basic_block_t *bb) {

    int i = 0;
    ph2_ir_t* ph2_ir;

    for(i = 0, ph2_ir = bb->ph2_ir_list.head; ph2_ir ;i++, ph2_ir = ph2_ir->next) {

        dag_node_arr[i].ir = ph2_ir;

        

        if (ph2_ir->op == OP_branch     || 
            ph2_ir->op == OP_jump       || 
            ph2_ir->op == OP_call       ||
            ph2_ir->op == OP_indirect   ||
            ph2_ir->op == OP_return) 
        {
            dag_node_arr[i].is_exit = 1;
        }
    }

    ir_nums = i;
}

int check_register(ph2_ir_t *fisrt, ph2_ir_t *second) {
    if (fisrt->dest == second->src0 || /* data dependency */ 
        fisrt->dest == second->src1 || /* data dependency */ 
        fisrt->dest == second->dest || /* output dependency */ 
        fisrt->src0 == second->dest || /* anti dependency */ 
        fisrt->src1 == second->dest    /* anti dependency */)
        return 1;

    return 0;
}

int check_alias(ph2_ir_t *fisrt, ph2_ir_t *second) {
    if (fisrt->op != OP_load)
}

void construct_dependency() {
    for(int i=0 ; i < ir_nums ; i++) {
        for(int j = i + 1 ; j < ir_nums ; j++) {
            dag_node_t* node1 = &dag_node_arr[i];
            dag_node_t* node2 = &dag_node_arr[j];
            if (check_register(node1->ir, node2->ir)) {
                node1->child[node1->child_size++] = node2;
                node2->parent[node1->parent_size++] = node1;
            }
        }
    }
}




void build_sched_grpah(basic_block_t *bb) {

    init_dag_nodes(bb);
    construct_dependency();
    
}
