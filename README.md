# Webserver für Windows in C und Python

Dieses Projekt enthält zwei einfache HTTP-Webserver:

- einen **C-Webserver mit Windows Winsock2**
- einen **Python-Webserver mit FastAPI und Uvicorn**

Beide Server können `index.html` ausliefern und Telefonnummern aus
`telefon.txt` anhand eines Namens suchen.

## 1. Vergleich

| Eigenschaft | C | Python |
|---|---|---|
| Programmiersprache | C | Python |
| Netzwerkbibliothek | Winsock2 | FastAPI / Uvicorn |
| Protokoll | HTTP über TCP | HTTP über TCP |
| IP-Version | IPv4 | IPv4 |
| Host | `INADDR_ANY` | `127.0.0.1` |
| Port | `80` | `8000` |
| Parallelität | sequenziell | durch Uvicorn verwaltet |
| HTTP-Parsing | manuell | FastAPI/Uvicorn |
| HTML-Auslieferung | `send()` | `FileResponse` |
| Telefonsuche | C-Dateiverarbeitung | Python-Dateiverarbeitung |
| URL-Decodierung | nein | nicht manuell |
| HTTPS | nein | nein |

---

## 2. Funktionen

| Funktion | C | Python |
|---|---|---|
| `index.html` ausliefern | ✓ | ✓ |
| Telefonnummer suchen | ✓ | ✓ |
| `telefon.txt` lesen | ✓ | ✓ |
| Parameter `name` | ✓ | ✓ |
| `200 OK` | ✓ | ✓ |
| `404 Not Found` | ✓ | ✓ |
| `400 Bad Request` | ✓ | ✓ |
| IPv4 | ✓ | ✓ |
| HTTP über TCP | ✓ | ✓ |

---

## 3. Voraussetzungen

### C

- Windows
- C-Compiler, z. B. Visual Studio oder GCC
- Winsock2
- `index.html`
- `telefon.txt`

### Python

- Windows
- Python
- FastAPI
- Uvicorn
- `index.html`
- `telefon.txt`

Installation:

```text
pip install fastapi uvicorn
```

Alternativ über `requirements.txt`:

```text
fastapi
uvicorn
```

Installation:

```text
pip install -r requirements.txt
```

---

## 4. Projektstruktur

```text
Projekt/
├── server.c
├── server.py
├── index.html
├── telefon.txt
└── README.md
```

| Datei | Funktion |
|---|---|
| `server.c` | C-Webserver |
| `server.py` | Python-Webserver |
| `index.html` | HTML-Datei |
| `telefon.txt` | Namen und Telefonnummern |
| `README.md` | Dokumentation |

---

## 5. Telefondatei

Beide Server verwenden dasselbe Format:

```text
Max Mustermann;0123456789
Erika Musterfrau;0987654321
Peter Beispiel;01555123456
```

Das Semikolon trennt Name und Telefonnummer.

Die Suche ist **exakt und case-sensitive**.

---

## 6. Netzwerk

| Schritt | C | Python |
|---|---|---|
| Socket | `socket()` | Uvicorn |
| Bindung | `bind()` | `host` / `port` |
| Warten | `listen()` | Uvicorn |
| Verbindung | `accept()` | Uvicorn |
| Empfangen | `recv()` | FastAPI/Uvicorn |
| Senden | `send()` | `Response` |
| Schließen | `closesocket()` | Server übernimmt |

### C

Der C-Server verwendet:

```c
#define PORT 80
#define BUFFER_SIZE 4096
```

Für Winsock2:

```c
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
```

Der Server bindet an:

```text
INADDR_ANY:80
```

Dadurch kann er grundsätzlich über die verfügbaren IPv4-Schnittstellen erreichbar sein.

### Python

Der Python-Server verwendet:

```python
uvicorn.run(
    app,
    host="127.0.0.1",
    port=8000
)
```

Dadurch ist er standardmäßig nur lokal erreichbar.

---

## 7. Starten

### C

Kompilieren mit Visual Studio:

```text
cl server.c
```

Alternativ mit GCC:

```text
gcc server.c -o server.exe -lws2_32
```

Start:

```text
.\server.exe
```

Der Server verwendet Port `80`.

### Python

Start über Python:

```text
python server.py
```

