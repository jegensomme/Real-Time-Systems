# Supplier-Consumer

Two threads - Provider and Consumer - work with some common integer variable Buffer,
in this case, each request of the Provider increases the Buffer value by 1, and the Consumer decreases

Task 1

1. Using the semaphore mechanism, implement the program model of the "Supplier" and "Consumer" operation, ensuring that the following conditions are met:
    
a. The "provider" cannot write information and is blocked if the Buffer value exceeds a certain allowed value ("buffer is full")
b. "Consumer" Blocked if Buffer is 0 ("buffer is empty")
c. Simultaneous recording and simultaneous operation of streams with the Buffer variable is not allowed

2. Fixing the values ​​of the current time at the beginning and end of work in the "critical section" of the Supplier and the Consumer, show the fulfillment of condition 1c.
Task 2.

       Implement the conditions of "Task 1" using a conditional variable mechanism for synchronization.

