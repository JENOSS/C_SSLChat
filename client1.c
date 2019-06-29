#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#define CERTFILE "client.pem"

char name[30]="[]";

SSL_CTX *setup_client_ctx(void)
{
	SSL_CTX *ctx;

	ctx = SSL_CTX_new(SSLv23_method( ));
	if (SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1)
		int_error("Error loading certificate from file");
	if (SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1)
		int_error("Error loading private key from file");
	return ctx;

}
void* send_msg(void* arg)
{
	SSL *ssl=(SSL*)arg;

	int err;
	char buf[80];

	char name_buf[110];

	for(;;)
	{
		if (!fgets(buf, sizeof(buf), stdin))
			break;
		sprintf(name_buf, "(%s) >> %s", name, buf);
		err = SSL_write(ssl,name_buf,sizeof(name_buf));
	
		if(err <=0)
			return 0;
	}
	return 1;
}
void* recv_msg(void* arg)
{
	SSL *ssl=(SSL*)arg;
	char buf[110];
	int err;
	
	do
	{

		err = SSL_read(ssl,buf,sizeof(buf));
		if(err <=0)
			break;
		fprintf(stdout, "%s", buf);
	}
	while(err> 0);
	
	return 1;
}

int do_client_loop(SSL *ssl)
{

	pthread_t snd_thread, rcv_thread;
	void* thread_return_send;
	void* thread_return_recv;

	pthread_create(&snd_thread, NULL, send_msg, (void*)ssl);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)ssl);
	pthread_join(snd_thread, &thread_return_send);
	pthread_join(rcv_thread, &thread_return_recv);

	return (int)thread_return_send & (int)thread_return_recv;
}

int main(int argc, char *argv[])
{
	BIO	*conn;
	SSL	*ssl;
	SSL_CTX *ctx;


	printf("SET YOUR NAME : ");
	scanf("%s",name);


	init_OpenSSL( );
	seed_prng( );

	ctx = setup_client_ctx( );

	conn = BIO_new_connect(SERVER ":" PORT);
	if (!conn)
		int_error("Error creating connection BIO");

	if (BIO_do_connect(conn) <= 0)
		int_error("Error connecting SSL object");
	
	if (!(ssl = SSL_new(ctx)))
		int_error("Error creating an SSL context");
	SSL_set_bio(ssl, conn, conn);
	if (SSL_connect(ssl) <= 0)
		int_error("Error connecting SSL object");

	system("clear");
	fprintf(stderr, "SSL Connection opened\n");
	printf("welome %s \n",name);
	printf("-------------------\n");
	
	//do_client_loop 가 0이 되면
	if (do_client_loop(ssl))
		SSL_shutdown(ssl);
	else
		SSL_clear(ssl);
	fprintf(stderr, "SSL Connection closed\n");

	SSL_free(ssl);
	SSL_CTX_free(ctx);
	return 0;
}