oder direkt über Uvicorn:

```text
uvicorn server:app --host 127.0.0.1 --port 8000
```

Der Server verwendet Port `8000`.

---

## 8. URLs

| Funktion | C | Python |
|---|---|---|
| Startseite | `http://localhost/` | `http://localhost:8000/` |
| HTML | `http://localhost/index.html` | `http://localhost:8000/index.html` |
| Suche | `http://localhost/?name=Max` | `http://localhost:8000/?name=Max` |

Der einzige grundlegende Unterschied bei den URLs ist der verwendete Port:

```text
C       → Port 80
Python  → Port 8000
```

---

## 9. Auslieferung von `index.html`

| C | Python |
|---|---|
| Datei mit `fopen()` öffnen | `FileResponse` |
| Datei mit `fread()` lesen | FastAPI/Starlette |
| HTTP-Header selbst erzeugen | Framework |
| Antwort mit `send()` senden | Response zurückgeben |

Python:

```python
@app.get("/index.html")
def index():
    if not INDEX_DATEI.exists():
        return Response(
            content="index.html nicht gefunden",
            status_code=404,
            media_type="text/plain"
        )

    return FileResponse(
        INDEX_DATEI,
        media_type="text/html"
    )
```

---

## 10. Telefonsuche

Eine Suchanfrage sieht bei beiden Servern so aus:

```text
/?name=Max
```

Bei folgender Zeile:

```text
Max;0123456789
```

wird zurückgegeben:

```text
0123456789
```

### Umsetzung

| C | Python |
|---|---|
| HTTP-Anfrage selbst analysieren | FastAPI/Uvicorn verarbeitet HTTP |
| Parameter selbst extrahieren | `request.url.query` |
| `fopen()` | `open()` |
| `fgets()` | `for zeile in file` |
| C-Stringvergleich | Python-Stringvergleich |
| `send()` | `Response` |

Die grundlegende Logik ist identisch:

```text
Name
 ↓
telefon.txt
 ↓
Name gefunden?
 ↓
Telefonnummer
```

---

## 11. HTTP-Statuscodes

| Situation | C | Python |
|---|---|---|
| HTML erfolgreich | `200 OK` | `200 OK` |
| Name gefunden | `200 OK` | `200 OK` |
| Datei nicht gefunden | `404 Not Found` | `404 Not Found` |
| Name nicht gefunden | `404 Not Found` | `404 Not Found` |
| Keine Suchanfrage | `404 Not Found` | `404 Not Found` |
| Unbekannter Parameter | `400 Bad Request` | `400 Bad Request` |

Beispiel für einen unbekannten Namen:

```text
GET /?name=Unbekannt
```

Antwort:

```text
404 Not Found

Name nicht gefunden
```

Unbekannter Parameter:

```text
GET /?telefon=Max
```

Antwort:

```text
400 Bad Request

Unbekannter Parameter
```

---

## 12. Dateipfade

Python verwendet:

```python
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent

TELEFON_DATEI = BASE_DIR / "telefon.txt"
INDEX_DATEI = BASE_DIR / "index.html"
```

Dadurch werden die Dateien relativ zum Speicherort von `server.py` gefunden.

| | C | Python |
|---|---|---|
| `index.html` | `index.html` | `BASE_DIR / "index.html"` |
| `telefon.txt` | `telefon.txt` | `BASE_DIR / "telefon.txt"` |
| Bezugspunkt | Arbeitsverzeichnis | Speicherort von `server.py` |

---

## 13. HTTP-Verarbeitung

### C

Der C-Server erhält die rohe HTTP-Anfrage:

```text
GET /?name=Max HTTP/1.1
Host: localhost
...
```

Diese wird selbst verarbeitet:

```text
recv()
 ↓
HTTP-Text
 ↓
URL analysieren
 ↓
Parameter auswerten
 ↓
Antwort erzeugen
 ↓
send()
```

### Python

FastAPI und Uvicorn übernehmen die grundlegende HTTP-Verarbeitung:

```text
HTTP-Anfrage
 ↓
Uvicorn
 ↓
FastAPI
 ↓
Route
 ↓
Python-Code
 ↓
Response
```

Dadurch ist der Python-Code deutlich kürzer.

---

