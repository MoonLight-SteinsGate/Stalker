/*
 * vim:ts=4:sw=4:expandtab
 *
 * log.h: Macros for logging functions
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
#ifndef _LOG_H
#define _LOG_H

extern int debuglog_on;

#define LOGV(f, args...) fprintf(stdout, f, ## args)
#define LOGD(f, args...) do { if (debuglog_on) fprintf(stderr, "%s:%s:%d - " f, __FILE__, __FUNCTION__, __LINE__, ## args); } while (0)
#define LOGE(f, args...) fprintf(stderr, "ERROR: " f, ## args)

#define OUTPUT(f, args...) fprintf(stdout, f, ## args)

#endif
