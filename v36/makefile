all:client server

client:client.cpp InetAddress.cpp
	g++ -g -o client client.cpp InetAddress.cpp

server:server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp ThreadPool.cpp Timestamp.cpp
	g++ -g -o server server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp \
	Buffer.cpp	EchoServer.cpp ThreadPool.cpp Timestamp.cpp -lpthread

clean:
	rm -f client server