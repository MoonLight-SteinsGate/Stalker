/*
 * vim:ts=4:sw=4:expandtab
 *
 * tracer-etmv4.c: Core of ETMv4 tracer
 * Copyright (C) 2013  Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdio.h>
#include "tracer.h"
#include "stream.h"
#include "output.h"

extern unsigned long long main_entry_addr;
extern unsigned long long text_start_addr;
extern unsigned long long text_end_addr;
extern unsigned long long IRQ_addr;
extern unsigned long long overflow_nums;
extern unsigned long long exception_nums;
extern unsigned long long branch_nums;
extern unsigned long long basic_block;
extern unsigned long long hash;
extern unsigned long long pre_basic_block = 0;

extern unsigned int entry_flag;
extern unsigned int exit_flag;
extern unsigned int branch_flag;
extern unsigned int count_atom;
extern unsigned int bb_mode;
extern unsigned int from_exception;
extern unsigned long long atom_nums;
extern unsigned long long atom_in_slide;
extern unsigned long long atom_nums_in_execution;

static const char *cond_result_token_apsr[] = 
{
    "C flag set",
    "N flag set",
    "Z and C flags set",
    "N and C flags set",
    "unknown",
    "unknown",
    "unknown",
    "No flag set",
    "unknown",
    "unknown",
    "unknown",
    "Z flag set",
    "unknown",
    "unknown",
    "unknown",
    "unknown"
};

static const char *cond_result_token_pass_fail[] =
{
    "failed the condition code check",
    "passed the condition code check",
    "don't know the result of the condition code check"
};

static const char *exp_name[32] = { "PE reset", "Debug halt", "Call", "Trap",
                                    "System error", NULL, "Inst debug", "Data debug",
                                    NULL, NULL, "Alignment", "Inst fault",
                                    "Data fault", NULL, "IRQ", "FIQ" };

void tracer_trace_info(void *t, unsigned int plctl, unsigned int info,\
                       unsigned int key, unsigned int spec, unsigned int cyct, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    RESET_ADDRESS_REGISTER(tracer);

    TRACE_INFO(tracer) = (plctl & 1)? info: 0;
    P0_KEY(tracer) = (plctl & 2)? key: 0;
    CURR_SPEC_DEPTH(tracer) = (plctl & 4)? spec: 0;
    CC_THRESHOLD(tracer) = (plctl & 8)? cyct: 0;
}

void tracer_trace_on(void *t, unsigned char *trace_bits)
{

}

void tracer_discard(void *t, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    tracer_cond_flush(tracer);

    CURR_SPEC_DEPTH(tracer) = 0;
}

void tracer_overflow(void *t, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    overflow_nums++;

    tracer_cond_flush(tracer);

    CURR_SPEC_DEPTH(tracer) = 0;
}

void tracer_ts(void *t, unsigned long long timestamp, int have_cc, unsigned int count,  \
               int nr_replace)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (timestamp) {
        TIMESTAMP(tracer) &= ~((1LL << nr_replace) - 1);
        TIMESTAMP(tracer) |= timestamp;
    }
}

void tracer_exception(void *t, int type)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    exception_nums++;
    // if(type == 14){
    //     exception_nums++;
    //     return 0;
    // }

    tracer_cond_flush(tracer);

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }

}

void tracer_exception_return(void *t)
{
    /* FIXME: for ARMv6-M and ARMv7-M PEs, exception_return is a P0 element */
}

void tracer_cc(void *t, int unknown, unsigned int count)
{

}

void tracer_commit(void *t, unsigned int commit)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    CURR_SPEC_DEPTH(tracer) -= commit;
}

void tracer_cancel(void *t, int mispredict, unsigned int cancel, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    CURR_SPEC_DEPTH(tracer) -= cancel;

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer) -= cancel;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    if (mispredict) {
        tracer_mispredict(tracer, 0, trace_bits);
        tracer_cond_flush(tracer);
    }
}

void tracer_mispredict(void *t, int param, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    switch (param) {
    case 0:
        break;

    case 1:
        tracer_atom(tracer, ATOM_TYPE_E, trace_bits);
        break;

    case 2:
        tracer_atom(tracer, ATOM_TYPE_E, trace_bits);
        tracer_atom(tracer, ATOM_TYPE_E, trace_bits);
        break;

    case 3:
        tracer_atom(tracer, ATOM_TYPE_N, trace_bits);
        break;

    default:
        break;
    }
}

static int __is_cond_key_special(struct etmv4_tracer *tracer, unsigned int key)
{
    return (key >= COND_KEY_MAX_INCR(tracer))? 1: 0;
}

void tracer_cond_inst(void *t, int format, unsigned int param1, unsigned int param2)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned int key;
    int ci, i, z, num;

    if (COND_KEY_MAX_INCR(tracer) == 0) {
        return ;
    }

    switch (format) {
    case 1:
        key = param1;
        if (__is_cond_key_special(tracer, key)) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        } else {
            COND_C_KEY(tracer) = key;
        }
        break;

    case 2:
        ci = param1;
        if (ci == 0) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        } else if (ci == 2) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        }
        break;

    case 3:
        z = param1;
        num = param2;
        for (i = 0; i < num; i++) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        }
        break;

    default:
        break;
    }
}

