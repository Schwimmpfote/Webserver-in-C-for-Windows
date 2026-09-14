#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define BUFFER_SIZE 4096


/**
 * @brief Startet einen einfachen HTTP-Webserver.
 *
 * Der Server akzeptiert GET-Anfragen nur für index.html
 * und beantwortet alle anderen Anfragen mit einem 404.
 */
int main(void) {
    WSADATA wsa;
    SOCKET server_socket;
    SOCKET client_socket;

    struct sockaddr_in server_addr;

    char buffer[BUFFER_SIZE];

    /** Testet die Windows Socket API.*/
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup fehlgeschlagen.\n");
        return 1;
    }

    /** Erstellt den TCP-Server Socket mit IPv4.*/
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /** Testet, ob die Server Socket richtig erstellt wurde.*/
    if (server_socket == INVALID_SOCKET) {
        printf("socket() fehlgeschlagen.\n");
        WSACleanup();
        return 1;
    }

    /** Konfiguriert die Serveradresse und den Port. auf IPv4 und Port 80*/
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /** Bindet den Server Socket an die konfigurierte Adresse.*/
    if (bind(server_socket,
             (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == SOCKET_ERROR) {

        printf("bind() fehlgeschlagen. Port 80 eventuell bereits belegt.\n");

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    /** Server Socket test, inwiefern zuhören funktioniert.*/
    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("listen() fehlgeschlagen.\n");

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Webserver laeuft auf Port %d...\n", PORT);

    /** Wartet dauerhaft auf Client Verbindungen.*/
    while (1) {
        client_socket = accept(server_socket, NULL, NULL);

        /** Testet, ob die Client-Verbindung erfolgreich hergestellt wurde.*/
        if (client_socket == INVALID_SOCKET) {
            printf("accept() fehlgeschlagen.\n");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));

        /** Empfängt die Anfrage des Clients.*/
        int received = recv(
            client_socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        /** Testet, ob die Anfrage erfolgreich empfangen wurde.*/
        if (received <= 0) {
            closesocket(client_socket);
            continue;
        }

        buffer[received] = '\0';

        printf("Anfrage:\n%s\n", buffer);

        /** Prüft, ob die Anfrage index.html haben will.*/
        if (strncmp(buffer, "GET /index.html HTTP/", 20) == 0) {
            FILE* file = fopen("index.html", "rb");

            /** Antwortet mit 404, falls die Datei nicht existiert.*/
            if (file == NULL) {
                const char* response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 23\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "index.html nicht gefunden";

                send(
                    client_socket,
                    response,
                    (int)strlen(response),
                    0
                );
            }
            /**Antwortet mit 200 und den Dateiinhalten.*/
            else {
                char body[BUFFER_SIZE];

                size_t length = fread(
                    body,
                    1,
                    sizeof(body) - 1,
                    file
                );
                fclose(file);
                body[length] = '\0';

                /** Erstellt die HTTP-Antwort mit dem Dateiinhalt. */
                char response[BUFFER_SIZE * 2];
                int response_length = snprintf(
                    response,
                    sizeof(response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: %zu\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "%s",
                    length,
                    body
                );

                send(
                    client_socket,
                    response,
                    response_length,
                    0
                );
            }
        }
        /** Beantwortet unbekannte Pfade mit 404.*/
        else {
            const char* response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 9\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Not Found";

            send(
                client_socket,
                response,
                (int)strlen(response),
                0
            );
        }
        /**Schließt Client Socket nach der Bearbeitung der Anfrage.
         */
        closesocket(client_socket);
    }

    /**
     * Wenn While irgendwie auf 0 gesetzt wird, wird erneut aufgeräumt.
     */
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
