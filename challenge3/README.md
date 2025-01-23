![Cabling clean](../images/Cabling-clean.jpg)
![Cabling disaster](../images/Cabling-disaster.jpg)

# Challenge #3

HPC applications use various communication patterns. The simplest pattern is *point-to-point* communication where a data transfer is performed between two endpoints. More advanced communication patterns involve a group of endpoints that exchanges data between group members. For instance, the *gather* operation allows one endpoint to receive data from every endpoints, the *scatter* operation allows to send data from one endpoint to all endpoints, or the *alltoall* operation allows to send data from all endpoints to all endpoints.

When dealing with complex communication patterns or large volume of data transfers, a key performance feature of the NICs is to be able to autonomously progress the communications without any intervention of the host CPU. This allows the application to progress on computing in parallel of data transfers. We call this feature *compute and communication overlap*.

The Portals API offers an interesting feature named *Triggered Operations* that allows to register communications that are automatically triggered by the NIC when network events occur. This mechanism is very helpful to build complex communication patterns with a good compute and communication overlap.

In this challenge, you will program **broadcast network communications using the Portals triggered operations**. During a broadcast, one task (the root) sends the same data to all other tasks in the group. The data is initially located on the root endpoint. It needs to be efficiently broadcasted to every other endpoints. The broadcast operation is completed when every task has received the broadcasted data.

![Broadcast pattern](../images/broadcast_pattern.png)

<font color="purple">Implement several broadcast algorithms and compare them with each other.</font>

<font color="purple">Which algorithm is the most performant ?</font>

<font color="purple">Which algorithm has the best compute and communication overlap ?</font>

