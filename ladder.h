/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef LADDER_H
#define LADDER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define LADDERLIB_VERSION_MAYOR 1 // indicate a really big change that can cause a lot of incompatibilities with previous versions
#define LADDERLIB_VERSION_MINOR 0 // indicate some change on API or opcode or very important correction in functionality
#define LADDERLIB_VERSION_PATCH 0 // indicate some minor change or correction

/**
 * @enum LADDER_INSTRUCTIONS
 * @brief Ladder Instructions codes
 * Must match with array of functions in TaskLadder
 */
typedef enum LADDER_INSTRUCTIONS {
    LADDER_INS_NOP,          /**< Instruction NOP */
    LADDER_INS_CONN,         /**< Instruction CONN */
    LADDER_INS_NEG,          /**< Instruction NEG */
    LADDER_INS_NO,           /**< Instruction NO */
    LADDER_INS_NC,           /**< Instruction NC */
    LADDER_INS_RE,           /**< Instruction RE */
    LADDER_INS_FE,           /**< Instruction FE */
    LADDER_INS_COIL,         /**< Instruction COIL */
    LADDER_INS_COILL,        /**< Instruction COILL */
    LADDER_INS_COILU,        /**< Instruction COILU */
    LADDER_INS_TON,          /**< Instruction TON */
    LADDER_INS_TOFF,         /**< Instruction TOFF */
    LADDER_INS_TP,           /**< Instruction TP */
    LADDER_INS_CTU,          /**< Instruction CTU */
    LADDER_INS_CTD,          /**< Instruction CTD */
    LADDER_INS_MOVE,         /**< Instruction MOVE */
    LADDER_INS_SUB,          /**< Instruction SUB */
    LADDER_INS_ADD,          /**< Instruction ADD */
    LADDER_INS_MUL,          /**< Instruction MUL */
    LADDER_INS_DIV,          /**< Instruction DIV */
    LADDER_INS_MOD,          /**< Instruction MOD */
    LADDER_INS_SHL,          /**< Instruction SHL */
    LADDER_INS_SHR,          /**< Instruction SHR */
    LADDER_INS_ROL,          /**< Instruction ROL */
    LADDER_INS_ROR,          /**< Instruction ROR */
    LADDER_INS_AND,          /**< Instruction AND */
    LADDER_INS_OR,           /**< Instruction OR */
    LADDER_INS_XOR,          /**< Instruction XOR */
    LADDER_INS_NOT,          /**< Instruction NOT */
    LADDER_INS_EQ,           /**< Instruction EQ */
    LADDER_INS_GT,           /**< Instruction GT */
    LADDER_INS_GE,           /**< Instruction GE */
    LADDER_INS_LT,           /**< Instruction LT */
    LADDER_INS_LE,           /**< Instruction LE */
    LADDER_INS_NE,           /**< Instruction NE */
    LADDER_INS_BAR,          /**< Instruction BAR */
    //...//
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
 * @enum LADDER_TYPE
 * @brief Data Types Codes
 * Must match with 'MnemonicsTypes' array
 */
typedef enum LADDER_TYPE {
    LADDER_TYPE_NONE, /**< Type None */
    LADDER_TYPE_M,    /**< Type M */
    LADDER_TYPE_Q,    /**< Type Q */
    LADDER_TYPE_I,    /**< Type I */
    LADDER_TYPE_Cd,   /**< Type Cd */
    LADDER_TYPE_Cr,   /**< Type Cr */
    LADDER_TYPE_Td,   /**< Type Td */
    LADDER_TYPE_Tr,   /**< Type Tr */
    LADDER_TYPE_IW,   /**< Type IW */
    LADDER_TYPE_QW,   /**< Type QW */
    LADDER_TYPE_C,    /**< Type C */
    LADDER_TYPE_T,    /**< Type T */
    LADDER_TYPE_D,    /**< Type D */
    LADDER_TYPE_K,    /**< Type K */
    LADDER_TYPE_R,    /**< Type R */
    LADDER_TYPE_KR,   /**< Type KR */
    LADDER_TYPE_INV,  /**< First invalid */
} ladder_type_t;

/**
 * @enum LADDER_BASETIME
 * @brief Base time for timers
 *
 */
typedef enum LADDER_BASETIME {
    LADDER_BASETIME_MS,   /**< LADDER_BASETIME_MS */
    LADDER_BASETIME_10MS, /**< LADDER_BASETIME_10MS */
    LADDER_BASETIME_100MS,/**< LADDER_BASETIME_100MS */
    LADDER_BASETIME_SEC,  /**< LADDER_BASETIME_SEC */
    LADDER_BASETIME_MIN,  /**< LADDER_BASETIME_MIN */
} ladder_basetime_t;

/**
 * @struct ladder_cell_s
 * @brief Type definition for Networks and global declarations
 *
 */
typedef struct ladder_cell_s {
    ladder_instruction_t code; /**< Code */
                 int16_t data; /**< Data */
                 uint8_t type; /**< Type */
} ladder_cell_t;

/**
 * @struct ladder_network_s
 * @brief
 *
 */
typedef struct ladder_network_s {
    ladder_cell_t **cells; /**< Cells */
         uint32_t *bars;   /**< Bars */
} ladder_network_t;

/**
 * @struct ladder_timer_s
 * @brief Timers
 *
 */
typedef struct ladder_timer_s {
    uint64_t time_stamp; /**< Time stamp */
    uint64_t acc;        /**< Activated counter */
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
         uint8_t net_columns; /**< Quantity of columns */
         uint8_t net_rows;    /**< Quantity of rows  */
    } quantity;
} ladder_t;

