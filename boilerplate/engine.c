#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "monitor_ioctl.h"

#define SOCKET_PATH "/tmp/engine_socket"
#define MAX_CONTAINERS 20

struct container {
    char id[50];
    pid_t pid;
    char state[20];
};

struct container containers[MAX_CONTAINERS];
int container_count = 0;

void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < container_count; i++) {
            if (containers[i].pid == pid) {
                strcpy(containers[i].state, "stopped");
                break;
            }
        }
    }
}

int child_func(void *arg) {
    char **args = (char **)arg;

    sethostname("container", 9);

    if (chroot(args[0]) != 0) {
        perror("chroot failed");
        return 1;
    }

    chdir("/");

    if (mount("proc", "/proc", "proc", 0, NULL) != 0) {
        perror("mount failed");
        return 1;
    }

    execvp(args[1], &args[1]);

    perror("exec failed");
    return 1;
}

void start_container(char *id, char *rootfs, char *cmd) {
    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();

    if (pid == 0) {
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        char *args[] = {rootfs, cmd, NULL};
        child_func(args);
        exit(0);
    }

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    close(pipefd[1]);

    printf("Started container PID: %d\n", pid);

    strcpy(containers[container_count].id, id);
    containers[container_count].pid = pid;
    strcpy(containers[container_count].state, "running");
    container_count++;

    // kernel registration
    int fd = open("/dev/container_monitor", O_RDWR);
    if (fd >= 0) {
        struct monitor_request req;
        req.pid = pid;
        req.soft_limit_bytes = 50 * 1024 * 1024;
        req.hard_limit_bytes = 80 * 1024 * 1024;
        strcpy(req.container_id, id);

        ioctl(fd, MONITOR_REGISTER, &req);
        close(fd);
    }

    system("mkdir -p logs");

    char filename[100];
    sprintf(filename, "logs/%s.log", id);

    pid_t log_pid = fork();

    if (log_pid == 0) {
        FILE *logfile = fopen(filename, "w");

        char buffer[256];
        int n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, n, logfile);
            fflush(logfile);
        }

        fclose(logfile);
        exit(0);
    }

    close(pipefd[0]);
}

void stop_container(char *id) {
    for (int i = 0; i < container_count; i++) {
        if (strcmp(containers[i].id, id) == 0) {
            kill(containers[i].pid, SIGTERM);
            strcpy(containers[i].state, "stopping");
            return;
        }
    }
}

void send_logs(int client_fd, char *id) {
    char filename[100];
    sprintf(filename, "logs/%s.log", id);

    FILE *f = fopen(filename, "r");
    if (!f) {
        write(client_fd, "No logs\n", 8);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        write(client_fd, line, strlen(line));
    }

    fclose(f);
}

void print_containers(int client_fd) {
    char output[512] = "ID\tPID\tSTATE\n";

    for (int i = 0; i < container_count; i++) {
        char line[100];
        sprintf(line, "%s\t%d\t%s\n",
                containers[i].id,
                containers[i].pid,
                containers[i].state);
        strcat(output, line);
    }

    write(client_fd, output, strlen(output));
}

void run_supervisor() {
    int server_fd, client_fd;
    struct sockaddr_un addr;

    signal(SIGCHLD, handle_sigchld);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Supervisor started...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        char buffer[256] = {0};
        read(client_fd, buffer, sizeof(buffer));

        printf("DEBUG: [%s]\n", buffer);

        char cmd[50] = {0}, id[50] = {0}, rootfs[100] = {0}, exec_cmd[100] = {0};
        sscanf(buffer, "%s %s %s %s", cmd, id, rootfs, exec_cmd);

        if (strcmp(cmd, "start") == 0) {
            start_container(id, rootfs, exec_cmd);
            write(client_fd, "STARTED\n", 8);
        }
        else if (strcmp(cmd, "ps") == 0) {
            print_containers(client_fd);
        }
        else if (strcmp(cmd, "logs") == 0) {
            send_logs(client_fd, id);
        }
        else if (strcmp(cmd, "stop") == 0) {
            stop_container(id);
            write(client_fd, "STOPPED\n", 8);
        }
        else {
            write(client_fd, "UNKNOWN\n", 8);
        }

        close(client_fd);
    }
}

void run_cli(int argc, char *argv[]) {
    int sock;
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect failed");
        exit(1);
    }

    char message[256] = {0};

    for (int i = 1; i < argc; i++) {
        strcat(message, argv[i]);
        strcat(message, " ");
    }

    write(sock, message, strlen(message));

    char response[1024] = {0};
    read(sock, response, sizeof(response));

    printf("%s\n", response);

    close(sock);
}

int main(int argc, char *argv[]) {

    if (argc >= 2 && strcmp(argv[1], "supervisor") == 0) {
        run_supervisor();
        return 0;
    }

    run_cli(argc, argv);
    return 0;
}
