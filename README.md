# LinuxOS_Project
- 此為 2024年中央大學許富皓教授所開設的**Linux作業系統**之專案作業 
- Implementation Environment  
```  
VirtualBox  
Description (OS): Ubuntu 22.04.5 LTS  
ARCH :  x86_64  
Source Version :  5.15.137  
```  

## Project1  
**Description:**  Write a new system call `void * my_get_physical_addresses(void *)` that allows a process to obtain the physical address corresponding to a virtual address.  
* **Q1:** Write test code to demonstrate the **Copy-On-Write (COW)** effect of the `my_get_physical_addresses` system call.  
  * Check the behavioral differences in physical addresses for the same virtual address (`global_a`) between the parent process and the child process after forking.  
  * This process needs to verify the concept of COW (Copy-On-Write), where the system allocates new physical memory to the child process when it attempts to write to the shared memory.  
* **Q2:** Write test code to check whether the **loader** has loaded all the process's data before executing the program. This involves verifying whether the system allocates physical memory for all of the program's data during **system initialization**.  

## Project2  
* **Description:**  Implement a system call for the **Wait Queue** functionality, allowing user applications to operate this feature via system calls.  
  > A Wait Queue is a synchronization mechanism used to put a process to sleep while waiting for a condition, avoiding resource waste.  
* **Objective:**  Enable Threads to enter and leave the Wait Queue while adhering to the FIFO (First In, First Out) rule.  
