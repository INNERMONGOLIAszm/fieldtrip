/*
 * Copyright (C) 2008, Robert Oostenveld
 * F.C. Donders Centre for Cognitive Neuroimaging, Radboud University Nijmegen,
 * Kapittelweg 29, 6525 EN Nijmegen, The Netherlands
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "buffer.h"

int main(int argc, char *argv[]) {
	int server, offset;
	int n;
	messagedef_t request, response;
	event_t event;
	eventsel_t eventsel;
	void *buf = NULL;

	if (argc != 3) {
		fprintf(stderr, "USAGE: application <server_ip> <port>\n");
		exit(1);
	}

	/* open the TCP socket */
	if ((server = open_connection(argv[1], atoi(argv[2]))) < 0) {
		fprintf(stderr, "ERROR; failed to create socket\n");
		exit(1);
	}

	request.version = VERSION;
	request.command = GET_EVT;
	request.bufsize = 0; // sizeof(eventsel_t);

	// eventsel.begevent = 0;
	// eventsel.endevent = 2;

	fprintf(stderr, "------------------------------\n");
	print_request(&request);
	write(server, &request, sizeof(messagedef_t));
	// write(server, &eventsel, sizeof(eventsel_t));

	read(server, &response, sizeof(messagedef_t));
	fprintf(stderr, "------------------------------\n");
	print_response(&response);
	fprintf(stderr, "------------------------------\n");

	if (response.command==GET_OK) {
		buf = malloc(response.bufsize);
		if ((n = bufread(server, buf, response.bufsize)) < response.bufsize) {
			fprintf(stderr, "problem reading enough bytes (%d)\n", n);
		}
		else {
			n = 0;
			offset = 0;
			while (offset<response.bufsize) {
				event.def = buf+offset;
				event.buf = buf+offset+sizeof(eventdef_t);
				fprintf(stderr, "\n");
				print_eventdef(event.def);
				offset += sizeof(eventdef_t) + event.def->bufsize;
				n++;
			}
		}
		FREE(buf);
	}

	close(server);
	exit(0);
}

