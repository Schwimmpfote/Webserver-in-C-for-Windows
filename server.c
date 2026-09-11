/**
 * Unterdrückt Warnungen von veralteten WinSock Funktionen.
 * @brief Unterdrückt Warnungen bezüglich veralteter Winsock-Funktionen.
 * @note Entsprechend ist der Code nur für Windows Server ausgelegt und nicht für andere Betriebsysteme geeignet, oder muss erst getestet werden.
 */
#define _WINSOCK_DEPRECATED_NO_WARNINGS /*Definiert eine WINSocket ohne Warnungen*/ /*benötigt: Compiler-Unterstützung für Präprozessor-Makros*/


#include <stdio.h> /*package stdio.h wird included/benötigt*/ /*Benötigt für Input Output Management*/ /*benötigt: printf(), FILE, fopen(), fread(), fclose(), snprintf()*/
#include <stdlib.h> /*package stdlib.h wird included/benötigt*/ /*benötigt für Speicher allocation*/ /*benötigt: Funktionen der Standardbibliothek*/
#include <string.h> /*package string.h wird included/benötigt(Standard Lässt Strings zu)*//*benötigt für strings*/ /*benötigt: memset(), strlen(), strncmp()*/
#include <winsock2.h> /*package winsock2.h wird included/benötigt*//*benötigt für Sockets auf Windows*/ /*benötigt: Windows und Winsock2 API*/


#pragma comment(lib, "ws2_32.lib") /*sorgt dafür, dass es auf Windows läuft*/ /*benötigt, damit Windows in der Lage ist das durchzuführen*/ /*benötigt: Microsoft Visual C++ Compiler und Winsock-Bibliothek*/


/**
 * Port 80 wird für jegliche Kommunikation verwendet.
 * 
 * @note Wenn der Port voll, oder kaputt ist, wird eine Exception geschmissen. Die Methode geht dann wieder in den Anfang des WHILEs.
 */
#define PORT 80 /*Port wird auf 80 definiert*/ /*benötigt für die Kommunikation mit dem Client*/ /*benötigt: wird später von htons() und printf() verwendet*/


/**
 * Die BUFFER_SIZE definiert die Größe die für Nachrichten zugelassen ist. So wird nur eine bestimmte Filegröße erlaubt.
 */
#define BUFFER_SIZE 4096 /*Max Größe des Buffers auf 4096*/ /*benötigt als Textgrößen Limit. (Bei Lokalen Servern klein, bei global groß*/ /*benötigt: wird für die Größe von buffer und body verwendet*/


/**
 * Startet den Webserver. mit Port 80. Es wird ausschließlich index.html als Request akzeptiert.
 * 
 * @return 1 wird bei einem Fehler und 0 bei Erfolg zurückgegeben.
 */
