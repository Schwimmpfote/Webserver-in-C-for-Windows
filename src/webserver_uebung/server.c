#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define BUFFER_SIZE 4096

/**
 * @brief Sucht die Telefonnummer zu einem Namen in telefon.txt.
 * @param gesuchter_name Name, nach dem gesucht wird.
 * @param telefon Speicher für die gefundene Telefonnummer.
 * @param telefon_size Größe des Telefon-Buffers.
 * @return 1 bei Erfolg, sonst 0.
 */
int telefonnummer_suchen(
    const char* gesuchter_name,
    char* telefon,
    size_t telefon_size
) {
    FILE* file = fopen("telefon.txt", "r");

    if (file == NULL) {
        printf("Fehler: telefon.txt konnte nicht geoeffnet werden.\n");
        return 0;
    }

    char zeile[BUFFER_SIZE];

    while (fgets(zeile, sizeof(zeile), file) != NULL) {

        zeile[strcspn(zeile, "\r\n")] = '\0';

        char* trennzeichen = strchr(zeile, ';');

        if (trennzeichen == NULL) {
            continue;
        }

        *trennzeichen = '\0';

        char* name = zeile;
        char* nummer = trennzeichen + 1;

        if (strcmp(name, gesuchter_name) == 0) {

            strncpy(
                telefon,
                nummer,
                telefon_size - 1
            );

            telefon[telefon_size - 1] = '\0';

            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/**
 * @brief Startet den HTTP-Webserver.
 * @return 0 bei richtigem Ende, sonst 1 bei Fehlern.
 */
int main(void) {
    WSADATA wsa;
    SOCKET server_socket;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup fehlgeschlagen.\n");
        return 1;
    }

    server_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (server_socket == INVALID_SOCKET) {
        printf("socket() fehlgeschlagen.\n");
        WSACleanup();
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /* Der Socket wird an Port 80 gebunden. */
    if (bind(
            server_socket,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)
        ) == SOCKET_ERROR) {

        printf(
            "bind() fehlgeschlagen. "
            "Port 80 eventuell bereits belegt.\n"
        );

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("listen() fehlgeschlagen.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Webserver laeuft auf Port %d...\n", PORT);

    while (1) {
        client_socket = accept(
            server_socket,
            NULL,
            NULL
        );

        if (client_socket == INVALID_SOCKET) {
            printf("accept() fehlgeschlagen.\n");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));

        int received = recv(
            client_socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        if (received <= 0) {
            closesocket(client_socket);
            continue;
        }

        buffer[received] = '\0';

        printf("Anfrage:\n%s\n", buffer);

        /** Die angeforderte URL wird in Pfad und Parameter aufgeteilt.*/
        char* url = buffer + 4;
        char* url_ende = strchr(url, ' ');

        if (url_ende != NULL) {
            *url_ende = '\0';

            char* fragezeichen = strchr(url, '?');

            if (fragezeichen == NULL) {

                if (strcmp(url, "/index.html") == 0) {
                    FILE* file = fopen("index.html", "rb");

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
            }
            else {
                *fragezeichen = '\0';

                char* parameter = fragezeichen + 1;
                char* gleich = strchr(parameter, '=');

                if (gleich != NULL) {
                    *gleich = '\0';

                    char* parameter_name = parameter;
                    char* parameter_wert = gleich + 1;
                    /**Parameter gibt die Suche an. */
                    if (strcmp(parameter_name, "name") == 0) {
                        char telefon[100];

                        int gefunden = telefonnummer_suchen(
                            parameter_wert,
                            telefon,
                            sizeof(telefon)
                        );

                        if (gefunden) {
                            char response[BUFFER_SIZE];

                            int response_length = snprintf(
                                response,
                                sizeof(response),
                                "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: %zu\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "%s",
                                strlen(telefon),
                                telefon
                            );

                            send(
                                client_socket,
                                response,
                                response_length,
                                0
                            );
                        }
                        else {
                            const char* response_body =
                                "Name nicht gefunden";

                            char response[BUFFER_SIZE];

                            int response_length = snprintf(
                                response,
                                sizeof(response),
                                "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: %zu\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "%s",
                                strlen(response_body),
                                response_body
                            );

                            send(
                                client_socket,
                                response,
                                response_length,
                                0
                            );
                        }
                    }
                    else {
                        const char* response_body =
                            "Unbekannter Parameter";

                        char response[BUFFER_SIZE];

                        int response_length = snprintf(
                            response,
                            sizeof(response),
                            "HTTP/1.1 400 Bad Request\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: %zu\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "%s",
                            strlen(response_body),
                            response_body
                        );

                        send(
                            client_socket,
                            response,
                            response_length,
                            0
                        );
                    }
                }
            }
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
