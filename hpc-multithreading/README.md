# System and Distributed Computing – HPC Multithreading

## Overview

Develop a distributed scientific computation service using a multi-threaded client-server system for high-performance computing (HPC). The server handles intensive mathematical operations on 32-bit integers and can process up to 12 concurrent requests using inter-process communication (IPC) and synchronisation techniques.

---

## Requirements

### General System Overview

1. **Multi-threaded Client-Server System**:
  - The client sends 32-bit integer queries to the server.
  - The server processes up to 12 concurrent requests using threads.

2. **Client Process**:
  - Non-blocking interface for continuous input of 32-bit integers.
  - Warns the user when 12 requests are outstanding.

3. **Server Process**:
  - Handles concurrent processing with threads (up to 320 threads).
  - Factorises variations of the input number by rotating bits.
  - Uses the trial division method for factorisation.

### Communication Protocol

4. **Shared Memory**:
  - Client writes to a shared 32-bit variable `clientslot`.
  - Server responds using a shared array `slot[12]` for up to 12 concurrent responses.

5. **Handshaking Protocol**:
  - **Clientflag** (`char`): Indicates new data availability for the server.
  - **Serverflag[12]** (`char array`): Indicates data availability for each slot.
  - Ensures proper synchronisation between client and server.

### Synchronisation & Progress Tracking

6. **Synchronisation**:
  - Use semaphores implemented with `pthread` mutexes and condition variables.
  - Prevent data loss when multiple threads write to shared memory slots.

7. **Progress Reporting**:
  - Threads report progress in increments ≤5%.
  - Server calculates overall progress and updates the client.
  - Progress is displayed every 500ms if no response or new request is made.

### User Interface

8. **Progress Display**:
  - **Format 1**: Textual representation.
  - **Format 2**: Horizontal progress bars.

    > Progress: Query 1: X% | Query 2: Y% | Query 3: Z%
    > Progress: Q1:50% ▓▓▓▓ | Q2:30% ▓▓ | Q3:80% ▓▓▓▓▓▓▓

9. **Completion Notification**:
  - Server sends a completion message indicating all factors found and time taken.

### Special Features

10. **Test Mode**:
  - Activated by entering `0` as the input number.
  - Launches 3 sets of 10 threads, simulating 3 user requests.
  - Each thread returns 10 numbers with a random delay (10-100ms).
  - Progress output is disabled during the test.

---

## Full Program Requirements

1. Your program should consist of a multi-threaded server and single- or multi-threaded client process.

2. The client will query the user for 32-bit integers to be processed and will pass each request to the server to process and will immediately request the user for more input numbers or quit to quit.

3. The server will start up either the number of specified threads if given (see Req.15) or as many threads as there are binary digits times the max number of queries (i.e. 320 threads). The server will take each input number (unsigned long) and create 32 numbers to be factorised from it. Each thread will be responsible for factorising an integer derived from the input number that is rotated right by a different number of bits.
  Given an input number K, each thread #X will factorise K rotated right by increasing number of bits. For example, thread #0 will factorise the number K rotated right by 0 bits, thread #1 will factorise K rotated right by 1 bit, thread #2 will factorise K rotated right by 2 bits etc.
  Tip: Rotating an integer K by B bits = ( K >> B) | (K << 32 – B).

4. The trial division method should be used for integer factorisation.

5. The server must handle up to 12 simultaneous requests without blocking.

6. The client is non-blocking. Up to 12 server responses may be outstanding at any time, if the user makes a request while 12 are outstanding, the client will warn the user that the system is busy.

7. The client will immediately report any responses from the server and in the case of the completion of a response to a query, the time taken for the server to respond to that query.

8. The client and server will communicate using shared memory. The client will write data for the server to a shared 32-bit variable called ‘clientslot’. The server will write data for the client to a shared array of 32-bit variables called a ‘slot’ that is 12 elements long. Each element in the array (slot) will correspond to an individual client query so only a maximum of 12 queries can be outstanding at any time. This means that any subsequent queries will be blocked until one of the 12 outstanding queries completes, at which times its slot can be reused by the server for its response to the new query.

9. Since the client and server use shared memory to communicate a handshaking protocol is required to ensure that the data gets properly transferred. The server and client need to know when data is available to be read and data waiting to be read must not be overwritten by new data until it has been read. For this purpose, some shared variables are needed for signalling the state of data. Tip: A single char variable and another array of 12 chars should be sufficient for this communication.

  You can follow the following handshaking protocol:
    Define char clientflag and char serverflag[12] (one for each query response/slot).
    The protocol operation is:
      - Both are initially 0 meaning that there is no new data available
      - A client can only write data to ‘clientslot’ for the server while clientflag == 0; the client must set clientflag = 1 to indicate to the server that new data is available for it to read
      - The server will only read data from ‘clientslot’ from the client if there is a free slot and if clientflag ==1. It will then write the index of the slot that will be used for the request back to ‘clientslot’ and set clientflag = 0 to indicate that the request has been accepted.
      - A server can only write data to slot x for the client while serverflag[x] == 0; the server must set serverflag[x] = 1 to indicate to the client that new data is available for it to read.
      - The client will only read data from slot x if serverflag[x] ==1 and will set serverflag[x] = 0 to indicate that the data has been read from slot x.

10. The server will not buffer factors but each thread will pass any factors as they are found one by one back to the client. Since the server may be processing multiple requests, each time a factor is found it should be written to the correct slot so the client can identify which request it belongs to. The slot used by the server for responding to its request will be identified to the client at the time the request is accepted by the server through the shared ‘clientslot’ variable.

11. Since many threads will be trying to write factors to the appropriate slot for the client simultaneously you will need to synchronise the thread’s access to the shared memory slots so that no factors are lost. You will need to write a semaphore class using pthread mutexes and condition variables to used for controlling access to the shared memory so that data is not lost.

12. Each factorising thread will report its progress as a percentage in increments not larger than 5% as it proceeds to the server primary thread. The server will use the individual thread progress values to calculate an overall progress % for each request being processed and pass this back to the client in increments not larger than 5%. This will require a second shared array char progress[12] that is 12 elements long but no handshaking protocol or synchronisation since it does not matter if the client misses some values or reads them multiple times.

13. While not processing a user request or there has been no server response for 500 milliseconds, the client should display a progress update messages for each outstanding request (repeating every 500ms until there is a server response or new user request). The repeated progress message should be displayed in a single row of text. The message should be similar to one of the following formats (each format gets a different marking score):
  Format 1:
    > Progress: Query 1: X% Query2: Y% Query3: Z%
  Format 2: use little horizontal progress bars that don’t scroll up, i.e.
    > Progress: Q1:50% ▓▓▓▓ | Q2:30% ▓▓ | Q3: 80% ▓▓▓▓▓▓▓ |

14. When the server has finished factorising all the variations of an input number for a query it will return an appropriate message to the client so that it can calculate the correct response time and alert the user that all the factors have been found.

15. The system will have a test mode that will be activated by the user entering 0 as the number to be factored. This will be passed to the server which will launch 3 sets of 10 threads each, each set will simulate one of three user requests. Each thread in each set will return to the client 10 numbers starting with the thread # times 10 and incrementing by 1, with a random delay of between 10ms100ms between return values. For example thread #0 will return numbers 0-9, thread #1 will return numbers 10-19, thread #2 returns numbers 20-29. Progress output will be disabled during the test and it will only run if the server is not processing any outstanding requests instead a warning will be issued to the user. If the test mode is not implemented no marks will be awarded for requirements 9 and 11.
