from pathlib import Path

from fastapi import FastAPI, Request
from fastapi.responses import FileResponse, PlainTextResponse, Response
import uvicorn

app = FastAPI()

"""
Die Dateien werden relativ zum Speicherort des Servers gesucht.
"""
BASE_DIR = Path(__file__).resolve().parent

TELEFON_DATEI = BASE_DIR / "telefon.txt"
INDEX_DATEI = BASE_DIR / "index.html"

@app.exception_handler(404)
async def not_found(request: Request, exc):
    return PlainTextResponse(
        "Not Found",
        status_code=404,
        headers={
            "Content-Length": "9",
            "Connection": "close"
        }
    )

def telefonnummer_suchen(
    gesuchter_name: str
) -> str | None:
    """
    Sucht einen Namen in der Telefondatei und gibt die zugehörige
    Telefonnummer zurück.

    Die Datei verwendet ein Semikolon als Trennzeichen.
    """

    try:
        file = open(
            TELEFON_DATEI,
            "r",
            encoding="utf-8"
        )
    except OSError:
        print(
            "Fehler: telefon.txt konnte nicht geoeffnet werden."
        )
        return None

    with file:
        for zeile in file:

            zeile = zeile.rstrip("\r\n")

            if ";" not in zeile:
                continue

            name, nummer = zeile.split(";", 1)

            if name == gesuchter_name:
                return nummer

    return None


@app.get("/index.html")
def index():
    """
    Sendet die HTML-Datei.
    Wenn die Datei nicht gefunden wird, wird eine 404-Antwort
    mit einer Fehlermeldung zurückgegeben.
    """

    if not INDEX_DATEI.exists():

        response_body = "index.html nicht gefunden"

        return Response(
            content=response_body,
            status_code=404,
            media_type="text/plain",
            headers={
                "Content-Length": "23",
                "Connection": "close"
            }
        )

    return FileResponse(
        INDEX_DATEI,
        media_type="text/html",
        headers={
            "Connection": "close"
        }
    )


@app.get("/")
async def suche(request: Request):
    """
    Verarbeitet die Telefonsuche über den Query-Parameter
    'name'. Eine Anfrage ohne Parameter ist keine gültige
    Suchanfrage und wird mit 404 beantwortet.
    """

    query = request.url.query

    if not query:
        response_body = "Not Found"

        return Response(
            content=response_body,
            status_code=404,
            media_type="text/plain",
            headers={
                "Content-Length": "9",
                "Connection": "close"
            }
        )

    if "=" not in query:
        response_body = "Not Found"

        return Response(
            content=response_body,
            status_code=404,
            media_type="text/plain",
            headers={
                "Content-Length": "9",
                "Connection": "close"
            }
        )

    parameter_name, parameter_wert = query.split(
        "=",
        1
    )

    if parameter_name == "name":

        telefon = telefonnummer_suchen(
            parameter_wert
        )

        if telefon is not None:

            return Response(
                content=telefon,
                status_code=200,
                media_type="text/plain",
                headers={
                    "Content-Length": str(
                        len(telefon)
                    ),
                    "Connection": "close"
                }
            )

        response_body = "Name nicht gefunden"

        return Response(
            content=response_body,
            status_code=404,
            media_type="text/plain",
            headers={
                "Content-Length": str(
                    len(response_body)
                ),
                "Connection": "close"
            }
        )

    response_body = "Unbekannter Parameter"

    return Response(
        content=response_body,
        status_code=400,
        media_type="text/plain",
        headers={
            "Content-Length": str(
                len(response_body)
            ),
            "Connection": "close"
        }
    )


if __name__ == "__main__":

    print(
        "Lokaler Webserver gestartet:"
    )

    print(
        "http://localhost:8000"
    )

    uvicorn.run(
        app,
        host="127.0.0.1",
        port=8000
    )
