#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>

typedef struct Process {
    pid_t pid;
    int id;
    struct Process *next;
    int blocked;
} Process;

Process *readyQueueHead = NULL, *readyQueueTail = NULL;
Process *blockedListHead = NULL;
Process *running = NULL;

void enqueueReady(Process *p) {
    p->next = NULL;
    if (!readyQueueTail) {
        readyQueueHead = readyQueueTail = p;
    } else {
        readyQueueTail->next = p;
        readyQueueTail = p;
    }
}

Process* dequeueReady() {
    if (!readyQueueHead) return NULL;
    Process *p = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    if (!readyQueueHead) readyQueueTail = NULL;
    p->next = NULL;
    return p;
}

void enqueueBlocked(Process *p) {
    p->next = NULL;
    if (!blockedListHead) {
        blockedListHead = p;
    } else {
        Process *temp = blockedListHead;
        while (temp->next) temp = temp->next;
        temp->next = p;
    }
    p->blocked = 1;
}

void unblockAll() {
    Process *p = blockedListHead;
    blockedListHead = NULL;

    while (p) {
        Process *next = p->next;
        p->next = NULL;
        p->blocked = 0;
        enqueueReady(p);
        printf("Unblocked: Process P%d (PID %d)\n", p->id + 1, p->pid);
        p = next;
    }
}


int wait_for_enter_or_exit(pid_t pid, int *finished) {
    while (1) {
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {
            *finished = 1;
            return 1;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 100000}; // 0.1 sec
        int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds)) {
            getchar(); // clear Enter
            kill(pid, SIGSTOP);
            *finished = 0;
            return 0;
        }
    }
}

void child_work(int id) {
    if (id == 0) {
        for (int i = 0; i <= 16; i += 2) {
            printf("[P1 - Even] %d\n", i);
            fflush(stdout);
            sleep(1);
        }
    } else if (id == 1) {
        for (int i = 1; i <= 16; i += 2) {
            printf("[P2 - Odd] %d\n", i);
            fflush(stdout);
            sleep(1);
        }
    } else if (id == 2) {
        for (char c = 'A'; c <= 'J'; ++c) {
            printf("[P3 - Char] %c\n", c);
            fflush(stdout);
            sleep(1);
        }
    }
    exit(0);
}

void printState() {
    printf("\n=== Current States ===\n");

    printf("RUNNING: ");
    if (running)
        printf("Process %d (PID %d)\n", running->id + 1, running->pid);
    else
        printf("None\n");

    printf("READY QUEUE: ");
    for (Process *p = readyQueueHead; p != NULL; p = p->next) {
        printf("P%d(PID %d) ", p->id + 1, p->pid);
    }
    printf("\n");

    printf("BLOCKED LIST: ");
    for (Process *p = blockedListHead; p != NULL; p = p->next) {
        printf("P%d(PID %d) ", p->id + 1, p->pid);
    }
    printf("\n======================\n");
}

int main() {
    Process processes[3];

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            raise(SIGSTOP);
            child_work(i);
        } else if (pid > 0) {
            processes[i].pid = pid;
            processes[i].id = i;
            processes[i].blocked = 0;
            processes[i].next = NULL;
            enqueueReady(&processes[i]);
        } else {
            perror("fork");
            exit(1);
        }
    }

    int finishedCount = 0;
    while (finishedCount < 3) {
        if (!readyQueueHead) {
            if (blockedListHead) {
                printf("\nNo READY process. Unblocking all blocked processes.\n");
                unblockAll();
            } else {
                break;
            }
        }

        running = dequeueReady();
        if (!running) {
            break;
        }

        kill(running->pid, SIGCONT);
        printState();
        printf("=== Press Enter to BLOCK the running process or wait for it to finish ===\n");

        int finished = 0;
        wait_for_enter_or_exit(running->pid, &finished);

        if (finished) {
            printf("[Process %d PID %d] Finished\n", running->id + 1, running->pid);
            finishedCount++;
            running = NULL;
        } else {
            printf("[Process %d PID %d] Blocked and moved to BLOCKED list\n", running->id + 1, running->pid);
            enqueueBlocked(running);
            running = NULL;
        }

        printf("Finished count: %d\n", finishedCount);
    }
    if(finishedCount==3)printf("\nAll processes finished.\n");
    else {printf("Processes incomplete");}
    return 0;
}
