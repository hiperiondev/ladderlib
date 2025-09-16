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
#define LADDERLIB_VERSION_MAYOR 3

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
 * @def OPTIONAL_CRON
 * @brief Include CRON
 *
 */
#define OPTIONAL_CRON 1

/**
 * @def LADDER_VERTICAL_BAR
 * @brief Point to vertical bar flag
 *
 */
#define LADDER_VERTICAL_BAR(lctx, n, r, c) ((*lctx).network[n].cells[r][c].vertical_bar)

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
    LADDER_INS_TOF,     /**< Instruction TOFF */
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
    //...//
    LADDER_INS_INV,     /**< First invalid */
    LADDER_INS_MULTI,   /**< cell is a part of multi cell instruction */
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
 * @enum LADDER_DATA_TYPE
 * @brief Data types
 *
 */
typedef enum LADDER_DATA_TYPE {
    LADDER_DATATYPE_BOOL,     /**< Boolean */
    LADDER_DATATYPE_U8,       /**< Unsigned 8 bits */
    LADDER_DATATYPE_U16,      /**< Unsigned 16 bits */
    LADDER_DATATYPE_U32,      /**< Unsigned 32 bits */
    LADDER_DATATYPE_I8,       /**< Signed 8 bits */
    LADDER_DATATYPE_I16,      /**< Signed 16 bits */
    LADDER_DATATYPE_I32,      /**< Signed 32 bits */
    LADDER_DATATYPE_REAL,     /**< Float 32 bits */
    LADDER_DATATYPE_CSTR,     /**< Constant string */
    LADDER_DATATYPE_MOD_PORT, /**< Module.port format */
} ladder_data_type_t;

/**
 * @enum LADDER_BASETIME
 * @brief Basetime type
 *
 */
typedef enum LADDER_BASETIME {
    LADDER_BASETIME_MS,    /**< Basetime ms */
    LADDER_BASETIME_10MS,  /**< Basetime 10 ms */
    LADDER_BASETIME_100MS, /**< Basetime 100 ms */
    LADDER_BASETIME_SEC,   /**< Basetime seconds */
    LADDER_BASETIME_MIN,   /**< Basetime minutes */
} ladder_basetime_t;

/**
 * @enum LADDER_REGISTERS
 * @brief Registers
 *
 */
typedef enum LADDER_REGISTERS {
    LADDER_REGISTER_NONE, /**< Register None */
    LADDER_REGISTER_M,    /**< Register M */
    LADDER_REGISTER_Q,    /**< Register Q */
    LADDER_REGISTER_I,    /**< Register I */
    LADDER_REGISTER_Cd,   /**< Register Cd */
    LADDER_REGISTER_Cr,   /**< Register Cr */
    LADDER_REGISTER_Td,   /**< Register Td */
    LADDER_REGISTER_Tr,   /**< Register Tr */
    LADDER_REGISTER_IW,   /**< Register IW */
    LADDER_REGISTER_QW,   /**< Register QW */
    LADDER_REGISTER_C,    /**< Register C */
    LADDER_REGISTER_T,    /**< Register T */
    LADDER_REGISTER_D,    /**< Register D */
    LADDER_REGISTER_S,    /**< Register string */
    LADDER_REGISTER_R,    /**< Register real (float) */
    //...//
    LADDER_REGISTER_INV,  /**< Register invalid */
} ladder_register_t;

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
 * @struct moduleportvalue_s
 * @brief Module/Port value
 *
 */
typedef struct moduleportvalue_s {
    uint8_t module; /**< Module */
    uint8_t port;   /**< Port */
} moduleport_t;

/**
 * @struct ladder_value_s
 * @brief Value container
 *
 */
typedef struct ladder_value_s {
    ladder_register_t type; /**< Data type */
    union {
                bool b;     /**< Boolean */
             uint8_t u8;    /**< Unsigned integer 8 bits */
            uint16_t u16;   /**< Unsigned integer 16 bits */
            uint32_t u32;   /**< Unsigned integer 32 bits */
              int8_t i8;    /**< Signed integer 8 bits */
             int16_t i16;   /**< Signed integer 16 bits */
             int32_t i32;   /**< Signed integer 32 bits */
          const char *cstr; /**< Constant string */
               float real;  /**< Float */
        moduleport_t mp;    /**< Module.port value */
    } value; /**< Data */
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
        uint32_t c;           /**< Quantity of counters */
        uint32_t t;           /**< Quantity of timers */
        uint32_t d;           /**< Quantity of regular registers */
        uint32_t r;           /**< Quantity of floating point registers */
        uint32_t networks;    /**< Quantity of networks */
    } quantity;
} ladder_t;

typedef struct ladder_ctx_s ladder_ctx_t;

/**
 * @fn void (*_io_read)(ladder_ctx_t *ladder_ctx, uint32_t id)
 * @brief Read hardware values
 *
 * @param ladder_ctx_t Ladder context
 * @param id Function id
 */
typedef void (*_io_read)(ladder_ctx_t* ladder_ctx, uint32_t id);

