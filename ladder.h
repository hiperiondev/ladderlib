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
 * @def micros
 * @brief Microseconds from boot
 * (ex. esp_timer_get_time())
 */
#define micros() 0

/**
 * @def delay
 * @brief Delay milliseconds
 * (ex. vTaskDelay(x / ((TickType_t)1000 / 100))
 *
 */
#define delay(x) printf("%d",x)

/**
 * @def DEFAULT_PROGRAM
 * @brief Default program
 *
 */
#define DEFAULT_PROGRAM "defprg" //

/**
 * @def TOTAL_NETWORKS
 * @brief Total Networks.
 *     Max quantity tested = 500 (for 6 x 5 matrix)
 *     Max stable quantity = 400 (for 6 x 5 matrix)
 */
#define TOTAL_NETWORKS 400 //

/**
 * @def QTY_M
 * @brief Memory Areas quantities. Marks. Regular flags
 *
 */
#define QTY_M  2000

/**
 * @def QTY_I
 * @brief Memory Areas quantities. Digital Inputs
 *
 */
#define QTY_I  200

/**
 * @def QTY_Q
 * @brief Memory Areas quantities. Digital Outputs
 *
 */
#define QTY_Q  200

/**
 * @def QTY_IW
 * @brief Memory Areas quantities. Analog Inputs
 *
 */
#define QTY_IW 50

/**
 * @def QTY_QW
 * @brief Memory Areas quantities. Analog Outputs
 *
 */
#define QTY_QW 50

/**
 * @def QTY_C
 * @brief Memory Areas quantities. Counter registers (16 bits)
 *
 */
#define QTY_C  200

/**
 * @def QTY_T
 * @brief Memory Areas quantities. Timers
 *
 */
#define QTY_T  300

/**
 * @def QTY_D
 * @brief Memory Areas quantities. Regular registers (16 bit signed)
 *
 */
#define QTY_D  10000

/**
 * @def QTY_R
 * @brief Memory Areas quantities. Float or Real registers
 *
 */
#define QTY_R  2000

/**
 * @def IO_LOCAL_IN_QTY
 * @brief
 *
 */
#define IO_LOCAL_IN_QTY 8

/**
 * @def IO_LOCAL_OUT_QTY
 * @brief
 *
 */
#define IO_LOCAL_OUT_QTY 6

/**
 * @def IO_LOCAL_IN_ANALOG_QTY
 * @brief
 *
 */
#define IO_LOCAL_IN_ANALOG_QTY 2

/**
 * @def IO_LOCAL_OUT_ANALOG_QTY
 * @brief
 *
 */
#define IO_LOCAL_OUT_ANALOG_QTY 2

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
 * @enum PLC_STATUS
 * @brief General status
 *
 */
enum PLC_STATUS {
    PLC_ST_STOPPED = 0, /**< PLC_ST_STOPPED */
    PLC_ST_RUNNING = 1, /**< PLC_ST_RUNNING */
    PLC_ST_ERROR   = 2, /**< PLC_ST_ERROR */
    PLC_ST_INV     = 20,/**< PLC_ST_INV */
};

/**
 * @enum PLC_FIRMWARE
 * @brief Firmware Status
 *
 */
enum PLC_FIRMWARE {
    PLC_FIRMWARE_ACCEPTED = 0,/**< PLC_FIRMWARE_ACCEPTED */
    PLC_FIRMWARE_UPDATED  = 1,/**< PLC_FIRMWARE_UPDATED */
};

/**
 * @enum PLC_BOOT_STATUS
 * @brief Booting status
 *
 */
enum PLC_BOOT_STATUS {
    PLC_BOOT_INIT          = 0, /**< PLC_BOOT_INIT */
    PLC_BOOT_TASK_UNLOCKED = 30,/**< PLC_BOOT_TASK_UNLOCKED */
};

/**
 * @enum PLC_IO_TYPE
 * @brief IO Types
 *
 */
