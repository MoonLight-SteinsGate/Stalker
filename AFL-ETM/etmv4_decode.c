/*
 * vim:ts=4:sw=4:expandtab
 *
 * ptm2human.c: Entry
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
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "output.h"
#include "tracer.h"
#include "stream.h"
#include "prototype.h"

int debuglog_on = 0;

static const struct option options[] = 
{
    { "input", 1, 0, 'i' },
    { "context", 1, 0, 'c' },
    { "cycle-accurate", 0, 0, 'C' },
    { "decode-ptm", 0, 0, 'p' },
    { "trcidr0", 1, 0, '0' },
    { "trcidr8", 1, 0, '8' },
    { "trcidr9", 1, 0, '9' },
    { "trcidr12", 1, 0, '2' },
    { "trcidr13", 1, 0, '3' },
    { "decode-etmv4", 0, 0, 'e' },
    { "debuglog", 0, 0, 'd' },
    { "help", 0, 0, 'h' },
    { NULL, 0, 0, 0   },
};

static const char *optstring = "i:c:Cp0:8:9:2:3:edh";

int file2buff(const char *input_file, const char *buff, unsigned int buff_len)
{
    int fd;

    if (!input_file) {
        LOGE("Invalid input_file\n");
        return -1;
    }
    if (!buff) {
        LOGE("Invalid buff\n");
        return -1;
    }

    fd = open(input_file, O_RDONLY);
    if (fd == -1) {
        LOGE("Fail to open %s (%s)\n", input_file, strerror(errno));
        return -1;
    }

    if (read(fd, (void *)buff, buff_len) != buff_len) {
        close(fd);
        return -1;
    } else {
        close(fd);
        return 0;
    }
}

int etmv4_decode(unsigned char *trace_bits, char *buf, int size)
{
    int ret;
    unsigned int trcidr12 = 0, trcidr13 = 0;
    struct stream stream;
    struct stat sb;

    /* disable output buffering */
    setbuf(stdout, NULL);

    memset(&stream, 0, sizeof(struct stream));

    decode_etmv4();

    /* validate context ID size */
    switch (CONTEXTID_SIZE(&(stream.tracer))) {
    case 0:
    case 1:
    case 2:
    case 4:
        break;
    default:
        LOGE("Invalid context ID size %d\n", CONTEXTID_SIZE(&(stream.tracer)));
        return EXIT_FAILURE;
        break;
    }

    /* validate CONDTYPE in trcidr0 */
    if (CONDTYPE(&(stream.tracer)) > 2) {
        LOGE("Invalid CONDTYPE in TRCIDR0: %d (should be either 0 or 1)\n", CONDTYPE(&(stream.tracer)));
        return EXIT_FAILURE;
    }

    /* validate trcidr12 and trcidr13 */
    if (trcidr12 < trcidr13) {
        LOGE("Invalid TRCIDR12/TRCIDR13: TRCIDR12 (%d) < TRCIDR13 (%d)\n", trcidr12, trcidr13);
        return EXIT_FAILURE;
    } else {
        COND_KEY_MAX_INCR(&(stream.tracer)) = trcidr12 - trcidr13;
    }

    stream.buff_len = size;
    stream.buff = malloc(stream.buff_len);
    if (!(stream.buff)) {
        LOGE("Fail to allocate memory (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }
    memset((void *)stream.buff, 0, stream.buff_len);
                   
    memcpy(stream.buff, buf, size);
                   
    ret = decode_etb_stream(&stream, trace_bits);

    free((void *)stream.buff);

    if (ret) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
