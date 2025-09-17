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

static const char *str_symbol[] = { "NOP", //
        "CONN", //
        "NEG", //
        "NO", //
        "NC", //
        "RE", //
        "FE", //
        "COIL", //
        "COILL", //
        "COILU", //
        "TON", //
        "TOFF", //
        "TP", //
        "CTU", //
        "CTD", //
        "MOV", //
        "SUB", //
        "ADD", //
        "MUL", //
        "DIV", //
        "MOD", //
        "SHL", //
        "SHR", //
        "ROL", //
        "ROR", //
        "AND", //
        "OR", //
        "XOR", //
        "NOT", //
        "EQ", //
        "GT", //
        "GE", //
        "LT", //
        "LE", //
        "NE", //
        "FOREIGN", //
        "TMOV", //
        "INV", //
        "occupied" //
        };

static const char *str_types[] = { "NONE", //
        "M", //
        "Q", //
        "I", //
        "Cd", //
        "Cr", //
        "Td", //
        "Tr", //
        "IW", //
        "QW", //
        "C", //
        "T", //
        "D", //
        "CSTR", //
        "REAL", //
        "INV", //
        };

static const char *str_basetime[] = { "MS", //
        "10MS", //
        "100MS", //
        "SEC", //
        "MIN" //
        };

static bool parse_module_port(const char *str, moduleport_t *result) {
    unsigned int n;
    unsigned int module_temp, port_temp;
    int ret = sscanf(str, "%u.%u%n", &module_temp, &port_temp, &n);
    if (ret == 2 && str[n] == '\0' && module_temp <= 255 && port_temp <= 255) {
        result->module = (uint8_t) module_temp;
        result->port = (uint8_t) port_temp;

        return true;
    }

    return false;
}

static ladder_instruction_t get_instruction_code(const char *symbol) {
    for (int i = 0; i < sizeof(str_symbol) / sizeof(str_symbol[0]); i++) {
        if (strcmp(symbol, str_symbol[i]) == 0) {
            return (ladder_instruction_t) i;
        }
    }
    return LADDER_INS_INV;
}

static ladder_register_t get_register_code(const char *type) {
    for (int i = 0; i < sizeof(str_types) / sizeof(str_types[0]); i++)
        if (strcmp(type, str_types[i]) == 0)
            return i;

    for (int i = 0; i < sizeof(str_basetime) / sizeof(str_basetime[0]); i++)
        if (strcmp(type, str_basetime[i]) == 0)
            return i;

    return LADDER_REGISTER_INV;
}

static char* read_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file)
        return NULL;
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char*) malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    fclose(file);
    return content;
}

static int write_file(const char *path, const char *content) {
    FILE *file = fopen(path, "w");
    if (!file)
        return 0;
    fprintf(file, "%s", content);
    fclose(file);
    return 1;
}

