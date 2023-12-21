run: client server
client:
	gcc -o TCP_Client/client TCP_Client/client.c TCP_Client/clientHandler.c TCP_Client/clientHandler.h
server:
	gcc -o TCP_Server/server TCP_Server/server.c TCP_Server/serverHandler.h TCP_Server/serverHandler.c
clean:
	rm -f TCP_Client/client
	rm -f TCP_Server/server