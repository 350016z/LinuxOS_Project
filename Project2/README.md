# Project2
## Description
Implement a system call for the **Wait Queue** functionality, allowing user applications to operate this feature via system calls.  
> A Wait Queue is a synchronization mechanism used to put a process to sleep while waiting for a condition, avoiding resource waste.

**Objective:**  Enable Threads to enter and leave the Wait Queue while adhering to the FIFO (First In, First Out) rule.

## System Call ( Custom wait queue-like functionality )
path: `/usr/src/linux-5.15.137` 
1. 在 `kernel/` 路徑下新增 `project2.c`
    >```c
    >while (next_to_exit == node->task) {
    >    mutex_unlock(&queue_lock); // 釋放鎖，允許其他進程操作共享資源
    >    schedule();               // 讓出 CPU，進入等待狀態
    >    mutex_lock(&queue_lock);  // 被喚醒後重新獲取鎖，繼續執行
    >}
    >```
    >main thread 在執行上面程式碼時，`schedule()` 的喚醒條件，不是來自 `wake_up_process(node->task)`，而是 **sub thread_i 成功退出 `wait_queue` 並完成執行後，交還 CPU 使用權**。

2. Edit the kernel Makefile
    ```bash
    vim kernel/Makefile
    ```
3. Declare the system call in the header file
    ```bash
    vim arch/x86/entry/syscalls/syscall_64.tbl
    450	common	call_my_wait_queue	sys_call_my_wait_queue
    ```