enum PLC_IO_TYPE {
    PLC_IO_TYPE_DIGITAL     = 0,/**< PLC_IO_TYPE_DIGITAL */
    PLC_IO_TYPE_ANALOG_0_5  = 1,/**< PLC_IO_TYPE_ANALOG_0_5 */
    PLC_IO_TYPE_ANALOG_0_10 = 2,/**< PLC_IO_TYPE_ANALOG_0_10 */
};

/**
 * @enum PLC_DATA_TYPE
 * @brief Data Types Codes
 * Must match with 'MnemonicsTypes' array
 */
enum PLC_DATA_TYPE {
    PLC_DT_M   = 0,   /**< PLC_DT_M */
    PLC_DT_Q   = 1,   /**< PLC_DT_Q */
    PLC_DT_I   = 2,   /**< PLC_DT_I */
    PLC_DT_Cd  = 3,   /**< PLC_DT_Cd */
    PLC_DT_Cr  = 4,   /**< PLC_DT_Cr */
    PLC_DT_Td  = 5,   /**< PLC_DT_Td */
    PLC_DT_Tr  = 6,   /**< PLC_DT_Tr */
    PLC_DT_IW  = 7,   /**< PLC_DT_IW */
    PLC_DT_QW  = 8,   /**< PLC_DT_QW */
    PLC_DT_C   = 9,   /**< PLC_DT_C */
    PLC_DT_T   = 10,  /**< PLC_DT_T */
    PLC_DT_D   = 11,  /**< PLC_DT_D */
    PLC_DT_K   = 12,  /**< PLC_DT_K */
    PLC_DT_R   = 13,  /**< PLC_DT_R */
    PLC_DT_KR  = 14,  /**< PLC_DT_KR */
    PLC_DT_INV = 15,  /**< first invalid */
};

/**
 * @enum PLC_INSTRUCTIONS
 * @brief Ladder Instructions codes
 * Must match with array of functions in TaskLadder
 */