int main(void) /*Anfang Main*/ /*benötigt um alles zu laufen*/ /*benötigt: gültiger C-Einstiegspunkt und Betriebssystem zum Ausführen des Programms*/
{
    /*benötigen WSA*/
    WSADATA wsa; /*Win Socket API DATA erstellen*/ /*Wichtig für die Sockets, wird später befüllt*/ /*benötigt: <winsock2.h> und wird von WSAStartup() verwendet*/


    SOCKET server_socket; /*Socket für den Server erstellen*/ /*Wichtig für die Verbindungen von Client & Server, wird später befüllt*/ /*benötigt: <winsock2.h> und erfolgreiche Ausführung von socket()*/


    SOCKET client_socket; /*Socket für den Client erstellen*//*Wichtig für die Verbindungen von Client & Server, wird später befüllt*/ /*benötigt: <winsock2.h> und erfolgreiche Ausführung von accept()*/


    struct sockaddr_in server_addr; /*definiert eine sockaddr_in als Server Addresse*/ /*benötigt für das definieren der Server Socket*/ /*benötigt: <winsock2.h> und wird anschließend für bind() konfiguriert*/


    char buffer[BUFFER_SIZE]; /*Ein char array der Größe der Buffer Size wird erstellt*/ /*benötigt um die Nachricht zu speichern und zu verarbeiten*/ /*benötigt: BUFFER_SIZE muss vorher definiert sein*/


    /* Winsock starten */ /*benötigen WSA*/
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) /*WinSocketAPI der Version 2.2 mit pointer zu WSADATA wird auf Funktion getestet*/ /*benötigt für Fehlerhandling*/ /*benötigt: <winsock2.h>, wsa und Windows Winsock muss verfügbar sein*/
    {
        printf("WSAStartup fehlgeschlagen.\n"); /* printed fehlernachricht, dass WSA nicht starten konnte.*/ /*benötigt für Fehlerhandling*/ /*benötigt: <stdio.h>*/
        return 1; /*1 return, weil es ein Fehler ist.*/ /*benötigt für Fehlerhandling*/ /*benötigt: muss innerhalb einer Funktion stehen*/
    }


    /* Socket erstellen*/
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* Die Server Socket wird auf IPv4, die nutzbare/funktionierende Stream Socket und auf TCP Protokoll gesetzt.*/ /*benötigt: erfolgreiche WSAStartup()-Initialisierung und <winsock2.h>*/


    if (server_socket == INVALID_SOCKET) /*Wenn die socket nicht gültig ist*/ /*benötigt für Fehlerhandling*/ /*benötigt: server_socket muss vorher durch socket() erstellt worden sein*/
    {
        printf("socket() fehlgeschlagen.\n"); /*wird eine Fehlermeldung geprinted*/ /*benötigt für Fehlerhandling*/ /*benötigt: <stdio.h>*/
        WSACleanup(); /*WinSocketAPI wird aufgeräumt(Speicher delete,etc)*/ /*benötigt um in den Ursprungszustand zu gelangen*/ /*benötigen WSA*/ /*benötigt: vorherige erfolgreiche WSAStartup()-Initialisierung*/
        return 1; /*und 1 returned, weil es ein Fehler ist.*/ /*benötigt für Fehlerhandling*/ /*benötigt: muss innerhalb einer Funktion stehen*/
    }


    /* Server-Adresse konfigurieren */
    memset(&server_addr, 0, sizeof(server_addr)); /*Die Serveradresse wird konfiguriert mit dem Wert 0 und der Größe der Serveradresse.*/ /*benötigt um den Platz für die Adressen zu haben*/ /*benötigt: <string.h> und eine vorhandene server_addr-Struktur*/


    server_addr.sin_family = AF_INET; /*Die Socket Adress Internet der Server Adress Familie wird auf IPv4 gesetzt.*/ /*Wichtig für klare Kommunikation von Client & Server*/ /*benötigt: server_addr muss vorher deklariert worden sein*/


    server_addr.sin_addr.s_addr = INADDR_ANY; /*Die Socket Adress Internet der Server Adresse wird auf WildCard 0.0.0.0 gesetzt.*/ /*Wichtig für klare Kommunikation von Client & Server*/ /*benötigt: server_addr und <winsock2.h>*/


    server_addr.sin_port = htons(PORT); /*Der Server Adress Port wird durch Host to Network auf die standartisierte Network Byte Order gesetzt.*/ /*Wichtig für klare Kommunikation von Client & Server*/ /*benötigt: PORT muss definiert sein, <winsock2.h> und server_addr muss existieren*/


    /* Socket an Port 80 binden */
    if (bind(server_socket,
             (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == SOCKET_ERROR) /*Wenn die Server Socket zu einem Socket Error führt*/ /*benötigt für Fehlerhandling*/ /*benötigt: erfolgreiche socket()-Ausführung, konfigurierte server_addr und WSAStartup()*/


    {
        printf("bind() fehlgeschlagen. Port 80 eventuell bereits belegt.\n"); /*Fehlermeldung printen, weil Socket evtl belegt ist.*/ /*benötigt für Fehlerhandling*/ /*benötigt: <stdio.h>*/


        closesocket(server_socket); /*Schließt die Server Socket*/ /*benötigt um in den Ursprungszustand zu gelangen*/ /*benötigt: gültiger server_socket und <winsock2.h>*/


        WSACleanup(); /*reinigt WinSocketAPI auf*/ /*benötigt um in den Ursprungszustand zu gelangen*/ /*benötigen WSA*/ /*benötigt: vorherige WSAStartup()-Initialisierung*/


        return 1; /*returned 1, weil Fehler*/ /*benötigt für Fehlerhandling*/ /*benötigt: muss innerhalb einer Funktion stehen*/
    }


    /* Auf Verbindungen warten */
    if (listen(server_socket, 10) == SOCKET_ERROR) /*Wenn das "zuhören" von der ServerSocket in einen Fehler rennt*/ /*benötigt für Fehlerhandling*/ /*benötigt: server_socket muss erfolgreich mit bind() verbunden worden sein*/


    {
        printf("listen() fehlgeschlagen.\n"); /*Fehlertext, dass listen fehlgeschlagen ist*/ /*benötigt für Fehlerhandling*/ /*benötigt: <stdio.h>*/


        closesocket(server_socket); /*schließt Socket*/ /*benötigt um in den Ursprungszustand zu gelangen*/ /*benötigt: gültiger server_socket*/


        WSACleanup(); /*cleaned WinSOcketAPI auf*/ /*benötigt um in den Ursprungszustand zu gelangen*/ /*benötigen WSA*/ /*benötigt: vorherige WSAStartup()-Initialisierung*/


        return 1; /*returned 1, weil Fehler*/ /*benötigt für Fehlerhandling*/ /*benötigt: muss innerhalb einer Funktion stehen*/
    }


    printf("Webserver laeuft auf Port %d...\n", PORT); /*Wenn alles läugt eine Info, dass alles läuft*/ /*wichtig für Admin des servers um Prozess zu verstehen*/ /*benötigt: <stdio.h> und vorher definierter PORT*/


    while (1) /*while true, also endlos loop bis break*/ /*wichtig um immer wieder zu checken ob eine Verbindung hergestellt werden kann*/ /*benötigt: gültige Programmausführung und einen vorher erfolgreich gestarteten Server-Socket*/
    {
        /* Verbindung akzeptieren */
        client_socket = accept(server_socket, NULL, NULL); /*Setzt den client Socket auf Server Socket, pointer zur Socket Adresse und Pointer zur Adress Länge. Null Null, weil beides uninteressant ist für unseren Fall.*/ /*wichtig damit der Server nach den Nachrichten des Clients suchen kann*/ /*benötigt: listen() muss erfolgreich ausgeführt worden sein*/


        if (client_socket == INVALID_SOCKET) /*Wenn die Client Socket invalid ist*/ /*benötigt für Fehlerhandling*/ /*benötigt: client_socket muss vorher durch accept() gesetzt worden sein*/
        {
            printf("accept() fehlgeschlagen.\n"); /*Fehlermeldung*/ /*benötigt für Fehlerhandling*/ /*benötigt: <stdio.h>*/


            continue; /*while wird zu start zurückgesetzt bzw in den nächsten Loop*/ /*benötigt, damit ein neuer Client definiert werden kann und weiter auf Requests gewartet wird.*/ /*benötigt: eine umschließende Schleife*/
        }


        /* Anfrage empfangen */
        memset(buffer, 0, sizeof(buffer)); /*nimmt Speicher für buffer mit wert 0 und der größe des buffers.*/ /*Wichtig um den Platz für den Buffer zu haben*/ /*benötigt entsprechenden Platz.*/ /*benötigt: <string.h> und vorher angelegten buffer*/


        int received = recv(
            client_socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        ); /*setzt received auf die erhaltene Nachricht von dem Client an der stelle von buffer mit der max länge von buffer minus 1 (wegen \0 termination)*/ /*benötigt um zu verstehen ob und wie viel erhalten wurde*/ /*benötigt: gültiger client_socket, buffer, BUFFER_SIZE und erfolgreiche accept()-Ausführung*/


        if (received <= 0) /*wenn received <= 0 ist, also nichts (oder was falsches) erhalten wurde*/ /*benötigt für neuen Loop, wenn keine Request*/ /*benötigt: vorheriger Aufruf von recv() und eine gültige Variable received*/
        {
            closesocket(client_socket); /*schließe die Socket*/ /*benötigt für neuen Loop, wenn keine Request*/ /*benötigt: gültiger client_socket und <winsock2.h>*/


            continue; /*while wird zu start zurückgesetzt bzw in den nächsten Loop*/ /*benötigt für neuen Loop, wenn keine Request*/ /*benötigt: eine umschließende Schleife*/
        }


        buffer[received] = '\0'; /*setzt buffer an der stelle recieved auf den terminationswert (\0)*/ /*benötigt, damit das Ende des Arr gefunden werden kann*/ /*benötigt: received muss kleiner als BUFFER_SIZE sein und buffer muss existieren*/


        printf("Anfrage:\n%s\n", buffer); /*Rückmeldung, dass die Anfrage an der Stelle des buffers ist.*/ /*benötigt für Admin Verständnis*/ /*benötigt: <stdio.h> und einen gültig terminierten buffer*/


        /*
         * Nur GET /index.html erlauben.
         */
        if (strncmp(buffer, "GET /index.html HTTP/", 20) == 0) /*Wenn die request mit Get nach index.html sucht*/  /*benötigt um das richtige File zu finden*/ /*benötigt: <string.h> und einen gültigen, terminierten buffer*/


        {
            FILE* file = fopen("index.html", "rb"); /*öffne die file*/ /*benötigt um das File zu senden, damit Länge etc eingeschätzt werden können*/ /*benötigt: <stdio.h> und eine vorhandene index.html im Arbeitsverzeichnis*/


            if (file == NULL) /*Wenn die File NULL ist, also keinen Wert hat*/ /*benötigt für Fehlerhandling*/ /*benötigt: vorheriger Aufruf von fopen()*/


            {
                const char* response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 23\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "index.html nicht gefunden"; /*erstelle einen 404 Text und schließe die Connection*/ /*benötigt für Fehlerhandling von User*/ /*benötigt: keine zusätzliche Bibliothek, const char* und String-Literale*/


                send(
                    client_socket,
                    response,
                    (int)strlen(response),
                    0
                ); /*sende die response*/ /*benötigt für Fehlerhandling von User*/ /*benötigt: gültiger client_socket, response, <string.h> und <winsock2.h>*/
            }


            else
            {
                char body[BUFFER_SIZE]; /*setzt body auf die größe der Buffer Size*/ /*benötigt um body mit file zu füllen*/ /*benötigt: BUFFER_SIZE muss vorher definiert sein*/


                size_t length = fread(
                    body,
                    1,
                    sizeof(body) - 1,
                    file
                ); /*Anzahl der Elemente die erfolgreich aus dem Stream gelesen wurden.*/ /*benötigt um body mit file zu füllen*/ /*benötigt: <stdio.h>, geöffnete file und ausreichend großen body-Buffer*/


                fclose(file); /*Schließe die File*/ /*schließen, weil Inhalt gespeichert*/ /*benötigt: vorher erfolgreich mit fopen() geöffnete Datei*/


                body[length] = '\0'; /*Setze das letzte element auf den Terminator*/ /*damit Ende des Array gefunden werden kann*/ /*benötigt: length muss kleiner als BUFFER_SIZE sein und body muss existieren*/


                char response[BUFFER_SIZE * 2]; /*char array response der Größe Buffer Size verdoppelt*/ /*benötigt damti die response das File und die Nachricht(mit Meta) zurückgeben kann*/ /*benötigt: BUFFER_SIZE muss vorher definiert sein*/


                int response_length = snprintf(
                    response, /**/
                    sizeof(response), /*Setze die response Länge anhand der response & bitlänge fest.*/ /*Antwort, damit Client die Anfrage erhält*/

                    "HTTP/1.1 200 OK\r\n" /*200 OK, damit klar ist, dass die Nachricht korrekt ist/alles fertig ist*/ /*Antwort, damit Client die Anfrage erhält*/
                    "Content-Type: text/html\r\n" /*form der datei*/ /*Antwort, damit Client die Anfrage erhält*/
                    "Content-Length: %zu\r\n" /*Platzhalter für die Länge des ganzen*/ /*Antwort, damit Client die Anfrage erhält*/
                    "Connection: close\r\n" /*schließen der Verbindung*/ /*Antwort, damit Client die Anfrage erhält*/
                    "\r\n" /*Absatz*/ /*Antwort, damit Client die Anfrage erhält*/
                    "%s", /*\0 Terminator Platzhalter*/ /*Antwort, damit Client die Anfrage erhält*/

                    length, /*länge des bodies*/ /*Antwort, damit Client die Anfrage erhält*/
                    body /*inhalt body*/ /*Antwort, damit Client die Anfrage erhält*/
                ); /*benötigt: <stdio.h>, response-Array, body und length müssen vorher existieren*/


                send(
                    client_socket,
                    response,
                    response_length,
                    0
                ); /*sendet die Response an den Client*/ /*sendet Antwort, damit Client die Anfrage erhält*/ /*benötigt: gültiger client_socket, response und gültige response_length*/
            }
        }


        else
        {
            const char* response = /*Wenn falsche File gesucht wird Fehlermeldung*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "HTTP/1.1 404 Not Found\r\n" /*404 weil nichts gefunden wird*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "Content-Type: text/plain\r\n" /*rückgabe ist in plaintext*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "Content-Length: 9\r\n" /*Länge ist 9*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "Connection: close\r\n" /*Verbindung wird geschlossen*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "\r\n" /*Absatz*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/
                "Not Found"; /*kein weiterer Inhalt, weil es keinen gibt*/ /*Antwort, damit Client & Server den Fehler versteht erhält*/ /*benötigt: keine zusätzliche Bibliothek, nur einen gültigen C-String*/


            send(
                client_socket,
                response,
                (int)strlen(response),
                0
            );/*sendet die Response an den Client*/ /*sendet Antwort, damit Client & Server den Fehler versteht erhält*/ /*benötigt: gültiger client_socket, response, <string.h> und <winsock2.h>*/
        }


        /* Verbindung schließen */
        closesocket(client_socket); /*Schließt die Verbindung mit dem Client*/ /*benötigt, damit nächste request verarbeitet werden kann*/ /*benötigt: gültiger client_socket und <winsock2.h>*/
    }


    /*Sollte nie hier ankommen, wegen endlos While*/
    closesocket(server_socket); /*schließt die Server Socket*/ /*benötigt, damit alles aufgeräumt wird*/ /*benötigt: gültiger server_socket und vorherige socket()-Erstellung*/


    WSACleanup(); /*WinSocketAPI wird aufgeräumt*/ /*benötigt, damit alles aufgeräumt wird*/ /*benötigen WSA*/ /*benötigt: vorherige erfolgreiche WSAStartup()-Initialisierung*/


    return 0; /*returned 0 weil alles richtig lief.*/ /*benötigt um Ende zu markieren.*/ /*benötigt: muss innerhalb einer Funktion stehen*/
}
