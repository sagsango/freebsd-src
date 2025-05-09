// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2007-2010 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2007 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Brian Behlendorf <behlendorf1@llnl.gov>.
 *  UCRL-CODE-235197
 *
 *  This file is part of the SPL, Solaris Porting Layer.
 *
 *  The SPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  The SPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the SPL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SPL_CRED_H
#define	_SPL_CRED_H

#include <linux/module.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/vfs.h>

typedef struct cred cred_t;

extern struct task_struct init_task;

#define	kcred		((cred_t *)(init_task.cred))
#define	CRED()		((cred_t *)current_cred())

/* Linux 4.9 API change, GROUP_AT was removed */
#ifndef GROUP_AT
#define	GROUP_AT(gi, i)	((gi)->gid[i])
#endif

#define	KUID_TO_SUID(x)		(__kuid_val(x))
#define	KGID_TO_SGID(x)		(__kgid_val(x))
#define	SUID_TO_KUID(x)		(KUIDT_INIT(x))
#define	SGID_TO_KGID(x)		(KGIDT_INIT(x))
#define	KGIDP_TO_SGIDP(x)	(&(x)->val)

extern zidmap_t *zfs_get_init_idmap(void);

/* Check if the user ns is the initial one */
static inline boolean_t
zfs_is_init_userns(struct user_namespace *user_ns)
{
#if defined(CONFIG_USER_NS)
	return (user_ns == kcred->user_ns);
#else
	return (B_FALSE);
#endif
}

static inline struct user_namespace *zfs_i_user_ns(struct inode *inode)
{
	return (inode->i_sb->s_user_ns);
}

static inline boolean_t zfs_no_idmapping(struct user_namespace *mnt_userns,
    struct user_namespace *fs_userns)
{
	return (zfs_is_init_userns(mnt_userns) ||
	    mnt_userns == fs_userns);
}

static inline uid_t zfs_uid_to_vfsuid(zidmap_t *mnt_userns,
    struct user_namespace *fs_userns, uid_t uid)
{
	struct user_namespace *owner;
#ifdef HAVE_IOPS_CREATE_IDMAP
	if (mnt_userns == zfs_init_idmap)
		return (uid);
#endif
#ifdef HAVE_IDMAP_NO_USERNS
	struct user_namespace ns;
	ns.uid_map = mnt_userns->uid_map;
	owner = &ns;
#else
	owner = idmap_owner(mnt_userns);
#endif
	if (zfs_no_idmapping(owner, fs_userns))
		return (uid);
	if (!zfs_is_init_userns(fs_userns))
		uid = from_kuid(fs_userns, KUIDT_INIT(uid));
	if (uid == (uid_t)-1)
		return (uid);
	return (__kuid_val(make_kuid(owner, uid)));
}

static inline gid_t zfs_gid_to_vfsgid(zidmap_t *mnt_userns,
    struct user_namespace *fs_userns, gid_t gid)
{
	struct user_namespace *owner;
#ifdef HAVE_IOPS_CREATE_IDMAP
	if (mnt_userns == zfs_init_idmap)
		return (gid);
#endif
#ifdef HAVE_IDMAP_NO_USERNS
	struct user_namespace ns;
	ns.gid_map = mnt_userns->gid_map;
	owner = &ns;
#else
	owner = idmap_owner(mnt_userns);
#endif
	if (zfs_no_idmapping(owner, fs_userns))
		return (gid);
	if (!zfs_is_init_userns(fs_userns))
		gid = from_kgid(fs_userns, KGIDT_INIT(gid));
	if (gid == (gid_t)-1)
		return (gid);
	return (__kgid_val(make_kgid(owner, gid)));
}

static inline uid_t zfs_vfsuid_to_uid(zidmap_t *mnt_userns,
    struct user_namespace *fs_userns, uid_t uid)
{
	struct user_namespace *owner;
#ifdef HAVE_IOPS_CREATE_IDMAP
	if (mnt_userns == zfs_init_idmap)
		return (uid);
#endif
#ifdef HAVE_IDMAP_NO_USERNS
	struct user_namespace ns;
	ns.uid_map = mnt_userns->uid_map;
	owner = &ns;
#else
	owner = idmap_owner(mnt_userns);
#endif
	if (zfs_no_idmapping(owner, fs_userns))
		return (uid);
	uid = from_kuid(owner, KUIDT_INIT(uid));
	if (uid == (uid_t)-1)
		return (uid);
	if (zfs_is_init_userns(fs_userns))
		return (uid);
	return (__kuid_val(make_kuid(fs_userns, uid)));
}

static inline gid_t zfs_vfsgid_to_gid(zidmap_t *mnt_userns,
    struct user_namespace *fs_userns, gid_t gid)
{
	struct user_namespace *owner;
#ifdef HAVE_IOPS_CREATE_IDMAP
	if (mnt_userns == zfs_init_idmap)
		return (gid);
#endif
#ifdef HAVE_IDMAP_NO_USERNS
	struct user_namespace ns;
	ns.gid_map = mnt_userns->gid_map;
	owner = &ns;
#else
	owner = idmap_owner(mnt_userns);
#endif
	if (zfs_no_idmapping(owner, fs_userns))
		return (gid);
	gid = from_kgid(owner, KGIDT_INIT(gid));
	if (gid == (gid_t)-1)
		return (gid);
	if (zfs_is_init_userns(fs_userns))
		return (gid);
	return (__kgid_val(make_kgid(fs_userns, gid)));
}

extern void crhold(cred_t *cr);
extern void crfree(cred_t *cr);
extern uid_t crgetuid(const cred_t *cr);
extern uid_t crgetruid(const cred_t *cr);
extern gid_t crgetgid(const cred_t *cr);
extern int crgetngroups(const cred_t *cr);
extern gid_t *crgetgroups(const cred_t *cr);
extern int groupmember(gid_t gid, const cred_t *cr);
#endif  /* _SPL_CRED_H */
