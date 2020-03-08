#include <myfs.h>

int get_n_operation_args(FS_OPERATION operation_code) {
    switch (operation_code) {
        case OP_LS:
            return 0;
        case OP_CAT:
            return 0;
    }
}