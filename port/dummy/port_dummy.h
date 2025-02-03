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

#ifndef PORT_DUMMY_H_
#define PORT_DUMMY_H_

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

    void dummy_read_inputs_local(ladder_ctx_t *ladder_ctx);
    void dummy_write_outputs_local(ladder_ctx_t *ladder_ctx);
    void dummy_read_inputs_remote(ladder_ctx_t *ladder_ctx);
    void dummy_write_outputs_remote(ladder_ctx_t *ladder_ctx);
    bool dummy_external_on_scan(ladder_ctx_t *ladder_ctx);
    bool dummy_external_on_task(ladder_ctx_t *ladder_ctx);
    void dummy_panic(ladder_ctx_t *ladder_ctx);

     int dummy_delay(long msec);
uint32_t dummy_micros(void);

#endif /* PORT_DUMMY_H_ */
