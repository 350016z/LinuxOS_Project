/*path: /usr/src/linux-5.15.137/kernel/project1.c*/
#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/sched.h>
#include<asm/pgtable.h>

SYSCALL_DEFINE1 (my_get_physical_addresses, void *, addr_p){

	unsigned long vaddr = (unsigned long)addr_p;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	unsigned long paddr=0;
	unsigned long page_addr=0;     /* from pte*/
	unsigned long page_offset=0;   /* from virtual addr*/

	/* linuxLecture_3_9-4*/
	pgd=pgd_offset(current->mm,vaddr);  /*獲得addr對應的pgd的地址*/
	if(pgd_none(*pgd) || unlikely(pgd_bad(*pgd))){
		return 0;
	}

	p4d=p4d_offset(pgd,vaddr);  /*獲得addr對應的p4d的地址*/
	if(p4d_none(*p4d) || unlikely(p4d_bad(*p4d))){
		return 0;
	}

	pud=pud_offset(p4d,vaddr);
	if(pud_none(*pud) || unlikely(pud_bad(*pud))){
		return 0;
	}

	pmd=pmd_offset(pud,vaddr);
	if(pmd_none(*pmd) || unlikely(pmd_bad(*pmd))){
		return 0;
	}

	pte=pte_offset_kernel(pmd,vaddr);
	if(pte_none(*pte)){
		return 0;
	}

	/* PAGE_MASK屏蔽掉offset區域的值*/
	/* PFN: Page Frame Number
	 * Extracts the PFN from a (pte|pmd|pud|pgd)val_t of a 4KB page */
	page_addr=pte_val(*pte) & PAGE_MASK & PTE_PFN_MASK;
	page_offset= vaddr & ~PAGE_MASK;
	paddr=page_addr | page_offset;
	printk("page_addr=0x%lx,page_offset=0x%lx\n",page_addr,page_offset);
	printk("vaddr=%lx,paddr=%lx\n",vaddr,paddr);

	return paddr;
}
