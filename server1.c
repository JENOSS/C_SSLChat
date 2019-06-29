#include "common.h"
#include <stdlib.h>

#define CERTFILE "server.pem"
#define MAX_CLNT 100

void send_msg(char *buf, int len, int current_user);

int ssl_cnt =0;
SSL* ssl_accept[MAX_CLNT];
pthread_mutex_t mutx;
SSL_CTX *setup_server_ctx(void)
{
	SSL_CTX *ctx;

	ctx = SSL_CTX_new(SSLv23_method( ));
	if (SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1)
		int_error("Error loading certificate from file");
	if (SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1)
		int_error("Error loading private key from file");
	return ctx;
}

int do_server_loop(SSL *ssl,int current_user)
{
	int err;
	char buf[80];
	
	do
	{
		err = SSL_read(ssl,buf,sizeof(buf));
		send_msg(buf, err,current_user);
		//fprintf(stdout,"%s",buf);
		
	}
	while (err > 0);
	return (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) ? 1 : 0;

		
}

void send_msg (char *buf, int len, int current_user)
{

	int i;
	pthread_mutex_lock(&mutx);
	for(i=0;i<ssl_cnt;i++)
		if(i != (current_user-1))
			SSL_write(ssl_accept[i],buf,len);
	pthread_mutex_unlock(&mutx);


}


void THREAD_CC server_thread(void *arg)
{
	SSL *ssl = (SSL *)arg;

	int current_user = ssl_cnt;
#ifndef WIN32
	pthread_detach(pthread_self( ));
#endif
	if (SSL_accept(ssl) <= 0)
		int_error("Error accepting SSL connection");
	fprintf(stderr, "SSL Connection opened\n");
	printf("welcome user %d \n",current_user);
	if (do_server_loop(ssl,current_user))
		SSL_shutdown(ssl);
	else
		SSL_clear(ssl);
	fprintf(stderr, "SSL Connection closed\n");
	SSL_free(ssl);

	ERR_remove_state(0);

#ifdef WIN32
	_endthread( );
#endif
}

int main(int argc, char *argv[])
{
	BIO	*acc, *client;
	SSL	*ssl;
	SSL_CTX *ctx;
	THREAD_TYPE tid;

	system("clear");

	pthread_mutex_init(&mutx, NULL);

	//pthread_mutex_init 
	init_OpenSSL( );
	seed_prng( );



	ctx = setup_server_ctx( );

	acc = BIO_new_accept(PORT);
	if (!acc)
		int_error("Error creating server socket");
	if (BIO_do_accept(acc) <= 0)
		int_error("Error binding server socket");

	for (;;)
	{
		if (BIO_do_accept(acc) <= 0)
			int_error("Error accepting connection");

		client = BIO_pop(acc);
		if (!(ssl = SSL_new(ctx)))
			int_error("Error creating SSL context");

		SSL_set_bio(ssl, client, client);
		
		pthread_mutex_lock(&mutx);
		ssl_accept[ssl_cnt++]=ssl;
		pthread_mutex_unlock(&mutx);

		
		//phterad_create(%tid,NULL,handler,arg)
		THREAD_CREATE(tid, (void *)server_thread, ssl);

	}

	SSL_CTX_free(ctx);
	BIO_free(acc);
	return 0;
}
