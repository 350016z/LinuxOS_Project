#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>    
#include <sys/wait.h>

void *my_get_physical_addresses(void *vaddr){
	return (void *)syscall(449, vaddr);
}

int a[2000000]; 

int main()
{ 
	int      loc_a;
	void     *phy_add_start, *phy_add_end;  

	phy_add_start = my_get_physical_addresses(&a[0]);
	printf("global element a[0]:\n");  
	printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[0], phy_add_start);              
	printf("========================================================================\n"); 

	phy_add_end = my_get_physical_addresses(&a[1999999]);
	printf("global element a[1999999]:\n");  
	printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[1999999], phy_add_end);              
	printf("========================================================================\n"); 

	printf("Offset of logical address difference: &a[1999999] - &a[0] = %ld\n", &a[1999999] - &a[0]);
	printf("Physical address difference: phy_add_end - phy_add_start = %ld\n", phy_add_end - phy_add_start);

	return 0;
}
