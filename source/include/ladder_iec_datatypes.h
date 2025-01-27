/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *    ccronexpr (https://github.com/staticlibs/ccronexpr)
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

/**
 * @def ALLOW_64BITS
 * @brief allow 64 bits data types
 *
 */
//#define ALLOW_64BITS

#ifndef LADDER_IEC_DATATYPES_H_
#define LADDER_IEC_DATATYPES_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @enum IEC_DATATYPE_ERRORS
 * @brief errors
 *
 */
enum IEC_DATATYPE_ERRORS {
    IEC_DATATYPE_OK  = 0x00, /**< ok */
    IEC_DATATYPE_NAT = 0x01, /**< not allowed data type in operation */
    IEC_DATATYPE_TRN = 0x02, /**< result function on variable are truncated */
    IEC_DATATYPE_OOR = 0x03, /**< variable out of range (ex: string too long) */
    IEC_DATATYPE_TND = 0x04, /**< data type not defined */
    IEC_DATATYPE_NLL = 0x05, /**< parameter is NULL */
    IEC_DATATYPE_TRU = 0x06, /**< result is true */
    IEC_DATATYPE_FLS = 0x07, /**< result is false */
    IEC_DATATYPE_ENL = 0x08, /**< element not in list or search behind length */
    // ... //
    IEC_DATATYPE_ERR = 0xff, /**< generic error */
};

/**
 * @name bit manipulation
 * @brief
 *
 */
/**@{*/
#define GET_BIT(v, b)         ((v >> b) & 1)
#define SET_BIT(v, b)         ((v) | (1 << b))
#define CLR_BIT(v, b)         ((v) & ~(1 << b))
#define GET_BIT_VB(v, b)      (v & b) ? 1 : 0
#define SET_BIT_VB(v, b)      ((v) | b)
#define CLR_BIT_VB(v, b)      ((v) & ~b)
/**@}*/

/**
 * @name any types
 * @brief check any types
 *
 */
/**@{*/
#ifdef ALLOW_64BITS
#define ANY_BIT(x)            (x == IEC_T_BOOL || x == IEC_T_UINT || x == IEC_T_WORD || x == IEC_T_DWORD || x == IEC_T_LWORD)
#define ANY_UNSIGNED(x)       (x == IEC_T_USINT || x == IEC_T_UINT || x == IEC_T_UDINT || x == IEC_T_ULINT)
#define ANY_SIGNED(x)         (x == IEC_T_SINT || x == IEC_T_INT || x == IEC_T_DINT || x == IEC_T_LINT)
#define ANY_INT(x)            (x == IEC_T_SINT || x == IEC_T_INT || x == IEC_T_DINT || x == IEC_T_LINT || x == IEC_T_USINT || x == IEC_T_UINT \
                                || x == IEC_T_UDINT || x == IEC_T_ULINT)
#define ANY_DATE(x)           (x == IEC_T_DATE || x == IEC_T_DT)
#else
#define ANY_BIT(x)            (x == IEC_T_BOOL || x == IEC_T_UINT || x == IEC_T_WORD || x == IEC_T_DWORD || x == IEC_T_R_EDGE || x == IEC_T_F_EDGE)
#define ANY_UNSIGNED(x)       (x == IEC_T_USINT || x == IEC_T_UINT || x == IEC_T_UDINT)
#define ANY_SIGNED(x)         (x == IEC_T_SINT || x == IEC_T_INT || x == IEC_T_DINT)
#define ANY_INT(x)            (x == IEC_T_SINT || x == IEC_T_INT || x == IEC_T_DINT || x == IEC_T_USINT || x == IEC_T_UINT || x == IEC_T_UDINT)
#define ANY_DATE(x)           (x == IEC_T_DATE)
#endif
#define ANY_BOOL(x)           (x == IEC_T_BOOL || x == IEC_T_R_EDGE || x == IEC_T_F_EDGE)
#define ANY_REAL(x)           (x == IEC_T_REAL || x == IEC_T_LREAL)
#define ANY_NUM(x)            (ANY_INT(x) || ANY_REAL(x))
#define ANY_STRING(x)         (x == IEC_T_STRING || x == IEC_T_WSTRING)
#define ANY_ELEMENTARY(x)     (ANY_BIT(x) || ANY_NUM(x) || ANY_DATE(x) || ANY_STRING(x) || x == IEC_T_TIME)
#define ANY_MAGNITUDE(x)      ((ANY_INT(x) || ANY_REAL(x) || x == IEC_T_TIME) && !(x == IEC_T_NULL))
#define ANY_INTEGRAL(x)       (ANY_INT(x) || ANY_BIT(x))
#define ANY_CHAR(x)           (x == IEC_T_CHAR || x == IEC_T_WCHAR)
#define ANY_CHARS(x)          (ANY_CHAR(x) || ANY_STRING(x))
/**@}*/

