# Testprotokoll für beide Webserver
Um die Server zu testen sollten beide Webserver parallel laufen, wie in dem ReadMe beschrieben. Dann kann man beide entweder per `localhost` oder `localhost:8000` auf den C oder python Server zugreifen. Bei den Testprotokollen stehen zu dem derzeitigen Zeitpunkt nur die Adressen von dem C Server drin. Es müsste entsprechend um die Angabe des Port 8000 ergänzt werden.
Das ganze kann sowohl im Browser, als auch mit curl getestet werden.
Beide Server wurden mit bestimmten Testcases konfrontiert und wiesen zum Teil unterschiedliches Verhalten auf:

| Testcase | erwartet | Python | C |
|---|---|---|---|
|`http://localhost/index.html`|Anzeige der index.html Seite(Standard Hello World)|✓|✓|
|`http://localhost/index`|Not Found|✓|✓|
|`http://localhost/`|Not Found|✓|✓|
|`http://localhost/?name=Arbeit`|Antwort mit der Telefonnummer des Namens Arbeit|✓|✓|
|`http://localhost/?name=Test`|Sollte nicht funktionieren und `Name nicht gefunden` anzeigen|✓|✓|
|`http://localhost/?name=`|`Name nicht gefunden`|✓|✓|
|`http://localhost/?age=3`|`unbekannter Parameter` Nachricht|✓|✓|
|`http://localhost/?name=Arbeit&age=3`|`unbekannter Parameter` Nachricht|zeigt `Name nicht gefunden` an|zeigt `Name nicht gefunden` an|
|`http://localhost/index.html?name=Arbeit`|Anzeigen der html Seite index.html|✓|zeigt stattdessen die Telefonnummer an|

Entsprechend sollte der C Server in Bezug auf das handeling von index.html verbessert werden und beide Server das handeling von Parametern, die mit & verknüpft sind überarbeiten.
Wenn man bei dem Testen in dem Browser, auf Untersuchen geht, dann auf Netzwerkanalyse den Cache deaktiviert und dann reloaded, stellt man bei der Parameter Verarbeitung zudem fest, dass der C Server doppelt so lange braucht, wie der python Server, dabei braucht python in etwa 150 - 200ms und C 300 - 350 ms.