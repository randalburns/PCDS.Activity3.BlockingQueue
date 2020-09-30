## Assignment #2: Java Blocking Queue

This project is derived from the Java BlockingQueue tutorial at [http://tutorials.jenkov.com/java-util-concurrent/blockingqueue.html](http://tutorials.jenkov.com/java-util-concurrent/blockingqueue.html). This is a fundamental data structure needed to implement a producer/consumer parallel progamming pattern. In this project, you are going to implement a simplified version of a BlockingQueue that put and takes Strings from many producer and many consumer threads.  Another tutorial provides a simplified implementation based on a linked list [http://tutorials.jenkov.com/java-concurrency/blocking-queues.html](http://tutorials.jenkov.com/java-concurrency/blocking-queues.html). Java itself provides many implementations of the BlockingQueue interface.

### Programing activities

Clone the repository at []() into an environment that has a reasonable version of Java.  This project was implemented on Java 11, but is pretty vanilla so should work everywhere.

#### Implement BlockingQueue in an Array.

In the file `BlockingQueue.c`, fill out the sections labelled ``TODO``. In doing so you will:

- Implement a queue on top of an array.
    - use the variable `head` to point to the next available element in a list.
    - use the variable `qlen` to encode the number of items in the queue
    - define logic to wrap from the last element in the array to the first element in the array
        - you will need to consider this for both `head`  and `qlen`.
        - `limit` is the maximum number of elements in the queue.
- Reimplement the wait/notify pattern of [http://tutorials.jenkov.com/java-concurrency/blocking-queues.html](http://tutorials.jenkov.com/java-concurrency/blocking-queues.html).  This code will:
   - have producers `wait()` when the queue is full
   - have consumers `wait()` when the queue is empty
   - have producers `notifyAll()` to activate consumers when putting an element into an empty list.
   - have consumer `notifyAll()` to activate producers when taking an element from a full list.

Debug your implementation on two threads and a small number of items.  Make sure that it works correctly for many threads and a large number of items.

Turn your implementation of `BlockingQueue.java` in on Gradescope as file XXXXX.

#### Change the synchronization discipline

In this alternate implementation, we can "safely" perform synchronization at a finer granularity. The goal is to synchronize only on the shared variables `head` and `qlen` and access the array itself outide of the synchronized blocks.

- Remove the `synchronized` from the function definitions.
- Implement a synchronized block within each function that makes accesses to `head` and `qlen` thread safe. The following lines should be outside of the synchronized blocks.
```java
    // in put()
    this.queue[slot] = item;

    // in take()
    String ret_obj = this.queue[tail];
    queue[tail]=null;
```

 Debug your implementation on two threads and the small data.  Make sure that it works correctly for many threads and the big data.

Turn your implementation of `BlockingQueue.java` in on Gradescope as file YYYYY.

#### Questions:

1. The code calls `notifyAll()`, what happens in multiple threads are waiting on a single queue?  Why is this thread safe?

2. When you implemented the synchronized blocks inside the function, did you synchronize on the class or the object?  Why is this the right decision?  In answering this questions, consider what would happen if a program used multiple queues for different tasks.

3. What performance advantage might arise from placing the array operations outside the synchronized block, i.e. when would we prefer the second implmentation?
 -  (Note: there is no noticeable performance difference between these implementations -- even if you use long Strings -- because Java Strings are immutable and lazily copied by reference. A performance difference would arise if we did deep copies of Objects onto the queue, but I decided to keep the code simpler.)

4. The synchronized blocks that do not include the array operations are not truly safe.
    1. In what scenario will having the array operations outside of synchronized blocks lead to a concurrency bug?
    2. What timing assumption needs to be made to avoid this bug?
    3. Is this assumption reasonable?

Turn in a PDF that contains the answers to this question on GradeScope.

**Due date**: Monday October 12, 2020. 11:59:59 pm.
