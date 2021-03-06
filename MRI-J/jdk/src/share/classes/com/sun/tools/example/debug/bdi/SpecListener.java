/*
 * Copyright 1999 Sun Microsystems, Inc.  All Rights Reserved.
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

package com.sun.tools.example.debug.bdi;

import java.util.EventListener;

public interface SpecListener extends EventListener {

    void breakpointSet(SpecEvent e);
    void breakpointDeferred(SpecEvent e);
    void breakpointDeleted(SpecEvent e);
    void breakpointResolved(SpecEvent e);
    void breakpointError(SpecErrorEvent e);

    void watchpointSet(SpecEvent e);
    void watchpointDeferred(SpecEvent e);
    void watchpointDeleted(SpecEvent e);
    void watchpointResolved(SpecEvent e);
    void watchpointError(SpecErrorEvent e);

    void exceptionInterceptSet(SpecEvent e);
    void exceptionInterceptDeferred(SpecEvent e);
    void exceptionInterceptDeleted(SpecEvent e);
    void exceptionInterceptResolved(SpecEvent e);
    void exceptionInterceptError(SpecErrorEvent e);
}
