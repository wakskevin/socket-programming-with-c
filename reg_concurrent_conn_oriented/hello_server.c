// hello_server.c
#include <stdio.h>
#include <rpc/rpc.h>
#include "hello.h"

char **hello_1_svc(char **msg, struct svc_req *req) {
  static char *reply;

  printf("Received message: %s\n", *msg);
  
  reply = malloc(20);
  strcpy(reply, "Hello back!");

  return &reply;
}
