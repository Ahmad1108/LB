#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#define BUF_SIZE 100

int guess_random_port(int lower, int upper) {

  int num = 0;
  num = (rand() % (upper - lower + 1)) + lower;
  return num;
}

void bind_and_find_port(FILE *fp, int socket, struct sockaddr_in sock_add)

{
  int port = 0;

  int bind_result = -1;
  while (bind_result != 0) {
    port = guess_random_port(1024, 64000);
    sock_add.sin_port = htons(port);

    bind_result = bind(socket, (struct sockaddr *)&sock_add, sizeof(sock_add));
  }

  fprintf(fp, "%d", port);
}

char *recieve_http_request(char *temp, char *buf, int socket) {
  int bytes_recieved = 0;

  bytes_recieved = recv(socket, temp, BUF_SIZE, 0);
  if (bytes_recieved == -1) {
    printf("error while recieving message\n");
    exit(-1);
  }
  strcpy(buf, temp);

  int added = 0;
  while (strstr(temp, "\r\n\r\n") == NULL) {

    bytes_recieved = recv(socket, temp, BUF_SIZE, 0);
    if (bytes_recieved == -1) {
      printf("error while recieving message\n");
      exit(-1);
    }
    added += bytes_recieved;
    buf = (char *)realloc(buf, added + BUF_SIZE + 1);
    if (buf == NULL) {
      exit(-1);
    }

    strcat(buf, temp);
  }

  return buf;
}

char *recieve_server_answer(char *temp, char *answer, int socket) {

  char *token = NULL;
  int occurr = 0;
  int bytes_recieved = 0;
  int added = 0;

  bytes_recieved = recv(socket, temp, BUF_SIZE, 0);
  if (bytes_recieved == -1) {
    printf("error while recieving message\n");
    exit(-1);
  }
  strcpy(answer, temp);
  token = strtok(temp, "\r\n\r\n");
  if (token == NULL) {
    occurr = 0;
  } else {

    while (token != NULL) {
      token = strtok(NULL, temp);
      occurr++;
    }
  }

  while (occurr < 2) {

    bytes_recieved = recv(socket, temp, BUF_SIZE, 0);
    if (bytes_recieved == -1) {
      printf("error while recieving message\n");
      exit(-1);
    }
    added += bytes_recieved;
    answer = realloc(answer, added + BUF_SIZE + 1);
    strcat(answer, temp);
    token = strtok(temp, "\r\n\r\n");
    if (token == NULL) {
      occurr = 0;
    } else {
      while (token != NULL) {
        token = strtok(NULL, temp);
        occurr++;
      }
    }
  }

  return answer;
}

void main() {

  FILE *fp1 = NULL;
  FILE *fp2 = NULL;
  fp1 = fopen("server_port", "w");
  fp2 = fopen("http_port", "w");

  if (fp1 == NULL || fp2 == NULL) {
    printf("Creating file failed.\n");
    exit(-1);
  }

  int port = 0, bind_result = 0;
  srand(time(NULL));
  int http_socket, server_socket;
  http_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in http_address;
  http_address.sin_family = AF_INET;
  http_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  char *temp = NULL;
  char *buf = NULL;
  char *answer = NULL;
  answer = (char *)malloc(BUF_SIZE * sizeof(char));
  temp = (char *)malloc(BUF_SIZE * sizeof(char));
  buf = (char *)malloc(BUF_SIZE * sizeof(char));

  if (temp == NULL || answer == NULL || buf == NULL) {
    printf("Memory allocation failed!\n");
    exit(-1);
  }
  int new_http_socket, new_server_socket;
  bind_and_find_port(fp1, server_socket, server_address);
  bind_and_find_port(fp2, http_socket, http_address);
  listen(server_socket, 5);
  listen(http_socket, 5);

  fclose(fp1);
  fclose(fp2);

  FILE *fp = fopen("temp.txt", "w");

  while (true) {
    listen(server_socket, 5);
    listen(http_socket, 5);
    new_server_socket = accept(server_socket, NULL, NULL);
    new_http_socket = accept(http_socket, NULL, NULL);

    buf = recieve_http_request(temp, buf, new_http_socket);

    fprintf(fp, "buf is %s\n", buf);
    send(new_server_socket, buf, strlen(buf), 0);

    buf = "";
    answer = recieve_server_answer(temp, answer, new_server_socket);
    fprintf(fp, "answer is is %s\n", answer);

    if (send(new_http_socket, answer, strlen(answer), 0) == -1) {
      printf("Error sending message\n");
    }
  }
}