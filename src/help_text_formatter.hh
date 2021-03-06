/**
 * Copyright (c) 2017, Timothy Stack
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * * Neither the name of Timothy Stack nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LNAV_HELP_TEXT_FORMATTER_HH
#define LNAV_HELP_TEXT_FORMATTER_HH

#include <map>
#include <string>
#include <vector>

#include "attr_line.hh"

enum help_context_t {
    HC_NONE,
    HC_PARAMETER,
    HC_COMMAND,
    HC_SQL_KEYWORD,
    HC_SQL_FUNCTION,
};

enum help_nargs_t {
    HN_REQUIRED,
    HN_OPTIONAL,
    HN_ZERO_OR_MORE,
    HN_ONE_OR_MORE,
};

enum help_parameter_format_t {
    HPF_STRING,
    HPF_REGEX,
    HPF_INTEGER,
    HPF_NUMBER,
    HPF_DATETIME,
    HPF_ENUM,
};

struct help_example {
    const char *he_cmd;
    attr_line_t he_result;
};

struct help_text {
    help_context_t ht_context{HC_NONE};
    const char *ht_name;
    const char *ht_summary;
    const char *ht_flag_name{nullptr};
    const char *ht_description{nullptr};
    std::vector<struct help_text> ht_parameters;
    std::vector<struct help_example> ht_example;
    help_nargs_t ht_nargs{HN_REQUIRED};
    help_parameter_format_t ht_format{HPF_STRING};
    std::vector<const char *> ht_enum_values;
    std::vector<const char *> ht_tags;
    std::vector<const char *> ht_opposites;

    help_text() {

    };

    help_text(const char *name, const char *summary = nullptr)
        : ht_name(name),
          ht_summary(summary) {
        if (name[0] == ':') {
            this->ht_context = HC_COMMAND;
            this->ht_name = &name[1];
        }
    };

    help_text &command() {
        this->ht_context = HC_COMMAND;
        return *this;
    };

    help_text &sql_function() {
        this->ht_context = HC_SQL_FUNCTION;
        return *this;
    };

    help_text &sql_keyword() {
        this->ht_context = HC_SQL_KEYWORD;
        return *this;
    };

    help_text &with_summary(const char *summary) {
        this->ht_summary = summary;
        return *this;
    };

    help_text &with_flag_name(const char *flag) {
        this->ht_flag_name = flag;
        return *this;
    }

    help_text &with_parameters(const std::initializer_list<help_text> &params) {
        this->ht_parameters = params;
        for (auto &param : this->ht_parameters) {
            param.ht_context = HC_PARAMETER;
        }
        return *this;
    }

    help_text &with_parameter(const help_text &ht) {
        this->ht_parameters.emplace_back(ht);
        this->ht_parameters.back().ht_context = HC_PARAMETER;
        return *this;
    };

    help_text &with_examples(const std::initializer_list<help_example> &examples) {
        this->ht_example = examples;
        return *this;
    }

    help_text &with_example(const help_example &example) {
        this->ht_example.emplace_back(example);
        return *this;
    }

    help_text &optional() {
        this->ht_nargs = HN_OPTIONAL;
        return *this;
    };

    help_text &zero_or_more() {
        this->ht_nargs = HN_ZERO_OR_MORE;
        return *this;
    };

    help_text &one_or_more() {
        this->ht_nargs = HN_ONE_OR_MORE;
        return *this;
    };

    help_text &with_format(help_parameter_format_t format) {
        this->ht_format = format;
        return *this;
    }

    help_text &with_enum_values(const std::initializer_list<const char*> &enum_values) {
        this->ht_enum_values = enum_values;
        return *this;
    };

    help_text &with_tags(const std::initializer_list<const char*> &tags) {
        this->ht_tags = tags;
        return *this;
    };

    help_text &with_opposites(const std::initializer_list<const char*> &opps) {
        this->ht_opposites = opps;
        return *this;
    };

    void index_tags() {
        for (const auto &tag: this->ht_tags) {
            TAGGED.insert(std::make_pair(tag, this));
        }
    };

    static std::multimap<std::string, help_text *> TAGGED;
};

void format_help_text_for_term(const help_text &ht, int width, attr_line_t &out, bool synopsis_only = false);
void format_example_text_for_term(const help_text &ht, int width, attr_line_t &out);

#endif //LNAV_HELP_TEXT_FORMATTER_HH
