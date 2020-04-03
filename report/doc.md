# EAU2 Project

By: Snowy Chen and Joe Song

## Introduction

The eau2 system is a distributed system meant to handle large pieces of data
across multiple devices. As a result, this system can be used as a larger scale
concurrency system manipulating or analyzing a large dataset in a shorter amount
of time by distributing the workload across multiple devices.

In order to use and run this project, the following tools must be installed:
- g++
- valgrind
- cmake
- git
- clang
- python3

The following are terminal commands to run within the root of the project folder
in order to build and run the various parts of the project system:
- `make build_all`: Builds the whole project.
- `make test_all`: Runs all of the test suites of the project
- `make demo_echo`: Runs the demo for a EchoServer and EchoClient network
system. Note: This system currently does not run within Docker.
- `make demo_distro_app`: Runs the demo for the distributed app network
system. Note: This system currently does not run in Docker.
- `make clean`: Cleans the builds
- `make docker_build_all`: Builds the whole project within a Docker image. This 
command only will run if Docker is online.
- `make docker_test_all`: Runs all of the test suites of the project within
a Docker image. This command only will run if the Docker is online.

## Architecture

The eau2 system is made up of a server, called the registrar, and a fixed
number of clients, called nodes. The registrar is at a fixed IP address and
port, and all nodes must first connect to the registrar and send a Register
message to the registrar in order to be a part of the eau2 network. Upon
being registered, a Directory message is broadcast to all of the connected
and registered nodes with an array of all currently connected and registered
nodes (include the new node), and a node can find its node id by searching for
its address within the Directory message. A registrar can only support a
fixed maximum number of registered nodes at any single moment in time. During 
normal operation, there should be a fixed number of nodes connected and
registered to the registrar. The only time when that is not the case is when
the eau2 network is starting up as the nodes register with the registrar 
one-by-one, or when the eau2 network is shutting down because the nodes shut
down via a Kill message, or registrar closes so all nodes can no longer 
communicate.

The nodes each contain one or more Key-Value pairs. A Key can be referred by a
String name, and each has a node id that determines which node contains the 
value associated with the key. The Value of the key is a dataframe or a portion
of a dataframe. The nodes themselves can define the data within the dataframe
by themselves, or the Key-Value pair can be assigned to them from the registrar
after all nodes have finished registering.

For nodes that do not contain the Value of a particular Key can send a Get or a
WaitAndGet message to the node id associated to that Key.

## Implementation

### Dataframe Folder

* `dataframe.h`
An implementation of thread that will be used to concurrently iterate
through the rows of a dataframe. A RowerThread_ will only every other
RowerThread_ id row index within the DataFrame.

* `dataframe_helper.h`
A helper class for Column, Row and Schema in order to have a resizeable
data structure with sort of O(1) algorithmic getting methods. This class
primarily stores items in DataItem_, which is a union of various other
primitives. The data type of the value stored within a DataItem must be
determined by the class that uses the ArrayOfArrays.
NOTE: This linked list of arrays does NOT support removing items. Only
replacing them with different values, or adding new ones. This is because
Row, Schema and Column APIs do not have a need for removing items.

* `dataframe_column.h`
Represents one column of a data frame which holds values of a single type.
This abstract class defines methods overriden in subclasses. There is
one subclass per element type. Columns are mutable, equality is pointer
equality. 

* `row.h`
This class represents a single row of data constructed according to a
dataframe's schema. The purpose of this class is to make it easier to add
read/write complete rows. Internally a dataframe hold data in columns.
Rows have pointer equality.

* `rower.h`
An interface for iterating through each row of a data frame. The intent
is that this class should subclassed and the accept() method be given
a meaningful implementation. Rowers can be cloned for parallel execution.

* `schema.h`
A schema is a description of the contents of a data frame, the schema
knows the number of columns and number of rows, the type of each column,
optionally columns and rows can be named by strings.
The valid types are represented by the chars 'S', 'B', 'I' and 'F'.

#### CS4500-A1-part1