/**
 * @fn void (*_io_write)(ladder_ctx_t *ladder_ctx, uint32_t id)
 * @brief Write hardware values
 *
 * @param ladder_ctx_t Ladder context
 * @param id Function id
 */
typedef void (*_io_write)(ladder_ctx_t* ladder_ctx, uint32_t id);

/**
 * @fn bool (*_io_init)(ladder_ctx_t* ladder_ctx, uint32_t id, bool init_deinit)
 * @brief Initialize/deinitialize io function
 *
 * @param ladder_ctx Ladder context
 * @param id Function id
 * @param init_deinit Initialize/deinitialize
 *
 * @return Status
 */
typedef bool (*_io_init)(ladder_ctx_t* ladder_ctx, uint32_t id, bool init_deinit);

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
               uint32_t fn_read_qty;  /**< Quantity of read functions */
               uint32_t fn_write_qty; /**< Quantity of read functions */
               _io_read *read;        /**< Read hardware values */
              _io_write *write;       /**< Write hardware values */
               _io_init *init_read;   /**< Initialize read functions */
               _io_init *init_write;  /**< Initialize write functions */
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
    uint32_t *C; /**< Counter registers */
     int32_t *D; /**< Regular registers */
       float *R; /**< Floating point registers */
} ladder_registers_t;

/**
 * @struct ladder_hw_input_vals_t
 * @brief Input scan values
 *
 */
typedef struct ladder_hw_input_vals_s {
    uint32_t fn_id;  /**< Function id */
    uint32_t i_qty;  /**< Digital inputs quantity */
    uint32_t iw_qty; /**< Analog inputs quantity */
    uint8_t *I;      /**< Digital inputs */
    int32_t *IW;     /**< Analog inputs */
    uint8_t *Ih;     /**< Digital inputs previous */
} ladder_hw_input_vals_t;

/**
 * @struct ladder_hw_output_vals_t
 * @brief Output scan values
 *
 */
typedef struct ladder_hw_output_vals_s {
    uint32_t fn_id;  /**< Function id */
    uint32_t q_qty;  /**< Digital outputs quantity*/
    uint32_t qw_qty; /**< Analog outputs quantity */
    uint8_t *Q;      /**< Digital outputs */
    int32_t *QW;     /**< Analog outputs */
    uint8_t *Qh;     /**< Digital outputs previous */
} ladder_hw_output_vals_t;

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
     ladder_hw_input_vals_t *input;         /**< Hw inputs */
    ladder_hw_output_vals_t *output;        /**< Hw outputs */
         ladder_registers_t registers;      /**< Registers */
             ladder_timer_t *timers;        /**< Timers */
    ladder_scan_internals_t scan_internals; /**< Scan internals */
           ladder_network_t *network;       /**< Networks */
           ladder_network_t *exec_network;  /**< Network in execution */
           ladder_foreign_t foreign;        /**< Foreign functions */
           #ifdef OPTIONAL_CRON
                      void *cron;           /*< Cron list */
           #endif
} ladder_ctx_t;

/**
 * @fn bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_c,
 *  uint32_t qty_t, uint32_t qty_d, uint32_t qty_r, bool init_netwok);
 * @brief Initialize context.
 *
 *
 * @param ladder_ctx Ladder context.
 * @param net_columns_qty Network Logic matrix columns size.
 * @param net_rows_qty Network Logic matrix rows size (Maximum: 32).
 * @param networks_qty Total Networks.
 * @param qty_m Memory Areas quantities. Marks. Regular flags.
 * @param qty_c Memory Areas quantities. Counter registers (16 bits).
 * @param qty_t Memory Areas quantities. Timers.
 * @param qty_d Memory Areas quantities. Regular registers (16 bit signed).
 * @param qty_r Memory Areas quantities. Float or Real registers.
 * @param init_netwok If false not initialize Networks
 * @return Error
 */
bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_c,
        uint32_t qty_t, uint32_t qty_d, uint32_t qty_r, bool init_netwok);

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
 * @fn bool ladder_add_read_fn(ladder_ctx_t*, _io_read read, _io_init read_init)
 * @brief Add read inputs function
 *
 * @param ladder_ctx Ladder context
 * @param read Read function
 * @param read_init Initialize/deinitialize read function
 * @return Status
 */
bool ladder_add_read_fn(ladder_ctx_t *ladder_ctx, _io_read read, _io_init read_init);

/**
 * @fn bool ladder_add_write_fn(ladder_ctx_t*, _io_write write, _io_init write_init)
 * @brief Add write outputs function
 *
 * @param ladder_ctx Ladder context
 * @param write Write function
 * @param write Initialize/deinitialize write function
 * @return Status
 */
bool ladder_add_write_fn(ladder_ctx_t *ladder_ctx, _io_write write, _io_init write_init);

/**
 * @fn bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty)
 * @brief Add a foreign function
 *
 * @param ladder_ctx Ladder context
 * @param fn_init Initialize internal foreign function
 * @param init_data Generic foreign function data
 * @param qty Quantity
 * @return Status
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