## 14. Parallelität

### C

Der Server verarbeitet Verbindungen nacheinander:

```text
Client 1
   ↓
Verarbeitung
   ↓
Antwort
   ↓
Client 2
   ↓
Verarbeitung
```

### Python

Uvicorn/FastAPI übernimmt die Verwaltung der HTTP-Verbindungen.

Die Telefonsuche selbst bleibt jedoch eine normale Dateisuche:

```python
def telefonnummer_suchen(gesuchter_name):
    ...
```

---

## 15. Unterschiede bei der Implementierung

| Bereich | C | Python |
|---|---|---|
| Netzwerk | Low-Level | Framework |
| Socket-Verwaltung | selbst | Uvicorn |
| HTTP-Parsing | selbst | FastAPI/Uvicorn |
| Routing | selbst | FastAPI |
| Dateizugriff | C-Standardbibliothek | Python |
| Antworten | `send()` | `Response` |
| Fehlerbehandlung | größtenteils selbst | teilweise Framework |
| Codeumfang | größer | kleiner |
| Kontrolle | sehr hoch | höheres Abstraktionsniveau |
| Lernschwerpunkt | TCP/HTTP/Winsock | Webframework/API |

---

## 16. Einschränkungen

| C | Python |
|---|---|
| Windows-spezifisch | Python/FastAPI |
| einfache HTTP-Verarbeitung | vereinfachte Query-Verarbeitung |
| keine HTTPS-Unterstützung | keine HTTPS-Unterstützung |
| sequenzielle Verarbeitung | Server verwaltet mehrere Verbindungen |
| keine Authentifizierung | keine Authentifizierung |
| keine Datenbank | keine Datenbank |
| keine URL-Decodierung | keine eigene URL-Decodierung |
| begrenzter Puffer | keine eigene `BUFFER_SIZE` |
| einfache Fehlerbehandlung | Framework übernimmt Teile davon |
| nicht für Produktion | nicht für Produktion |

---

## 17. Bekannte Besonderheit im Python-Code

Bei einer fehlenden `index.html` sollte `Content-Length` nicht fest eingetragen werden.

Ungünstig:

```python
"Content-Length": "23"
```

Besser:

```python
"Content-Length": str(
    len(response_body.encode("utf-8"))
)
```

So wird die tatsächliche Byte-Länge des UTF-8-Inhalts verwendet.

---

## 18. Ablauf beider Server

| Schritt | C | Python |
|---|---|---|
| 1 | Winsock initialisieren | FastAPI erstellen |
| 2 | Socket erstellen | Uvicorn starten |
| 3 | `bind()` | auf `127.0.0.1:8000` lauschen |
| 4 | `listen()` | Anfrage empfangen |
| 5 | `accept()` | Route bestimmen |
| 6 | `recv()` | Request verarbeiten |
| 7 | HTTP selbst analysieren | FastAPI verarbeitet HTTP |
| 8 | Datei suchen | Datei suchen |
| 9 | `send()` | Response |
| 10 | `closesocket()` | Verbindung verwalten |

---

## 19. Fazit

Beide Server erfüllen grundsätzlich dieselben Aufgaben, unterscheiden sich aber beim Abstraktionsgrad.

Der **C-Server** arbeitet direkt mit Winsock2. Dadurch werden TCP-Sockets, HTTP-Anfragen und HTTP-Antworten weitgehend selbst verarbeitet. Er eignet sich besonders zum Verständnis der grundlegenden Netzwerkprogrammierung.

Der **Python-Server** verwendet FastAPI und Uvicorn. Netzwerkkommunikation, HTTP-Parsing und Routing werden weitgehend von den Bibliotheken übernommen. Dadurch ist die Implementierung kürzer und übersichtlicher.

| C | Python |
|---|---|
| Low-Level | High-Level |
| mehr eigener Code | weniger eigener Code |
| mehr Kontrolle | mehr Abstraktion |
| Winsock2 direkt | FastAPI/Uvicorn |
| gut zum Lernen von TCP/HTTP | gut zum Lernen moderner Webentwicklung |

Beide Implementierungen sind als Lern- und Demonstrationsprojekte gedacht und sollten nicht unverändert als öffentlich erreichbare Produktionsserver eingesetzt werden.
