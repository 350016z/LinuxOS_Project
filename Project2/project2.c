#include <linux/syscalls.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/list.h>

static DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);
static DEFINE_MUTEX(queue_lock);

struct fifo_node {
    struct task_struct *task; 
    struct list_head list;    
    bool woken_up;           
};

static LIST_HEAD(fifo_list); 

static void print_fifo_list(void) {
    struct fifo_node *node;
    printk(KERN_INFO "Current FIFO list: ");
    list_for_each_entry(node, &fifo_list, list) {
        printk(KERN_INFO "thread %d ---> ", node->task->pid);
    }
    printk(KERN_INFO "END\n");
}

static struct task_struct *next_to_exit = NULL;
static int enter_wait_queue(void) {
    struct fifo_node *node;

    node = kmalloc(sizeof(struct fifo_node), GFP_KERNEL);
    if (!node)
        return 0;

    node->task = current;
    node->woken_up = false;

    mutex_lock(&queue_lock);
    list_add_tail(&node->list, &fifo_list);
    printk(KERN_INFO "[enter] Thread %d waiting...\n", current->pid);
    print_fifo_list();
    mutex_unlock(&queue_lock);

    DEFINE_WAIT(__wait);
    prepare_to_wait(&my_wait_queue, &__wait, TASK_INTERRUPTIBLE);

    while (true) {
        mutex_lock(&queue_lock);

        if (node->woken_up && next_to_exit == current) {
            list_del(&node->list);
            next_to_exit = NULL;  
            kfree(node);
            printk(KERN_INFO "[enter] Thread %d exit wait queue\n", current->pid);
            mutex_unlock(&queue_lock);
            break;
        }
        mutex_unlock(&queue_lock);

        schedule();
        if (signal_pending(current)) {
            return 0;
        }
    }

    finish_wait(&my_wait_queue, &__wait);
    return 1;
}

static int clean_wait_queue(void) {
    struct fifo_node *node;

    mutex_lock(&queue_lock);
    printk(KERN_INFO "[clean] Cleaning wait queue...\n");
    print_fifo_list();

    if (list_empty(&fifo_list)) {
        mutex_unlock(&queue_lock);
        return 0;
    }

    while (!list_empty(&fifo_list)) {
        node = list_first_entry(&fifo_list, struct fifo_node, list);

        next_to_exit = node->task;
        node->woken_up = true;
        
        wake_up_process(node->task);
        printk(KERN_INFO "[clean] Waking up thread %d\n", node->task->pid);

        while (next_to_exit == node->task) {
            mutex_unlock(&queue_lock);
            schedule(); 
            mutex_lock(&queue_lock);
        }
    }

    print_fifo_list();
    mutex_unlock(&queue_lock);
    return 1;
}


SYSCALL_DEFINE1(call_my_wait_queue, int, id) {
    switch (id) {
        case 1:
            return enter_wait_queue();
        case 2:
            return clean_wait_queue();
    }
    return 0;
}
