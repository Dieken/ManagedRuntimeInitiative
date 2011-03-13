/*
 * Copyright 1998-2005 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

#ifndef JDWP_SYS_H
#define JDWP_SYS_H

#define SYS_OK         0
#define SYS_ERR        -1
#define SYS_INTRPT     -2
#define SYS_TIMEOUT    -3
#define SYS_NOMEM      -5
#define SYS_NORESOURCE -6
#define SYS_INUSE      -7
#define SYS_DIED       -8

/* Implemented in linker_md.c */

void    dbgsysBuildLibName(char *, int, char *, char *);
void *  dbgsysLoadLibrary(const char *, char *err_buf, int err_buflen);
void    dbgsysUnloadLibrary(void *);
void *  dbgsysFindLibraryEntry(void *, const char *);

/* Implemented in exec_md.c */
int     dbgsysExec(char *cmdLine);

#endif