4. Add the new system call to the syscall table
    ```bash
    vim include/linux/syscalls.h
    asmlinkage	long 	sys_call_my_wait_queue(int id)
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
### Terminal
```clike
enter wait queue thread_id: 0
enter wait queue thread_id: 1
enter wait queue thread_id: 2
enter wait queue thread_id: 3
enter wait queue thread_id: 4
enter wait queue thread_id: 5
enter wait queue thread_id: 7
enter wait queue thread_id: 8
enter wait queue thread_id: 9
enter wait queue thread_id: 6
start clean queue ...
exit wait queue thread_id: 0
exit wait queue thread_id: 1
exit wait queue thread_id: 2
exit wait queue thread_id: 3
exit wait queue thread_id: 4
exit wait queue thread_id: 5
exit wait queue thread_id: 7
exit wait queue thread_id: 8
exit wait queue thread_id: 9
exit wait queue thread_id: 6
```

### kernel (`dmesg`)
```clike
[ 1416.067848] [enter] Thread 4155 waiting...
[ 1416.067855] Current FIFO list: 
[ 1416.067857] thread 4155 ---> 
[ 1416.067860] END
[ 1416.067938] [enter] Thread 4156 waiting...
[ 1416.067942] Current FIFO list: 
[ 1416.067944] thread 4155 ---> 
[ 1416.067947] thread 4156 ---> 
[ 1416.067950] END
[ 1416.067966] [enter] Thread 4157 waiting...
[ 1416.067971] Current FIFO list: 
[ 1416.067973] thread 4155 ---> 
[ 1416.067975] thread 4156 ---> 
[ 1416.067978] thread 4157 ---> 
[ 1416.067981] END
[ 1416.068122] [enter] Thread 4158 waiting...
[ 1416.068126] Current FIFO list: 
[ 1416.068128] thread 4155 ---> 
[ 1416.068135] thread 4156 ---> 
[ 1416.068138] thread 4157 ---> 
[ 1416.068141] thread 4158 ---> 
[ 1416.068143] END
[ 1416.068375] [enter] Thread 4159 waiting...
[ 1416.068379] Current FIFO list: 
[ 1416.068381] thread 4155 ---> 
[ 1416.068384] thread 4156 ---> 
[ 1416.068386] thread 4157 ---> 
[ 1416.068389] thread 4158 ---> 
[ 1416.068392] thread 4159 ---> 
[ 1416.068394] END
[ 1416.068733] [enter] Thread 4160 waiting...
[ 1416.068736] Current FIFO list: 
[ 1416.068737] thread 4155 ---> 
[ 1416.068738] thread 4156 ---> 
[ 1416.068739] thread 4157 ---> 
[ 1416.068740] thread 4158 ---> 
[ 1416.068741] thread 4159 ---> 
[ 1416.068742] thread 4160 ---> 
[ 1416.068742] END
[ 1416.069683] [enter] Thread 4162 waiting...
[ 1416.069688] Current FIFO list: 
[ 1416.069690] thread 4155 ---> 
[ 1416.069693] thread 4156 ---> 
[ 1416.069695] thread 4157 ---> 
[ 1416.069698] thread 4158 ---> 
[ 1416.069701] thread 4159 ---> 
[ 1416.069704] thread 4160 ---> 
[ 1416.069706] thread 4162 ---> 
[ 1416.069709] END
[ 1416.069883] [enter] Thread 4163 waiting...
[ 1416.069887] Current FIFO list: 
[ 1416.069889] thread 4155 ---> 
[ 1416.069892] thread 4156 ---> 
[ 1416.069895] thread 4157 ---> 
[ 1416.069897] thread 4158 ---> 
[ 1416.069900] thread 4159 ---> 
[ 1416.069903] thread 4160 ---> 
[ 1416.069905] thread 4162 ---> 
[ 1416.069944] thread 4163 ---> 
[ 1416.069947] END
[ 1416.069950] [enter] Thread 4164 waiting...
[ 1416.069954] Current FIFO list: 
[ 1416.069956] thread 4155 ---> 
[ 1416.069959] thread 4156 ---> 
[ 1416.069962] thread 4157 ---> 
[ 1416.069964] thread 4158 ---> 
[ 1416.069967] thread 4159 ---> 
[ 1416.069970] thread 4160 ---> 
[ 1416.069972] thread 4162 ---> 
[ 1416.069975] thread 4163 ---> 
[ 1416.069978] thread 4164 ---> 
[ 1416.069980] END
[ 1416.070245] [enter] Thread 4161 waiting...
[ 1416.070250] Current FIFO list: 
[ 1416.070252] thread 4155 ---> 
[ 1416.070254] thread 4156 ---> 
[ 1416.070257] thread 4157 ---> 
[ 1416.070260] thread 4158 ---> 
[ 1416.070262] thread 4159 ---> 
[ 1416.070265] thread 4160 ---> 
[ 1416.070268] thread 4162 ---> 
[ 1416.070270] thread 4163 ---> 
[ 1416.070273] thread 4164 ---> 
[ 1416.070275] thread 4161 ---> 
[ 1416.070278] END
[ 1417.070001] [clean] Cleaning wait queue...
[ 1417.070013] Current FIFO list: 
[ 1417.070019] thread 4155 ---> 
[ 1417.070028] thread 4156 ---> 
[ 1417.070036] thread 4157 ---> 
[ 1417.070043] thread 4158 ---> 
[ 1417.070051] thread 4159 ---> 
[ 1417.070058] thread 4160 ---> 
[ 1417.070066] thread 4162 ---> 
[ 1417.070074] thread 4163 ---> 
[ 1417.070081] thread 4164 ---> 
[ 1417.070088] thread 4161 ---> 
[ 1417.070096] END
[ 1417.070136] [clean] Waking up thread 4155
[ 1417.070388] [enter] Thread 4155 exit wait queue
[ 1417.071735] [clean] Waking up thread 4156
[ 1417.072748] [enter] Thread 4156 exit wait queue
[ 1417.072796] [clean] Waking up thread 4157
[ 1417.073185] [enter] Thread 4157 exit wait queue
[ 1417.073339] [clean] Waking up thread 4158
[ 1417.073502] [enter] Thread 4158 exit wait queue
[ 1417.073548] [clean] Waking up thread 4159
[ 1417.073807] [enter] Thread 4159 exit wait queue
[ 1417.073846] [clean] Waking up thread 4160
[ 1417.073948] [enter] Thread 4160 exit wait queue
[ 1417.073995] [clean] Waking up thread 4162
[ 1417.074663] [enter] Thread 4162 exit wait queue
[ 1417.074709] [clean] Waking up thread 4163
[ 1417.075546] [enter] Thread 4163 exit wait queue
[ 1417.075588] [clean] Waking up thread 4164
[ 1417.076625] [enter] Thread 4164 exit wait queue
[ 1417.076656] [clean] Waking up thread 4161
[ 1417.077008] [enter] Thread 4161 exit wait queue
[ 1417.077023] Current FIFO list: 
[ 1417.077032] END
```

---
>[time= Dec, 2024]