void tracer_cond_flush(void *t)
{

}

/*
 * __interpret_tokens: Interpret tokens for conditional result elements
 * @tracer: pointer to the etmv4_tracer structure
 * @tokens: tokens to interpret
 * @pos: start position in tokens
 * Return the next position for the next token, or 0 for Null (no R element indicated)
 */
static int __interpret_tokens(struct etmv4_tracer *tracer, unsigned int tokens, int pos)
{
    unsigned char token;

    if (pos % 2) {
        return pos + 1;
    }

    if (CONDTYPE(tracer)) {
        token = (tokens >> pos) & 0x0F;
        if ((token & 0x03) == 0x03) {
            /* 2-bit tokens */
            return pos + 2;
        } else {
            /* 4-bit otkens */
            if (token == 0x0F) {
                /* NULL, no R element indicated */
                return 0;
            } else {
                return pos + 4;
            }
        }
    } else {
        token = (tokens >> pos) & 0x03;
        if (token == 3) {
            /* NULL, no R element indicated */
            return 0;
        } else {
            return pos + 2;
        }
    }
}

void tracer_cond_result(void *t, int format, unsigned int param1, \
                        unsigned int param2, unsigned int param3)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned int key, ci, result, k, tokens, token;
    int pos, next_pos;
    const int MAX_TOKENS_POS = 12;

    if (COND_KEY_MAX_INCR(tracer) == 0) {
        return ;
    }

    switch (format) {
    case 1:
        key = param1;
        ci = param2;
        result = param3;
        if (__is_cond_key_special(tracer, key)) {
            COND_R_KEY(tracer)++;
            COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        } else {
            COND_R_KEY(tracer) = key;
        }
        if (ci) {
            do {
                tracer_cond_inst(tracer, 3, 0, 1);
            } while (COND_C_KEY(tracer) == COND_R_KEY(tracer));
        }
        break;

    case 2:
        k = param1 & 0x01;
        token = param2 & 0x03;
        COND_R_KEY(tracer) += 1 + k;
        COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        do {
            tracer_cond_inst(tracer, 3, 0, 1);
        } while (COND_C_KEY(tracer) == COND_R_KEY(tracer));
        break;

    case 3:
        tokens = param1 & 0x0FFF;
        pos = 0;
        do {
            next_pos = __interpret_tokens(tracer, tokens, pos);
            if (next_pos) {
                token = (tokens & ((1 << next_pos) - 1)) >> pos;
                pos = next_pos;
            }
        } while (next_pos != 0 || pos < MAX_TOKENS_POS);
        break;

    case 4:
        token = param1 & 0x03;
        COND_R_KEY(tracer)--;
        COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        break;

    default:
        break;
    }
}

void tracer_context(void *t, int p, int el, int sf, int ns, \
                    int v, unsigned int vmid,   \
                    int c, unsigned int contextid)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (p) {
        EX_LEVEL(tracer) = el;
        SIXTY_FOUR_BIT(tracer) = sf;
        SECURITY(tracer) = !ns;
        if (v) {
            VMID(tracer) = vmid;
        }
        if (c) {
            CONTEXT_ID(tracer) = contextid;
        }
    }
}

void tracer_address(unsigned long long address, unsigned char *trace_bits)
{
    unsigned short int addr;
    unsigned short int index;
    if (address == main_entry_addr) {
        entry_flag = 1;
    }
    if (from_exception){
        if (IRQ_addr == 0){
            IRQ_addr = address;
            return 0;
        }
    }
    if (entry_flag && branch_flag && address >= text_start_addr && address <= text_end_addr) {
        if (from_exception){
            if(IRQ_addr == address){
                from_exception = 0;
                IRQ_addr = 0;
                return 0;
            }
        }
        branch_nums += atom_in_slide;
        if (bb_mode) {
            if (atom_in_slide > 4) {
                atom_in_slide = 5;
            }
            index = (address + atom_in_slide - 1) ^ pre_basic_block & 0x0000ffff;
            trace_bits[index]++;
            pre_basic_block = (address + atom_in_slide - 1) >> 1;
        } else {
            hash = hash & 0x7fffffffffffffff;
            index = pre_basic_block ^ (address + hash) & 0x0000ffff;
            trace_bits[index]++;
            pre_basic_block = (address + hash) >> 1;
            hash = 0;
        }
        atom_in_slide = 1;
        branch_flag = 0;
        IRQ_addr = 0;
    }
}

void tracer_atom(void *t, int type, unsigned char *trace_bits)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    int test_type;
    if (type == ATOM_TYPE_E) {
	    test_type = 0b1;
    } else if (type == ATOM_TYPE_N) {
        test_type = 0b0;
    } else {
        return ;
    }

    if (entry_flag) {
        basic_block = (basic_block << 1) | test_type;
        count_atom++;
        atom_nums++;
        if (count_atom == MAX_ATOMS) {
            count_atom = 0;
            branch_nums++;
            trace_bits[basic_block ^ pre_basic_block]++;
            pre_basic_block = basic_block >> 1;
            basic_block = 0;
        }
    }

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }
}

void tracer_q(void *t, unsigned int count)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }
}
