#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3002
#define BUFFER_SIZE 1024

// {"message":"Key 'name' set to 'nadir'."}
// {"value":"nadir"}
// {"message":"Key 'nadir' not found."}

void send_request(const char *json_data)
{
  int sock = 0;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE] = {0};
  char request[BUFFER_SIZE] = {0};
  int content_length = strlen(json_data);

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket creation error");
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  // Convert IPv4 address from text to binary
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
  {
    perror("Invalid address or address not supported");
    close(sock);
    return;
  }

  // Connect to the server
  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Connection failed");
    close(sock);
    return;
  }

  // Prepare the HTTP POST request
  snprintf(request, BUFFER_SIZE,
           "POST /data HTTP/1.1\r\n"
           "Host: 127.0.0.1:%d\r\n"
           "Content-Type: application/json\r\n"
           "Content-Length: %d\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           PORT, content_length, json_data);

  // Send the request
  send(sock, request, strlen(request), 0);

  // Receive response from the server
  int valread = read(sock, buffer, BUFFER_SIZE - 1);
  if (valread > 0)
  {
    buffer[valread] = '\0'; // Null-terminate the response

    // Find the start of the body
    char *body = strstr(buffer, "\r\n\r\n");
    if (body)
    {
      body += 4; // Skip over the "\r\n\r\n"
      printf("Server Response: %s\n", body);
    }
    else
    {
      printf("Unable to find response body.\n");
    }
  }
  else
  {
    printf("No response from server.\n");
  }

  close(sock);
}

int main()
{
  char action[10], key[50], value[100];
  char json_data[BUFFER_SIZE];
  char continue_choice;

  while (1)
  {
    while (1)
    {
      printf("\nEnter action (set/get/delete): ");
      scanf("%s", action);
      if (strcmp(action, "set") == 0 || strcmp(action, "get") == 0 || strcmp(action, "delete") == 0)
      {
        break;
      }
      else
      {
        printf("Invalid action. Please enter a valid action.\n");
      }
    }

    printf("Enter key: ");
    scanf("%s", key);

    if (strcmp(action, "set") == 0)
    {
      printf("Enter value: ");
      scanf("%s", value);
      snprintf(json_data, BUFFER_SIZE, "{\"action\": \"%s\", \"key\": \"%s\", \"value\": \"%s\"}", action, key, value);
    }
    else
    {
      snprintf(json_data, BUFFER_SIZE, "{\"action\": \"%s\", \"key\": \"%s\"}", action, key);
    }

    send_request(json_data);
  }

  return 0;
}