/**
 * @name any type bit
 * @brief
 *
 */
/**@{*/
#define ANY_NUM_BIT           0x0001
#define ANY_DATE_BIT          0x0002
#define ANY_BIT_BIT           0x0004
#define ANY_REAL_BIT          0x0008
#define ANY_STRING_BIT        0x0010
#define ANY_ELEMENTARY_BIT    0x0020
#define ANY_MAGNITUDE_BIT     0x0040
#define ANY_INT_BIT           0x0080
#define ANY_INTEGRAL_BIT      0x0100
#define ANY_UNSIGNED_BIT      0x0200
#define ANY_SIGNED_BIT        0x0400
#define ANY_CHAR_BIT          0x0800
#define ANY_CHARS_BIT         0x1000
#define _BIT                  0
/**@}*/

/**
 * @name tt manipulation
 * @brief
 *
 */
/**@{*/
#define iec_is_mark(v)           (GET_BIT_VB((v)->tt, TT_MARK))
#define iec_is_persist(v)        (GET_BIT_VB((v)->tt, TT_PERSIST))
#define iec_is_retain(v)         (GET_BIT_VB((v)->tt, TT_RETAIN))
#define iec_is_maintain(v)       (GET_BIT_VB((v)->tt, TT_MAINTAIN))
#define iec_is_constant(v)       (GET_BIT_VB((v)->tt, TT_CONSTANT))
#define iec_is_initialized(v)    (GET_BIT_VB((v)->tt, TT_INITILZD))
#define iec_is_flag1(v)          (GET_BIT_VB((v)->tt, TT_FLAG1))
#define iec_is_flag2(v)          (GET_BIT_VB((v)->tt, TT_FLAG2))
#define iec_set_mark(v)          ((v)->tt = SET_BIT_VB((v)->tt, TT_MARK))
#define iec_set_persist(v)       ((v)->tt = SET_BIT_VB((v)->tt, TT_PERSIST))
#define iec_set_retain(v)        ((v)->tt = SET_BIT_VB((v)->tt, TT_RETAIN))
#define iec_set_maintain(v)      ((v)->tt = SET_BIT_VB((v)->tt, TT_MAINTAIN))
#define iec_set_constant(v)      ((v)->tt = SET_BIT_VB((v)->tt, TT_CONSTANT))
#define iec_set_initialized(v)   ((v)->tt = SET_BIT_VB((v)->tt, TT_INITILZD))
#define iec_set_flag1(v)         ((v)->tt = SET_BIT_VB((v)->tt, TT_FLAG1))
#define iec_set_flag2(v)         ((v)->tt = SET_BIT_VB((v)->tt, TT_FLAG2))
#define iec_unset_mark(v)        ((v)->tt = CLR_BIT_VB((v)->tt, TT_MARK))
#define iec_unset_persist(v)     ((v)->tt = CLR_BIT_VB((v)->tt, TT_PERSIST))
#define iec_unset_retain(v)      ((v)->tt = CLR_BIT_VB((v)->tt, TT_RETAIN))
#define iec_unset_maintain(v)    ((v)->tt = CLR_BIT_VB((v)->tt, TT_MAINTAIN))
#define iec_unset_constant(v)    ((v)->tt = CLR_BIT_VB((v)->tt, TT_CONSTANT))
#define iec_unset_initialized(v) ((v)->tt = CLR_BIT_VB((v)->tt, TT_INITILZD))
#define iec_unset_flag1(v)       ((v)->tt = CLR_BIT_VB((v)->tt, TT_FLAG1))
#define iec_unset_flag2(v)       ((v)->tt = CLR_BIT_VB((v)->tt, TT_FLAG2))
/**@}*/

