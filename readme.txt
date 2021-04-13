#Server Terminal
gcc *.c -o server -Wall -l pthread

./server

#Client Terminal
gcc *.c -o client -Wall -lsocket -lnsl

./client


############################
#Helpful Debugging Commands#
############################

top

ipcs -a

cat /proc/sys/kernel/sem