The folder contains code for the sorer application created by team CS4500ne. 
As it stands, the folder is a git submodule within the project and then
treated as a library. As a result, this project aims to maintain complete
fidelity to the original source code without making any modifications to the
original source code. In order to integrate the sorer into the project, the
class `SorerIntegrator` was created to transform the data read from the sorer
into a DataFrame. In addition, a separation from the object implementation
was separated by naming them differently: The library code used `Object`, so
the project implementation had to be updated to be `CustomObject` in order to
avoid linking issues upon compile time.

### Network Folder

The communication layer was implemented in two pieces: One is the Registrar, 
and the other is Node. The Registrar is an implementation of an abstract class 
Server, that supports basic server functionality in a thread. The Node is an 
implementation of an abstract class, Client, that supports basic client 
functionality in a thread. The Node has a second thread running that implements 
the same Server abstract class in order to manage client-to-client direct 
communication.

Two abstract classes were implemented in order to support general functionality 
for any type of server or client. They are meant to be built upon in order to 
support more specific functionality through the use of callback functions. This 
means that the abstract Server class has implemented the following 
functionality that all servers must be able to support: supporting services for 
multiple clients, handling new incoming connections, handling incoming messages 
from already connected clients, sending messages to clients, and handling 
closing connections. Similarly, the abstract class Client supports the 
following necessary functions: establishing a connection to the server, 
handling incoming messages from the server, sending messages to the server, 
and closing the connection.

#### Server

The foundation of the server is implemented in the abstract class called 
Server in network.h. It runs on an infinite loop on a separate thread that 
polls for activity on an array of sockets that contain the mater socket, which 
handles incoming connections, and the sockets to each client connected. The 
polling has a timeout of 100ms, so the infinite loop blocks for 100ms before 
running through its other tasks, namely sending messages, and other specific 
functionality.

The infinite loop starts and runs with the `start()` function call, where it 
calls various virtual functions depending on the event that is being handled. 
These function calls can be implemented by concrete Server classes in order to 
support more specific functionality.

The following are the functions with information on its purpose and the event 
that is meant to handle:
- `void init()`: Completes specific tasks needed to start running the server. 
This function will be called after the server has already established a socket 
to listen for incoming connections. Thus, this function will be run just before 
the start of the infinite loop.
- `void run_server()`: A generic function that will be run during every
iteration of the infinite loop, regardless of what had happened (i.e., a 
new connection has been established, a new message has been received, etc.). 
This will be the last function called after polling for activity on the open 
master and client sockets, and after sending messages that were queued up
- `void handle_incoming_message(size_t client_id, unsigned char * buffer, 
size_t num_bytes)`: A function that will be called when a message from a 
client arrives to the server. It will provide a unique id for the client 
ranging from 0 to 1 less than the maximum number of clients the server instance 
can support. It will always provide a non-null pointer to a buffer containing 
the message with the specified number of bytes. This buffer is owned by the 
abstract Server, and will be reused for all incoming messages. As a result, 
the Server implementation should not free the buffer or take ownership of it.
- `void handle_incoming_connection(size_t new_client_id, String *addr, int
port_num)`: A function called after a new client has established connection 
to the server. The new established connection will contain a unique client 
id ranging from 0 to 1 less than the maximum nubmer of clients the server 
instance can support. In addition, it has a string representation of the 
client's IP address and an integer containing the port number of the client.
- `void handle_closing_connection(size_t client_id)`: A function called when a
client has closed its connection to the server. The provided id is the unique 
identifier of the client instance. This function will be called after the 
connection has already been closed.

The Registrar implemented the following virtual functions in order to support 
its specific functionality:
- `void handle_incoming_message(size_t client_id, unsigned char *buffer, size_t
num_bytes)`: New clients will send a message to the server detailing its 
ip address and port number that it will listen for incoming connections in the 
format of "ip:port". The server will broadcast that message to the other 
clients, and add it to its array of clients.
- `void handle_closing_connection(size_t client_id)`: It will remove the
specified client from the array of connected clients. Then it will broadcast 
a message in the format of "c:ip:port" to all of the currently connected clients
within that modified array.

