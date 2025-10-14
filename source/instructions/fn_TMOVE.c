#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

static int32_t ladder_get_table_i32(ladder_ctx_t *ladder_ctx, uint32_t table_net, uint32_t flat_pos, uint8_t *err) {
    *err = LADDER_INS_ERR_OK;
    if (table_net >= ladder_ctx->ladder.quantity.networks) {
        *err = LADDER_INS_ERR_NOTABLE;
        return 0;
    }
    ladder_network_t *net = &ladder_ctx->network[table_net];

    if (flat_pos >= net->rows * net->cols) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return 0;
    }
    uint32_t r = flat_pos / net->cols;
    uint32_t c = flat_pos % net->cols;
    if (r >= net->rows || c >= net->cols || net->cells[r][c].data_qty < 1) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return 0;
    }
    // Check type for i32 compatibility (D/IW/QW as proxy)
    if (net->cells[r][c].data[0].type != LADDER_REGISTER_D && net->cells[r][c].data[0].type != LADDER_REGISTER_IW
            && net->cells[r][c].data[0].type != LADDER_REGISTER_QW) {
        *err = LADDER_INS_ERR_TYPEMISMATCH;
        return 0;
    }
    return (int32_t) net->cells[r][c].data[0].value.i32;
}

static void ladder_set_table_i32(ladder_ctx_t *ladder_ctx, uint32_t table_net, uint32_t flat_pos, int32_t val, uint8_t *err) {
    *err = LADDER_INS_ERR_OK;
    if (table_net >= ladder_ctx->ladder.quantity.networks) {
        *err = LADDER_INS_ERR_NOTABLE;
        return;
    }
    ladder_network_t *net = &ladder_ctx->network[table_net];

    if (flat_pos >= net->rows * net->cols) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return;
    }
    uint32_t r = flat_pos / net->cols;
    uint32_t c = flat_pos % net->cols;
    if (r >= net->rows || c >= net->cols || net->cells[r][c].data_qty < 1) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return;
    }
    // Check dest type for i32 compatibility
    if (net->cells[r][c].data[0].type != LADDER_REGISTER_D && net->cells[r][c].data[0].type != LADDER_REGISTER_IW
            && net->cells[r][c].data[0].type != LADDER_REGISTER_QW) {
        *err = LADDER_INS_ERR_TYPEMISMATCH;
        return;
    }
    net->cells[r][c].data[0].value.i32 = val;
}

ladder_ins_err_t fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // MAX_MOVE=1024 (safe for 32x255=8160, but cap low)
#define MAX_MOVE 1024
    int32_t dest_net = ladder_get_data_value(ladder_ctx, row, column, 0);
    int32_t dest_pos = ladder_get_data_value(ladder_ctx, row, column, 1);
    int32_t src_net = ladder_get_data_value(ladder_ctx, row, column, 2);
    int32_t src_pos = ladder_get_data_value(ladder_ctx, row, column, 3);
    int32_t qty_raw = ladder_get_data_value(ladder_ctx, row, column, 4);
    uint32_t qty = (qty_raw < 0 || qty_raw > MAX_MOVE) ? 0 : (uint32_t) qty_raw; // Handle negative as 0, cap

    if (qty == 0) {
        CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row); // Success for zero
        return LADDER_INS_ERR_OK;
    }

    // Check table enables (disabled for data tables)
    if (ladder_ctx->network[dest_net].enable || ladder_ctx->network[src_net].enable) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return LADDER_INS_ERR_NOTABLE;
    }

    // Bounds: Ensure full range fits
    uint32_t dest_size = ladder_ctx->network[dest_net].rows * ladder_ctx->network[dest_net].cols;
    uint32_t src_size = ladder_ctx->network[src_net].rows * ladder_ctx->network[src_net].cols;
    if (dest_pos + qty > dest_size || src_pos + qty > src_size) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return LADDER_INS_ERR_OUTOFRANGE;
    }

    // Atomic via temp buffer (all read or err, all write or none)
    int32_t temp[MAX_MOVE];
    uint8_t read_err = LADDER_INS_ERR_OK;
    for (uint32_t k = 0; k < qty; ++k) {
        temp[k] = ladder_get_table_i32(ladder_ctx, src_net, src_pos + k, &read_err);
        if (read_err != LADDER_INS_ERR_OK) {
            break;
        }
    }
    if (read_err != LADDER_INS_ERR_OK) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return read_err;
    }

    uint8_t write_err = LADDER_INS_ERR_OK;
    for (uint32_t k = 0; k < qty; ++k) {
        ladder_set_table_i32(ladder_ctx, dest_net, dest_pos + k, temp[k], &write_err);
        if (write_err != LADDER_INS_ERR_OK) {
            break;
        }
    }
    if (write_err != LADDER_INS_ERR_OK) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return write_err;
    }

    // Success: Propagate power
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);
    return LADDER_INS_ERR_OK;
}
