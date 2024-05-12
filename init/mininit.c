/*
 * mininit - init program for QEMU MiniVMs
 *
 * This application is a basic init program for QEMU MiniVMs.
 * It starts /etc/init.sh and then waits for its termination.
 *
 * Created by Michael Müller / Sysmagine GmbH
 * Modified by Bernhard Ehlers
 */

/*
 * History:
 * V0.1  2021-06-16
 *   Initial version uploaded by Michael Müller to
 *   https://mergeboard.com/blog/2-qemu-microvm-docker/
 * V0.2  2024-04-19
 *   Run initial program in a new session
 *   Handle shutdown
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* environment for initial program */
char *const default_environment[] = {
    "PATH=/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin",
    "HOME=/root",
    "LOGNAME=root",
    "USER=root",
    "TERM=vt220",
    NULL,
};

/* print error and exit */
void perror_exit(const char *s) {
    perror(s);
    exit(1);
}

/* halt VM */
void do_halt(int do_reboot) {
    static int run_once = 0;
    pid_t pid;

    if (++run_once != 1) return;

    /* Send signals to every process _except_ pid 1 */
    fprintf(stderr, "Sent SIGTERM to all processes...\n");
    kill(-1, SIGTERM);
    sleep(1);

    fprintf(stderr, "Sent SIGKILL to all processes...\n");
    kill(-1, SIGKILL);
    sleep(1);

    fprintf(stderr, "Unmounting all filesystems...\n");
    pid = fork();
    if (pid < 0)
        perror_exit("fork");
    else if (pid == 0) {	/* child */
        execl("/bin/umount", "umount", "-a", "-r", NULL);
        perror_exit("Exec umount failed");
    }
    waitpid(pid, NULL, 0);

    /* reboot should not be called by the init process,
       otherwise the kernel will panic. */
    pid = fork();
    if (pid < 0)
        perror_exit("fork");
    else if (pid == 0) {	/* child */
        sync();
        reboot(do_reboot ? RB_AUTOBOOT : RB_POWER_OFF);
        _exit(0);
    }
}

/* signal handler for halting a VM */
void sig_halt(int sig) {
    do_halt(sig == SIGTERM);
}

/* mount a filesystem with error checking */
void mount_check (
    const char *source, const char *target,
    const char *filesystemtype, unsigned long mountflags,
    const void *data) {
    struct stat info;

    /* create mount point, if it doesn't exist */
    if (stat(target, &info) < 0 && errno == ENOENT) {
        if (mkdir(target, 0755) < 0) {
            fprintf(stderr, "mkdir %s: ", target); perror_exit(NULL);
        }
    }

    /* mount filesystem */
    if (mount(source, target, filesystemtype, mountflags, data) < 0) {
        fprintf(stderr, "mount %s: ", target); perror_exit(NULL);
    }
}

int main(int argc, char *argv[]) {
    pid_t child_pid, wait_pid;
    char *const *env;

    if (getpid() != 1) {
        fprintf(stderr, "init must run as pid 1\n");
        return 1;
    }

    /* update environment */
    for (env = default_environment; *env != NULL; env++) {
        putenv(*env);
    }

    /* mount important filesystems */
    mount_check("none", "/proc", "proc", MS_NOSUID|MS_NODEV|MS_NOEXEC, "");
    mount_check("none", "/dev", "devtmpfs", MS_NOSUID, "");
    mount_check("none", "/dev/pts", "devpts", MS_NOSUID|MS_NOEXEC, "");
    mount_check("none", "/dev/mqueue", "mqueue", MS_NOSUID|MS_NODEV|MS_NOEXEC, "");
    mount_check("none", "/dev/shm", "tmpfs", MS_NOSUID|MS_NODEV, "");
    mount_check("none", "/sys", "sysfs", MS_NOSUID|MS_NODEV|MS_NOEXEC, "");
    mount_check("none", "/sys/fs/cgroup", "cgroup", MS_NOSUID|MS_NODEV|MS_NOEXEC, "");

    sethostname("minivm", strlen("minivm"));

    /* start initial program */
    ioctl(STDIN_FILENO, TIOCNOTTY);		/* detach controlling tty */
    child_pid = fork();
    if (child_pid < 0)
        perror_exit("fork");
    else if (child_pid == 0) {
        /* child */
        if (setsid() < 0)			/* start a new session */
            perror("setsid");
        ioctl(STDIN_FILENO, TIOCSCTTY, 0);	/* attach controlling tty */
        /* start /etc/init.sh */
        argv[0] = "/etc/init.sh";
        execv(argv[0], argv);
        perror("Exec /etc/init.sh failed");
        /* fallback to /bin/sh */
        fprintf(stderr, "Falling back to /bin/sh\n");
        execl("/bin/sh", "/bin/sh", NULL);
        perror_exit("Exec /bin/sh failed");
    }

    /* catch signals for halt/poweroff/reboot */
    signal(SIGUSR1, sig_halt);		/* halt */
    signal(SIGUSR2, sig_halt);		/* poweroff */
    signal(SIGTERM, sig_halt);		/* reboot */

    /* wait for child to exit */
    do
        wait_pid = wait(NULL);
    while (!(wait_pid == child_pid || (wait_pid < 0 && errno != EINTR)));
    if (wait_pid < 0) perror("wait");

    do_halt(0);

    /* init does not exit */
    while (1) sleep(1);
}