Other general purpose functions implemented by the abstract server class are 
the following:
- `void close_server()`: A thread-safe function to close the server instance. 
It will also close all the connections to the clients as well.
- `bool running()`: A function to query whether or not the server is running.
- `bool send_message(size_t client_id, unsigned char *message, size_t bytes)`: 
A function to send a message to a client at a specified client id. The provided 
buffer is set to be unsigned char in order to support messages that are encoded 
in bytes, and are not ASCII messages. The buffer will also be acquired by the 
abstract Server class, and be put on a queue of messages to send.
- `size_t get_max_clients()`: Gets the maximum number of clients the server
will support at any one time.
- `size_t get_max_receive_size()`: Gets the maximum size of incoming messages.

#### Client

The client abstract class employs the same architecture as the server, 
where it will run on an infinite loop that handles multiple events, polling 
for activity on the socket connected to the server for 100ms. The infinite 
loop can be initiated by `start_client()`.

The following are the functions with information on its purpose and the event 
that is meant to handle:
- `void init()`: Completes specific tasks needed to start running the client. 
This function will be called after the client has already established a 
connection to the server specified in the `start_client()`.
- `void run_client()`: A generic function that will be run during every
iteration of the infinite loop, regardless of what had happened. This will be 
the last function called after polling for activity incoming from the server, 
and after sending messages that were queued up.
- `void handle_incoming_message(unsigned char *buffer, size_t num_bytes)`: A
function that will be called when a message from server arrives. It will always 
provide a non-null pointer to a buffer containing the message with the 
specified number of bytes. This buffer is owned by the abstract Client, and 
will be reused for all incoming messages. As a result, the Client 
implementation should not free the buffer or take ownership of it.
- `void handle_closing_connection()`: A function called when server has closed
its connection. The provided id is the unique identifier of the client 
instance. This function will be called just prior to the client shutting down, 
and the infinite loop is broken.

The Node implemented the following virtual functions in order to support its 
specific functionality:
- `void init()`: Starts the client-to-client direct communication manager and
sends the register message to the registrar server.
- `void handle_incoming_message(unsigned char *buffer, size_t num_bytes)`: The
Node receives directory messages from the server and then passes it to the 
communication manager

The Node's communication manager implemented the following virtual functions 
of server in order to support direct communication between clients:
- `void handle_incoming_message(size_t client_id, unsigned char * buffer, 
size_t num_bytes)`: Handles incoming messages from other clients. 
The client ids should correlate to the same client ids within the directory 
that had been broadcasted out to all of the Nodes.

The communication manager also has functions in order to initiate and 
close direct connection to other nodes in the directory, and can send 
direct messages to the nodes that the communication manager has established 
direct communication with.

Other general purpose functions implemented by the abstract client class are 
the following:
- `void close_client()`: A thread-safe function to close the client instance.
- `bool running()`: A function to query whether or not the client is running.
- `bool send_message(unsigned char *message, size_t bytes)`: A function to send
a message the server. The provided buffer is set to be unsigned char in order 
to support messages that are encoded in bytes, and are not ASCII messages. 
The buffer will also be acquired by the abstract Client class, and be put on a 
queue of messages to send.
- `size_t get_max_receive_size()`: Gets the maximum size of incoming messages.

#### Serialization

All objects have the following functions to assist with their serialization:
```C++
// Provides the number of bytes required in order to serialize the object.
virtual size_t serialization_required_bytes();

// Serializes the object within the buffer provided by the Serializer.
virtual void serialize(Serializer &serializer);
```
All objects that have implemented the above two functions should have a static
function to assist with deserialization with a return type of that object type,
and an argument to a Deserializer passed in by reference.

Helper classes Serializer and Deserializers are provided in order to assist
with serialization and deserialization respectively.

##### Serializer

The Serializer contains a variety of functions to serialize primitive data
types, such as `bool`, `int`, and `double`, that will grow a buffer
quadratically in order to accomodate newly added serialized data. Then when
the desired data has been serialized within the Serializer, a function is 
provided to gain ownership of the buffer containing the serialized data.

The following static functions gets the number of bytes required in order
to serialize the passed in primitive data type value:
- `static size_t get_required_bytes(bool value)`
- `static size_t get_required_bytes(size_t value)`
- `static size_t get_required_bytes(int value)`
- `static size_t get_required_bytes(double value)`