enum PLC_INSTRUCTIONS {
    PLC_INS_NOP   = 0,  /**< PLC_INS_NOP */
    PLC_INS_CONN  = 1,  /**< PLC_INS_CONN */
    PLC_INS_NEG   = 2,  /**< PLC_INS_NEG */
    PLC_INS_NO    = 3,  /**< PLC_INS_NO */
    PLC_INS_NC    = 4,  /**< PLC_INS_NC */
    PLC_INS_RE    = 5,  /**< PLC_INS_RE */
    PLC_INS_FE    = 6,  /**< PLC_INS_FE */
    PLC_INS_COIL_ = 7,  /**< PLC_INS_COIL_ */
    PLC_INS_COILL = 8,  /**< PLC_INS_COILL */
    PLC_INS_COILU = 9,  /**< PLC_INS_COILU */
    PLC_INS_TON   = 10, /**< PLC_INS_TON */
    PLC_INS_TOFF  = 11, /**< PLC_INS_TOFF */
    PLC_INS_TP    = 12, /**< PLC_INS_TP */
    PLC_INS_CTU   = 13, /**< PLC_INS_CTU */
    PLC_INS_CTD   = 14, /**< PLC_INS_CTD */
    PLC_INS_MOVE  = 15, /**< PLC_INS_MOVE */
    PLC_INS_SUB   = 16, /**< PLC_INS_SUB */
    PLC_INS_ADD   = 17, /**< PLC_INS_ADD */
    PLC_INS_MUL   = 18, /**< PLC_INS_MUL */
    PLC_INS_DIV   = 19, /**< PLC_INS_DIV */
    PLC_INS_MOD   = 20, /**< PLC_INS_MOD */
    PLC_INS_SHL   = 21, /**< PLC_INS_SHL */
    PLC_INS_SHR   = 22, /**< PLC_INS_SHR */
    PLC_INS_ROL   = 23, /**< PLC_INS_ROL */
    PLC_INS_ROR   = 24, /**< PLC_INS_ROR */
    PLC_INS_AND   = 25, /**< PLC_INS_AND */
    PLC_INS_OR    = 26, /**< PLC_INS_OR */
    PLC_INS_XOR   = 27, /**< PLC_INS_XOR */
    PLC_INS_NOT   = 28, /**< PLC_INS_NOT */
    PLC_INS_EQ    = 29, /**< PLC_INS_EQ */
    PLC_INS_GT    = 30, /**< PLC_INS_GT */
    PLC_INS_GE    = 31, /**< PLC_INS_GE */
    PLC_INS_LT    = 32, /**< PLC_INS_LT */
    PLC_INS_LE    = 33, /**< PLC_INS_LE */
    PLC_INS_NE    = 34, /**< PLC_INS_NE */
    PLC_INS_INV   = 35, /**< first invalid */
    PLC_INS_BAR   = 99, /**< bar mnemonic */
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
 * @struct plc_timer_s
 * @brief Timers
 *
 */
typedef struct plc_timer_s{
    unsigned long time_stamp; /**< 32bits same as unsigned int */
         uint16_t acc;        /**< */
} plc_timer_t;

/**
 * @struct plc_settings_s
 * @brief
 *
 */
typedef struct plc_settings_s {
    uint16_t first_run;       /**< */
    uint16_t firmware;        /**< */
    uint16_t verbosity_level; /**< */
} plc_settings_t;

/**
 * @struct plc_ladder_s
 * @brief
 *
 */
typedef struct plc_ladder_s {
    uint16_t plc_state;         /**< */
    uint16_t networks_quantity; /**< */
} plc_ladder_t;

typedef struct plc_ctx_s plc_ctx_t;

/**
 * @fn void (*_read_inputs_local)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef void (*_read_inputs_local)(plc_ctx_t *plc_ctx);

/**
 * @fn void (*_write_outputs_local)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef void (*_write_outputs_local)(plc_ctx_t *plc_ctx);

/**
 * @fn void (*_configure_local_io)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef void (*_configure_local_io)(plc_ctx_t *plc_ctx);

/**
 * @fn uint16_t (*_analog_in_filter)(plc_ctx_t *plc_ctx, uint16_t index, uint16_t value)
 * @brief
 *
 * @param plc_ctx
 * @param index
 * @param value
 * @return
 */
typedef uint16_t (*_analog_in_filter)(plc_ctx_t *plc_ctx, uint16_t index, uint16_t value);

/**
 * @fn void (*_read_inputs_remote)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef void (*_read_inputs_remote)(plc_ctx_t *plc_ctx);

/**
 * @fn void (*_write_outputs_remote)(plc_ctx_t *plc_ctx
 * @brief
 *
 * @param plc_ctx
 */
typedef void (*_write_outputs_remote)(plc_ctx_t *plc_ctx);

/**
 * @fn bool (*_external_on_scan)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef bool (*_external_on_scan)(plc_ctx_t *plc_ctx);

/**
 * @fn bool (*_external_on_task)(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 */
typedef bool (*_external_on_task)(plc_ctx_t *plc_ctx);

/**
 * @fn int (*_load_program)(const char *filename)
 * @brief
 *
 * @param filename
 * @return
 */
typedef int (*_load_program)(const char *filename);

/**
 * @fn int (*_save_program)(const char *filename)
 * @brief
 *
 * @param filename
 * @return
 */
typedef int (*_save_program)(const char *filename);

/**
 * @struct plc_io_s
 * @brief
 *
 */
typedef struct plc_io_s {
    uint8_t local_inputs[IO_LOCAL_IN_QTY];          /**< */
    uint8_t local_outputs[IO_LOCAL_OUT_QTY];        /**< */
    struct {
           _read_inputs_local read_inputs_local;    /**< */
         _write_outputs_local write_outputs_local;  /**< */
            _analog_in_filter analog_in_filter;     /**< */
          _read_inputs_remote read_inputs_remote;   /**< */
        _write_outputs_remote write_outputs_remote; /**< */
          _configure_local_io configure_local_io;   /**< */
            _external_on_scan external_on_scan;     /**< */
            _external_on_task external_on_task;     /**< */
                _load_program load_program;         /**< */
                _save_program save_program;         /**< */
    } fn;                                           /**< */
} plc_io_t;

/**
 * @struct plc_memory_s
 * @brief Memory types definitions
 * Byte is used for Bits for increased performance of Scan time
 */
typedef struct plc_memory_s {
    uint8_t M[QTY_M];  /**< Regular flags */
    uint8_t I[QTY_I];  /**< Digital Inputs */
    uint8_t Q[QTY_Q];  /**< Digital Outputs */
    uint8_t Cr[QTY_C]; /**< Counter running */
    uint8_t Cd[QTY_C]; /**< Counter done */
    uint8_t Tr[QTY_T]; /**< Timer running */
    uint8_t Td[QTY_T]; /**< Timer done */
} plc_memory_t;

/**
 * @struct plc_prev_scan_vals_s
 * @brief Bits Previous Scan Values (history) for FE and RE instructions
 *
 */
typedef struct plc_prev_scan_vals_s {
    uint8_t Mh[QTY_M];  /**< */
    uint8_t Ih[QTY_I];  /**< */
    uint8_t Qh[QTY_Q];  /**< */
    uint8_t Crh[QTY_C]; /**< */
    uint8_t Cdh[QTY_C]; /**< */
    uint8_t Trh[QTY_T]; /**< */
    uint8_t Tdh[QTY_T]; /**< */
} plc_prev_scan_vals_t;

/**
 * @struct plc_registers_s
 * @brief Registers declaration
 *
 */
typedef struct plc_registers_s {
    uint16_t IW[QTY_IW]; /**< Analog Inputs  (word) */
    uint16_t QW[QTY_QW]; /**< Analog Outputs (word) */
    uint16_t C[QTY_C];   /**< Counter registers (16 bits) */
     int16_t D[QTY_D];   /**< Regular registers (signed 16 bits) */
       float R[QTY_R];   /**< Floating point registers */
} plc_registers_t;

/**
 * @struct plc_internals_s
 * @brief
 *
 */
typedef struct plc_internals_s {
         uint16_t boot_sequence;                                /**< Manage the boot sequence between tasks */
         uint16_t load_selected_program;                        /**< */
         uint16_t update_selected_program_ram;                  /**< */
         uint16_t update_selected_program_disk;                 /**< */

         uint16_t flags_mask[NET_ROWS];                         /**< */
         uint16_t flags_mask_and[NET_ROWS];                     /**< */

         uint16_t ladder_network_flags[NET_COLUMNS - 1];        /**< */
    unsigned long ladder_actual_scan_time;                      /**< */
    unsigned long ladder_start_time;                            /**< */

         uint16_t ladder_network_flags_online[NET_COLUMNS - 1]; /**< */
} plc_internals_t;

/**
 * @struct plc_ctx_s
 * @brief
 *
 */
typedef struct plc_ctx_s {
          plc_settings_t settings;                 /**< */
            plc_ladder_t ladder;                   /**< */
                plc_io_t io;                       /**< */
            plc_memory_t memory;                   /**< */
    plc_prev_scan_vals_t prev_scan_vals;           /**< */
         plc_registers_t registers;                /**< */
             plc_timer_t timers[QTY_T];            /**< */
         plc_internals_t internals;                /**< */
        ladder_network_t networks[TOTAL_NETWORKS]; /**< */
        ladder_network_t exec_network;             /**< network in execution */
} plc_ctx_t;

/**
 * @fn int ladder_ctx_init(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 * @return
 */
int ladder_ctx_init(plc_ctx_t *plc_ctx);

/**
 * @fn int ladder_ctx_deinit(plc_ctx_t *plc_ctx)
 * @brief
 *
 * @param plc_ctx
 * @return
 */
int ladder_ctx_deinit(plc_ctx_t *plc_ctx);

/**
 * @fn void ladder_task(void *pvParameters)
 * @brief
 *
 * @param pvParameters
 */
void ladder_task(void *pvParameters);

#endif /* LADDER_H */
