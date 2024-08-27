all:client tcpepoll

client:client.cpp InetAddress.cpp
	g++ -g -o client client.cpp InetAddress.cpp

tcpepoll:tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp

clean:
	rm -f client tcpepoll