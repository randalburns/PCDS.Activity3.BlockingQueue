// Implementation of a subset of the java.util.concurrent.BlockingQueue interface 
public class BlockingQueue {

    // queue and dequeue string data -- not objects -- makes it easier to read
    private String [] queue;

    // queue metadata  
    private int limit = 10;
    private int head = 0;
    private int qlen = 0;

    // Create an array of strings as the queue
    public BlockingQueue(int limit){
        this.limit = limit;
        this.queue = new String [limit];
    }

    public void put(String item)
    throws InterruptedException  {
    
        // variable for slot item goes in
        int slot;

        boolean notifyflag = false;

        synchronized(this) {

            // don't add if the queue is full
            while(this.qlen == this.limit) {
                wait();
            }

            // get slot and update head and length
            slot = this.head;
            this.head++;
            if (this.head == this.limit) {
                this.head = 0;
            }

            this.qlen++;

            // notify takers if this is the first item in queue
            if (this.qlen == 1 ) {
               notifyflag = true; 
               this.queue[slot] = item;
               notifyAll();
            }
        }

        // add the item
        if (!notifyflag)
        {
            this.queue[slot] = item;
        }
    }

    public String take()
    throws InterruptedException {

        // slot to be taken and deleted
        int tail;

        boolean notifyflag = false;

        String ret_obj = null;

        synchronized(this) {

            // don't take from an empty queue
            while(this.qlen == 0) {
                wait();
            }

            //get slot 
            tail = (this.head - this.qlen + this.limit) % this.limit;

            // if taking from a full queue, notify putters
            if (this.qlen == this.limit)  {
                notifyflag = true;
                ret_obj = this.queue[tail];
                queue[tail]=null;
                notifyAll();   
            }
            
            // update queue length
            this.qlen--;
        }

        if (!notifyflag)
        {   
            // take the item and dereference pointer for garbage collection
            ret_obj = this.queue[tail];
            queue[tail]=null;
        }

        // return item
        return ret_obj;
    }
}
