import java.nio.charset.Charset;
import java.util.Random;

// The Producer of Producer/Consumer
//   a thread the will put items on the queue

public class Producer implements Runnable {

    protected BlockingQueue queue = null;
    private int count;

    public Producer(BlockingQueue queue, int count) {
        this.queue = queue;
        this.count = count;
    }

    public void run() {
        try {
            // Place count objects on the queue
            for (int i=0; i<this.count; i++)
            {
                // a small string for testing
                queue.put(Integer.toString(i));

                // a large string for evaluation
//                byte[] array = new byte[4096]; // length is bounded by 7
//                new Random().nextBytes(array);
//                String putstr = new String(array, Charset.forName("UTF-8"));
//                queue.put(putstr);

                // output if your debugging
                System.out.println("put" + Integer.toString(i));

                // sleep can slow things down to make output understandable
//                Thread.sleep(1000);
            }
        } catch (InterruptedException e) {

            e.printStackTrace();
        }
    }
}
