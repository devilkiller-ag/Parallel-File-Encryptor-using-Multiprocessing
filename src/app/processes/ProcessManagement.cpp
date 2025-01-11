#include <atomic>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <semaphore.h>

#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp"


ProcessManagement::ProcessManagement() {
    itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0);
    emptySlotsSemaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, SHAREDMEM_CAPACITY);
    if(itemsSemaphore == SEM_FAILED || emptySlotsSemaphore == SEM_FAILED) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(shmFd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }
    ftruncate(shmFd, sizeof(SharedMemory));

    sharedMem = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}


bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    sem_wait(emptySlotsSemaphore);
    std::unique_lock<std::mutex> lock(queueLock);

    if(sharedMem->size.load() >= SHAREDMEM_CAPACITY) {
        return false;
    }

    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
    sharedMem->rear = (sharedMem->rear + 1) % SHAREDMEM_CAPACITY;
    sharedMem->size.fetch_add(1);

    lock.unlock();
    sem_post(itemsSemaphore);

    int pid = fork();
    if(pid < 0) {
        // Failed to create a child process
        return false;
    } 
    // else if(pid > 0) {
    //     // parent process
    //     std::cout << "Entering the parent process" << std::endl;
    // } 
    else if(pid == 0) {
        // pid == 0: child process
        std::cout << "Entering the child process" << std::endl;
        executeTask();
        std:: cout << "Exiting the child process" << std::endl;
        exit(0);
    }

    return true;
}


void ProcessManagement::executeTask() {
    sem_wait(itemsSemaphore);
    std::unique_lock<std::mutex> lock(queueLock);

    char taskString[256];
    strcpy(taskString, sharedMem->tasks[sharedMem->front]);

    sharedMem->front = (sharedMem->front + 1) % SHAREDMEM_CAPACITY;
    sharedMem->size.fetch_sub(1);

    lock.unlock();
    sem_post(emptySlotsSemaphore);

    std::cout << "Executing task: " << taskString << std::endl;    
    executeCryption(taskString);
}


ProcessManagement::~ProcessManagement() {
    munmap(sharedMem, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
}
