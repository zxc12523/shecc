/*
 * shecc - Self-Hosting and Educational C Compiler.
 *
 * shecc is freely redistributable under the BSD 2 clause license. See the
 * file "LICENSE" for information on usage and redistribution of this file.
 */
#include<defs.h>

enum dag_dep_type {
    DATA = 1, 
    OUTPUT, 
    ANTI
};

struct dag_dep
{
    int is_pred;
    int dep_type;
    int pred_num;
    struct dag_node* pred;
    int succ_num;
    struct dag_node* succ;
};

struct dag_node
{
    /* data */
    int is_exit;
    int delay_cycle;
    int sched_cycle;
    int may_load;
    int may_store;
    ph2_ir_t* ir;
    struct dag_dep* succ[64];
    int succ_size;
    struct dag_dep* pred[64];
    int pred_size;
};


typedef struct dag_node dag_node_t;
typedef struct dag_dep dag_dep_t;

int ir_nums;
dag_node_t dag_node_arr[64];

dag_node_t* def[REG_CNT];

int may_load(opcode_t op) {
    return  op == OP_load           || 
            op == OP_global_load    || 
            op == OP_read           || 
            op == OP_return;
}

int may_store(opcode_t op) {
    return  op == OP_define         || 
            op == OP_store          || 
            op == OP_global_store   || 
            op == OP_write          || 
            op == OP_address_of_func;
}

int may_jmp(opcode_t op) {
    return  op == OP_branch     || 
            op == OP_jump       || 
            op == OP_call       ||
            op == OP_indirect   ||
            op == OP_return;
}

int check_register(ph2_ir_t *fisrt, ph2_ir_t *second) {

    /* data dependency */ 
    if (fisrt->dest == second->src0 ||  fisrt->dest == second->src1)
        return DATA;

    /* output dependency */ 
    if (fisrt->dest == second->dest) {
        return OUTPUT;
    } 

    /* anti dependency */
    if (fisrt->src0 == second->dest || fisrt->src1 == second->dest    )
        return ANTI;

    return 0;
}

void add_dependency(dag_node_t* node1, dag_node_t* node2, int dep_type) {

    dag_dep_t* succ_dep = node1->succ[node1->succ_size] = malloc(sizeof(dep_type));
    dag_dep_t* pred_dep = node2->pred[node2->pred_size] = malloc(sizeof(dep_type));

    succ_dep->dep_type = dep_type;
    succ_dep->is_pred = 0;
    succ_dep->succ_num = node2->pred_size++;
    succ_dep->succ = node2;


    pred_dep->dep_type = dep_type;
    pred_dep->is_pred = 1;
    pred_dep->pred_num = node1->succ_size++;
    pred_dep->pred = node1;

    return ;
}


void build_sched_grpah(basic_block_t *bb) {

    /* Initialize Dag nodes */

    int i = 0;
    ph2_ir_t* ph2_ir;

    for(i = 0, ph2_ir = bb->ph2_ir_list.head; ph2_ir ;i++, ph2_ir = ph2_ir->next) {

        dag_node_arr[i].ir = ph2_ir;

        /*TODO: add letency*/

        if (may_load(ph2_ir->op)) {
            dag_node_arr[i].may_load = 1;
        }

        if (may_store(ph2_ir->op)) {
            dag_node_arr[i].may_store = 1;
        }

        if (may_jmp(ph2_ir->op)) {
            dag_node_arr[i].is_exit = 1;
        }
    }

    ir_nums = i;

    /* Build Dag Dependency*/

    for(int i=0 ; i < ir_nums ; i++) {
        for(int j = i + 1 ; j < ir_nums ; j++) {

            int dep_type;
            dag_node_t* node1 = &dag_node_arr[i];
            dag_node_t* node2 = &dag_node_arr[j];

            if (dep_type = check_register(node1->ir, node2->ir)) {
                add_dependency(&dag_node_arr[i], &dag_node_arr[j], dep_type);
            }
        }
    }

    /* may change in future after adding alias analysis*/

    int i = 0, j = 0;

    while(j < ir_nums && dag_node_arr[j].may_store == 0) {
        j++;
    }

    for(; j < ir_nums;) {

        while(j < ir_nums && i < j) {
            if (dag_node_arr[i].may_load || dag_node_arr[i].may_store) {
                add_dependency(&dag_node_arr[i], &dag_node_arr[j], );
            }
            i++;
        }

        j++;

        while(j < ir_nums && dag_node_arr[j].may_store == 0) {
            if (dag_node_arr[j].may_load) {
                add_dependency(&dag_node_arr[i], &dag_node_arr[j]);
            }

            j++;
        }
    }

    
    
}
