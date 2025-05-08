#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define PORT 10000

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
    	printf("Failed to create socket\n");
    	return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        printf("Failed to set socket option. ERRNO: %d\n", errno);
        return 1;
    }

    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    	printf("Failed to bind socket. ERRNO: %d\n", errno);
    	return -1;
    }

    if (listen(fd, 2) == -1) {
    	printf("Failed to listen socket. ERRNO: %d\n", errno);
 	    return -1;
    }

    printf("Server listening on port %d\n", PORT);

	while (1) {
	    int client_fd = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&addr);
	    if (client_fd == -1) {
	    	printf("Failed to accept client. ERRNO: %d\n", errno);
	    	close(client_fd);
	    	return -1;
	    }

	    char request[1024];
	    int bytes = recv(client_fd, request, sizeof(request), 0);
	    if (bytes == -1) {
	    	printf("Failed to receive from client. ERRNO: %d\n", errno);
	    	close(client_fd);
	    	return -1;
	    }
	    request[bytes] = '\0';
	    printf("Request:\n%s", request);

	    char method[5], route[32];
	    sscanf(request, "%s %s", method, route);

		const char* response_header = 
	        "HTTP/1.1 200 OK\r\n"
	        "Content-Type: text/html; charset=UTF-8\r\n";

		// const char* response_body = "{\"message\": \"Hello from C server!\", \"status\": \"success\"}";

		const char* response_body = "<h1>404!</h1><h2>Page Not Found!</h2>";
		if (strcmp(method, "GET") == 0) {
			if (strcmp(route, "/") == 0) {
				response_body =
			        "<html><body>"
			        "<h1>Hello!</h1>"
			        "<p>This message is from a web server built using C 💪🔥!</p>"
			        "<h4>Hey Google! Hire me!!</h4>"
					"</body></html>";
			} else if (strcmp(route, "/login") == 0) {
				response_body =
			        "<html><body>"
			        "<h1>Hello!</h1>"
			        "<p>This is the <h2>Login Page</h2>  from a web server built using C 💪🔥!</p>"
			        "<h4>Hey Google! Hire me!!</h4>"
					"</body></html>";
			}
		}

	    char content_length_header[100];
	    snprintf(content_length_header, sizeof(content_length_header), "Content-Length: %lu\r\n\r\n", strlen(response_body));

		// Headers
	    send(client_fd, response_header, strlen(response_header), 0);
	    send(client_fd, content_length_header, strlen(content_length_header), 0);

		// Body
		send(client_fd, response_body, strlen(response_body), 0);

		close(client_fd);
	}
	
    close(fd);
}
