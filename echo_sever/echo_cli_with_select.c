#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc != 2)
		err_quit("usage: echo_cli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	str_cli_1(stdin, sockfd);

	exit(0);
}

void str_cli_with_select(FILE *fp, int sockfd)
{
	int  maxfdp1;
	fd_set  rset;
	char  sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);
	for ( ; ; ) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {  // socket is readable
			if (Readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
			Fputs(recvline, stdout);
		}
				
		if (FD_ISSET(fileno(fp), &rset)) {
			if (Fgets(sendline, MAXLINE, fp) == NULL)
				return;
			Writen(sockfd, sendline, strlen(sendline));
		}
	}
}

void str_cli_1(FILE *fp, int sockfd)
{
	char   sendline[MAXLINE],  recvline[MAXLINE];
	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, strlen(sendline));
		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli_1: server teminated permaturely");
		Fputs(recvline, stdout);
	}

}
