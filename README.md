# C/C++ Network Programming Projects Overview

- There's essentially two types of C programs in this repository
  1. a client server program which implements a **class registration platform**. The program should allow the user to register themselves by entering their details which include the student registration number and names. The program should maintain a list of registered students in a text file with the following format. No double registrations
  2. a client server program which implements a **simple calculator**. The calculator can perform the following operations (+, -, ×, /) chosen by the user on any two integers supplied by the user.

- Each of these programs have **tcp (connection-oriented)** and **udp (connectionless)** implementations.
- Furthermore, those implementations each have **concurrent versions** and **iterative versions**
- To achieve concurrency, the following techniques have been employed:

  a) Use of *__fork()__* function <br>
  b) Use of *__select()__* function. Note that select() has only been implemented for concurrent-oriented (tcp) versions.<br>
  c) Use of threads. Note that servers implemented with threads are of .cpp file extension and not .c<br>
