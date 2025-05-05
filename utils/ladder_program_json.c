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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __linux__
#include <cjson/cJSON.h>
#else
#include "cJSON.h"
#endif

#include "ladder.h"
#include "ladder_program_json.h"

static const char *str_symbol[] = {
        "NOP",     //
        "CONN",    //
        "NEG",     //
        "NO",      //
        "NC",      //
        "RE",      //
        "FE",      //
        "COIL",    //
        "COILL",   //
        "COILU",   //
        "TON",     //
        "TOFF",    //
        "TP",      //
        "CTU",     //
        "CTD",     //
        "MOV",     //
        "SUB",     //
        "ADD",     //
        "MUL",     //
        "DIV",     //
        "MOD",     //
        "SHL",     //
        "SHR",     //
        "ROL",     //
        "ROR",     //
        "AND",     //
        "OR",      //
        "XOR",     //
        "NOT",     //
        "EQ",      //
        "GT",      //
        "GE",      //
        "LT",      //
        "LE",      //
        "NE",      //
        "FOREIGN", //
        "TMOV",    //
        "INV",     //
        "occupied" //
        };

static const char *str_types[] = {
        "NONE",  //
        "M",     //
        "Q",     //
        "I",     //
        "Cd",    //
        "Cr",    //
        "Td",    //
        "Tr",    //
        "IW",    //
        "QW",    //
        "C",     //
        "T",     //
        "D",     //
        "CSTR",  //
        "REAL",  //
        "INV",   //
        "MS",    //
        "10MS",  //
        "100MS", //
        "SEC",   //
        "MIN"    //
        };

static ladder_instruction_t get_instruction_code(const char *symbol) {
    for (int i = 0; i < sizeof(str_symbol) / sizeof(str_symbol[0]); i++) {
        if (strcmp(symbol, str_symbol[i]) == 0) {
            return (ladder_instruction_t) i;
        }
    }
    return LADDER_INS_INV;
}

static ladder_type_t get_type_code(const char *type) {
    for (int i = 0; i < sizeof(str_types) / sizeof(str_types[0]); i++) {
        if (strcmp(type, str_types[i]) == 0) {
            if (i > LADDER_TYPE_INV)
                return (ladder_type_t) (i - LADDER_TYPE_INV - 1 + 0xf0);
            return (ladder_type_t) i;
        }
    }
    return LADDER_TYPE_INV;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool ladder_json_to_program(const char *prg, ladder_ctx_t *ladder_ctx) {

    FILE *fp = fopen(prg, "r");
    if (!fp)
        return false;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json_string = malloc(size + 1);
    if (!json_string) {
        fclose(fp);
        return false;
    }
    fread(json_string, 1, size, fp);
    json_string[size] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(json_string);
    if (!root) {
        fprintf(stderr, "Error al analizar JSON\n");
        free(json_string);
        return false;
    }

    int network_count = cJSON_GetArraySize(root);

    (*ladder_ctx).network = malloc(network_count * sizeof(ladder_network_t));
    if (!(*ladder_ctx).network) {
        cJSON_Delete(root);
        free(json_string);
        return false;
    }

    for (int n = 0; n < network_count; n++) {
        cJSON *network_json = cJSON_GetArrayItem(root, n);
        ladder_network_t *network = &((*ladder_ctx).network[n]);
        network->enable = true;

        cJSON *rows_json = cJSON_GetObjectItem(network_json, "rows");
        network->rows = (int) cJSON_GetNumberValue(rows_json);
        cJSON *cols_json = cJSON_GetObjectItem(network_json, "cols");
        network->cols = (int) cJSON_GetNumberValue(cols_json);

        network->cells = malloc(network->rows * sizeof(ladder_cell_t*));
        for (int r = 0; r < network->rows; r++) {
            network->cells[r] = malloc(network->cols * sizeof(ladder_cell_t));
        }

        cJSON *networkData = cJSON_GetObjectItem(network_json, "networkData");
        for (int r = 0; r < network->rows; r++) {
            cJSON *row_json = cJSON_GetArrayItem(networkData, r);
            for (int c = 0; c < network->cols; c++) {
                cJSON *cell_json = cJSON_GetArrayItem(row_json, c);
                ladder_cell_t *cell = &network->cells[r][c];
                cell->state = false;

                cJSON *bar_json = cJSON_GetObjectItem(cell_json, "bar");
                cell->vertical_bar = cJSON_IsTrue(bar_json);

                cJSON *symbol_json = cJSON_GetObjectItem(cell_json, "symbol");
                char *symbol = cJSON_GetStringValue(symbol_json);
                cell->code = get_instruction_code(symbol);
                if(cell->code == LADDER_INS_INV)
                    return false;

                cJSON *data_json = cJSON_GetObjectItem(cell_json, "data");
                int data_qty = cJSON_GetArraySize(data_json);
                cell->data_qty = data_qty;
                cell->data = malloc(data_qty * sizeof(ladder_value_t));

                for (int d = 0; d < data_qty; d++) {
                    cJSON *data_item = cJSON_GetArrayItem(data_json, d);
                    cJSON *type_json = cJSON_GetObjectItem(data_item, "type");
                    char *type_str = cJSON_GetStringValue(type_json);
                    cell->data[d].type = get_type_code(type_str);;
                    if (cell->data[d].type == LADDER_TYPE_INV)
                        return false;

                    cJSON *value_json = cJSON_GetObjectItem(data_item, "value");
                    char *value_str = cJSON_GetStringValue(value_json);
                    if (cell->data[d].type == LADDER_TYPE_CSTR) {
                        cell->data[d].value.cstr = strdup(value_str);
                    } else if (cell->data[d].type == LADDER_TYPE_REAL) {
                        cell->data[d].value.real = atof(value_str);
                    } else {
                        cell->data[d].value.u32 = strtoul(value_str, NULL, 10);
                    }
                }
            }
        }
    }

    cJSON_Delete(root);
    free(json_string);

    return true;
}

bool ladder_program_to_json(const char *prg, ladder_ctx_t *ladder_ctx) {

    return LADDER_INS_ERR_OK;
}
