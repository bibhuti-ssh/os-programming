#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h> 
#include <unistd.h> 
typedef enum { NEW , READY , RUNNING  , BLOCKED , FINISHED } State ; 
typedef struct Process{
	int id ;
	State state ; 
	int cpuCycles ; 
	int eventId ; 
	struct Process* next ; // the linked list stuff 
} Process ;

Process *readyHead = NULL ; 
Process *readyTail = NULL ; 
Process *running = NULL ; 
Process *blockedHead = NULL ; 

int nextPid =  1; 

void enqueueReady(Process *p) {
	p->state = READY ; 
	p->next = NULL ; 
	if(!readyHead) readyHead = readyTail = p ;
	else{ 
		readyTail->next = p ; 
		readyTail= p ; 
	}
}
//deq frm the ready queue 
Process* dequeueReady() {
    if (!readyHead) return NULL;
    Process *p = readyHead;
    readyHead = readyHead->next;
    if (!readyHead) readyTail = NULL;
    p->next = NULL;
    return p;
}
void addBlocked(Process *p){ 
	p->state = BLOCKED ; 
	p->next = blockedHead ; 
	blockedHead = p ; 
} 
Process* removeBlocked(int eventID ) { 
	Process *prev = NULL ; 
	Process *curr = blockedHead ; 
	while(curr ){ 
		if(curr->eventId == eventID ) {
			if(prev) prev->next = curr->next ; 
			else blockedHead = curr->next ; 
			curr->next= NULL ; 
			return curr ; 
		} 
		prev= curr ;
		curr = curr->next ; 
	}
	return NULL ; 
} 

void newProcess() {
    Process *p = (Process*)malloc(sizeof(Process));
    p->id = nextPid++;
    p->state = NEW;
    p->cpuCycles = 0;
    p->eventId = -1;
    p->next = NULL;

    enqueueReady(p);
    printf("New: Process %d created and added to READY queue\n", p->id);
}
void cpuEvent() {
    if (running) {
        running->cpuCycles++;
        printf("CPU: Process %d ran for 1 cycle (total: %d)\n",
               running->id, running->cpuCycles);
    } else {
        running = dequeueReady();
        if (running) {
            running->state = RUNNING;
            printf("Dispatch: Process %d is now RUNNING\n", running->id);
        } else {
            printf("CPU: No process to run\n");
        }
    }
}
void blockEvent(int eventId) {
    if (!running) {
        printf("Block: No RUNNING process to block\n");
        return;
    }
    running->eventId = eventId;
    printf("Block: Process %d is BLOCKED on event %d\n", running->id, eventId);
    addBlocked(running);
    running = NULL;
}

void unblockEvent(int eventId) {
    Process *p = removeBlocked(eventId);
    if (p) {
        enqueueReady(p);
        printf("Unblock: Process %d moved to READY queue\n", p->id);
    } else {
        printf("Unblock: No process waiting on event %d\n", eventId);
    }
}

void doneEvent() {
    if (!running) {
        printf("Done: No RUNNING process to finish\n");
        return;
    }
    printf("Done: Process %d has finished execution\n", running->id);
    running->state = FINISHED;
    free(running);
    running = NULL;
}
void printState() {
    printf("\n=== Current State ===\n");
    if (running)
        printf("RUNNING: Process %d (cycles=%d)\n", running->id, running->cpuCycles);
    else
        printf("RUNNING: None\n");

    printf("READY: ");
    for (Process *p = readyHead; p; p = p->next)
        printf("P%d ", p->id);
    printf("\n");

    printf("BLOCKED: ");
    for (Process *p = blockedHead; p; p = p->next)
        printf("P%d(ev=%d) ", p->id, p->eventId);
    printf("\n=====================\n");
}








int main(){ 
    newProcess();
    cpuEvent();
    cpuEvent();
    blockEvent(101);
    newProcess();
    cpuEvent();
    doneEvent();
    newProcess();
    cpuEvent();
    unblockEvent(101);
    cpuEvent();
    doneEvent();
    cpuEvent();
    doneEvent();
} 
