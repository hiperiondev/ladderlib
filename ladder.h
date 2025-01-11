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
 * @brief
 *
 */
//#define DEBUG

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
 * @def LADDER_INS_CELL_USED_MASK
 * @brief
 *
 */
#define LADDER_INS_CELL_USED_MASK 0xf000

/**
 * @def LADDER_INS_CELL_CODE_MASK
 * @brief
 *
 */
#define LADDER_INS_CELL_CODE_MASK 0x0fff

/**
 * @enum LADDER_STATUS
 * @brief General status
 *
 */
enum LADDER_STATUS {
    LADDER_ST_STOPPED  = 0, /**< LADDER_ST_STOPPED */
    LADDER_ST_RUNNING  = 1, /**< LADDER_ST_RUNNING */
    LADDER_ST_ERROR    = 2, /**< LADDER_ST_ERROR */
    LADDER_ST_EXIT_TSK = 3, /**< LADDER_ST_EXIT_TSK */
    LADDER_ST_INV     = 20, /**< LADDER_ST_INV */
};

/**
 * @enum LADDER_TYPE
 * @brief Data Types Codes
 * Must match with 'MnemonicsTypes' array
 */
enum LADDER_TYPE {
    LADDER_TYPE_M   = 0,   /**< LADDER_TYPE_M */
    LADDER_TYPE_Q   = 1,   /**< LADDER_TYPE_Q */
    LADDER_TYPE_I   = 2,   /**< LADDER_TYPE_I */
    LADDER_TYPE_Cd  = 3,   /**< LADDER_TYPE_Cd */
    LADDER_TYPE_Cr  = 4,   /**< LADDER_TYPE_Cr */
    LADDER_TYPE_Td  = 5,   /**< LADDER_TYPE_Td */
    LADDER_TYPE_Tr  = 6,   /**< LADDER_TYPE_Tr */
    LADDER_TYPE_IW  = 7,   /**< LADDER_TYPE_IW */
    LADDER_TYPE_QW  = 8,   /**< LADDER_TYPE_QW */
    LADDER_TYPE_C   = 9,   /**< LADDER_TYPE_C */
    LADDER_TYPE_T   = 10,  /**< LADDER_TYPE_T */
    LADDER_TYPE_D   = 11,  /**< LADDER_TYPE_D */
    LADDER_TYPE_K   = 12,  /**< LADDER_TYPE_K */
    LADDER_TYPE_R   = 13,  /**< LADDER_TYPE_R */
    LADDER_TYPE_KR  = 14,  /**< LADDER_TYPE_KR */
    LADDER_TYPE_INV = 15,  /**< first invalid */
};

/**
 * @struct ladder_cell_s
 * @brief Type definition for Networks and global declarations
 *
 */
typedef struct ladder_cell_s {
    uint16_t code; /**< code */
     int16_t data; /**< data */
    uint16_t type; /**< type */
} ladder_cell_t;

/**
 * @struct ladder_network_s
 * @brief
 *
 */
typedef struct ladder_network_s {
    ladder_cell_t cells[NET_ROWS][NET_COLUMNS]; /**< */
         uint16_t bars[NET_COLUMNS - 1];        /**< */
} ladder_network_t;

/**
 * @struct ladder_memory_column_s
 * @brief
 *
 */
typedef struct ladder_memory_column_s {
    ladder_cell_t column[NET_ROWS]; /**< */
         uint16_t bar;              /**< */
} ladder_memory_column_t;

/**
 * @struct ladder_memory_row_s
 * @brief
 *
 */
typedef struct ladder_memory_row_s {
    ladder_cell_t row[NET_COLUMNS]; /**< */
         uint16_t bar;              /**< */
} ladder_memory_row_t;

/**
 * @struct ladder_timer_s
 * @brief Timers
 *
 */
typedef struct ladder_timer_s{
    unsigned long time_stamp; /**< 32bits same as unsigned int */
         uint16_t acc;        /**< */
} ladder_timer_t;

/**
 * @struct ladder_s
 * @brief
 *
 */
typedef struct ladder_s {
    uint16_t state;             /**< */
    uint16_t networks_quantity; /**< */
    uint32_t QTY_M;             /**< */
    uint32_t QTY_I;             /**< */
    uint32_t QTY_Q;             /**< */
    uint32_t QTY_IW;            /**< */
    uint32_t QTY_QW;            /**< */
    uint32_t QTY_C;             /**< */
    uint32_t QTY_T;             /**< */
    uint32_t QTY_D;             /**< */
    uint32_t QTY_R;             /**< */
    uint32_t total_networks;    /**< */
} ladder_t;

typedef struct ladder_ctx_s ladder_ctx_t;

/**
 * @fn void (*_read_inputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef void (*_read_inputs_local)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_write_outputs_local)(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef void (*_write_outputs_local)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_read_inputs_remote)(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef void (*_read_inputs_remote)(ladder_ctx_t *ladder_ctx);

/**
 * @fn void (*_write_outputs_remote)(ladder_ctx_t *ladder_ctx
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef void (*_write_outputs_remote)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_external_on_scan)(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef bool (*_external_on_scan)(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool (*_external_on_task)(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx_t
 */
typedef bool (*_external_on_task)(ladder_ctx_t *ladder_ctx);

/**
 * @fn int (*_delay)(long msec)
 * @brief
 *
 * @param msec
 * @return
 */
typedef int (*_delay)(long msec);

/**
 * @fn int64_t (*_micros)(void)
 * @brief
 *
 * @return
 */
typedef int64_t (*_micros)(void);

/**
 * @struct ladder_io_s
 * @brief
 *
 */
typedef struct ladder_io_s {
       _read_inputs_local read_inputs_local;    /**< */
     _write_outputs_local write_outputs_local;  /**< */
      _read_inputs_remote read_inputs_remote;   /**< */
    _write_outputs_remote write_outputs_remote; /**< */
        _external_on_scan external_on_scan;     /**< */
        _external_on_task external_on_task;     /**< */

    _micros micros; /**< */
     _delay delay;  /**< */
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
    uint8_t *Cr; /**< Counter running */
    uint8_t *Cd; /**< Counter done */
    uint8_t *Tr; /**< Timer running */
    uint8_t *Td; /**< Timer done */
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
         uint16_t flags_mask[NET_ROWS];                         /**< */
         uint16_t flags_mask_and[NET_ROWS];                     /**< */

         uint16_t ladder_network_flags[NET_COLUMNS - 1];        /**< */
         uint32_t ladder_actual_scan_time;                      /**< */
         uint32_t ladder_start_time;                            /**< */

         uint16_t ladder_network_flags_online[NET_COLUMNS - 1]; /**< */
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
 * @fn int ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
int ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r);

/**
 * @fn int ladder_ctx_deinit(ladder_ctx_t *ladder_ctx)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
int ladder_ctx_deinit(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_task(void *pvParameters)
 * @brief
 *
 * @param pvParameters
 */
void ladder_task(void *pvParameters);


/**
 * @fn void ladder_clear_program(ladder_ctx_t *ladder_ctx, ladder_network_t networks[])
 * @brief Deletes all Networks (all values to 0)
 *
 * @param ladder_ctx
 * @param networks
 */
void ladder_clear_program(ladder_ctx_t *ladder_ctx);

#endif /* LADDER_H */
