#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <agile_upgrade.h>

extern const struct agile_upgrade_ops agile_upgrade_file_ops;

static uint32_t _written_len = 0;
static uint32_t _total_len = 0;

static void step_hook(int step) { printf("step: %d\r\n", step); }

static void error_hook(int step, int code) { printf("step: %d, err: %d\r\n", step, code); }

static void progress_hook(uint32_t cur_size, uint32_t total_size) {
    _written_len = cur_size;
    _total_len = total_size;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return -1;

    agile_upgrade_set_step_hook(step_hook);
    agile_upgrade_set_error_hook(error_hook);
    agile_upgrade_set_progress_hook(progress_hook);

    agile_upgrade_t src_agu = {0};
    src_agu.name = "src";
    src_agu.user_data = argv[1];
    src_agu.ops = &agile_upgrade_file_ops;

    agile_upgrade_t dst_agu = {0};
    dst_agu.name = "dst";
    dst_agu.user_data = "./upgrade.bin";
    dst_agu.ops = &agile_upgrade_file_ops;

    agile_upgrade_release(&src_agu, &dst_agu, 0);

    printf("Written len: %u, total len: %u\r\n", _written_len, _total_len);

    return 0;
}
