#!/bin/ksh -p
# SPDX-License-Identifier: CDDL-1.0
#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or https://opensource.org/licenses/CDDL-1.0.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#

#
# Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

#
# Copyright (c) 2013, 2016 by Delphix. All rights reserved.
#

. $STF_SUITE/tests/functional/cli_root/zfs_get/zfs_get_list_d.kshlib

#
# DESCRIPTION:
# A negative depth or a non numeric depth should fail in 'zfs list -d <n>'
#
# STRATEGY:
# 1. Run zfs list -d with negative depth or non numeric depth
# 2. Verify that zfs list returns error
#

verify_runnable "both"

log_assert "A negative depth or a non numeric depth should fail in 'zfs list -d <n>'"

set -A  badargs "a" "AB" "aBc" "2A" "a2b" "aB2" "-1" "-32" "-999"

typeset -i i=0
while (( i < ${#badargs[*]} ))
do
	log_mustnot eval "zfs list -d ${badargs[i]} $DEPTH_FS >/dev/null 2>&1"
	(( i = i + 1 ))
done

log_pass "A negative depth or a non numeric depth should fail in 'zfs list -d <n>'"
