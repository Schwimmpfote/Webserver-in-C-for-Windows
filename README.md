# Webserver für Windows in C

Einfacher HTTP-Webserver in C für Windows mithilfe der **Windows Winsock2 API**.

## Beschreibung

Dieses Projekt implementiert einen einfachen HTTP-Webserver in der Programmiersprache **C** mithilfe der **Windows Winsock2 API**.

Der Webserver verwendet TCP über IPv4 und wartet dauerhaft auf eingehende Client-Verbindungen auf **Port 80**.

Der Server unterstützt aktuell zwei grundlegende Funktionen:

- Ausliefern der Datei `index.html`
- Suchen einer Telefonnummer anhand eines Namens über einen HTTP-Parameter

Die Telefonnummern werden aus der Datei `telefon.txt` gelesen.

> **Hinweis:** Der Server ist speziell für Windows ausgelegt und verwendet die Windows-spezifische Winsock2-API. Eine direkte Verwendung unter Linux oder macOS ist mit diesem Code nicht möglich.

## Funktionen

Der aktuelle Server bietet folgende Funktionen:

- TCP-Socket über Winsock2
- IPv4-Unterstützung
- Verwendung von Port 80
- Annahme eingehender Client-Verbindungen
- Empfangen von HTTP-Anfragen
- Ausgabe der empfangenen Anfrage in der Konsole
- Auslieferung von `index.html`
- HTTP-Status `200 OK` bei erfolgreicher Auslieferung
- HTTP-Status `404 Not Found` bei nicht gefundenen Ressourcen
- HTTP-Status `400 Bad Request` bei unbekannten Parametern
- Suchen von Telefonnummern anhand eines Namens
- Lesen der Telefonnummern aus `telefon.txt`
- Schließen der Client-Verbindung nach der Antwort
- Verarbeitung der Client-Anfragen nacheinander in einer Endlosschleife

## Voraussetzungen

Für die Ausführung werden benötigt:

- Windows-Betriebssystem
- C-Compiler mit Winsock2-Unterstützung
- Microsoft Visual C++ / Visual Studio
- Eine Datei `index.html`
- Eine Datei `telefon.txt`
- Berechtigung zur Verwendung von Port 80

### Verwendete Bibliotheken

Der Quellcode verwendet:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
```

Die Winsock2-Bibliothek wird über folgende Compiler-Anweisung eingebunden:

```c
#pragma comment(lib, "ws2_32.lib")
```

Zusätzlich befindet sich am Anfang des Programms:

```c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
```

Damit werden bestimmte Warnungen des Microsoft-C-Compilers bezüglich veralteter Winsock-Funktionen unterdrückt.

## Projektstruktur

```text
Projekt/
├── server.c
├── index.html
├── telefon.txt
└── README.md
```

### Dateien

- `server.c` – Quellcode des Webservers
- `index.html` – HTML-Datei, die über den Server ausgeliefert werden kann
- `telefon.txt` – Datei mit Namen und Telefonnummern
- `README.md` – Projektdokumentation

## Datei `telefon.txt`

Die Datei `telefon.txt` enthält die Namen und Telefonnummern.

Als Trennzeichen zwischen Name und Telefonnummer wird ein Semikolon (`;`) verwendet.

Beispiel:

```text
Max Mustermann;0123456789
Erika Musterfrau;0987654321
Peter Beispiel;01555123456
```

Der Server liest die Datei bei jeder Suchanfrage erneut ein und sucht nach einer Zeile, deren Name exakt mit dem übergebenen Suchbegriff übereinstimmt.

Die Suche ist **case-sensitive**.

Beispielsweise wird:

```text
Max
```

nicht automatisch als gleichwertig zu:

```text
max
```

behandelt.

## Kompilieren

Bei Verwendung der **Visual Studio Developer Command Prompt** kann das Programm beispielsweise mit folgendem Befehl kompiliert werden:

```text
cl server.c
```
Sollte dies nicht installiert sein, geht alternativ auch gcc:

```text
gcc server.c -o server.exe -lws2_32
```

Durch

```c
#pragma comment(lib, "ws2_32.lib")
```

wird die benötigte Winsock2-Bibliothek automatisch eingebunden.

Anschließend kann der Server gestartet werden:

```text
.\server.exe
```

## Starten des Servers

Nach dem Start versucht der Server, den TCP-Port `80` zu verwenden.

Bei erfolgreichem Start wird folgende Meldung ausgegeben:

```text
Webserver laeuft auf Port 80...
```

Der Server wartet anschließend dauerhaft auf Client-Verbindungen.

Da der Server an `INADDR_ANY` gebunden wird, akzeptiert er Verbindungen über die verfügbaren IPv4-Netzwerkschnittstellen des Rechners und nicht ausschließlich über `localhost`.

## Auslieferung von `index.html`

Wird folgende URL aufgerufen:

```text
http://localhost/index.html
```

versucht der Server, die Datei `index.html` im aktuellen Arbeitsverzeichnis zu öffnen.

Wenn die Datei gefunden wird, wird ihr Inhalt mit einer HTTP-Antwort mit dem Status:

```text
HTTP/1.1 200 OK
```

zurückgegeben.

Beispielsweise enthält die Antwort:

```text
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: ...
Connection: close
```

Danach folgt der Inhalt der HTML-Datei.

### Datei nicht vorhanden

Wenn `index.html` nicht geöffnet werden kann, sendet der Server eine:

```text
HTTP/1.1 404 Not Found
```

Antwort.

## Telefonnummernsuche

Neben der Auslieferung von `index.html` unterstützt der Server eine Suche nach Telefonnummern über einen URL-Parameter.

Die Suchanfrage verwendet den Parameter:

```text
name
```

Beispielsweise:

```text
http://localhost/?name=Max
```

Der Server sucht anschließend in `telefon.txt` nach einem passenden Eintrag.

Beispielsweise:

```text
Max;0123456789
```

Wenn der Name gefunden wird, wird die Telefonnummer als Text zurückgegeben.

Beispielantwort:

```text
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 10
Connection: close

