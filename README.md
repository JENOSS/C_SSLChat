@ssl/multi thread/mutex ���@

(pem �� ��й�ȣ : 1539)

1. �͹̳� �� �װ� ����

1. �͹̳� �ϳ����� ./server1 

2. ������ �͹̳ο��� ./client1
 - user name ����

----------------------------------------------

������ : 1.  (sudo) gcc -g -Wall -pthread -c client1.c -o client1.o  (������ client1 �κ��� server1���� ����)
            2.  (sudo) gcc -g -Wall -pthread -o client1 client1.o common.o reentrant.o -lssl -lcrypto (������ //)

----------------------------------------------


