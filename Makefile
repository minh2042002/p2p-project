run: client server
client:
	gcc -pthread -o TCP_Client/client TCP_Client/client.c TCP_Client/clientHandler.c TCP_Client/clientHandler.h TCP_Client/helper.h TCP_Client/helper.c common/socketp2p.h common/socketp2p.c
server:
	gcc -pthread -o TCP_Server/server TCP_Server/server.c TCP_Server/serverHandler.h TCP_Server/serverHandler.c common/socketp2p.h common/socketp2p.c
clean:
	rm -f TCP_Client/client
	rm -f TCP_Server/server