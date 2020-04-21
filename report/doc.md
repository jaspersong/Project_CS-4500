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

### Tests

The following are terminal commands to run within the root of the project folder
in order to build and run the individual tests:
- `make build_all`: Builds the whole project.
- `make test_all`: Runs all of the test suites of the project
- `make test_valgrind`: Runs all the test suites of the project with valgrind
 turned on
- `make clean`: Cleans the builds
- `make docker_build_all`: Builds the whole project within a Docker image. This 
command only will run if Docker is online.
- `make docker_test_all`: Runs all of the test suites of the project within
a Docker image. This command only will run if the Docker is online.
- `make docker_test_valgrind`: Runs all of the test suites of the project
within a Docker image with valgrind turned on. This command will only run if
the Docker is online

### Demos

All applications that make use of socket programming have individual demo
builds and runs. At the moment, none of the projects run on the Docker image.

All demos can be built with the `make build_all` command mentioned in the
Test section. The executables will then be in the `build` folder automatically
generated within the root of the project folder. 

#### Demo Basic Distributed Application

The basic distributed application demo contains two executables: 
`demo_registrar` and `demo_node`. The demo runs with hard-coded IP address
and port numbers over the same local machine.

Steps:
1. In one terminal window, run `./demo_registrar`.
1. In a separate terminal window, but on the same local machine, run 
`./demo_node`.

#### Demo Application

The demo application contains two executables: `demo_app_registrar` and 
`demo_app_node`. 

Steps:
1. In one terminal window, run `./demo_app_registrar <IP> <Port>`
where the `<IP>` is the IP address that the registrar will listen for
incoming connections from, and `<Port>` is the port number that the
registrar will listen for incoming connections from.
1. In two other terminal windows, run 
`./demo_app_node <RegistrarIP> <RegistrarPort> <IP> <Port>`, where
`<RegistrarIP>` and `<RegistrarPort>` is the IP address and the port number of
the registrar, and `<IP>` and `<Port>` is the IP address and the port number
that the node will listen for incoming direct connections from other nodes.

#### Word Count Application

The demo application contains two executables: `demo_wordcount_registrar` and 
`demo_wordcount_node`. It takes in a similar set of arguments as the demo
application mentioned above, except with one additional argument for the
registrar.

Steps:
1. In one terminal window, run 
`./demo_wordcount_registrar <IP> <Port> <WordCountFile>`
where the `<WordCountFile>` is the file that the applicatino will read and count
words from.
1. In two other terminal windows, run 
`./demo_wordcount_node <RegistrarIP> <RegistrarPort> <IP> <Port>`.

#### Linus Application

The demo application contains two executables: `demo_linus_registrar` and 
`demo_linus_node`. It takes in the same set of arguments as the demo application
mentioned above.

Steps:
1. In one terminal window, run `./demo_linus_registrar <IP> <Port>`.
1. In two other terminal windows, run 
`./demo_linus_node <RegistrarIP> <RegistrarPort> <IP> <Port>`.

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
String name. It also contains a node id to determine where the key is stored. 
If its home node id is -1, then it is referring to a DistributedValue, which 
references multiple chunks of a dataframe distributed throughout the entire
eau2 system. If the home node id is 0 and greater, then it is referring to a
a dataframe, which can be the entire DataFrame the DistributedValue is
referencing, or be a chunk of it. 

For nodes that do not contain the value of a particular Key can send a
WaitAndGet message to the node id associated to that Key.

## Implementation

### Dataframe

A Dataframe is a collection of columns containing rows of data of data types 
boolean, integer, float, and string. Each dataframe has a schema, which contains
an ordered list of data types for each column from index 0 to any positive 
index. The dataframe and all of its rows and columns are expected to follow the
schema. Then each column is a list of data of the specified data type.

The Dataframe contains interfaces of Rower and Writer. A Rower is an interface 
that can be used to iterate each row of a dataframe using the `map` functions.
A Writer is used to generate a completely new dataframe row by row.

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
and the other is Node. They are both an implementation of an abstract class
called the SocketNetwork. A registrar has all the same functionality as a Node,
except it does not broadcast a Register message, and it is responsible to 
manage the Directory of Nodes, which includes the IP address and port number
of the Registrar itself.

The SocketNetwork supports general functionality for any server/client network.
Each instance is able to support incoming new connections by listening to a
socket established by a fixed IP address and port number. And each instance
can initiate a new connection to another instance of the SocketNetwork that is
at a known, fixed IP address and port number.

