![Telegraph Office](../images/Telegraph-Office.png)

# Challenge #1

The goal of this challenge is to perform your **first Portals communication**: transmit an "Hello World" message from an initiator process to a target process.

Message transmission and reception requires some preparation. Here are the preparation steps for both sides of the communication.

*On the initiator side*, which is the communication endpoint that initiates the network operation, you have to:
* create a Portals communication endpoint - `ezpInit()`
* allocate an Event Queue - `ezpEQAlloc()`
* prepare for transmission the memory buffer that contains the message - `ezpMDBind()`

*On the target side*, which is the communication endpoint that undergoes the network operation, you have to:
* create a Portals communication endpoint - `ezpInit()`
* allocate an Event Queue - `ezpEQAlloc()`
* allocate a memory buffer large enough to receive the message in it
* prepare this memory buffer for reception - `ezpMEAppend()`
* wait for the NIC notification that the buffer is ready for reception - `ezEQWait()` returning an LINK event

After the preparation phase, it is important to synchronize the two communication endpoints to be sure resources for reception are in place before message is emitted. You can use `barrier()` on both sides for this.

Then, the network communication operation can be performed.

On the initiator side
* launch the transmission - `ezpPut()`
* wait for the NIC notification that the transmission has completed - `ezpEQWait()`

On the target side
* wait for the NIC notification that a reception has occured in the buffer - `ezpEQWait()`
* display the message received in the buffer

<font color="blue">What are the types of the events returned by the NIC after the data transfer ?</font>

<font color="blue">What happens when the reception buffer is too small ?</font>

## Going further

Portals communications are **reliable**. It means, the NIC (Network Interface Controller) handles message retransmission in case of error in the network. If the network operation cannot be achieved due to a persistent hardware issue, the initiator of the operation is notified of the failure thanks to the event status.

Try to transmit your "Hello World" message to an uninitialized communication endpoint.
<font color="blue">What happens ?</font>


Portals communications can be controlled directly from the user-space process without any system calls. This feature is named OS-bypass and it allows to achieve very low latency communications.

Using the `clock_gettime()` function with CLOCK_MONOTONIC clock, evaluate the mean time for a Portals transmission. Time will be measured on the initiator from the launch of the Put operation to the notification of completion with the event arrival.
<font color="blue">What latency have you measured ?</font>

