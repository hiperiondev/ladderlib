/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef LADDER_H
#define LADDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def LADDERLIB_VERSION_MAYOR
 * @brief Indicate a really big change that can cause a incompatibilities with previous versions.
 */
#define LADDERLIB_VERSION_MAYOR 2

/**
 * @def LADDERLIB_VERSION_MINOR
 * @brief Indicate some change on API or opcode or very important correction in functionality
 */
#define LADDERLIB_VERSION_MINOR 1

/**
 * @def LADDERLIB_VERSION_PATCH
 * @brief Indicate some minor change or correction
 */
#define LADDERLIB_VERSION_PATCH 0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def LADDER_VERTICAL_BAR
 * @brief Point to vertical bar flag
 *
 */
#define LADDER_VERTICAL_BAR(lctx, n, r, c) ((*lctx).network[n].cells[r][c].vertical_bar)

/**
 * @def ladder_cell_data
 * @brief
 *
 */
#define ladder_cell_data(lctx, n, r, c, i, typ, val)                                                                                                           \
    (*lctx).network[n].cells[r][c].data[i].type = typ;                                                                                                         \
    switch (typ) {                                                                                                                                             \
        case LADDER_TYPE_NONE: {                                                                                                                               \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_M: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_Q: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_I: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_Cd: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_Cr: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_Td: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_Tr: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_IW: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_QW: {                                                                                                                                 \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_C: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_T: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_D: {                                                                                                                                  \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
        case LADDER_TYPE_REAL: {                                                                                                                               \
            (*lctx).network[n].cells[r][c].data[i].value.real = (float)val;                                                                                    \
        } break;                                                                                                                                               \
        default: {                                                                                                                                             \
            (*lctx).network[n].cells[r][c].data[i].value.i32 = (int32_t)val;                                                                                   \
        } break;                                                                                                                                               \
    }

/**
 * @def ladder_cell_data_cstr
 * @brief
 *
 */
#define ladder_cell_data_cstr(lctx, n, r, c, i, val)                                                                                                           \
    (*lctx).network[n].cells[r][c].data[i].type = LADDER_TYPE_CSTR;                                                                                            \
    (*lctx).network[n].cells[r][c].data[i].value.cstr = val

/**
 * @enum LADDER_INSTRUCTIONS
 * @brief Ladder Instructions codes
 * Must match with array of functions in TaskLadder
 */
typedef enum LADDER_INSTRUCTIONS {
    LADDER_INS_NOP,     /**< Instruction NOP */
    LADDER_INS_CONN,    /**< Instruction CONN */
    LADDER_INS_NEG,     /**< Instruction NEG */
    LADDER_INS_NO,      /**< Instruction NO */
    LADDER_INS_NC,      /**< Instruction NC */
    LADDER_INS_RE,      /**< Instruction RE */
    LADDER_INS_FE,      /**< Instruction FE */
    LADDER_INS_COIL,    /**< Instruction COIL */
    LADDER_INS_COILL,   /**< Instruction COILL */
    LADDER_INS_COILU,   /**< Instruction COILU */
    LADDER_INS_TON,     /**< Instruction TON */
    LADDER_INS_TOFF,    /**< Instruction TOFF */
    LADDER_INS_TP,      /**< Instruction TP */
    LADDER_INS_CTU,     /**< Instruction CTU */
    LADDER_INS_CTD,     /**< Instruction CTD */
    LADDER_INS_MOVE,    /**< Instruction MOVE */
    LADDER_INS_SUB,     /**< Instruction SUB */
    LADDER_INS_ADD,     /**< Instruction ADD */
    LADDER_INS_MUL,     /**< Instruction MUL */
    LADDER_INS_DIV,     /**< Instruction DIV */
    LADDER_INS_MOD,     /**< Instruction MOD */
    LADDER_INS_SHL,     /**< Instruction SHL */
    LADDER_INS_SHR,     /**< Instruction SHR */
    LADDER_INS_ROL,     /**< Instruction ROL */
    LADDER_INS_ROR,     /**< Instruction ROR */
    LADDER_INS_AND,     /**< Instruction AND */
    LADDER_INS_OR,      /**< Instruction OR */
    LADDER_INS_XOR,     /**< Instruction XOR */
    LADDER_INS_NOT,     /**< Instruction NOT */
    LADDER_INS_EQ,      /**< Instruction EQ */
    LADDER_INS_GT,      /**< Instruction GT */
    LADDER_INS_GE,      /**< Instruction GE */
    LADDER_INS_LT,      /**< Instruction LT */
    LADDER_INS_LE,      /**< Instruction LE */
    LADDER_INS_NE,      /**< Instruction NE */
    LADDER_INS_FOREIGN, /**< Instruction FOREIGN */
    LADDER_INS_TMOVE,   /**< Instruction TMOVE */
    ///////////////////
    LADDER_INS_INV,          /**< First invalid */
    LADDER_INS_MULTI = 0xff, /**< cell is a part of multi cell instruction */
} ladder_instruction_t;

/**
 * @enum LADDER_STATE
 * @brief General state
 *
 */
typedef enum LADDER_STATE {
    LADDER_ST_STOPPED,  /**< Stopped */
    LADDER_ST_RUNNING,  /**< Running */
    LADDER_ST_ERROR,    /**< Error */
    LADDER_ST_EXIT_TSK, /**< Exit task */
    LADDER_ST_NULLFN,   /**< pointer to NULL function */
    LADDER_ST_INV,      /**< Invalid */
} ladder_state_t;

/**
 * @enum LADDER_INS_ERROR
 * @brief Instruction status
 *
 */
typedef enum LADDER_INS_ERROR {
    LADDER_INS_ERR_OK,         /**< OK */
    LADDER_INS_ERR_GETPREVVAL, /**< Error get previous value */
    LADDER_INS_ERR_GETDATAVAL, /**< Error get data value */
    LADDER_INS_ERR_SETDATAVAL, /**< Error set data value */
    LADDER_INS_ERR_NOFOREIGN,  /**< Error foreign function not exist */
    LADDER_INS_ERR_NOTABLE,    /**< Table not exist or net not disabled */
    LADDER_INS_ERR_OUTOFRANGE, /**< Value out of range */
    //...//
    LADDER_INS_ERR_FAIL,       /**< Generic fail */
} ladder_ins_err_t;

/**
 * @enum LADDER_TYPE
 * @brief Data types
 */
typedef enum LADDER_TYPE {
    LADDER_TYPE_NONE,          /**< Type None */
    LADDER_TYPE_M,             /**< Type M */
    LADDER_TYPE_Q,             /**< Type Q */
    LADDER_TYPE_I,             /**< Type I */
    LADDER_TYPE_Cd,            /**< Type Cd */
    LADDER_TYPE_Cr,            /**< Type Cr */
    LADDER_TYPE_Td,            /**< Type Td */
    LADDER_TYPE_Tr,            /**< Type Tr */
    LADDER_TYPE_IW,            /**< Type IW */
    LADDER_TYPE_QW,            /**< Type QW */
    LADDER_TYPE_C,             /**< Type C */
    LADDER_TYPE_T,             /**< Type T */
    LADDER_TYPE_D,             /**< Type D */
    LADDER_TYPE_CSTR,          /**< Type constant string */
    LADDER_TYPE_REAL,          /**< Type float */
    LADDER_TYPE_INV,           /**< First invalid */
    LADDER_BASETIME_MS = 0xf0, /**< Basetime ms */
    LADDER_BASETIME_10MS,      /**< Basetime 10 ms */
    LADDER_BASETIME_100MS,     /**< Basetime 100 ms */
    LADDER_BASETIME_SEC,       /**< Basetime seconds */
    LADDER_BASETIME_MIN,       /**< Basetime minutes */
} ladder_type_t;

/**
 * @struct ladder_instructions_ioc_s
 * @brief Instruction description: inputs, outputs, occupied cells
 *
 */
typedef struct ladder_instructions_ioc_s {
    uint8_t inputs;   /**< Inputs quantity */
    uint8_t outputs;  /**< Outputs quantity */
    uint8_t cells;    /**< Cells quantity */
    uint8_t data_qty; /**< Quantity */
} ladder_instructions_iocd_t;

extern const ladder_instructions_iocd_t ladder_fn_iocd[];

/**
 * @struct ladder_value_s
 * @brief Value container
 *
 */
typedef struct ladder_value_s {
    ladder_type_t type;   /**< Data type */
    union {
          uint32_t u32;   /**< Unsigned integer */
           int32_t i32;   /**< Integer */
        const char *cstr; /**< Constant string */
             float real;  /**< Real */
    } value;              /**< Data */
} ladder_value_t;

/**
 * @struct ladder_cell_s
 * @brief Type definition for Networks and global declarations
 *
 */
typedef struct ladder_cell_s {
                    bool state;        /**< Output state */
                    bool vertical_bar; /**< Have vertical bar */
    ladder_instruction_t code;         /**< Code */
                 uint8_t data_qty;     /**< Data quantity */
         ladder_value_t *data;         /**< Data */
} ladder_cell_t;

/**
 * @struct ladder_network_s
 * @brief
 *
 */
typedef struct ladder_network_s {
             bool enable;  /**< Enabled for execution */
         uint32_t rows;    /**< Rows qty */
         uint32_t cols;    /**< Columns qty */
    ladder_cell_t **cells; /**< Cells */
} ladder_network_t;

/**
 * @struct ladder_timer_s
 * @brief Timers
 *
 */
typedef struct ladder_timer_s {
    uint64_t time_stamp; /**< Time stamp */
    uint32_t acc;        /**< Activated counter */
} ladder_timer_t;

/**
 * @struct ladder_s
 * @brief Ladder internals
 *
 */
typedef struct ladder_s {
    ladder_state_t state;     /**< State */

    struct {
         uint8_t instr;       /**< Last executed instruction */
        uint32_t network;     /**< Last executed network */
        uint32_t cell_column; /**< Last executed cell column */
        uint32_t cell_row;    /**< Last executed cell row */
         uint8_t err;         /**< Last executed error */
    } last;

    struct {
        uint32_t m;           /**< Quantity of regular flags */
        uint32_t i;           /**< Quantity of digital inputs */
        uint32_t q;           /**< Quantity of digital outputs */
        uint32_t iw;          /**< Quantity of analog inputs */
        uint32_t qw;          /**< Quantity of analog outputs */
        uint32_t c;           /**< Quantity of counters */
        uint32_t t;           /**< Quantity of timers */
        uint32_t d;           /**< Quantity of regular registers */
        uint32_t r;           /**< Quantity of floating point registers */
        uint32_t networks;    /**< Quantity of networks */
        // uint8_t net_columns; /**< Quantity of columns */
        // uint8_t net_rows;    /**< Quantity of rows  */
    } quantity;
} ladder_t;

typedef struct ladder_ctx_s ladder_ctx_t;

/**
 * @fn void (*_read_inputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief Read local hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_read_inputs_local)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_write_outputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief Write local hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_write_outputs_local)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_read_inputs_remote)(ladder_ctx_t *ladder_ctx)
 * @brief Read remote hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_read_inputs_remote)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_write_outputs_remote)(ladder_ctx_t *ladder_ctx
 * @brief Write remote hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_write_outputs_remote)(ladder_ctx_t* ladder_ctx);

/**
 * @fn bool (*_on_scan_end)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every scan cycle ant the end
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_scan_end)(ladder_ctx_t* ladder_ctx);

/**
 * @fn bool (*_on_instruction)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every scan instruction
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_instruction)(ladder_ctx_t* ladder_ctx);

/**
 * @fn bool (*_on_task_before)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every task cycle before scan
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_task_before)(ladder_ctx_t* ladder_ctx);

/**
 * @fn bool (*_on_task_after)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every task cycle after scan
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_task_after)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_on_panic)(ladder_ctx_t *ladder_ctx)
 * @brief Panic management
 *
 * @param ladder_ctx Ladder context
 */
typedef void (*_on_panic)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_on_end_task)(ladder_ctx_t *ladder_ctx)
 * @brief Function for end task
 *
 */
typedef void (*_on_end_task)(ladder_ctx_t* ladder_ctx);

/**
 * @fn void (*_delay)(long msec)
 * @brief Delay in milliseconds
 *
 * @param msec Milliseconds
 */
typedef void (*_delay)(long msec);

/**
 * @fn uint64_t (*_millis)(void)
 * @brief Microseconds from system start
 *
 * @return
 */
typedef uint64_t (*_millis)(void);

/**
 * @struct ladder_hw_s
 * @brief Hardware/os dependent functions
 *
 */
typedef struct ladder_hw_s {
    struct {
           _read_inputs_local read_inputs_local;    /**< Read local hardware values */
         _write_outputs_local write_outputs_local;  /**< Write local hardware values */
          _read_inputs_remote read_inputs_remote;   /**< Read remote hardware values */
        _write_outputs_remote write_outputs_remote; /**< Write local hardware values */
    } io;

    struct {
        _millis millis; /**< Milliseconds from system start */
         _delay delay;  /**< Delay in milliseconds */
    } time;
} ladder_hw_t;

/**
 * @typedef ladder_manage_t
 * @brief Internal manage functions
 *
 */
typedef struct ladder_manage_s {
       _on_scan_end scan_end;    /**< Manage for every scan cycle */
    _on_instruction instruction; /**< Manage for every instruction call */
    _on_task_before task_before; /**< Manage for every task cycle before scan */
     _on_task_after task_after;  /**< Manage for every task cycle after scan */
          _on_panic panic;       /**< Manage panic status */
       _on_end_task end_task;    /**< Manage for end task */
} ladder_manage_t;

/**
 * @struct ladder_memory_s
 * @brief Memory registers
 */
typedef struct ladder_memory_s {
    uint8_t *M;  /**< Regular flags */
    uint8_t *I;  /**< Digital Inputs */
    uint8_t *Q;  /**< Digital Outputs */
       bool *Cr; /**< Counter running */
       bool *Cd; /**< Counter done */
       bool *Tr; /**< Timer running */
       bool *Td; /**< Timer done */
} ladder_memory_t;

/**
 * @struct ladder_prev_scan_vals_s
 * @brief Previous Scan Values (history)
 *
 */
typedef struct ladder_prev_scan_vals_s {
    uint8_t *Mh;  /**< Regular flags previous */
    uint8_t *Ih;  /**< Digital Inputs previous */
    uint8_t *Qh;  /**< Digital Outputs previous */
       bool *Crh; /**< Counter running previous */
       bool *Cdh; /**< Counter done previous */
       bool *Trh; /**< Timer running previous */
       bool *Tdh; /**< Timer done previous */
} ladder_prev_scan_vals_t;

/**
 * @struct ladder_registers_s
 * @brief Registers
 *
 */
typedef struct ladder_registers_s {
     int32_t *IW; /**< Analog Inputs */
     int32_t *QW; /**< Analog Outputs */
    uint32_t *C;  /**< Counter registers */
     int32_t *D;  /**< Regular registers */
       float *R;  /**< Floating point registers */
} ladder_registers_t;

/**
 * @struct plc_scan_internals_s
 * @brief Scan internals
 *
 */
typedef struct ladder_scan_internals_s {
    uint64_t actual_scan_time; /**< Actual scan time */
    uint64_t start_time;       /**< Start time for calculate scan time */
} ladder_scan_internals_t;

/**
 * @fn  ladder_ins_err_t (*ladder_fn_t)(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool *state)
 * @brief Instruction prototype
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
typedef ladder_ins_err_t (*ladder_fn_t)(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

typedef struct ladder_foreign_function_s ladder_foreign_function_t;

/**
 * @fn bool (_foreign_fn_init)(ladder_ctx_t *ladder_ctx, void *data, uint32_t qty);
 * @brief Prototype for foreign function initializer
 *
 * @param ladder_ctx Function
 * @param data Internal initialize values
 * @param qty Quantity
 * @return Status
 */
typedef bool (*_foreign_fn_init)(ladder_foreign_function_t *function, void *data, uint32_t qty);

/**
 * @fn bool (_foreign_fn_deinit)(ladder_foreign_function_t *function);
 * @brief Prototype for foreign functions deinitializer
 *
 * @param ladder_ctx Function
 * @return Status
 */
typedef bool (*_foreign_fn_deinit)(ladder_foreign_function_t *function);

/**
 * @struct ladder_foreign_function_s
 * @brief Main foreign function definition
 *
 */
typedef struct ladder_foreign_function_s {
                      uint32_t id;          /**< Foreign function id */
                          char name[4];     /**< Foreign function name */
    ladder_instructions_iocd_t description; /**< Foreign function description */
                   ladder_fn_t exec;        /**< Foreign functions pointers */
            _foreign_fn_deinit deinit;      /**< Foreign functions deinitializer pointers */
                          void *data;       /**< Internal data for foreign functions */
} ladder_foreign_function_t;

/**
 * @struct ladder_foreign_s
 * @brief Foreign functions
 *
 */
typedef struct ladder_foreign_s {
                     uint32_t qty; /**< Foreign functions quantity */
    ladder_foreign_function_t *fn; /**< Main Main foreign function definition */
} ladder_foreign_t;

/**
 * @struct ladder_ctx_s
 * @brief Ladder context
 *
 */
typedef struct ladder_ctx_s {
                   ladder_t ladder;         /**< Internals */
                ladder_hw_t hw;             /**< Hardware functions */
            ladder_manage_t on;             /**< Manage functions */
            ladder_memory_t memory;         /**< Memory */
    ladder_prev_scan_vals_t prev_scan_vals; /**< Previous scan values */
         ladder_registers_t registers;      /**< Registers */
             ladder_timer_t *timers;        /**< Timers */
    ladder_scan_internals_t scan_internals; /**< Scan internals */
           ladder_network_t *network;       /**< Networks */
           ladder_network_t *exec_network;  /**< Network in execution */
           ladder_foreign_t foreign;        /**< Foreign functions */
} ladder_ctx_t;

/**
 * @fn bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i,
 uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw, uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r);
 * @brief Initialize context.
 *
 *
 * @param ladder_ctx Ladder context.
 * @param net_columns_qty Network Logic matrix columns size.
 * @param net_rows_qty Network Logic matrix rows size (Maximum: 32).
 * @param networks_qty Total Networks.
 * @param qty_m Memory Areas quantities. Marks. Regular flags.
 * @param qty_i Memory Areas quantities. Digital Inputs.
 * @param qty_q Memory Areas quantities. Digital Outputs.
 * @param qty_iw Memory Areas quantities. Analog Inputs.
 * @param qty_qw Memory Areas quantities. Analog Outputs.
 * @param qty_c Memory Areas quantities. Counter registers (16 bits).
 * @param qty_t Memory Areas quantities. Timers.
 * @param qty_d Memory Areas quantities. Regular registers (16 bit signed).
 * @param qty_r Memory Areas quantities. Float or Real registers.
 * @return Error
 */
bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i,
        uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw, uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r);

/**
 * @fn bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx)
 * @brief Delete context
 *
 * @param ladder_ctx Ladder context
 * @return Error
 */
bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_task(void *ladderctx)
 * @brief Main task
 *
 * @param ladderctx void ladder context pointer
 */
void ladder_task(void *ladderctx);

/**
 * @fn void ladder_clear_program(ladder_ctx_t *ladder_ctx)
 * @brief Delete all networks
 *
 * @param ladder_ctx Ladder context
 */
void ladder_clear_program(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty)
 * @brief Add a foreign function
 *
 * @param ladder_ctx Ladder context
 * @param fn_init Initialize internal foreign function
 * @param init_data Generic foreign function data
 * @param qty Quantity
 * @return
 */
bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty);

/**
 * @fn bool ladder_fn_cell(ladder_ctx_t *ladder_ctx, uint32_t network, uint32_t row, uint32_t column, ladder_instruction_t function, uint32_t foreign_id)
 * @brief Add function and allocate data in cell
 *
 * @param ladder_ctx Ladder context
 * @param network Network
 * @param row Row
 * @param column Column
 * @param function Function
 * @param foreign_id Foreign function id
 * @return Status
 */
bool ladder_fn_cell(ladder_ctx_t *ladder_ctx, uint32_t network, uint32_t row, uint32_t column, ladder_instruction_t function, uint32_t foreign_id);

#endif /* LADDER_H */