static bool validate_json(cJSON *json, cJSON *schema) {
    if (cJSON_IsBool(schema)) {
        return cJSON_IsTrue(schema);
    }

    if (!cJSON_IsObject(schema)) {
        return false; // Invalid schema
    }

    // Handle if-then
    cJSON *if_schema = cJSON_GetObjectItemCaseSensitive(schema, "if");
    cJSON *then_schema = cJSON_GetObjectItemCaseSensitive(schema, "then");
    if (if_schema && then_schema) {
        if (validate_json(json, if_schema)) {
            return validate_json(json, then_schema);
        } else {
            return true;
        }
    }

    // Handle allOf
    cJSON *allOf = cJSON_GetObjectItemCaseSensitive(schema, "allOf");
    if (allOf && cJSON_IsArray(allOf)) {
        cJSON *sub = allOf->child;
        while (sub) {
            if (!validate_json(json, sub)) {
                return false;
            }
            sub = sub->next;
        }
    }

    // Get type
    cJSON *type = cJSON_GetObjectItemCaseSensitive(schema, "type");
    const char *typestr = NULL;
    if (type && cJSON_IsString(type)) {
        typestr = type->valuestring;
    }

    // Check type if specified
    bool type_ok = true;
    if (typestr) {
        if (strcmp(typestr, "null") == 0) {
            type_ok = cJSON_IsNull(json);
        } else if (strcmp(typestr, "boolean") == 0) {
            type_ok = cJSON_IsBool(json);
        } else if (strcmp(typestr, "number") == 0) {
            type_ok = cJSON_IsNumber(json);
        } else if (strcmp(typestr, "integer") == 0) {
            type_ok = cJSON_IsNumber(json) && (json->valuedouble == (double) ((int) json->valuedouble));
        } else if (strcmp(typestr, "string") == 0) {
            type_ok = cJSON_IsString(json);
        } else if (strcmp(typestr, "array") == 0) {
            type_ok = cJSON_IsArray(json);
        } else if (strcmp(typestr, "object") == 0) {
            type_ok = cJSON_IsObject(json);
        } else {
            type_ok = false;
        }
    }
    if (!type_ok) {
        return false;
    }

    // General keywords: enum, const
    cJSON *enum_arr = cJSON_GetObjectItemCaseSensitive(schema, "enum");
    if (enum_arr && cJSON_IsArray(enum_arr)) {
        bool found = false;
        cJSON *e = enum_arr->child;
        while (e) {
            if (cJSON_Compare(json, e, 1)) {
                found = true;
                break;
            }
            e = e->next;
        }
        if (!found) {
            return false;
        }
    }

    cJSON *const_val = cJSON_GetObjectItemCaseSensitive(schema, "const");
    if (const_val) {
        if (!cJSON_Compare(json, const_val, 1)) {
            return false;
        }
    }

    // Type-specific or general validations
    if (typestr && (strcmp(typestr, "number") == 0 || strcmp(typestr, "integer") == 0)) {
        cJSON *min = cJSON_GetObjectItemCaseSensitive(schema, "minimum");
        if (min && cJSON_IsNumber(min) && json->valuedouble < min->valuedouble) {
            return false;
        }
        cJSON *max = cJSON_GetObjectItemCaseSensitive(schema, "maximum");
        if (max && cJSON_IsNumber(max) && json->valuedouble > max->valuedouble) {
            return false;
        }
    }

    if (typestr && strcmp(typestr, "string") == 0) {
        cJSON *minlen = cJSON_GetObjectItemCaseSensitive(schema, "minLength");
        if (minlen && cJSON_IsNumber(minlen) && strlen(json->valuestring) < (size_t) minlen->valueint) {
            return false;
        }
        cJSON *maxlen = cJSON_GetObjectItemCaseSensitive(schema, "maxLength");
        if (maxlen && cJSON_IsNumber(maxlen) && strlen(json->valuestring) > (size_t) maxlen->valueint) {
            return false;
        }
    }

    if (typestr && strcmp(typestr, "array") == 0) {
        cJSON *items = cJSON_GetObjectItemCaseSensitive(schema, "items");
        if (items) {
            if (cJSON_IsArray(items)) {
                // Tuple validation
                int schema_size = cJSON_GetArraySize(items);
                int json_size = cJSON_GetArraySize(json);
                for (int i = 0; i < schema_size; ++i) {
                    if (i >= json_size) {
                        break;
                    }
                    cJSON *item_schema = cJSON_GetArrayItem(items, i);
                    cJSON *item_json = cJSON_GetArrayItem(json, i);
                    if (!validate_json(item_json, item_schema)) {
                        return false;
                    }
                }
                // additionalItems
                cJSON *additems = cJSON_GetObjectItemCaseSensitive(schema, "additionalItems");
                bool allow_add = true;
                if (additems && cJSON_IsFalse(additems)) {
                    allow_add = false;
                }
                if (!allow_add && json_size > schema_size) {
                    return false;
                }
            } else {
                // Uniform items
                cJSON *item = json->child;
                while (item) {
                    if (!validate_json(item, items)) {
                        return false;
                    }
                    item = item->next;
                }
            }
        }
        cJSON *minitems = cJSON_GetObjectItemCaseSensitive(schema, "minItems");
        if (minitems && cJSON_IsNumber(minitems) && cJSON_GetArraySize(json) < minitems->valueint) {
            return false;
        }
        cJSON *maxitems = cJSON_GetObjectItemCaseSensitive(schema, "maxItems");
        if (maxitems && cJSON_IsNumber(maxitems) && cJSON_GetArraySize(json) > maxitems->valueint) {
            return false;
        }
    }

    if (typestr && strcmp(typestr, "object") == 0) {
        cJSON *props = cJSON_GetObjectItemCaseSensitive(schema, "properties");
        if (props && cJSON_IsObject(props)) {
            cJSON *prop = props->child;
            while (prop) {
                const char *key = prop->string;
                cJSON *subjson = cJSON_GetObjectItemCaseSensitive(json, key);
                bool is_required = false;
                cJSON *required = cJSON_GetObjectItemCaseSensitive(schema, "required");
                if (required && cJSON_IsArray(required)) {
                    cJSON *req_item = required->child;
                    while (req_item) {
                        if (cJSON_IsString(req_item) && strcmp(req_item->valuestring, key) == 0) {
                            is_required = true;
                            break;
                        }
                        req_item = req_item->next;
                    }
                }
                if (!subjson) {
                    if (is_required) {
                        return false;
                    }
                    prop = prop->next;
                    continue;
                }
                if (!validate_json(subjson, prop)) {
                    return false;
                }
                prop = prop->next;
            }
        }
        // additionalProperties
        cJSON *addprops = cJSON_GetObjectItemCaseSensitive(schema, "additionalProperties");
        bool allow_additional = true;
        if (addprops && cJSON_IsFalse(addprops)) {
            allow_additional = false;
        }
        if (!allow_additional && cJSON_IsObject(json)) {
            cJSON *json_prop = json->child;
            while (json_prop) {
                const char *key = json_prop->string;
                if (!props || !cJSON_HasObjectItem(props, key)) {
                    return false;
                }
                json_prop = json_prop->next;
            }
        }
    }

    if (!typestr) {
        // No type specified, apply relevant keywords based on json type
        // enum and const already handled above

        // Number keywords
        if (cJSON_IsNumber(json)) {
            cJSON *min = cJSON_GetObjectItemCaseSensitive(schema, "minimum");
            if (min && cJSON_IsNumber(min) && json->valuedouble < min->valuedouble) {
                return false;
            }
            cJSON *max = cJSON_GetObjectItemCaseSensitive(schema, "maximum");
            if (max && cJSON_IsNumber(max) && json->valuedouble > max->valuedouble) {
                return false;
            }
        }

        // String keywords
        if (cJSON_IsString(json)) {
            cJSON *minlen = cJSON_GetObjectItemCaseSensitive(schema, "minLength");
            if (minlen && cJSON_IsNumber(minlen) && strlen(json->valuestring) < (size_t) minlen->valueint) {
                return false;
            }
            cJSON *maxlen = cJSON_GetObjectItemCaseSensitive(schema, "maxLength");
            if (maxlen && cJSON_IsNumber(maxlen) && strlen(json->valuestring) > (size_t) maxlen->valueint) {
                return false;
            }
        }

        // Array keywords
        if (cJSON_IsArray(json)) {
            cJSON *items = cJSON_GetObjectItemCaseSensitive(schema, "items");
            if (items) {
                if (cJSON_IsArray(items)) {
                    // Tuple
                    int schema_size = cJSON_GetArraySize(items);
                    int json_size = cJSON_GetArraySize(json);
                    for (int i = 0; i < schema_size; ++i) {
                        if (i >= json_size) {
                            break;
                        }
                        cJSON *item_schema = cJSON_GetArrayItem(items, i);
                        cJSON *item_json = cJSON_GetArrayItem(json, i);
                        if (!validate_json(item_json, item_schema)) {
                            return false;
                        }
                    }
                    cJSON *additems = cJSON_GetObjectItemCaseSensitive(schema, "additionalItems");
                    bool allow_add = true;
                    if (additems && cJSON_IsFalse(additems)) {
                        allow_add = false;
                    }
                    if (!allow_add && json_size > schema_size) {
                        return false;
                    }
                } else {
                    // Uniform
                    cJSON *item = json->child;
                    while (item) {
                        if (!validate_json(item, items)) {
                            return false;
                        }
                        item = item->next;
                    }
                }
            }
            cJSON *minitems = cJSON_GetObjectItemCaseSensitive(schema, "minItems");
            if (minitems && cJSON_IsNumber(minitems) && cJSON_GetArraySize(json) < minitems->valueint) {
                return false;
            }
            cJSON *maxitems = cJSON_GetObjectItemCaseSensitive(schema, "maxItems");
            if (maxitems && cJSON_IsNumber(maxitems) && cJSON_GetArraySize(json) > maxitems->valueint) {
                return false;
            }
        }

        // Object keywords
        if (cJSON_IsObject(json)) {
            cJSON *props = cJSON_GetObjectItemCaseSensitive(schema, "properties");
            if (props && cJSON_IsObject(props)) {
                cJSON *prop = props->child;
                while (prop) {
                    const char *key = prop->string;
                    cJSON *subjson = cJSON_GetObjectItemCaseSensitive(json, key);
                    bool is_required = false;
                    cJSON *required = cJSON_GetObjectItemCaseSensitive(schema, "required");
                    if (required && cJSON_IsArray(required)) {
                        cJSON *req_item = required->child;
                        while (req_item) {
                            if (cJSON_IsString(req_item) && strcmp(req_item->valuestring, key) == 0) {
                                is_required = true;
                                break;
                            }
                            req_item = req_item->next;
                        }
                    }
                    if (!subjson) {
                        if (is_required) {
                            return false;
                        }
                        prop = prop->next;
                        continue;
                    }
                    if (!validate_json(subjson, prop)) {
                        return false;
                    }
                    prop = prop->next;
                }
            }
            // additionalProperties
            cJSON *addprops = cJSON_GetObjectItemCaseSensitive(schema, "additionalProperties");
            bool allow_additional = true;
            if (addprops && cJSON_IsFalse(addprops)) {
                allow_additional = false;
            }
            if (!allow_additional) {
                cJSON *json_prop = json->child;
                while (json_prop) {
                    const char *key = json_prop->string;
                    if (!props || !cJSON_HasObjectItem(props, key)) {
                        return false;
                    }
                    json_prop = json_prop->next;
                }
            }
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

ladder_json_error_t ladder_json_to_program(const char *prg, ladder_ctx_t *ladder_ctx) {

    FILE *fp = fopen(prg, "r");
    if (!fp) {
        return JSON_ERROR_OPENFILE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json_string = malloc(size + 1);
    if (!json_string) {
        fclose(fp);
        return JSON_ERROR_ALLOC_STRING;
    }
    fread(json_string, 1, size, fp);
    json_string[size] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(json_string);
    if (!root) {
        free(json_string);
        return JSON_ERROR_PARSE;
    }

    (*ladder_ctx).ladder.quantity.networks = cJSON_GetArraySize(root);

    (*ladder_ctx).network = calloc((*ladder_ctx).ladder.quantity.networks, sizeof(ladder_network_t));
    if (!(*ladder_ctx).network) {
        cJSON_Delete(root);
        free(json_string);
        return JSON_ERROR_ALLOC_NETWORK;
    }

    for (int n = 0; n < (*ladder_ctx).ladder.quantity.networks; n++) {
        cJSON *network_json = cJSON_GetArrayItem(root, n);
        ladder_network_t *network = &((*ladder_ctx).network[n]);
        network->enable = true;

        cJSON *rows_json = cJSON_GetObjectItem(network_json, "rows");
        network->rows = (int) cJSON_GetNumberValue(rows_json);
        cJSON *cols_json = cJSON_GetObjectItem(network_json, "cols");
        network->cols = (int) cJSON_GetNumberValue(cols_json);

        network->cells = calloc(network->rows, sizeof(ladder_cell_t*));
        for (int r = 0; r < network->rows; r++) {
            network->cells[r] = calloc(network->cols, sizeof(ladder_cell_t));
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
                if (cell->code == LADDER_INS_INV)
                    return JSON_ERROR_INS_INV;

                cJSON *data_json = cJSON_GetObjectItem(cell_json, "data");
                int data_qty = cJSON_GetArraySize(data_json);
                cell->data_qty = data_qty;
                cell->data = calloc(data_qty, sizeof(ladder_value_t));

                for (int d = 0; d < data_qty; d++) {
                    cJSON *data_item = cJSON_GetArrayItem(data_json, d);
                    cJSON *type_json = cJSON_GetObjectItem(data_item, "type");
                    char *type_str = cJSON_GetStringValue(type_json);
                    cell->data[d].type = get_register_code(type_str);

                    if (cell->data[d].type == LADDER_REGISTER_INV)
                        return JSON_ERROR_TYPE_INV;

                    cJSON *value_json = cJSON_GetObjectItem(data_item, "value");
                    char *value_str = cJSON_GetStringValue(value_json);

                    if (cell->code == LADDER_INS_TON || cell->code == LADDER_INS_TOF || cell->code == LADDER_INS_TP) {
                        cell->data[d].value.u32 = strtoul(value_str, NULL, 10);
                    } else {
                        switch (cell->data[d].type) {
                            case LADDER_REGISTER_I:
                            case LADDER_REGISTER_Q:
                                if (!parse_module_port(value_str, &(cell->data[d].value.mp)))
                                    return JSON_ERROR_INVALIDVALUE;
                                break;

                            case LADDER_REGISTER_S:
                                cell->data[d].value.cstr = strdup(value_str);
                                break;
                            case LADDER_REGISTER_R:
                                cell->data[d].value.real = atof(value_str);
                                break;

                            default:
                                cell->data[d].value.u32 = strtoul(value_str, NULL, 10);
                                break;
                        }
                    }
                }
            }
        }
    }

    cJSON_Delete(root);
    free(json_string);

    return JSON_ERROR_OK;
}

ladder_json_error_t ladder_program_to_json(const char *prg, ladder_ctx_t *ladder_ctx) {
    FILE *fp = fopen(prg, "w");
    if (fp == NULL) {
        return JSON_ERROR_OPENFILE;
    }

    cJSON *root = cJSON_CreateArray();
    if (root == NULL) {
        fclose(fp);
        return JSON_ERROR_CREATEARRAY;
    }

    for (uint32_t n = 0; n < (*ladder_ctx).ladder.quantity.networks; n++) {
        cJSON *network_obj = cJSON_CreateObject();
        if (network_obj == NULL) {
            cJSON_Delete(root);
            fclose(fp);
            return JSON_ERROR_CREATENETOBJT;
        }

        cJSON_AddNumberToObject(network_obj, "id", n);
        cJSON_AddNumberToObject(network_obj, "rows", (*ladder_ctx).network[n].rows);
        cJSON_AddNumberToObject(network_obj, "cols", (*ladder_ctx).network[n].cols);

        cJSON *networkData = cJSON_CreateArray();
        if (networkData == NULL) {
            cJSON_Delete(network_obj);
            cJSON_Delete(root);
            fclose(fp);
            return JSON_ERROR_CREATENETDATA;
        }

        for (uint32_t r = 0; r < (*ladder_ctx).network[n].rows; r++) {
            cJSON *row_array = cJSON_CreateArray();
            if (row_array == NULL) {
                cJSON_Delete(networkData);
                cJSON_Delete(network_obj);
                cJSON_Delete(root);
                fclose(fp);
                return JSON_ERROR_CREATEROWARRAY;
            }

            for (uint32_t c = 0; c < (*ladder_ctx).network[n].cols; c++) {
                ladder_cell_t *cell = &((*ladder_ctx).network[n].cells[r][c]);
                cJSON *cell_obj = cJSON_CreateObject();
                if (cell_obj == NULL) {
                    cJSON_Delete(row_array);
                    cJSON_Delete(networkData);
                    cJSON_Delete(network_obj);
                    cJSON_Delete(root);
                    fclose(fp);
                    return JSON_ERROR_CREATECELLOBJ;
                }

                const char *symbol = (cell->code < sizeof(str_symbol) / sizeof(str_symbol[0])) ? str_symbol[cell->code] : "INV";
                cJSON_AddStringToObject(cell_obj, "symbol", symbol);
                cJSON_AddBoolToObject(cell_obj, "bar", cell->vertical_bar);

                cJSON *data_array = cJSON_CreateArray();
                if (data_array == NULL) {
                    cJSON_Delete(cell_obj);
                    cJSON_Delete(row_array);
                    cJSON_Delete(networkData);
                    cJSON_Delete(network_obj);
                    cJSON_Delete(root);
                    fclose(fp);
                    return JSON_ERROR_CREATEDATAARRAY;
                }

                for (uint8_t d = 0; d < cell->data_qty; d++) {
                    ladder_value_t *val = &cell->data[d];
                    cJSON *data_obj = cJSON_CreateObject();
                    if (data_obj == NULL) {
                        cJSON_Delete(data_array);
                        cJSON_Delete(cell_obj);
                        cJSON_Delete(row_array);
                        cJSON_Delete(networkData);
                        cJSON_Delete(network_obj);
                        cJSON_Delete(root);
                        fclose(fp);
                        return JSON_ERROR_CREATEDATAOBJ;
                    }

                    const char *name = "";
                    switch (cell->code) {
                        case LADDER_INS_NO:
                        case LADDER_INS_NC:
                        case LADDER_INS_RE:
                        case LADDER_INS_FE:
                        case LADDER_INS_COIL:
                        case LADDER_INS_COILL:
                        case LADDER_INS_COILU:
                        case LADDER_INS_NEG:
                        case LADDER_INS_NOT:
                        case LADDER_INS_SHL:
                        case LADDER_INS_SHR:
                        case LADDER_INS_ROL:
                        case LADDER_INS_ROR:
                            if (d == 0)
                                name = "value";
                            if (d == 1 && (cell->code == LADDER_INS_SHL || cell->code == LADDER_INS_SHR))
                                name = "n";
                            if (d == 1 && (cell->code == LADDER_INS_NOT || cell->code == LADDER_INS_ROL || cell->code == LADDER_INS_ROR))
                                name = "result";
                            if (d == 2)
                                name = "result";
                            break;
                        case LADDER_INS_TON:
                        case LADDER_INS_TOF:
                        case LADDER_INS_TP:
                            if (d == 0)
                                name = "timer";
                            if (d == 1)
                                name = "basetime";
                            break;
                        case LADDER_INS_CTU:
                        case LADDER_INS_CTD:
                            if (d == 0)
                                name = "counter";
                            if (d == 1)
                                name = "preset value";
                            break;
                        case LADDER_INS_MOVE:
                            if (d == 0)
                                name = "value";
                            if (d == 1)
                                name = "to";
                            break;
                        case LADDER_INS_SUB:
                        case LADDER_INS_ADD:
                        case LADDER_INS_MUL:
                        case LADDER_INS_DIV:
                        case LADDER_INS_MOD:
                        case LADDER_INS_AND:
                        case LADDER_INS_OR:
                        case LADDER_INS_XOR:
                            if (d == 0)
                                name = "value1";
                            if (d == 1)
                                name = "value2";
                            if (d == 2)
                                name = "result";
                            break;
                        case LADDER_INS_EQ:
                        case LADDER_INS_GT:
                        case LADDER_INS_GE:
                        case LADDER_INS_LT:
                        case LADDER_INS_LE:
                        case LADDER_INS_NE:
                            if (d == 0)
                                name = "value1";
                            if (d == 1)
                                name = "value2";
                            break;
                        case LADDER_INS_TMOVE:
                            if (d == 0)
                                name = "value1";
                            if (d == 1)
                                name = "value2";
                            if (d == 2)
                                name = "to";
                            break;
                        default:
                            name = "value";
                            break;
                    }
                    cJSON_AddStringToObject(data_obj, "name", name);

                    const char *type_str;
                    if (val->type != LADDER_REGISTER_NONE) {
                        if ((cell->code == LADDER_INS_TON || cell->code == LADDER_INS_TOF || cell->code == LADDER_INS_TP) && d == 1) {
                            type_str = (val->type < sizeof(str_basetime) / sizeof(str_basetime[0])) ? str_basetime[val->type] : "INV";
                        } else {
                            type_str = (val->type < sizeof(str_types) / sizeof(str_types[0])) ? str_types[val->type] : "INV";
                        }
                    } else {
                        if (strcmp(name, "preset value") == 0) {
                            type_str = "NONE";
                        } else {
                            type_str = "K";
                        }
                    }
                    cJSON_AddStringToObject(data_obj, "type", type_str);

                    char value_str[32];
                    if ((cell->code == LADDER_INS_TON || cell->code == LADDER_INS_TOF || cell->code == LADDER_INS_TP) && d == 1) {
                        snprintf(value_str, sizeof(value_str), "%lu", (unsigned long) val->value.u32);
                    } else {
                        switch (val->type) {
                            case LADDER_REGISTER_I:
                            case LADDER_REGISTER_Q:
                                snprintf(value_str, sizeof(value_str), "%u.%u", val->value.mp.module, val->value.mp.port);
                                break;
                            case LADDER_REGISTER_S:
                                snprintf(value_str, sizeof(value_str), "%s", val->value.cstr ? val->value.cstr : "");
                                break;
                            case LADDER_REGISTER_R:
                                snprintf(value_str, sizeof(value_str), "%f", val->value.real);
                                break;
                            default:
                                snprintf(value_str, sizeof(value_str), "%lu", (unsigned long) val->value.u32);
                                break;
                        }
                    }

                    cJSON_AddStringToObject(data_obj, "value", value_str);
                    cJSON_AddItemToArray(data_array, data_obj);
                }

                cJSON_AddItemToObject(cell_obj, "data", data_array);
                cJSON_AddItemToArray(row_array, cell_obj);
            }

            cJSON_AddItemToArray(networkData, row_array);
        }

        cJSON_AddItemToObject(network_obj, "networkData", networkData);
        cJSON_AddItemToArray(root, network_obj);
    }

    char *json_str = cJSON_Print(root);
    if (json_str == NULL) {
        cJSON_Delete(root);
        fclose(fp);
        return JSON_ERROR_PRINTOBJ;
    }

    fprintf(fp, "%s", json_str);
    free(json_str);
    cJSON_Delete(root);
    fclose(fp);
    return JSON_ERROR_OK;
}

ladder_json_error_t ladder_compact_json_file(const char *input_path, const char *output_path) {
    char *json_str = read_file(input_path);
    if (!json_str) {
        return JSON_ERROR_OPENFILE;
    }
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        free(json_str);
        return JSON_ERROR_PARSE;
    }
    char *compact_json = cJSON_PrintUnformatted(root);
    if (!compact_json) {
        cJSON_Delete(root);
        free(json_str);
        return JSON_ERROR_COMPACTFILE;
    }
    if (!write_file(output_path, compact_json)) {
        free(compact_json);
        cJSON_Delete(root);
        free(json_str);
        return JSON_ERROR_WRITEFILE;
    }

    free(compact_json);
    cJSON_Delete(root);
    free(json_str);

    return JSON_ERROR_OK;
}

bool ladder_validate_json_file(const char *json_file, const char *schema_file) {
    char *json_str = read_file(json_file);
    if (!json_str)
        return false;

    char *schema_str = read_file(schema_file);
    if (!schema_str) {
        free(json_str);
        return false;
    }

    cJSON *json = cJSON_Parse(json_str);
    free(json_str);
    if (!json) {
        free(schema_str);
        return false;
    }

    cJSON *schema = cJSON_Parse(schema_str);
    free(schema_str);
    if (!schema) {
        cJSON_Delete(json);
        return false;
    }

    bool result = validate_json(json, schema);

    cJSON_Delete(json);
    cJSON_Delete(schema);

    return result;
}
