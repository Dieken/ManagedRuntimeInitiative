#
# Copyright 1997-2004 Sun Microsystems, Inc.  All Rights Reserved.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# This code is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 only, as
# published by the Free Software Foundation.  Sun designates this
# particular file as subject to the "Classpath" exception as provided
# by Sun in the LICENSE file that accompanied this code.
#
# This code is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# version 2 for more details (a copy is included in the LICENSE file that
# accompanied this code).
#
# You should have received a copy of the GNU General Public License version
# 2 along with this work; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
# CA 95054 USA or visit www.sun.com if you need additional information or
# have any questions.
#

BUILDDIR = ../../..

PACKAGE = org.omg.CORBA
PRODUCT = org

include $(BUILDDIR)/common/Defs.gmk

#
# Files
#
CORBA_JMK_DIRECTORY=$(TOPDIR)/make/com/sun/corba/minclude/

include $(CORBA_JMK_DIRECTORY)org_omg_CORBA.jmk
include $(CORBA_JMK_DIRECTORY)org_omg_SendingContext.jmk
include $(CORBA_JMK_DIRECTORY)org_omg_CORBA_2_3.jmk


ifdef STANDALONE_CORBA_WS
	ORG_OMG_CORBA_java = $(org_omg_CORBA_java_1_2) $(org_omg_CORBA_java_1_3) $(org_omg_CORBA_java_new) $(org_omg_CORBA30_java) $(org_omg_CORBA_TSIdentification_java)
else
	ORG_OMG_CORBA_java = $(org_omg_CORBA_java_1_2) $(org_omg_CORBA_java_1_3) $(org_omg_CORBA_java_new) $(org_omg_CORBA30_java)
endif

FILES_java = \
        $(org_omg_SendingContext_java) \
        $(org_omg_CORBA_2_3_java) \
        $(ORG_OMG_CORBA_java)

#
# Include
#
include $(BUILDDIR)/common/Classes.gmk

