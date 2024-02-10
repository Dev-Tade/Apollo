#define XWSOCKS_IMPLEMENTATION
#include "../xwsocks.h"

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define PORT 8080

char *make_http_response(char *http_version, int status_code);

int main() {
  if (xwSocks_init() < 0) {
    fprintf(stderr, "Error initializing xwSocks\n");
    return 1;
  }

  xwSocket server_sock = xwSocks_socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    fprintf(stderr, "Error creating socket\n");
    return 1;
  }

  int opt = 1;
  xwSocks_setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  xwSockaddr_in saddr_in;
  memset(&saddr_in, 0, sizeof(xwSockaddr_in));

  saddr_in.sin_family = AF_INET;
  saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr_in.sin_port = htons(PORT);

  if (xwSocks_bind(server_sock, (xwSockaddr*)&saddr_in, sizeof(saddr_in)) < 0) {
    fprintf(stderr, "Error binding socket\n");
    return 1;
  }

  if (xwSocks_listen(server_sock, 10) < 0) {
    fprintf(stderr, "Error listening\n");
    return 1;
  }

  char buffer[4096];
  int running = 1;

  while (running) {
    xwSocket client_socket = xwSocks_accept(server_sock, NULL, NULL);
    if (client_socket < 0) {
      fprintf(stderr, "Error accepting\n");
      running = 0;
    }

    memset(buffer, 0, 4096);

    if (xwSocks_recv(client_socket, buffer, 4096, 0) < 0) {
      fprintf(stderr, "Error receiving\n");
      
      if (xwSocks_close(client_socket) < 0)
        fprintf(stderr, "Error closing client socket\n");

      running = 0;
    }

    printf("%s\n", buffer);

    char *request = strdup(buffer);
    char *method = strtok(request, " ");
    char *route = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    char *response = make_http_response(version, 200);
    if (xwSocks_send(client_socket, response, strlen(response), 0) < 0) {
      fprintf(stderr, "Error sending\n");

      free(request);
      free(response);

      if (xwSocks_close(client_socket) < 0)
        fprintf(stderr, "Error closing client socket\n");

      running = 0;
    }

    free(request);
    free(response);

    if (xwSocks_close(client_socket) < 0) {
      fprintf(stderr, "Error closing client socket\n");
      running = 0;
    }
  }

  if (xwSocks_close(server_sock) < 0) {
    fprintf(stderr, "Error closing server socket\n");
    return 1;
  }

  return 0;
}

const char* status_codes_text[] = {
	[200] = "OK",
};

char *make_http_response(char *http_version, int status_code) {
  char* response = (char*)malloc(2048);
	memset(response, 0, 2048);

	snprintf(response, 2048,
		"%s %d %s\r\n",
		http_version,
		status_code,
		status_codes_text[status_code]
	);

	return response;
}