0123456789
```

## Verhalten bei der Namenssuche

Wird der Name gefunden, antwortet der Server mit:

```text
HTTP/1.1 200 OK
```

Wird der Name nicht gefunden, antwortet der Server mit:

```text
HTTP/1.1 404 Not Found
```

und dem Text:

```text
Name nicht gefunden
```

Wenn `telefon.txt` nicht geöffnet werden kann, gibt der Server zusätzlich eine Fehlermeldung in der Konsole aus:

```text
Fehler: telefon.txt konnte nicht geoeffnet werden.
```

## HTTP-Parameter

Der Server erwartet bei einer Suchanfrage den Parameter:

```text
name
```

Beispiel:

```text
/?name=Max
```

Intern wird die Anfrage in ihre Bestandteile aufgeteilt:

```text
Parametername: name
Parameterwert: Max
```

Wenn ein anderer Parameter verwendet wird, beispielsweise:

```text
/?telefon=Max
```

antwortet der Server mit:

```text
HTTP/1.1 400 Bad Request
```

und:

```text
Unbekannter Parameter
```

## URL-Verarbeitung

Der Server verarbeitet die URL vereinfacht direkt aus dem empfangenen HTTP-Request.

Zunächst wird davon ausgegangen, dass die Anfrage mit:

```text
GET 
```

beginnt.

Anschließend wird die URL bis zum nächsten Leerzeichen extrahiert.

Danach wird geprüft, ob die URL ein Fragezeichen (`?`) enthält.

Beispiel:

```text
/index.html
```

enthält keinen Parameter.

Dagegen:

```text
/?name=Max
```

enthält einen Query-Parameter.

Der aktuelle Code führt dabei keine vollständige HTTP-Parser-Implementierung durch.

## Ablauf des Servers

Der grundlegende Ablauf des Programms ist:

1. `WSAStartup()` initialisiert Winsock.
2. Mit `socket()` wird ein TCP-Socket erstellt.
3. Die Serveradresse wird konfiguriert.
4. Der Socket wird mit `bind()` an Port 80 gebunden.
5. Mit `listen()` wird auf eingehende Verbindungen gewartet.
6. Mit `accept()` wird eine Client-Verbindung angenommen.
7. Mit `recv()` wird die HTTP-Anfrage empfangen.
8. Die Anfrage wird in der Konsole ausgegeben.
9. Die angeforderte URL wird aus der Anfrage extrahiert.
10. Es wird geprüft, ob die URL einen Query-Parameter enthält.
11. Bei `/index.html` wird die Datei `index.html` geöffnet und übertragen.
12. Bei einem `name`-Parameter wird `telefon.txt` durchsucht.
13. Je nach Ergebnis wird eine HTTP-Antwort mit `200`, `404` oder `400` gesendet.
14. Die Client-Verbindung wird mit `closesocket()` geschlossen.
15. Der Server wartet auf die nächste Verbindung.

Der Server läuft dabei dauerhaft in einer Endlosschleife.

## Wichtige Konstanten

### `PORT`

```c
#define PORT 80
```

Legt den TCP-Port fest, auf dem der Server auf eingehende Verbindungen wartet.

Port 80 ist der Standardport für HTTP.

### `BUFFER_SIZE`

```c
#define BUFFER_SIZE 4096
```

Legt die Größe verschiedener verwendeter Puffer auf 4096 Bytes fest.

Dadurch ergeben sich unter anderem folgende Begrenzungen:

- HTTP-Anfragen können nur begrenzt empfangen werden.
- Der Inhalt von `index.html` wird nur bis zur Größe des verwendeten Puffers eingelesen.
- Eine einzelne Zeile aus `telefon.txt` ist ebenfalls auf die Puffergröße begrenzt.

## HTTP-Statuscodes

Der Server verwendet aktuell folgende HTTP-Statuscodes:

### `200 OK`

Wird verwendet, wenn:

- `index.html` erfolgreich gelesen wurde oder
- ein gesuchter Name in `telefon.txt` gefunden wurde.

### `404 Not Found`

Wird verwendet, wenn:

- `index.html` nicht gefunden werden kann,
- eine unbekannte URL ohne Suchparameter angefordert wird oder
- der gesuchte Name nicht in `telefon.txt` vorhanden ist.

### `400 Bad Request`

Wird verwendet, wenn ein unbekannter Parameter angegeben wird.

Beispielsweise:

```text
/?telefon=Max
```

führt zu:

```text
HTTP/1.1 400 Bad Request
```

## Einschränkungen

Der Webserver ist bewusst einfach gehalten und dient hauptsächlich zum Verständnis der grundlegenden Funktionsweise von TCP, HTTP und Client-Server-Kommunikation.

Aktuell bestehen unter anderem folgende Einschränkungen:

- Nur Windows wird unterstützt.
- IPv4 wird verwendet.
- Es wird Port 80 verwendet.
- Es werden keine parallelen Client-Verbindungen verarbeitet.
- Der Server bearbeitet Verbindungen nacheinander.
- HTTPS wird nicht unterstützt.
- Es gibt keine Authentifizierung.
- Es findet keine vollständige HTTP-Header-Verarbeitung statt.
- Die HTTP-Methode wird nicht vollständig überprüft.
- Es wird keine vollständige HTTP-Syntaxprüfung durchgeführt.
- URL-Encoding wird nicht dekodiert.
- Es werden keine komplexen Query-Parameter unterstützt.
- Die Namenssuche ist case-sensitive.
- Die Datei `telefon.txt` wird für jede Suchanfrage erneut geöffnet und vollständig durchsucht.
- Die Größe der verarbeiteten HTTP-Anfrage ist durch `BUFFER_SIZE` begrenzt.
- Der Inhalt von `index.html` ist durch den verwendeten Puffer begrenzt.
- `index.html` wird über `%s` als Text verarbeitet und ist daher nicht für beliebige Binärdateien geeignet.
- Fehler von `send()` und `fread()` werden nicht vollständig behandelt.
- Es wird keine URL-Decodierung durchgeführt.
- Der Server bindet an `INADDR_ANY` und kann dadurch über andere IPv4-Netzwerkschnittstellen erreichbar sein.
- Es gibt keine Protokollierung in eine Datei.

> **Wichtig:** Aufgrund der einfachen Implementierung sollte der Server nicht als produktiver oder öffentlich erreichbarer Webserver eingesetzt werden.

## Fehlerbehandlung

Der Code überprüft wichtige Fehler beim Start des Servers.

Beispielsweise werden Fehler bei folgenden Funktionen behandelt:

```c
WSAStartup()
socket()
bind()
listen()
accept()
```

Bei einem Fehler beim Start wird der Socket geschlossen und Winsock mit:

```c
WSACleanup();
```

bereinigt.

Bei einer ungültigen Client-Verbindung wird die entsprechende Verbindung geschlossen und der Server wartet anschließend auf die nächste Verbindung.

## Netzwerkaufbau

Der Server verwendet:

```c
AF_INET
```

für IPv4 und:

```c
SOCK_STREAM
```

für eine TCP-Verbindung.

Das verwendete Protokoll ist:

```c
IPPROTO_TCP
```

Die Serveradresse wird mit:

```c
INADDR_ANY
```

konfiguriert.

Dadurch lauscht der Server auf allen verfügbaren IPv4-Netzwerkschnittstellen des Rechners.

## Verwendete Technologien

- **C**
- **TCP/IP**
- **HTTP**
- **Windows Winsock2**
- **IPv4**
- **Microsoft Visual C++**
- **Dateiverarbeitung mit der C-Standardbibliothek**

## Ziel des Projekts

Das Projekt dient dazu, grundlegende Konzepte der Netzwerkprogrammierung und der Client-Server-Kommunikation praktisch umzusetzen.

Dabei werden unter anderem folgende Konzepte behandelt:

- Erstellen eines TCP-Sockets
- Initialisieren von Winsock
- Konfigurieren einer Serveradresse
- Binden eines Sockets an einen Port
- Warten auf eingehende Verbindungen
- Akzeptieren von Client-Verbindungen
- Empfangen von Netzwerkdaten
- Verarbeiten einfacher HTTP-Anfragen
- Erstellen von HTTP-Antworten
- Senden von Daten an einen Client
- Lesen von Dateien
- Suchen in einer Textdatei
- Verarbeitung von URL-Parametern
- Verwendung von HTTP-Statuscodes
- Schließen von Netzwerkverbindungen
- Grundlegende Fehlerbehandlung

## Sicherheitshinweis

Der Server wurde zu Lern- und Demonstrationszwecken entwickelt.

Es fehlen zahlreiche Funktionen, die für einen produktiven Webserver notwendig wären, beispielsweise:

- robuste HTTP-Parsing-Logik
- vollständige Validierung von Anfragen
- URL-Decodierung
- Behandlung großer Dateien
- parallele Verbindungen
- vollständige Fehlerbehandlung
- Sicherheitsprüfungen
- HTTPS/TLS
- Schutz vor manipulierten Anfragen
- Logging und Monitoring

Der Server sollte daher nur in einer kontrollierten Entwicklungs- oder Testumgebung eingesetzt werden.

## Lizenz

Dieses Projekt wurde zu Lern- und Demonstrationszwecken erstellt.
