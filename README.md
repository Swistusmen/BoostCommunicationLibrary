# BoostCommunicationLibrary
Simple library which handles tcp communication in a nice way. Created as one file: CommunicationMenager.cpp-it has 2 classes needed for nice communication on client side, and  example main code. Server is a modified version from Boost Asio site

Compiled under boost asio v1.7.4

<h3> Test Server </h3>

```
g++ server.cpp -o asyncServer.out -pthread -lboost_thread
./asyncServer.out
```

<h3> Example code- use of library </h3>

```
g++ CommunicationMenager.cpp -o client.out -pthread -lboost_thread
./client.out
```
