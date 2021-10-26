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
