//
// Created by 杨翊铎 on 2022/5/15.
//
/*
 * 编写一个程序，使用UNIX系统调用在两个进程之间通过一对管道 "ping-pong "一个字节，每个方向一个。
 * 父进程应该向子进程发送一个字节；
 * 子进程应该打印"<pid>: received ping"，其中<pid>是它的进程ID，将管道上的字节写给父进程，然后退出；
 * 父进程应该从子进程读取字节，打印"<pid>: received pong"，然后退出。
 *
 * 使用pipe来创建一个管道。
 * 使用fork来创建一个子程序。
 * 使用read从管道中读取，使用write向管道中写入。
 * 使用getpid查找调用进程的进程ID。
 * 在Makefile中把程序添加到UPROGS。
 * xv6上的用户程序有一组有限的库函数可供其使用。
 * 你可以在user/user.h中看到这个列表；源代码（除系统调用外）在user/ulib.c、user/printf.c和user/umalloc.c中。
 *
 * 1 -> --- -> 0
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
int main(int arg, char *argv[]) {
    int fd[2];
    char buf[1];
    int pid = fork();
    if (pipe(fd) == -1) {
        fprintf(2, "error in pipe \n");
        exit(1);
    }
    if (pid < 0) {
        fprintf(2, "error in fork \n");
        exit(1);
    } else if (pid == 0) {
        //child
        pid = getpid();
        if (read(fd[0], buf, 1) != 1) {
            fprintf(2, "error in read \n");
            exit(1);
        }
        printf("%d: received ping \n", pid);
        close(fd[0]);
        if (write(fd[1], buf, 1) != 1) {
            fprintf(2, "error in write \n");
            exit(1);
        }
        close(fd[1]);
        exit(0);
    } else {
        //parent
        char info[1] = 'a';
        if (write(fd[1], info, 1) != 1) {
            fprintf(2, "error in write \n");
            exit(1);
        }
        close(fd[1]);
        if (read(fd[0], buf, 1) != 1) {
            fprintf(2, "error in read \n");
            exit(1);
        }
        printf("%d: received pong \n", pid);
        close(fd[0]);
        exit(0);
    }
}
/*
 * 1、当读一个写端已经被关闭的管道时，
 * 在所有的数据都被读取后，read返回0(read返回0表示已经读到文件结束符)；
 * 2、如果写一个读端已经被关闭的管道，
 * 则会产生相关信号对写段的进程进行终止，如果忽略该信号或捕捉该信号并从处理程序返回，则write会返回-1,errno会设置为EPIPE;
 * 3、如果管道的读端和写端都没有关闭，但是管道的写端没有再向管道写数据了。
 * 这时如果管道中没有数据了，那么在此read进程会产生阻塞，直到管道中有数据了才读取数据并返回。
 * 4、如果有指向管道读端的文件描述符没有关闭，而持有管道读端的没有从管道中读数据，
 * 这时有进程向管道中写数据，如果管道被写满再向管道写数据是，再次write会导致进程阻塞，
 * 直到管道中有空间了才会继续向管道中写数据并返回。
 */