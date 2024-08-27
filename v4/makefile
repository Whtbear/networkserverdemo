all:client tcpepoll

client:client.cpp InetAddress.cpp
	g++ -g -o client client.cpp InetAddress.cpp

tcpepoll:tcpepoll.cpp InetAddress.cpp Socket.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp Socket.cpp

clean:
	rm -f client tcpepoll