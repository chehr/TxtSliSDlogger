# TxtSliSDlogger
fischtertechnik ROBOPro TXT Data logger to SD-Card

Source code um Daten von ROBOPro auf die TXT SD-Karte zu loggen bzw speichern.

Die Library "libTxtSliSDlogger.so" muß auf den TXT in folgendes Verzeichnis kopiert werden: 
/opt/knobloch/libs

ROBOPro muss Zugriffsrechte auf die SD-Karte bekommen via "root" auf dem TXT und folgendem Befehl: 
chown -cR ROBOPro:ROBOPro /opt/knobloch/ExternalSD/


Dann das Programm ROBOPro öffnen, anbei ein Beispiel:
https://github.com/chehr/TxtSliSDlogger/blob/master/support/RoboPro/TxtSliSDlogger.rpp
