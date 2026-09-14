# Webserver für Windows in C

Einfacher HTTP-Webserver in C für Windows mithilfe der **Windows Winsock2 API**.

## Beschreibung

Dieses Projekt implementiert einen einfachen HTTP-Webserver in der Programmiersprache **C** mithilfe der **Windows Winsock2 API**.

Der Webserver läuft auf **Port 80** und wartet dauerhaft auf eingehende TCP-Verbindungen. Bei einer Anfrage wird überprüft, ob der Client die Datei `index.html` anfordert.

Aktuell wird ausschließlich folgende Anfrage akzeptiert:

```text
GET /index.html HTTP/...
```

Wenn die Datei vorhanden ist, wird sie mit dem HTTP-Status **200 OK** an den Client zurückgegeben. Bei einer nicht vorhandenen Datei oder einer anderen Anfrage wird eine **404 Not Found**-Antwort gesendet.

> **Hinweis:** Der Server ist speziell für Windows ausgelegt und verwendet die Windows-spezifische Winsock2-API. Eine Verwendung unter Linux oder macOS ist mit diesem Code nicht direkt möglich.

## Funktionen

- TCP-Socket über Winsock2
- IPv4-Unterstützung
- Verwendung von Port 80
- Akzeptieren eingehender Client-Verbindungen
- Empfangen von HTTP-Anfragen
- Ausgabe der Anfrage in der Konsole
- Unterstützung von `GET /index.html`
- Auslesen und Übertragen der Datei `index.html`
- HTTP-Status `200 OK` bei erfolgreicher Anfrage
- HTTP-Status `404 Not Found` bei unbekannten Anfragen
- Schließen der Client-Verbindung nach der Antwort

## Voraussetzungen

Für die Ausführung werden benötigt:

- Windows-Betriebssystem
- C-Compiler mit Winsock2-Unterstützung
- Microsoft Visual C++ / Visual Studio
- Eine Datei namens `index.html`
- Berechtigung zur Verwendung von Port 80

### Verwendete Bibliotheken

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
```

Die Winsock2-Bibliothek wird über folgenden Compiler-Befehl eingebunden:

```c
#pragma comment(lib, "ws2_32.lib")
```

## Projektstruktur

```text
Projekt/
├── server.c
├── index.html
└── README.md
```

- `server.c` – Quellcode des Webservers
- `index.html` – HTML-Datei, die vom Server ausgeliefert wird
- `README.md` – Projektdokumentation

## Kompilieren

Bei Verwendung der **Visual Studio Developer Command Prompt** kann das Programm beispielsweise mit folgendem Befehl kompiliert werden:

```text
cl server.c
```

Anschließend kann der Server gestartet werden:

```text
server.exe
```

## Verwendung

Nach dem Start versucht der Server, Port 80 zu verwenden.

Bei erfolgreichem Start erscheint:

```text
Webserver laeuft auf Port 80...
```

Der Server wartet anschließend auf eingehende Verbindungen.

Im Browser kann anschließend folgende Adresse aufgerufen werden:

```text
http://localhost/index.html
```

Wenn die Datei `index.html` vorhanden ist, wird deren Inhalt an den Browser übertragen.

## HTTP-Antwort

Bei einer erfolgreichen Anfrage wird beispielsweise folgende HTTP-Antwort erzeugt:

```text
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: ...
Connection: close
```

Anschließend folgt der Inhalt der `index.html`.

Wenn die Datei nicht gefunden wird, wird eine `404 Not Found`-Antwort zurückgegeben.

## Ablauf des Servers

Der grundlegende Ablauf des Programms:

1. `WSAStartup()` wird aufgerufen.
2. Ein Socket wird erstellt.
3. Die Server-Adresse wird konfiguriert.
4. Der Socket wird mit `bind()` an Port 80 gebunden.
5. Mit `listen()` wartet der Server auf Verbindungen.
6. Mit `accept()` wird eine Client-Verbindung angenommen.
7. Die HTTP-Anfrage wird mit `recv()` empfangen.
8. Es wird geprüft, ob `GET /index.html` angefordert wurde.
   - **Ja:** `index.html` wird geöffnet und mit `200 OK` gesendet.
   - **Nein:** Eine `404 Not Found`-Antwort wird gesendet.
9. Die Verbindung zum Client wird mit `closesocket()` geschlossen.
10. Der Server wartet auf die nächste Client-Verbindung.

Der Server befindet sich in einer Endlosschleife und kann dadurch nacheinander mehrere Client-Anfragen bearbeiten.

## Wichtige Konstanten

### PORT

```c
#define PORT 80
```

Legt den TCP-Port fest, auf dem der Server auf Verbindungen wartet.

### BUFFER_SIZE

```c
#define BUFFER_SIZE 4096
```

Legt die Größe der verwendeten Puffer auf 4096 Bytes fest.

Dadurch ist die Größe der aktuell verarbeiteten HTTP-Anfrage sowie der geladenen `index.html` begrenzt.

## Einschränkungen

Der Webserver ist bewusst einfach gehalten und dient hauptsächlich zum Verständnis der grundlegenden Funktionsweise eines HTTP-Servers.

Aktuell bestehen unter anderem folgende Einschränkungen:

- Nur Windows wird unterstützt.
- Es wird ausschließlich `index.html` akzeptiert.
- Es werden nur einfache HTTP-GET-Anfragen verarbeitet.
- Die maximale Dateigröße ist durch `BUFFER_SIZE` begrenzt.
- Es werden keine parallelen Verbindungen verarbeitet.
- HTTPS wird nicht unterstützt.
- Es gibt keine Authentifizierung.
- Es gibt keine umfangreiche HTTP-Header-Verarbeitung.
- Fehler von `send()` und `fread()` werden nicht vollständig behandelt.
- Der Server verwendet Port 80, wofür je nach Windows-Konfiguration erhöhte Berechtigungen erforderlich sein können.

> **Wichtig:** Der Server sollte aufgrund seiner einfachen Implementierung nicht als produktiver oder öffentlich erreichbarer Webserver eingesetzt werden.

## Verwendete Technologien

- **C**
- **TCP/IP**
- **HTTP**
- **Windows Winsock2**
- **IPv4**
- **Microsoft Visual C++**

## `_WINSOCK_DEPRECATED_NO_WARNINGS`

Am Anfang des Programms befindet sich:

```c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
```

Damit werden Compiler-Warnungen bezüglich bestimmter als veraltet markierter Winsock-Funktionen unterdrückt.

Dies verändert nicht die Funktionsweise des Servers, sondern unterdrückt lediglich entsprechende Compiler-Warnungen.

## Ziel des Projekts

Das Projekt dient dazu, grundlegende Konzepte der Netzwerkprogrammierung und der Client-Server-Kommunikation praktisch umzusetzen.

Dabei werden unter anderem folgende Konzepte behandelt:

- Erstellen eines Netzwerk-Sockets
- Konfigurieren einer Server-Adresse
- Binden eines Sockets an einen Port
- Warten auf Verbindungen
- Akzeptieren von Clients
- Empfangen und Senden von Daten
- Verarbeitung von HTTP-Anfragen
- Lesen von Dateien
- Erstellen von HTTP-Antworten
- Fehlerbehandlung
- Schließen von Netzwerkverbindungen

## Lizenz

Dieses Projekt wurde zu Lern- und Demonstrationszwecken erstellt.
