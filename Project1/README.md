# Project1
## Description
Write a new system call `void * my_get_physical_addresses(void *)` that allows a process to obtain the physical address corresponding to a virtual address.  
* **Q1:** Write test code to demonstrate the **Copy-On-Write (COW)** effect of the `my_get_physical_addresses` system call.  
  * Check the behavioral differences in physical addresses for the same virtual address (`global_a`) between the parent process and the child process after forking.  
  * This process needs to verify the concept of COW (Copy-On-Write), where the system allocates new physical memory to the child process when it attempts to write to the shared memory.  
* **Q2:** Write test code to check whether the **loader** has loaded all the process's data before executing the program. This involves verifying whether the system allocates physical memory for all of the program's data during **system initialization**.  
## System Call 
path: `/usr/src/linux-5.15.137` 
1. 在 `kernel/` 路徑下新增 `project1.c`
    > **Page frame physical address mechanism | offset** 
	  > * `PAGE_MASK`：這個遮罩用來屏蔽掉位址中的偏移量，只保留 page frame 的基址。它適用於獲得 4 KB page 的實體位址。
	  > * `PTE_PFN_MASK`：這個遮罩用來屏蔽掉 page table 中的一些額外標誌位
	  > 在某些情況下，僅使用 `PAGE_MASK` 可能不足以過濾出純粹的 page frame 地址，因為 page table 中可能包含額外的控制位元。而 `PTE_PFN_MASK` 可以進一步確保我們只保留 page frame 部分的位元

2. Edit the kernel Makefile
    ```bash
    vim kernel/Makefile
    ```
3. Declare the system call in the header file
    ```bash
    vim arch/x86/entry/syscalls/syscall_64.tbl
    449 common   my_get_physical_addresses   sys_my_get_physical_addresses      
    ```
4. Add the new system call to the syscall table
    ```bash
    vim include/linux/syscalls.h
    asmlinkage long sys_my_get_physical_addresses(void *);
    ```
    * `asmlinkage`：確保函數的參數是從堆疊傳遞的，而不是透過寄存器。
    * `long`：系統呼叫的返回值一般使用 long 型別，以支持錯誤碼（負數）或成功返回值（非負數）。

## Compile the kernel
```bash
make -j4
sudo make modules_install
sudo make install
sudo reboot
```

## Create the User Program (Write a User-Space Test Program)
```bash
vim user_program.c
gcc -o user_program user_program.c 	
./user_program
```
* **pthread_create**
  >```c
  >int pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
  >                   void *(*start_routine)(void *), void *arg);
  >```
  >* `pthread_t *thread`: 傳遞指向執行緒 ID 的指標，執行緒創建成功後會將其 ID 存入這個指標變數
  >* `pthread_attr_t *attr`: 用來設定執行緒屬性，`NULL` 表示使用預設屬性
  >* `void *(*start_routine)(void *)`: 執行緒的入口函數，當執行緒啟動時，會執行這個函數。該函數必須接受 `void *` 參數並返回 `void *`
  >* `void *arg`: 傳遞給入口函數的參數。這裡傳入的是 `&thread_args[i]`
  >
  >**實際執行**:
  >* 每次迴圈創建一個執行緒，並執行 `enter_wait_queue` 函數
  >* 傳入的參數是 `thread_args[i]` 的地址（`(void *)&thread_args[i]`），代表每個執行緒的唯一序號
  >
  >**檢查返回值**:
  >如果 `pthread_create` 返回非零值，表示創建執行緒失敗，透過 perror 輸出錯誤訊息，並強制退出程式
* **pthread_join**
  >```c
  >int pthread_join(pthread_t thread, void **retval);
  >```
  >* `pthread_t thread`: 要等待的執行緒的 ID
  >* `void **retval`: 一個指向指標的指標，用於接收執行緒的返回值。如果不需要返回值，可以傳入 `NULL`

## Output
### Q1
```c
===========================Before Fork===============================
[Parent] pid=23396, global variable global_a: 123
[Parent] Offest of logical address:[0x4c70f0], Physical address:[0x73dd90f0]
===========================After Fork===============================
[Child] pid=23397, global variable global_a:123
[Child] Logical address::[0x4c70f0], Physical address:[0x73dd90f0]
===========================trigger CoW===============================
[Child] pid=23397, global variable global_a:789
[Child] Logical address::[0x4c70f0], Physical address:[0x6e9860f0]
____________________________________________________________________________
[Parent] pid=23396, global variable global_a:123
[Parent] Logical address::[0x4c70f0], Physical address:[0x73dd90f0]
```
### Q2
```c
global element a[0]:
Offest of logical address:[0xbe48e2fa0018]   Physical address:[0x127b95018]
========================================================================
global element a[1999999]:
Offest of logical address:[0xbe48e3741214]   Physical address:[0x11e206214]
========================================================================
```
可以發現 `a[1999999]`-`a[0]` != 4*(1999999-0) = 79999996
所以在一開始宣告 `int a[2000000];` 時，`a[1999999]` 並還沒有被分配實體記憶體位址

---
>[time= Nov, 2024]
