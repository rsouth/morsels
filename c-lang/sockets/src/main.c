#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "date_time.h"
#include "main.h"
#include "server.h"

/**
 *
 * -c - Client
 */

int main(int argc, char* argv[]) {
  if (argc == 2) {
    printf("The argument supplied is %s\n", argv[1]);

    if (!(strcmp(argv[1], FLAG_C)) || !(strcmp(argv[1], FLAG_CLIENT))) {
      printf("Starting client\n");
      start_client();

    } else if (!(strcmp(argv[1], FLAG_S)) || !(strcmp(argv[1], FLAG_SERVER))) {
      printf("Starting server\n");
      return start_server();

    } else {
      printf("Invalid arg: %s\n", argv[1]);
      return EXIT_FAILURE;
    }

  } else {
    printf("One argument expected.\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
