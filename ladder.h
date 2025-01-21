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

/**
 * @def DEBUG
 * @brief Enable debug mode
 *
 */
//#define DEBUG

#ifdef DEBUG
extern const char *ladder_state_str[];
extern const char *ladder_type_str[];
#endif

/**
 * @def NET_COLUMNS
 * @brief Network Logic matrix columns size
 *
 */
#define NET_COLUMNS 6

/**
 * @def NET_ROWS
 * @brief Network Logic matrix rows size
 *
 */
#define NET_ROWS 5

/**
 * @def LADDER_INS_MASK
 * @brief Masked cell. For use on multiple cell instruction
 *
 */
#define LADDER_INS_MASK 0xf000

/**
 * @enum LADDER_INSTRUCTIONS
 * @brief Ladder Instructions codes
 * Must match with array of functions in TaskLadder
 */
typedef enum LADDER_INSTRUCTIONS {
    LADDER_INS_NOP,   /**< Instruction NOP */
    LADDER_INS_CONN,  /**< Instruction CONN */
    LADDER_INS_NEG,   /**< Instruction NEG */
    LADDER_INS_NO,    /**< Instruction NO */
    LADDER_INS_NC,    /**< Instruction NC */
    LADDER_INS_RE,    /**< Instruction RE */
    LADDER_INS_FE,    /**< Instruction FE */
    LADDER_INS_COIL,  /**< Instruction COIL */
    LADDER_INS_COILL, /**< Instruction COILL */
    LADDER_INS_COILU, /**< Instruction COILU */
    LADDER_INS_TON,   /**< Instruction TON */
    LADDER_INS_TOFF,  /**< Instruction TOFF */
    LADDER_INS_TP,    /**< Instruction TP */
    LADDER_INS_CTU,   /**< Instruction CTU */
    LADDER_INS_CTD,   /**< Instruction CTD */
    LADDER_INS_MOVE,  /**< Instruction MOVE */
    LADDER_INS_SUB,   /**< Instruction SUB */
    LADDER_INS_ADD,   /**< Instruction ADD */
    LADDER_INS_MUL,   /**< Instruction MUL */
    LADDER_INS_DIV,   /**< Instruction DIV */
    LADDER_INS_MOD,   /**< Instruction MOD */
    LADDER_INS_SHL,   /**< Instruction SHL */
    LADDER_INS_SHR,   /**< Instruction SHR */
    LADDER_INS_ROL,   /**< Instruction ROL */
    LADDER_INS_ROR,   /**< Instruction ROR */
    LADDER_INS_AND,   /**< Instruction AND */
    LADDER_INS_OR,    /**< Instruction OR */
    LADDER_INS_XOR,   /**< Instruction XOR */
    LADDER_INS_NOT,   /**< Instruction NOT */
    LADDER_INS_EQ,    /**< Instruction EQ */
    LADDER_INS_GT,    /**< Instruction GT */
    LADDER_INS_GE,    /**< Instruction GE */
    LADDER_INS_LT,    /**< Instruction LT */
    LADDER_INS_LE,    /**< Instruction LE */
    LADDER_INS_NE,    /**< Instruction NE */
    LADDER_INS_INV,   /**< First invalid */
    LADDER_INS_BAR,   /**< Bar mnemonic */
} ladder_instruction_t;

/**
 * @enum LADDER_STATUS
 * @brief General status
 *
 */
enum LADDER_STATUS {
    LADDER_ST_STOPPED,  /**< Stopped */
    LADDER_ST_RUNNING,  /**< Running */
    LADDER_ST_ERROR,    /**< Error */
    LADDER_ST_EXIT_TSK, /**< Exit task */
    LADDER_ST_INV,      /**< Invalid */
};

/**
 * @enum LADDER_TYPE
 * @brief Data Types Codes
 * Must match with 'MnemonicsTypes' array
 */
typedef enum LADDER_TYPE {
    LADDER_TYPE_M,   /**< Type M */
    LADDER_TYPE_Q,   /**< Type Q */
    LADDER_TYPE_I,   /**< Type I */
    LADDER_TYPE_Cd,  /**< Type Cd */
    LADDER_TYPE_Cr,  /**< Type Cr */
    LADDER_TYPE_Td,  /**< Type Td */
    LADDER_TYPE_Tr,  /**< Type Tr */
    LADDER_TYPE_IW,  /**< Type IW */
    LADDER_TYPE_QW,  /**< Type QW */
    LADDER_TYPE_C,   /**< Type C */
    LADDER_TYPE_T,   /**< Type T */
    LADDER_TYPE_D,   /**< Type D */
    LADDER_TYPE_K,   /**< Type K */
    LADDER_TYPE_R,   /**< Type R */
    LADDER_TYPE_KR,  /**< Type KR */
    LADDER_TYPE_INV, /**< first invalid */
} ladder_type_t;

/**
 * @struct ladder_cell_s
 * @brief Type definition for Networks and global declarations
 *
 */
typedef struct ladder_cell_s {
    ladder_instruction_t code; /**< Code */
                 int16_t data; /**< Data */
           ladder_type_t type; /**< Type */
} ladder_cell_t;

/**
 * @struct ladder_network_s
 * @brief
 *
 */
typedef struct ladder_network_s {
    ladder_cell_t cells[NET_ROWS][NET_COLUMNS]; /**< Cells */
         uint16_t bars[NET_COLUMNS - 1];        /**< Bars */
} ladder_network_t;

