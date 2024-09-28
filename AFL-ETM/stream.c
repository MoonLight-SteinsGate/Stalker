/*
 * vim:ts=4:sw=4:expandtab
 *
 * stream.c: synchronize trace stream and decode it
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "tracer.h"
#include "stream.h"
#include "output.h"
#include "prototype.h"

struct tracepkt **tracepkts;
sync_func synchronization;

#define ETB_PACKET_SIZE 16
#define NULL_TRACE_SOURCE 0

extern unsigned long long main_entry_addr;
extern unsigned long long exit_addr;
extern unsigned long long text_end_addr;
extern unsigned long long IRQ_addr;
extern unsigned long long overflow_nums;
extern unsigned long long exception_nums;
extern unsigned long long branch_nums;
extern unsigned long long basic_block;
extern unsigned long long hash;
extern unsigned long long pre_basic_block;

extern unsigned int entry_flag;
extern unsigned int exit_flag;
extern unsigned int branch_flag;
extern unsigned int count_atom;
extern unsigned int bb_mode;
extern unsigned int from_exception;
extern unsigned long long atom_nums;
extern unsigned long long atom_in_slide;
extern unsigned long long atom_nums_in_execution;

int decode_etb_stream(struct stream *etb_stream, unsigned char *trace_bits)
{
    if (!etb_stream) {
        LOGE("Invalid stream pointer\n");
        return -1;
    }

    decode_stream(etb_stream, trace_bits);

    return 0;
}

void sdbm(unsigned long atom_serial, int len)
{
    while (len--) {
        hash = (atom_serial >> (len) & 0b1) + (hash << 1) + (hash << 3) - hash;
    }
}

int decode_stream(struct stream *stream, unsigned char *trace_bits)
{
    int cur, i, ret;
    unsigned char c;
    int len = 0;
    unsigned long atom_serial = 0;
    unsigned int this_serial;
    int this_len = 0;
    hash = 0;

    if (!stream) {
        return -1;
    }
    if (stream->state == READING) {
        /* READING -> SYNCING */
        stream->state++;
    } else {
        return -1;
    }

    cur = synchronization(stream, trace_bits);
    if (cur < 0) {
        return -1;
    }

    /* INSYNC -> DECODING */
    stream->state++;
    while (cur < stream->buff_len) {
        c = stream->buff[cur];

        i = packet_index[c];
        
        if (i == -1) {
            cur++;
            continue;
        }

        if (overflow_nums > 0) {
            return 0;
        }

        if (i >= 35 && i <= 54){
            if (entry_flag) {
                from_exception = 0;
                IRQ_addr = 0;
                len = atom_length[c];
                atom_serial = atom_map[c];
                branch_flag = atom_flag[c];
                atom_in_slide += len;
                if(bb_mode == 0) sdbm(atom_serial, len);
            }
            cur++;
            continue;
        }

        ret = tracepkts[i]->decode((const unsigned char *)&(stream->buff[cur]), stream, trace_bits);
        if (ret <= 0) {
            cur++;
        } else {
            cur += ret;
        }
    }

    return 0;
}