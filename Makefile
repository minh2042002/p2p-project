run : server client1 client2 client3
client1 : server
	gcc -pthread -o client1/client TCP_Client/client.c TCP_Client/clientHandler.c TCP_Client/clientHandler.h TCP_Client/helper.h TCP_Client/helper.c common/socketp2p.h common/socketp2p.c 
client2 : server
	gcc -pthread -o client2/client TCP_Client/client.c TCP_Client/clientHandler.c TCP_Client/clientHandler.h TCP_Client/helper.h TCP_Client/helper.c common/socketp2p.h common/socketp2p.c 
client3 : server
	gcc -pthread -o client3/client TCP_Client/client.c TCP_Client/clientHandler.c TCP_Client/clientHandler.h TCP_Client/helper.h TCP_Client/helper.c common/socketp2p.h common/socketp2p.c 
server : 
	gcc -pthread -o TCP_Server/server TCP_Server/server.c TCP_Server/serverHandler.h TCP_Server/serverHandler.c common/socketp2p.h common/socketp2p.c TCP_Server/Client.h TCP_Server/Client.c  TCP_Server/serverUtils.h TCP_Server/serverUtils.c
clean : 
	rm -f TCP_Server/server
	rm -f client1/client
	rm -f client2/client
	rm -f client3/client
