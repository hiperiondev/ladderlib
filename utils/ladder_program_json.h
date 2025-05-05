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

#ifndef LADDER_PROGRAM_PARSER_H
#define LADDER_PROGRAM_PARSER_H

#include "ladder.h"

typedef enum JSON_ERROR {
    JSON_ERROR_OK,              //
    JSON_ERROR_OPENFILE,        //
    JSON_ERROR_PARSE,           //
    JSON_ERROR_ALLOC_STRING,    //
    JSON_ERROR_ALLOC_NETWORK,   //
    JSON_ERROR_INS_INV,         //
    JSON_ERROR_TYPE_INV,        //
    JSON_ERROR_CREATEARRAY,     //
    JSON_ERROR_CREATENETOBJT,   //
    JSON_ERROR_CREATENETDATA,   //
    JSON_ERROR_CREATEROWARRAY,  //
    JSON_ERROR_CREATECELLOBJ,   //
    JSON_ERROR_CREATEDATAARRAY, //
    JSON_ERROR_CREATEDATAOBJ,   //
    JSON_ERROR_PRINTOBJ,        //
    JSON_ERROR_COMPACTFILE,     //
    JSON_ERROR_WRITEFILE,       //
    //////////////////////////////
    JSON_ERROR_FAIL             //

} ladder_json_error_t;

/**
 * @fn ladder_json_error_t ladder_json_to_program(const char *prg, ladder_ctx_t* ladder_ctx)
 * @brief
 *
 * @param prg prg file name of JSON program
 * @param ladder_ctx
 * @return Status
 */
ladder_json_error_t ladder_json_to_program(const char *prg, ladder_ctx_t *ladder_ctx);

/**
 * @fn ladder_json_error_t ladder_program_to_json(const char *prg, ladder_ctx_t* ladder_ctx)
 * @brief
 *
 * @param prg prg file name of JSON program
 * @param ladder_ctx
 * @return Status
 */
ladder_json_error_t ladder_program_to_json(const char *prg, ladder_ctx_t *ladder_ctx);

/**
 * @fn ladder_json_error_t ladder_compact_json_file(const char *input_path, const char *output_path)
 * @brief
 *
 * @param input_path Input file
 * @param output_path Output_file
 * @return Status
 */
ladder_json_error_t ladder_compact_json_file(const char *input_path, const char *output_path);


#endif /* LADDER_PROGRAM_PARSER_H */