/**
 * @struct ladder_timer_s
 * @brief Timers
 *
 */
typedef struct ladder_timer_s {
    uint32_t time_stamp; /**< */
    uint16_t acc;        /**< */
} ladder_timer_t;

/**
 * @struct ladder_s
 * @brief
 *
 */
typedef struct ladder_s {
    uint16_t state;                  /**< State */
     uint8_t last_instr;             /**< Last executed instruction */
    uint32_t last_instr_network;     /**< Last executed network */
    uint32_t last_instr_cell_column; /**< Last executed cell column */
    uint32_t last_instr_cell_row;    /**< Last executed cell row */
     uint8_t last_instr_err;         /**< Last executed error */
    uint32_t qty_m;                  /**< Quantity of regular flags */
    uint32_t qty_i;                  /**< Quantity of digital inputs */
    uint32_t qty_q;                  /**< Quantity of digital outputs */
    uint32_t qty_iw;                 /**< Quantity of analog inputs */
    uint32_t qty_qw;                 /**< Quantity of analog outputs */
    uint32_t qty_c;                  /**< Quantity of counters */
    uint32_t qty_t;                  /**< Quantity of timers */
    uint32_t qty_d;                  /**< Quantity of regular registers */
    uint32_t qty_r;                  /**< Quantity of floating point registers */
    uint32_t total_networks;         /**< Quantity of networks */
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
 * @fn bool (*_external_on_scan)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every scan cycle
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_external_on_scan)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_external_on_task)(ladder_ctx_t *ladder_ctx)
 * @brief Manage for every task cycle
 *
 * @param ladder_ctx_t Ladder context
 */
typedef bool (*_external_on_task)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_panic)(ladder_ctx_t *ladder_ctx)
 * @brief Panic management
 *
 * @param ladder_ctx Ladder context
 */
typedef void (*_panic)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_end_task)(void)
 * @brief Function for end task
 *
 */
typedef void (*_end_task)(void);

/**
 * @fn int (*_delay)(long msec)
 * @brief Delay in milliseconds
 *
 * @param msec Milliseconds
 * @return
 */
typedef int (*_delay)(long msec);

/**
 * @fn int32_t (*_micros)(void)
 * @brief Microseconds from system start
 *
 * @return
 */
typedef int32_t (*_micros)(void);

/**
 * @struct ladder_io_s
 * @brief
 *
 */
typedef struct ladder_io_s {
       _read_inputs_local read_inputs_local;    /**< Read local hardware values */
     _write_outputs_local write_outputs_local;  /**< Write local hardware values */
      _read_inputs_remote read_inputs_remote;   /**< Read remote hardware values */
    _write_outputs_remote write_outputs_remote; /**< Write local hardware values */
        _external_on_scan external_on_scan;     /**< Manage for every scan cycle */
        _external_on_task external_on_task;     /**< Manage for every task cycle */
                   _panic panic;                /**< Manage panic state*/
                _end_task end_task;             /**< Function for end task */

    _micros micros; /**< Microseconds from system start */
     _delay delay;  /**< Delay in milliseconds */
} ladder_io_t;

/**
 * @struct ladder_memory_s
 * @brief Memory types definitions
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
 * @brief Bits Previous Scan Values (history) for FE and RE instructions
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
 * @brief Registers declaration
 *
 */
typedef struct ladder_registers_s {
    uint16_t *IW; /**< Analog Inputs  (word) */
    uint16_t *QW; /**< Analog Outputs (word) */
    uint16_t *C;  /**< Counter registers (16 bits) */
     int16_t *D;  /**< Regular registers (signed 16 bits) */
       float *R;  /**< Floating point registers */
} ladder_registers_t;

/**
 * @struct plc_internals_s
 * @brief
 *
 */
typedef struct ladder_internals_s {
         uint16_t flags_mask[NET_ROWS];                  /**< */
         uint16_t ladder_network_flags[NET_COLUMNS - 1]; /**< */
         uint32_t ladder_actual_scan_time;               /**< */
         uint32_t ladder_start_time;                     /**< */
} ladder_internals_t;

/**
 * @struct ladder_ctx_s
 * @brief
 *
 */
typedef struct ladder_ctx_s {
                 ladder_t ladder;                   /**< */
              ladder_io_t io;                       /**< */
          ladder_memory_t memory;                   /**< */
  ladder_prev_scan_vals_t prev_scan_vals;           /**< */
       ladder_registers_t registers;                /**< */
           ladder_timer_t *timers;                  /**< */
       ladder_internals_t internals;                /**< */
         ladder_network_t *network;                 /**< */
         ladder_network_t exec_network;             /**< network in execution */
} ladder_ctx_t;

/**
 * @fn bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @return Status
 */
bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r);

/**
 * @fn bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @return Status
 */
bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_task(void *parameters)
 * @brief Main task
 *
 * @param parameters  void ladder context pointer
 */
void ladder_task(void *parameters);

/**
 * @fn void ladder_clear_program(ladder_ctx_t *ladder_ctx)
 * @brief Deletes all networks (all values to 0)
 *
 * @param ladder_ctx Ladder context
 */
void ladder_clear_program(ladder_ctx_t *ladder_ctx);

#endif /* LADDER_H */
