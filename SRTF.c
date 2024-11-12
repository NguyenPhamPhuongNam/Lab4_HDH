#include <stdio.h>
#include <stdlib.h>
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("Enter PID, Arrival time, and Burst time for Process %d: ", i + 1);
        scanf("%d %d %d", &P[i].iPID, &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("PID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n", P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart, P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}

void exportGanttChart (int n, PCB P[]) {
    printf("Gantt Chart:\n");
    //  process IDs
    for (int i = 0; i < n; i++) {
        int j;
        for (j = 0; j < P[i].iBurst - 1; j++) printf(" ");
        printf("P%d", P[i].iPID);
        for (j = 0; j < P[i].iBurst - 1; j++) printf(" ");
        printf("|");
    }
    printf("\n ");

    printf("0");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < P[i].iBurst; j++) printf("  ");
        if (P[i].iFinish > 9) printf("\b");
        printf("%d", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int idx, PCB P[]) {
    for (int i = idx; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int partition(PCB P[], int l, int h, int iCriteria) {
    PCB pivot = P[h];
    int i = l - 1;
    for (int j = l; j <= h- 1; j++) {
        int compare = 0;
        if (iCriteria == SORT_BY_ARRIVAL) compare = P[j].iArrival < pivot.iArrival;
        else if (iCriteria == SORT_BY_PID) compare = P[j].iPID < pivot.iPID;
        else if (iCriteria == SORT_BY_BURST) {
            if (P[j].iBurst == pivot.iBurst)
                compare = P[j].iArrival < pivot.iArrival;
            else
                compare = P[j].iBurst < pivot.iBurst;
        }
        else if (iCriteria == SORT_BY_START) compare = P[j].iStart < pivot.iStart;
        if (compare) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[h]);
    return i + 1;
}

void quickSort(PCB P[], int l, int h, int iCriteria) {
    if (l < h) {
        int pi = partition(P, l, h, iCriteria);
        quickSort(P, l, pi - 1, iCriteria);
        quickSort(P, pi + 1, h, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++) {
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[50];
    PCB TerminatedArray[10];
    int iNumberOfProcess;
    PCB Gantt[50];

    int burstmap[50];
    int startmap[50];

    for (int i = 0; i < 50; i++) {
        startmap[i] = -1;
    }

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0, iGantt = 0;
    inputProcess(iNumberOfProcess, Input);

    for (int i = 0; i < iNumberOfProcess; i++)
        burstmap[Input[i].iPID]  = Input[i].iBurst;

    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    int currentTime = 0;
    while (iTerminated < iNumberOfProcess) {
        while (iRemain > 0) {
            if (Input[0].iArrival <= ReadyQueue[0].iFinish) {
                if (startmap[ReadyQueue[0].iPID] == -1) {
                    startmap[ReadyQueue[0].iPID] = currentTime;
                }
                pushProcess(&iGantt, Gantt, ReadyQueue[0]);
                Gantt[iGantt - 1].iBurst = Input[0].iArrival - currentTime;
                currentTime = Input[0].iArrival;
                Gantt[iGantt - 1].iFinish = currentTime;
                ReadyQueue[0].iBurst = ReadyQueue[0].iBurst - Gantt[iGantt - 1].iBurst;

                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);
                ReadyQueue[0].iFinish = currentTime + ReadyQueue[0].iBurst;
            }
            else break;
        }

        if (iReady > 0) {
            ReadyQueue[0].iStart = startmap[ReadyQueue[0].iPID];
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting = ReadyQueue[0].iTaT - burstmap[ReadyQueue[0].iPID];

            currentTime = ReadyQueue[0].iFinish;

            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            pushProcess(&iGantt, Gantt, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            if (startmap[ReadyQueue[0].iPID] == -1)
                startmap[ReadyQueue[0].iPID] = currentTime;
            ReadyQueue[0].iFinish = currentTime + ReadyQueue[0].iBurst;
        }
    }
    int pos = 0;
    for (int i = 1; i < iGantt; i++) {
        if (Gantt[pos].iPID == Gantt[i].iPID) {
            Gantt[pos].iBurst = Gantt[pos].iBurst + Gantt[i].iBurst;
            Gantt[pos].iFinish = Gantt[i].iFinish;
        }
        else {
            ++pos;
            Gantt[pos] = Gantt[i];
        }
    }
    iGantt = ++pos;

    printf("\n===== SRTF Scheduling =====\n");
    exportGanttChart(iGantt, Gantt);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);

    printProcess(iTerminated, TerminatedArray);

    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    printf("\n");
    return 0;
}

