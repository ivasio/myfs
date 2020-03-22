#ifndef CLIENT_MYFS_H
#define CLIENT_MYFS_H


#define N_FS_OPERATIONS 9

// List of all supported operations
typedef enum FS_OPERATION {
    OP_CD,
    OP_LS,
    OP_CP,
    OP_CP_TO_REMOTE,
    OP_CP_FROM_REMOTE,
    OP_RM,
    OP_MV,
    OP_MKDIR,
    OP_CAT
} FS_OPERATION;


// Number of arguments that should be passed to any of these operations on client side
const char n_operations_args[N_FS_OPERATIONS] = {
    1, // cd takes the directory path to navigate to
    1, // ls takes the path to display info about
    2, // all cp variations take the destination and source paths
    2,
    2,
    1, // rm takes a path to be deleted
    2, // mv takes the destination and source paths
    1, // mkdir takes the path of directory to be created
    1, // cat takes the path of file to be shown
};


// Number of arguments that should be sent via the network. It is the same as number of
// operation arguments except for `cp` variations
const char n_request_args[N_FS_OPERATIONS] = {
    1,
    1,
    2, // remote cp sends the destination and source paths on the server
    2, // cp to remote sends destination path on the server and the content of file to be copied
    1, // cp from remote sends source path
    1,
    2,
    1,
    1,
};


#endif //CLIENT_MYFS_H
