# ProducerConsumer
Written in C for Linux OS using shared memory and semaphores. 
Producer places items on a table. Consumer picks them up. The 
table can only hold two items at a time; If there are no items
on the table, the consumer must wait to consume. If there are 
two items on the table, the producer must wait to produce. 

These programs are meant to be run in two terminal windows
to allow for interactivity. Either program may be started first,
but the consumer program will wait for the producer program 
to set up shared memory and be ready. 

The two programs are compiled as follows:
$ gcc producer.c -pthread -lrt -o producer
$ gcc consumer.c -pthread -lrt -o consumer

In one terminal, run producer with ./producer
In another terminal, run consumer with ./comsumer

The producer program will ask for input in the form of a string.
The producer will write the strings to shared memory. If there are 
two strings produced that have not been consumed, entering a third
item will cause the producer to wait for a consumption. Once one of 
the items is consumed, the new item will be put on the table. 

The consumer program waits for the instruction to consume, given
by pressing enter. If there are no items on the table, the consumer
will wait for one to be produced. Once one is, it will consume it, 
by printing out the item and removing it from memory. the consumer
will consume one item at a time. 

Examples:
Here we have the consumer consuming the items as the producer produces them. 
![pctrade](/images/pctrade.png "pctrade")
