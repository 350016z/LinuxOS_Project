#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//void * my_get_physical_addresses(void *);
void* my_get_physical_addresses(void* address) {
	return (void*)syscall(449, (void*)address);
};


int global_a = 123;

int main() {
	void* parent_phys_addr, * child_phys_addr;

	printf("===========================Before Fork===========================\n");
	parent_phys_addr = my_get_physical_addresses(&global_a);
	printf("[Parent] pid=%d, global variable global_a: %d\n", getpid(), global_a);
	printf("[Parent] Logical address: [%p], Physical address: [%p]\n", &global_a, parent_phys_addr);

	printf("===========================After Fork===========================\n");
	pid_t pid = fork();
	if (pid > 0) {
		/*parent code*/
		// wait for child
		wait(NULL);

		parent_phys_addr = my_get_physical_addresses(&global_a);
		printf("[Parent] pid=%d, global variable global_a: %d\n", getpid(), global_a);
		printf("[Parent] Logical address: [%p], Physical address: [%p]\n", &global_a, parent_phys_addr);
	} else {
		/*child code*/
		child_phys_addr = my_get_physical_addresses(&global_a);
		printf("[Child] pid=%d, global variable global_a: %d\n", getpid(), global_a);
		printf("[Child] Logical address: [%p], Physical address: [%p]\n", &global_a, child_phys_addr);

		printf("===========================Trigger CoW===========================\n");
		/*----------------------- trigger CoW (Copy on Write) -----------------------------------*/
		global_a = 789;
		child_phys_addr = my_get_physical_addresses(&global_a);
		printf("[Child] pid=%d, global variable global_a: %d\n", getpid(), global_a);
		printf("[Child] Logical address: [%p], Physical address: [%p]\n", &global_a, child_phys_addr);		
		print("____________________________________________________________________________\n");
	}			  
	return 0;
}
