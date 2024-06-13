/*  
 *  openwith.c - The simplest kernel module restrict file_open.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/security.h>
#include <linux/lsm_hooks.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavel V Samsonov <pvsamsonov76@gmail.com>");

#define OPENWITH_MAX 4096
#define OPENWITH_XATTR "user.openwith"

char cmdline_buf[PATH_MAX + 1];
char openwith_buf[OPENWITH_MAX + 1];



int openwith_file_open(struct file *file)
{
	char *openwith, *argv0, *openwith_end, *cmdline;
	ssize_t len;

	strcpy(openwith_buf, "");
	len = vfs_getxattr(mnt_idmap(file->f_path.mnt), file->f_path.dentry,
			OPENWITH_XATTR, openwith_buf, OPENWITH_MAX);
	openwith_buf[len] = '\0';

	if (strlen(openwith_buf) == 0) return 0;

	cmdline = cmdline_buf;
	get_cmdline(current, cmdline, PATH_MAX);
	cmdline[PATH_MAX] = '\0';
	argv0 = strsep(&cmdline, " ");

	openwith_end = openwith_buf;
	while ((openwith = strsep(&openwith_end, " ")) != NULL)
		if (strcmp(openwith, argv0) == 0) return 0;

	strcpy(openwith_buf, "");
	len = vfs_getxattr(mnt_idmap(file->f_path.mnt), file->f_path.dentry,
			OPENWITH_XATTR, openwith_buf, OPENWITH_MAX);
	openwith_buf[len] = '\0';

	printk(KERN_INFO "openwith: uid=%d attempt open %s by %s, allowed only by %s\n",
		current_uid().val, file->f_path.dentry->d_name.name, argv0, openwith_buf);

	return -EPERM;
}

static struct security_hook_list openwith_hooks_list[] =
{
	LSM_HOOK_INIT(file_open, openwith_file_open),
};

static __init int init_openwith(void)
{
	security_add_hooks(openwith_hooks_list, ARRAY_SIZE(openwith_hooks_list), "openwith");
	printk(KERN_INFO "Security module openwith up\n");

	return 0;
}

void cleanup_openwith(void)
{
	printk(KERN_INFO "Security module openwith unloaded\n");
}

module_init(init_openwith);
module_exit(cleanup_openwith);

