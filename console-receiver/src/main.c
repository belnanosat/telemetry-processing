#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <strings.h>
#include <pb.h>
#include <pb_decode.h>
#include <proto/telemetry.pb.h>

#include "stuffer.h"

uint8_t buffer[256];
uint8_t data[512];
uint8_t data2[512];

void check_packet(int tlen) {
	int len = unstuff_data(data, tlen, data2);
	TelemetryPacket packet;

	pb_istream_t stream = pb_istream_from_buffer(data2, len);
	if (pb_decode(&stream, TelemetryPacket_fields, &packet)) {
		printf("Works!!!\r\n");
	} else {
		printf("Failed to decode packet !\r\n");
	}
}

int main() {
	int data_id = 0;
	int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sockaddr;
	struct hostent *server;
	int n;
	if (sd != -1) {
		printf("Succesfully opened a socket!\r\n");
	}
	server = gethostbyname("localhost");
	if (server == NULL) {
		printf("Null server!\r\n");
		return 1;
	}
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(7322);
	bcopy((char*)server->h_addr,
	      (char*)&sockaddr.sin_addr.s_addr,
	      server->h_length);
	if (connect(sd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		printf("Failed to bind a socket! %d\r\n", errno);
		return 1;
	}

	while (1) {
		n = read(sd, buffer, 255);
		for (int i = 0; i < n; ++i) {
			data[data_id++] = buffer[i];
			printf("%x ", (unsigned int)buffer[i]);
			if (buffer[i] == 0) {
				check_packet(data_id);
				printf("\n");
			}
		}
		fflush(stdout);
	}
	return 0;
}
