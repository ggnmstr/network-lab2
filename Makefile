client:
	g++ ./Client/client.cpp ./Client/main.cpp -o client.out -lboost_filesystem 
server:
	g++ ./Server/server.cpp ./Server/session.cpp ./Server/main.cpp  -o server.out -lboost_filesystem 
app:
	make -j8 client server
diff:
	diff -u <(sha1sum upload/linux-6.5.5.tar.xz ) <(sha1sum ~/Downloads/linux-6.5.5.tar.xz ) | diff-so-fancy