/**
 * @def IEC_ANYTYPE
 * @brief mask for type
 *
 */
#define IEC_ANYTYPE(type)                  \
              (ANY_NUM(type))              \
            | (ANY_DATE(type)       << 1)  \
            | (ANY_BIT(type)        << 2)  \
            | (ANY_REAL(type)       << 3)  \
            | (ANY_STRING(type)     << 4)  \
            | (ANY_ELEMENTARY(type) << 5)  \
            | (ANY_MAGNITUDE(type)  << 6)  \
            | (ANY_INT(type)        << 7)  \
            | (ANY_INTEGRAL(type)   << 8)  \
            | (ANY_UNSIGNED(type)   << 9)  \
            | (ANY_SIGNED(type)     << 10) \
            | (ANY_CHAR(type)       << 11) \
            | (ANY_CHARS(type)      << 12)

/**
 * @def iec_anytype_allowed
 * @brief check if any_type if allowed
 *
 */
#define iec_anytype_allowed(data, A, B, C, D, E, F)                                                                             \
            if((data) == NULL || !((data)->any_type & (A ## _BIT | B ## _BIT | C ## _BIT | D ## _BIT | E ## _BIT | F ## _BIT))) \
                return IEC_DATATYPE_NAT

/**
 * @def iec_type_allowed
 * @brief check for type allowed
 *
 */
#define iec_type_allowed(data, A)                      \
            if(data == NULL || !((data)->type == (A))) \
                return IEC_DATATYPE_NAT

/**
 * @typedef iectype_t
 * @brief data types
 *
 */
typedef enum {
    IEC_T_NULL     = 0x00, /**< not value */
    IEC_T_BOOL     = 0x01, /**< bool */
    IEC_T_SINT     = 0x02, /**< int8_t */
    IEC_T_USINT    = 0x03, /**< uint8_t */
    IEC_T_BYTE     = 0x04, /**< uint8_t */
    IEC_T_INT      = 0x05, /**< int16_t */
    IEC_T_UINT     = 0x06, /**< uint16_t */
    IEC_T_WORD     = 0x07, /**< uint16_t */
    IEC_T_DINT     = 0x08, /**< int32_t */
    IEC_T_UDINT    = 0x09, /**< uint32_t */
    IEC_T_DWORD    = 0x0a, /**< uint32_t */
#ifdef ALLOW_64BITS
    IEC_T_LINT     = 0x0b, /**< int64_t */
    IEC_T_ULINT    = 0x0c, /**< uint64_t */
    IEC_T_LWORD    = 0x0d, /**< int64_t */
#else
    IEC_T_NDEF_0B  = 0x0b, /**< not defined */
    IEC_T_NDEF_0C  = 0x0c, /**< not defined */
    IEC_T_NDEF_0D  = 0x0d, /**< not defined */
#endif
    IEC_T_REAL     = 0x0e, /**< float */
    IEC_T_LREAL    = 0x0f, /**< double */
    IEC_T_TIME     = 0x10, /**< time_t */
    IEC_T_DATE     = 0x11, /**< date_t */
    IEC_T_TOD      = 0x12, /**< tod_t */
#ifdef ALLOW_64BITS
    IEC_T_DT       = 0x13, /**< dat_t */
#else
    IEC_T_NDEF_13  = 0x13, /**< not defined */
#endif
    IEC_T_CHAR     = 0x14, /**< char */
    IEC_T_WCHAR    = 0x15, /**< wchar */
    IEC_T_STRING   = 0x16, /**< string_t */
    IEC_T_WSTRING  = 0x17, /**< string_t */
#ifdef ALLOW_64BITS
    IEC_T_POINTER  = 0x18, /**< pointer_t */
#else
    IEC_T_NDEF_18  = 0x18, /**< not defined */
#endif
    IEC_T_TABLE    = 0x19, /**< table_t */
    IEC_T_USER     = 0x1a, /**< user_t */
    IEC_T_R_EDGE   = 0x1b, /**< bool */
    IEC_T_F_EDGE   = 0x1c, /**< bool */
    IEC_T_TIMER    = 0x1d, /**< timer_t */
    IEC_T_NDEF_1E  = 0x1e, /**< not defined */
    IEC_T_NDEF_1F  = 0x1f, /**< not defined */
} iectype_t;

/**
 * @typedef tt_t
 * @brief status flags
 *
 */
typedef enum {
    TT_MARK = 0x01,     /**< for gc */
    TT_PERSIST = 0x02,  /**< is persist */
    TT_RETAIN = 0x04,   /**< is retain */
    TT_MAINTAIN = 0x08, /**< is persist local */
    TT_CONSTANT = 0x10, /**< is constant */
    TT_INITILZD = 0x20, /**< value initialized */
    TT_FLAG1 = 0x40,    /**< generic flag*/
    TT_FLAG2 = 0x80     /**< generic flag*/
} tt_t;

/**
 * @typedef date_t
 * @brief
 *
 */
typedef union date_t {
    struct {
         uint8_t day;   /**< */
         uint8_t month; /**< */
        uint16_t year;  /**< */
    } v; /**< union */
    uint32_t dw_date;   /**< */
} date_t;

/**
 * @typedef tod_t
 * @brief
 *
 */
typedef union tod_t {
    struct {
        uint8_t csec; /**< */
        uint8_t sec;  /**< */
        uint8_t min;  /**< */
        uint8_t hour; /**< */
    } v; /**< elements */
    uint32_t dw_tod;  /**< */
} tod_t;

#ifdef ALLOW_64BITS
/**
 * @typedef dat_t
 * @brief
 *
 */
typedef union dat_t {
    struct {
         tod_t tod;  /**< */
        date_t date; /**< */
    } v; /**< elements */
    uint64_t dw_dat; /**< */
} dat_t;
#endif

/**
 * @typedef pointer_t
 * @brief
 *
 */
#ifdef ALLOW_64BITS
typedef uint64_t pointer_t;
#else
typedef uint32_t pointer_t;
#endif

/**
 * @typedef user_t
 * @brief
 *
 */
typedef struct user_t {
       void *data;  /**< */
    uint32_t value; /**< */
} user_t;

/**
 * @typedef string_t
 * @brief
 *
 */
typedef struct string_t {
      bool wstring; /**< true if wide character type */
    uint32_t len;   /**< string length*/
    uint32_t hash;  /**< string hash*/
    char *str;      /**< string pointer*/
} string_t;

/**
 * @typedef table_t
 * @brief
 *
 */
typedef struct table_t {
    uint32_t len;    /**< */
        void *table; /**< */
} table_t;

/**
 * @typedef t_timer_t
 * @brief
 *
 */
typedef struct t_timer_t {
        bool q;         /**< output */
      time_t pt;        /**< preset time */
      time_t et;        /**< elapsed time */
        bool timer_run; /**< timer is running */
#ifdef ALLOW_64BITS
    uint64_t t0;        /**< start internal time */
#else
    uint32_t t0;        /**< start internal time */
#endif
} t_timer_t;

/**
 * @typedef iec_t
 * @brief main data container
 *
 */
typedef struct iec_t {
    iectype_t type;     /**< data type */
         tt_t tt;       /**< data tt */
     uint16_t any_type; /**< any type flags */
         void *value;   /**< data value */
} *iec_t;

#endif /* LADDER_IEC_DATATYPES_H_ */
