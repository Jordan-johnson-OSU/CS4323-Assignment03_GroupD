#Server Terminal
gcc *.c -o server -lrt -Wall -l pthread

./server 9090

#Client Terminal
gcc *.c -o client -Wall -l pthread

./client 9090


############################
#Helpful Debugging Commands#
############################

top

ipcs -a

cat /proc/sys/kernel/sem