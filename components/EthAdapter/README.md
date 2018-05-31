# EthAdapter

This component connects two `from` ends of the Ethdriver interface and connection type together.
This is to support a setup where there are two servers and one client. This component is the client, and the Ethdriver-like
components it is connected to are the servers. The client has two connections, one to each server.
The purpose of this client is to forward messages from one server to the other. Messages can be sent in both directions.

## Interface and connector

This component can be connected to other components only via the Ethdriver interface over the seL4Ethdriver connection type.
Because this component has two _client-end_ connections, each connection can only be connected to one other component.

```c
procedure Ethdriver {
    int tx(in int len);
    int rx(out int len);
    void mac(out uint8_t b1, out uint8_t b2, out uint8_t b3, out uint8_t b4, out uint8_t b5, out uint8_t b6);
};

```
- `tx`: Transmits a packet of length `len`.  Returns 0 on succes, other value on error.
- `rx`: Receives a packet and stores length in `len`
  - returns 1 on successful receive and indicates there are more packets in receive queue
  - returns 0 on successful receive but there are no more packets to receive
  - returns -1 if no packet was received.
- `mac`: This is ignored

## Implicit objects


```
component EthAdapter {
    has mutex amutex;

    uses Ethdriver a;
    uses Ethdriver b;

    consumes HasData a_has_data;
    consumes HasData b_has_data;

    // These are implicitly created by the Ethdriver connector
    // dataport Buf a_buf;
    // dataport Buf b_buf;

    // Ignore these, they are required to support the global endpoint template
    // They serve as `from` ends for the `HasData` events above.
    emits HasData dummy_a;
    emits HasData dummy_b;

```

The seL4Ethdriver connector creates both explicit and implicit connections communication between the client and server.
- A RPC connection which is used to implement the above Ethdriver RPC procedure.
- A 4K shared memory buffer is used to store the packet that is being received or sent.
- A notification object that the server uses to indicate that a packet is available. The client then calls rx to
  receive the packet

The only explicit connection below between the two servers in the client implies an RPC connection.
The shared memory dataport and notification object are connected implictly.  There are two dummy connections that are
required to create a callback thread that runs in the EthAdapter component whenever HasData events are received.
```c
connection seL4Ethdriver adapter_con_a(from ethadapter.a, to firewall.client);
connection seL4Ethdriver adapter_con_b(from ethadapter.b, to ethdriver.client);


// Two dummy connections are required to create a thread for each receive event
connection seL4GlobalAsynchCallback adapter_global_callback_a(from ethadapter.dummy_a, to ethadapter.a_has_data);
connection seL4GlobalAsynchCallback adapter_global_callback_b(from ethadapter.dummy_b, to ethadapter.b_has_data);

```

## Configuration

In addition configuration settings have to be provided so that the implicit connections can be connected correctly.
```c
    // Each client connection needs to be given a unique ID for the server that it is being connected to.
    ethadapter.a_attributes = "1"; // This is the client id for the a component interface
    ethadapter.b_attributes = "2"; // This is the client id for the b component interface

    // The mac address is by each server to identify the client.
    // If a and b are both ethernet cards, then a likely configuration is:
    //   a_mac = the mac address of server b
    //   b_mac = the mac address of server a
    // This will mean that a packet sent from a to b via this component will appear to b as if it has a's mac address
    ethadapter.a_mac = [6, 0, 0, 11, 12, 13];
    ethadapter.b_mac = [6, 0, 0, 11, 12, 15];


    // These connect the HasData events from the two clients to each of the callback threads
    ethadapter.a_global_endpoint = "etha";
    ethadapter.a_has_data_global_endpoint = "etha";
    ethadapter.b_global_endpoint = "ethb";
    ethadapter.b_has_data_global_endpoint = "ethb";

```

## Behavior

Two threads are created which each wait on one of the `*_has_data` callbacks. When an event is received from either of
the connections, a callback is called. A mutex is used to synchronise between the two threads to prevent the buffers
getting overwritten by each thread.

The component only responds to`*_has_data` events. When a packet is received by the zv component connected to `a`:
1. That component decides that it has a packet available for the EthAdapter and sends an event.
2. The thread for that event will resume and call `a_has_data_callback()`
3. `a_rx` is called to receive the packet. The packet will be copied into the shared mem `a_buf` and the length
   returned in `len`
4. Assuming that result is either 0 or 1, the contents of a_buf is copied to b_buf
5. The packet is transmitted to the other Ethdriver-like component connected to `b` through `b_tx`
6. The process is repeated until -1 is returned by `a_rx` which corresponds to no more packets available.


This is a simplified version of the implemention in the Component.
```c

void* a_buf; // Shared memory between self and component a
void* b_buf; // Shared memory between self and component b

void a_has_data_callback() {
    // We received an event from component a. This means it has a packet for us
    lock(); // Acquire lock
    int result = a_rx(&len); // call rx() on a. THis instructs component a to copy the packet into the shared mem between us
    while (result != -1) { // -1 means no packed received.  Otherwise we have a packed in a_buf to send on.
        memcpy(b_buf, a_buf, len); // Copy from a_buf to b_buf. The lock we hold means that b_buf is safe to copy to
        int tx_result = b_tx(len); // call tx() on component b.  This instructs it to copy packet from shared mem and send it.
        if (tx_result) {  // This action should succeed unless the tx queue is full. We don't expect high throughput
                          // so we print an error and keep going.
            error("Could not tx packet");
            result = -1;
        }
        result = a_rx(&len); // Try and receive another packet from a and go back to the top of the loop
    }
    unlock();  // a has no more packets to give us, so we release the lock.
}

// Same as above, only we are transferring packets from component b to component a.
void b_has_data_callback() {
    lock();
    int result = b_rx(&len);
    while (result != -1) {
        memcpy(a_buf, b_buf, len);
        int tx_result = a_tx(len);
        if (tx_result) {
            error("Could not tx packet");
            result = -1;
        }
        result = b_rx(&len);
    }
    unlock();
}

```
