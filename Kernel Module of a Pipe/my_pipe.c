#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>


MODULE_LICENSE("GPL");

#define DEVICE_NAME "numpipe"


static struct semaphore full;
static struct semaphore empty;
static struct semaphore mutex_read;
static struct semaphore mutex_write;

char** pipe;
static int pipe_size;
static int pipe_space_left;

module_param(pipe_size, int, 0);

//static struct miscdevic pipe_device;
static int open_numpipe (struct inode *pinode, struct file *pfile );
static ssize_t read_numpipe (struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
static ssize_t write_numpipe (struct file *pfile , const char __user *buffer, size_t length, loff_t *offset);
static int close_numpipe (struct inode *pinode, struct file *pfile);

static struct file_operations numpipe_fileops = {
        .open = open_numpipe,
        .read = read_numpipe,
        .write = write_numpipe,
        .release = close_numpipe

};
static struct miscdevice pipe_device  = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "numpipe",
        .fops = &numpipe_fileops
};

static int open_numpipe (struct inode *pinode, struct file *pfile )
{
        printk(KERN_INFO "Pipe Opened\n");
        return 0;

}

static ssize_t read_numpipe (struct file* pfile, char* buffer, size_t length, loff_t* offset)
{
        int y=0;
        if(down_interruptible(&mutex_read) < 0)
        return -1;

        if(down_interruptible(&full) < 0)
        return -1;

        copy_to_user(&buffer[y], &pipe[y], length);
        pipe_space_left++;
        y++;

        up(&empty);
        up(&mutex_read);

        printk(KERN_ALERT "In read function!\n");
        return length;

}


static ssize_t write_numpipe (struct file *pfile , const char* buffer, size_t length, loff_t *offset)
{
        int y=0;
        if(down_interruptible(&mutex_write) < 0)
        return -1;

        if(down_interruptible(&empty) < 0)
        return -1;

        copy_from_user(&pipe[y], &buffer[y], length);
        pipe_space_left--;
        y++;

        up(&full);
        up(&mutex_write);

        printk(KERN_ALERT "In %s function!\n", __FUNCTION__);
        return length;

}




static int close_numpipe (struct inode *pinode, struct file *pfile)
{
        printk(KERN_INFO "Pipe Closed\n");
        return 0;
}

int init_pipe(void)
{
        int x=0;
        int device_return;
        //fifo_pipe.name = DEVICE_NAME;
        //fifo_pipe.minor = MISC_DYNAMIC_MINOR;
        //fifo_pipe.fops = &numpipe_fileops;
        device_return = misc_register(&pipe_device);
        if(device_return < 0)
        {
                printk(KERN_INFO "Pipe Add Failed\n");
                return device_return;
        }

        printk(KERN_INFO "Pipe Added. Pipe size : %d\n", pipe_size );
        pipe=(char**)kmalloc(sizeof(char*) * pipe_size,GFP_KERNEL);

        for( x = 0; x< pipe_size; x++)
        {
                pipe[x] = (char*)kmalloc(sizeof(char)*100,GFP_KERNEL);
        }

        sema_init(&full, 0);
        sema_init(&empty, pipe_size);
        sema_init(&mutex_read,1);
        sema_init(&mutex_write,1);

        pipe_space_left = pipe_size;

        return 0;
}

// called when module is removed
void exit_pipe(void)
{
        int y;
        for( y = 0; y<pipe_size; y++)
        {
                kfree(pipe[y]);
        }
        kfree(pipe);
        misc_deregister(&pipe_device);
        //if (ret < 0)
        //{
              printk(KERN_ALERT "Pipe Unregistered \n");
        //}

}

module_init(init_pipe);
module_exit(exit_pipe);

