/*
 * vim:ts=4:sw=4:expandtab
 *
 * tracer.h: header of tracer
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
#ifndef _TRACER_H
#define _TRACER_H

#define MAX_ATOMS 64

enum { CONDTYPE_PASS_FAIL = 0, CONDTYPE_APSR = 1 };

enum { ADDR_REG_IS_UNKNOWN = -1, ADDR_REG_IS0 = 0, ADDR_REG_IS1 = 1 };

enum { ATOM_TYPE_N = 0, ATOM_TYPE_E = 1 };

static int packet_index[256] = {0, 1, 3, 3, 2, -1, 4, 4, -1, -1, -1, -1, 6, 6, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 13, 13, 13, 13, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 15, 15, 15, 15, 21, 21, 21, 21, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, -1, -1, -1, -1, -1, -1, -1, -1, 18, 18, 18, 18, 14, 16, 18, 18, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 30, 30, 31, 32, -1, 33, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, 29, 29, 29, 29, -1, 23, 24, -1, -1, -1, 25, 26, -1, 27, 28, -1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 43, 44, 45, 46, 47, 40, 41, 42, 36, 36, 36, 36, 38, 38, 38, 38, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 48, 49, 50, 51, 52, 39, 35, 35, 37, 37, 37, 37, 37, 37, 37, 37};
static int atom_length[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 5, 5, 5, 2, 2, 2, 2, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 5, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3};
static unsigned int atom_map[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535, 131071, 262143, 524287, 1048575, 2097151, 4194303, 8388607, 16777215, 0, 10, 21, 0, 2, 1, 3, 7, 0, 5, 10, 14, 30, 62, 126, 254, 510, 1022, 2046, 4094, 8190, 16382, 32766, 65534, 131070, 262142, 524286, 1048574, 2097150, 4194302, 8388606, 16777214, 15, 0, 1, 0, 4, 2, 6, 1, 5, 3, 7};
static unsigned int atom_flag[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1};

struct address_register
{
    unsigned long long address;
    int IS;
};

struct etmv4_tracer
{
    /* exactly the INFO field value in the TraceInfo packet */
    unsigned int info;

    /* Conditional tracing field. The Permitted values are:
       CONDTYPE_PASS_FAIL - Indicate if a conditional instruction passes or fails its check
       CONDTYPE_APSR      - Provide the value of the APSR condition flags
     */
    int condtype;

    /* Trace analyzer state between receiving packets */
    unsigned long long timestamp;
    struct address_register address_register[3];
    unsigned int context_id;
    unsigned int contextid_size;
    unsigned int vmid:8;
    unsigned int ex_level:2;
    unsigned int security:1;
    unsigned int sixty_four_bit:1;
    unsigned int curr_spec_depth;
    unsigned int p0_key;
    unsigned int cond_c_key;
    unsigned int cond_r_key;
    unsigned int p0_key_max;
    unsigned int cond_key_max_incr;
    unsigned int max_spec_depth;
    unsigned int cc_threshold;
};

#define TRACE_INFO(t) (((struct etmv4_tracer *)(t))->info)
#define CONDTYPE(t) (((struct etmv4_tracer *)(t))->condtype)
#define TIMESTAMP(t) (((struct etmv4_tracer *)(t))->timestamp)
#define ADDRESS_REGISTER(t) ((struct etmv4_tracer *)(t))->address_register
#define RESET_ADDRESS_REGISTER(t)   \
        do {    \
            ((struct etmv4_tracer *)(t))->address_register[0].address = 0;    \
            ((struct etmv4_tracer *)(t))->address_register[0].IS = ADDR_REG_IS_UNKNOWN;   \
            ((struct etmv4_tracer *)(t))->address_register[1].address = 0;    \
            ((struct etmv4_tracer *)(t))->address_register[1].IS = ADDR_REG_IS_UNKNOWN;   \
            ((struct etmv4_tracer *)(t))->address_register[2].address = 0;    \
            ((struct etmv4_tracer *)(t))->address_register[2].IS = ADDR_REG_IS_UNKNOWN;   \
        } while (0)
#define CONTEXT_ID(t) (((struct etmv4_tracer *)(t))->context_id)
#define CONTEXTID_SIZE(t) (((struct etmv4_tracer *)(t))->contextid_size)
#define VMID(t) (((struct etmv4_tracer *)(t))->vmid)
#define EX_LEVEL(t) (((struct etmv4_tracer *)(t))->ex_level)
#define SECURITY(t) (((struct etmv4_tracer *)(t))->security)
#define SIXTY_FOUR_BIT(t) (((struct etmv4_tracer *)(t))->sixty_four_bit)
#define CURR_SPEC_DEPTH(t) (((struct etmv4_tracer *)(t))->curr_spec_depth)
#define P0_KEY(t) (((struct etmv4_tracer *)(t))->p0_key)
#define COND_C_KEY(t) (((struct etmv4_tracer *)(t))->cond_c_key)
#define COND_R_KEY(t) (((struct etmv4_tracer *)(t))->cond_r_key)
#define P0_KEY_MAX(t) (((struct etmv4_tracer *)(t))->p0_key_max)
#define COND_KEY_MAX_INCR(t) (((struct etmv4_tracer *)(t))->cond_key_max_incr)
#define MAX_SPEC_DEPTH(t) (((struct etmv4_tracer *)(t))->max_spec_depth)
#define CC_THRESHOLD(t) (((struct etmv4_tracer *)(t))->cc_threshold)

extern void tracer_trace_info(void *t, unsigned int plctl, unsigned int info,\
                              unsigned int key, unsigned int spec,\
                              unsigned int cyct, unsigned char *trace_bits);
extern void tracer_trace_on(void *t, unsigned char *trace_bits);
extern void tracer_discard(void *t, unsigned char *trace_bits);
extern void tracer_overflow(void *t, unsigned char *trace_bits);
extern void tracer_ts(void *t, unsigned long long timestamp, int have_cc, unsigned int count,\
                      int nr_replace);
extern void tracer_exception(void *t, int type);
extern void tracer_exception_return(void *t);
extern void tracer_cc(void *t, int unknown, unsigned int count);
extern void tracer_commit(void *t, unsigned int commit);
extern void tracer_cancel(void *t, int mispredict, unsigned int cancel, unsigned char *trace_bits);
extern void tracer_mispredict(void *t, int arg, unsigned char *trace_bits);
extern void tracer_cond_inst(void *t, int format, unsigned int param1, unsigned int param2);
extern void tracer_cond_flush(void *t);
extern void tracer_cond_result(void *t, int format, unsigned int param1, unsigned int param2, unsigned int param3);
extern void tracer_context(void *t, int p, int el, int sf, int ns, \
                           int v, unsigned int vmid,   \
                           int c, unsigned int contextid);
extern void tracer_address(unsigned long long address, unsigned char *trace_bits);
extern void tracer_atom(void *t, int type, unsigned char *trace_bits);
extern void tracer_q(void *t, unsigned int count);

#endif