typedef struct ladder_ctx_s ladder_ctx_t;

/**
 * @fn void (*_read_inputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief Read local hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_read_inputs_local)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_write_outputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief Write local hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_write_outputs_local)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_read_inputs_remote)(ladder_ctx_t *ladder_ctx)
 * @brief Read remote hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_read_inputs_remote)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_write_outputs_remote)(ladder_ctx_t *ladder_ctx
 * @brief Write remote hardware values
 *
 * @param ladder_ctx_t Ladder context
 */
typedef void (*_write_outputs_remote)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_on_scan_end)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every scan cycle ant the end
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_scan_end)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_on_instruction)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every scan instruction
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_instruction)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_on_task_before)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every task cycle before scan
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_task_before)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_on_task_after)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every task cycle after scan
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_on_task_after)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_on_panic)(ladder_ctx_t *ladder_ctx)
 * @brief Panic management
 *
 * @param ladder_ctx Ladder context
 */
typedef void (*_on_panic)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_on_end_task)(void)
 * @brief Function for end task
 *
 */
typedef void (*_on_end_task)(void);

/**
 * @fn int (*_delay)(long msec)
 * @brief Delay in milliseconds
 *
 * @param msec Milliseconds
 * @return
 */
typedef int (*_delay)(long msec);

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
           _on_scan_end on_scan_end;    /**< Manage for every scan cycle */
        _on_instruction on_instruction; /**< Manage for every instruction call */
        _on_task_before on_task_before; /**< Manage for every task cycle before scan */
         _on_task_after on_task_after;  /**< Manage for every task cycle after scan */
              _on_panic on_panic;       /**< Manage panic status */
           _on_end_task on_end_task;    /**< Manage for end task */
    } manage;

    struct {
        _millis millis; /**< Milliseconds from system start */
         _delay delay;  /**< Delay in milliseconds */
    } time;
} ladder_hw_t;

/**
 * @struct ladder_memory_s
 * @brief Memory registers
 * Byte is used for Bits for increased performance of Scan time
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
    uint8_t *Crh; /**< Counter running previous */
    uint8_t *Cdh; /**< Counter done previous */
    uint8_t *Trh; /**< Timer running previous */
    uint8_t *Tdh; /**< Timer done previous */
} ladder_prev_scan_vals_t;

/**
 * @struct ladder_registers_s
 * @brief Registers
 *
 */
typedef struct ladder_registers_s {
    uint16_t *IW; /**< Analog Inputs (word) */
    uint16_t *QW; /**< Analog Outputs (word) */
    uint16_t *C;  /**< Counter registers (16 bits) */
     int16_t *D;  /**< Regular registers (signed 16 bits) */
       float *R;  /**< Floating point registers */
} ladder_registers_t;

/**
 * @struct plc_scan_internals_s
 * @brief Scan internals
 *
 */
typedef struct ladder_scan_internals_s {
         uint32_t *network_flags;   /**< Flags */
         uint64_t actual_scan_time; /**< Actual scan time */
         uint64_t start_time;       /**< Start time for calculate scan time */
} ladder_scan_internals_t;

/**
 * @struct ladder_ctx_s
 * @brief Ladder context
 *
 */
typedef struct ladder_ctx_s {
                 ladder_t ladder;         /**< Internals */
              ladder_hw_t hw;             /**< Hardware functions */
          ladder_memory_t memory;         /**< Memory */
  ladder_prev_scan_vals_t prev_scan_vals; /**< Previous scan values */
       ladder_registers_t registers;      /**< Registers */
           ladder_timer_t *timers;        /**< Timers */
  ladder_scan_internals_t scan_internals; /**< Scan internals */
         ladder_network_t *network;       /**< Networks */
         ladder_network_t *exec_network;  /**< Network in execution */
} ladder_ctx_t;

/**
 * @fn bool ladder_ctx_init(ladder_ctx_t*, uint8_t, uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
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
 * @return Status
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
 * @brief Deletes all networks (all values to 0)
 *
 * @param ladder_ctx Ladder context
 */
void ladder_clear_program(ladder_ctx_t *ladder_ctx);

#endif /* LADDER_H */
