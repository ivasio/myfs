#ifndef CLIENT_MYFS_H
#define CLIENT_MYFS_H


#define N_FS_OPERATIONS 2

typedef enum FS_OPERATION {
    OP_LS,
    OP_CAT
} FS_OPERATION;

int get_n_operation_args(FS_OPERATION operation_code);

#endif //CLIENT_MYFS_H
