# C_SSLChat 

C로 구현한 SSL Chat Program

# About
- OpensSSL을 사용한 SSL 소켓 프로그래밍
- Server는 multi Thread로 구현, Mutex로 상호배제
- Client는 server에 접속하여 Client간 Chatting 수행

# Youtube
[![Video Label](http://img.youtube.com/vi/jtW0UPuzkzs/0.jpg)](https://www.youtube.com/watch?v=jtW0UPuzkzs)

# Run

@ssl/multi thread/mutex 사용@

(pem 의 비밀번호 : 1539)

1. 터미널 총 네개 오픈

1. 터미널 하나에서 ./server1 

2. 나머지 터미널에서 ./client1
 - user name 세팅

# Compile

컴파일 : 1.  (sudo) gcc -g -Wall -pthread -c client1.c -o client1.o  (서버는 client1 부분을 server1으로 변경)
            2.  (sudo) gcc -g -Wall -pthread -o client1 client1.o common.o reentrant.o -lssl -lcrypto (서버는 //)