The following functions serializes the passed in primitive data type value
into the resizeable Seralizer buffer:
- `bool set_bool(bool value)`
- `bool set_size_t(size_t value)`
- `bool set_int(int value)`
- `bool set_double(double value)`
- `bool set_generic(unsigned char *value, size_t num_bytes)`

Once the serialization has been completed, the function 
`unsigned char *get_serialized_buffer()` can retrieve the completed buffer
from the Serializer. Note: Once that function has been called, the Serializer
will not allow new data to be serialized, as the Serializer no longer owns
the buffer.

The function `size_t get_size_serialized_data()` can be used to get the
number of bytes of concurrent serialized data has been written into the 
Serializer buffer.

##### Deserializer

The Deserializer is the reverse of the Serializer class, where it takes 
in a buffer, which it does not own and will not manipulate, and provide
helper functions that facilitate interpreting the serialized data into its
primitive data types. The Deserializer will read the data byte-by-byte as
needed.

The following functions can be used to determine whether or not there is
data that can be read from the buffer:
- `size_t get_num_bytes_left()`
- `bool has_byte()`
- `bool has_bool()`
- `bool has_size_t()`
- `bool has_int()`
- `bool has_double()`

The following functions are used to read data from the buffer as a certain 
data type:
- `unsigned char get_byte()`
- `bool get_bool()`
- `size_t get_size_t()`
- `int get_int()`
- `double get_double()`

##### Serialized Messages

All messages are serialized in a binary format compacted in a C-like struct
format and split into two parts: a header and a payload. All messages will
contain the same structure of a header, made up of the following values in
the following order:
```C++
MsgKind message_type
size_t sender_id
size_t target_id
size_t id
size_t payload_length
```

MsgKind is an enumeration value that determines what sort of message the payload
contains, thus determining how to deserialize the serialized payload.

###### Put

This message is sent between nodes in order to update a data chunk in its
home node. It contains the updated dataframe data associated with the Key
specified within its payload.

The values of the payload are formatted in the following order:
```C++
Key key
Dataframe updated_dataframe
```

The Key is serialized in the following C-struct binary format:
```C++
size_t home_id
String key_name
```
where the `key_id` is a unique id of a key.

The dataframe is serialized in the following C-struct binary format:
```C++
size_t num_columns
size_t num_rows
DataType data_type
bool/int/double/String data[num_rows]
DataType data_type
bool/int/double/String data[num_rows]
DataType data_type
bool/int/double/String data[num_rows]
...
```
where `DataType` is an enumeration value that determines the data type the data
array following directly after it. The `data_type` and `data` repeat in 
accordance to how many columns are there.

###### Reply

This message is sent between nodes in response to either a Get or a WaitAndGet
message. It contains the dataframe data associated with the Key specified in
a Get or a WaitAndGet.

The values of the payload are formatted in the following order:
```C++
Key key
Dataframe dataframe
```

In order to prevent redundancy and stale data, the recipient node is not
intended to keep the data within a cache after it has finished using it.

###### WaitAndGet

This message can be sent between nodes. It is a query message asking the
receiving node for the dataframe value of the key specified in the payload.
Unlike the Get message, this notifies the receiving node is expecting the Reply
within the specified timeout. If the sender receives a Reply within the timeout
it will respond with an Ack. Otherwise, the sender would send the recipient
a Nack message.

The values of the payload are formatted in the following order:
```C++
Key key
struct timeval timeout
```

If the timeout value is 0, that means that the client will wait indefinitely.

###### Status

This message can be sent directly between nodes, from registrar to node, or
node to registrar. It is a simple ASCII message in order to notify other parts
of the eau2 network about the status of the sender.

The values of the payload are formatted in the following order:
```C++
String message
```

A string value is formatted in the following C-struct binary format:
```C++
size_t length
char string[length + 1]
```

###### Register

This message can only be sent from a node to a registrar when a node is first
connecting to the registrar. It contains the address that other nodes can
connect to in order to directly communicate with the node.

The values of the payload are formatted in the following order:
```C++
struct sockaddr_in client_address
size_t port_num
```

