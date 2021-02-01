# TxtSliSDlogger
##fischtertechnik ROBOPro TXT Data logger to SD-Card

Quellcode um Daten von ROBOPro auf die TXT SD-Karte zu loggen.


Installation der Bibliothek

Am TXT müssen folgende Einstellungen aktiviert werden:
1. Einstellungen / WLAN 
2. Sicherheit / BootSD, SSH Deamon und WEB-Server müssen eingeschaltet sein

Die Bibliothek "libTxtSliSDlogger.so" muß auf dem TXT in folgendes Verzeichnis kopiert werden:
```
/opt/knobloch/libs
```
Die Bibliothek kann entweder via FTP, z.b. "FileZilla" oder via [Web Browser](/support/Bibliothek.png) erfolgen.



ROBOPro muss Zugriffsrechte auf die SD-Karte bekommen via "root" auf dem TXT und folgendem Befehl: 
```
chown -cR ROBOPro:ROBOPro /opt/knobloch/ExternalSD/
```

Dann das Programm lokal speichern und in ROBOPro öffnen, hier der Link:
[ROBOPro Programm Daten auf SD-Karte loggen](/support/RoboPro/)
mit rechter Maustaste "Ziel speichern unter.."
