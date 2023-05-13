// hello_client.c
#include <stdio.h>
#include <rpc/rpc.h>
#include "hello.h"

int main(int argc, char *argv[]) {
  CLIENT *cl;
  char *server;
  char *message;
  char **result;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s host message\n", argv[0]);
    exit(1);
  }

  server = argv[1];
  message = argv[2];

  cl = clnt_create(server, HELLO, HELLO_V1, "tcp");
  if (cl == NULL) {
    clnt_pcreateerror(server);
    exit(2);
  }

  result = hello_1(&message, cl);
  if (result == NULL) {
    clnt_perror(cl, server);
    exit(3);
  }

  printf("Reply from server: %s\n", *result);

  clnt_destroy(cl);

  return 0;
}