###### Directory

This message can only be sent from a registrar to a node. It is a broadcast
message sent when a new node has registered itself to the registrar, and it
contains all the addresses of the registered nodes associated with a node id.

The values of the payload are formatted in the following order:
```C++
size_t max_num_clients
size_t port_num[max_num_clients]
String ip_addresses[max_num_clients]
```

### Application

All applications are run on a thread that has access to a KeyValueStore, 
which allows the application to store and retrieve dataframes. Its functionality
is implemented by overriding the `main()` function.

A distributed application that makes use of more than 1 node requires an
interface in order to communicate over the network through their distributed 
KeyValueStore. In order to facilitate that, 3 functions are provided to
configure the application to communicate:
- `LocalNetworkMessageManager *connect_local(size_t node_id)`: Prepares the 
application to communicate through an interface that fakes a "network" across 
different threads within the same process. This function will be paired with
`register_local()`. The return value is a communicate interface that will allow
another application to communicate with this application.
- `void register_local(LocalNetworkMessageManager *msg_manager`: This function
must be called after `connect_local()`. This passes the provided interface into
an application to allow that application to communicate with the application
that has been configured in.
- `connect_network()`: This is a function is TODO and projected to be 
implemented at a different moment in time.

## Use cases

### Dataframe

The following code demonstrates the basics of creating a Dataframe containing
two columns of 1,000,000 rows of integers:
```C++
Schema s("II");

DataFrame df(s);
Row  r(df.get_schema());
for(size_t i = 0; i <  1000 * 1000; i++) {
  r.set(0,(int)i);
  r.set(1,(int)i+1);
  df.add_row(r);
}
```

In addition, a DataFrame can be read from a `*.sor` file using the
SorerIntegrator class, which is the API interface that integrates the code
provided by the 4500ne team's Sorer implementation with the rest of the code
base. 

An example of a sor file being read into a Dataframe is as follows:
```C++
SorerIntegrator integrator("../data/spacey.sor");
integrator.parse();
DataFrame *df = integrator.convert();
```

### Application

Applications are essentially threads that run concurrently to the network
layer of the eau2 system. An application will have two types: one that is
run on the server, and one that is run on teh client.

The server application is the main application, and can deploy KeyValue stores
to the nodes that are registered and connected to it. It is the main
application that is interfacing with the user directly.

The client application is a subsection of the main application, and contains
only the key value store that the main application had deployed to it.
The client application runs only a portion of the main goal of the main
application.

To run an application on a node/server, it should be calling the API functions
similar to starting, running and ending a thread:
```C++
#include "application.h"

int main(int argc, char **argv) {
  Application demo;

  demo.start(); // Start the application
  demo.join(); // Wait for the application to finish

  return 0;
}
```

## Open questions

## Comments

- "this->" within the codebase will remain as a styling choice, because we have
gotten bitten during the debugging phase when we did not use the keyword to
explicitly reference class fields.

## Status

### Completed Tasks

- Finished transferring the code to this new project repository
- Paid technical debt: Creating a serializer helper class to assist
serializing messages and data
- Paid technical debt: Simplified the dataframe implementation, stripping out
unnecessary functionality, such as row and column names.
- Paid technical debt: Improved the performance of the dataframe's data
structure in regard to a growing buffer of large amounts of data
- Integrated 4500ne's sorer into the project
- Created hooks to receive put, get, reply, and wait and get message types
- Fixed memory leaks found within the test suites
- Implemented from_array API functions within the DataFrame class
- Created a trivial application that run standalone.
- Simplify the target source code lists for the CMakeLists.txt
- Connect multiple local KV-Stores to support communicating to each other
over threads
- Assigning and aligning of the node ids in order to maintain ids throughout
the direct communication between nodes

### Technical Debt and TODOs

- Make polling timeouts configurable within the server and clients
- Create unit tests for the dataframe's array of arrays
- Get the network code to run within Docker
- Make use of templates for similar classes and methods
- Connect multiple local KV-Stores to support communicating to each other
over the network
- Update the schema field in the dataframe to not be a pointer
- Minimize copying strings when going when adding them to dataframes
 