The SocketNetwork includes a variety of virtual callback functions that will
handle events that occur during normal network uptime:
- `void handle_initialization()`: A callback function called moments after
the listener socket is established to listen for incoming connections, but
before any formal connections are made.
- `void handle_closing_connection()`: A callback function called when an
established connection gets closed.
- `void handle_incoming_message(size_t connection_id, Message *msg)`: A callback
function called to handle messages that have come from the specified connection
id. The callback function is expected to take complete ownership of the passed
in dynamically allocated message object.

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
```

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
struct sockaddr_in address
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
- `void register_local(LocalNetworkMessageManager *msg_manager)`: This function
must be called after `connect_local()`. This passes the provided interface into
an application to allow that application to communicate with the application
that has been configured in.
- `SocketNetworkMessageManager *connect_network()`: This function prepares the 
application to communicate an interface that hooks up to an instance of either
a Registrar or a Node.

## Use cases

### Key

A Key can be referred by a String name, and it is associated with a
DistributedValue within the application Key-Value Store.

Keys also has home node ids, but they are not intended to be referenced
within the application layer, and should only be handled within the
implementation of the key-value store.

```C++
Key key0("Hello");
Key key1("world");
Key key2("Hello");

assert(key0.equals(&key2));
assert(!key0.equals(&key1));
assert(key0.get_name()->equals(key2.get_name()));
assert(!key0.get_name()->equals(key1.get_name()));
```

### DistributedValue

The Key-Value Store has multiple static functions that can create a
DistributedValue which will automatically become available to the rest of the
eau2 system. The following are examples of creating distributed values: 

```C++
Key key0("0");
Key key1("1");
Key key2("2");
Key key3("3");
Key key4("4");
KeyValueStore kv(1);

// Creates a distributed value using an array of values
size_t SZ = 10;
bool bool_vals[SZ];
int int_vals[SZ];
float float_vals[SZ];
String *str_vals[SZ];

KeyValueStore::from_array(key0, &kv, SZ, bool_vals);
KeyValueStore::from_array(key1, &kv, SZ, int_vals);
KeyValueStore::from_array(key2, &kv, SZ, float_vals);
KeyValueStore::from_array(key3, &kv, SZ, str_vals);

// Creates a distributed value using a scalar value
KeyValueStore::from_scalar(key0, &map, true);
KeyValueStore::from_scalar(key1, &map, 5);
KeyValueStore::from_scalar(key2, &map, 6.3f);
KeyValueStore::from_scalar(key3, &map, &str0);

// Creates a distributed value using a visitor
Writer writer;
KeyValueStore::from_visitor(key0, &kv, "I", writer);

// Creates a distributed value using a sor file
KeyValueStore::from_file(key0, &kv, "../data/testfile.sor");
```

Once the distributed dataframe has been created, it is meant to be read-only.

```C++
Key key0("test");
KeyValueStore kv(1);
String hello("hello");

KeyValueStore::from_file(key0, &kv, "../data/testfile.sor");
DistributedValue *value = kv->wait_and_get(key0);

assert(value->get_int(0, 0) == 1);
assert(!value->get_bool(3, 0));
assert(value->get_float(1, 2) == 6.6f);
assert(value->get_string(2, 0)->equals(&hello));

assert(value->nrows() == 3);
assert(value->ncols() == 4);
```

A value can be iterated over row-by-row by using `void local_map(Rower &r)` and 
`void map(Rower &r)`. While the `map` one iterates through all of the rows
available through the whole eau2 system, `local_map` only iterates through the
rows that are stored within the node that the caller is running from. 

### Key-Value Store

A map that stores a DistributedValue associated with unique Keys. Once a
value has been add to the store, it will be available throughout the system. 
It has functions that allow an application to store key-value pairs. The
functions available to store values with a key can be found in the
DistributedValue section. The value can then be retrieved by calling
`wait_and_get` with the key of the desired value. 

```C++
assert(kv->get_home_id() == 0);
assert(kv->get_num_nodes() == 2);

KeyValueStore::from_file(key0, &kv, "../data/testfile.sor");
DistributedValue *value = kv->wait_and_get(key0);
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

## Comments

- "this->" within the codebase will remain as a styling choice, because we have
gotten bitten during the debugging phase when we did not use the keyword to
explicitly reference class fields.

## Status

### Technical Debt and TODOs

- Make polling timeouts configurable within the server and clients
- Get the network code to run within Docker
- Minimize copying strings when going when adding them to dataframes
- Have the Linus application work on the socket network