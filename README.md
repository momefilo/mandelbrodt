**Fraktalgenerator nach Mandelbrodt**

Edit: Das PDF hat mehr Anhang\
Das Programm enthält eine, hier nicht weiter dokumentierte
Serienaufnahme-Funktion die alle Apfelmännchen mit einem Depth-Wert bei 1 beginnend, bis zu im Programm
bereits berechneten Depth-Wert und der anderer Parameter berechnet und deren BMP-Bilddateien im Ordner „bmp"
speichert. Der Button „Seq" startet die recht zeitaufwendige Prozedur.
Doch danach können mittels „ffmpeg" interessante Videos der Änderung der Mandelbrodtmenge über die
Iterationstiefe erstellt werden.

**Wichtig:** Zum Start der Anwendung muss das „start.sh"-Script mit
sudo-rechten aufgerufen werden

![](vertopal_7043454174a54f35bb26a94c2224a585/media/image1.png)
Fraktale nach Mandelbrodt sind Selbstähnliche Strukturen welche mit
diesem Programm, in Abhängigkeit der Iterationstiefe gefärbt und als
BMP-Bilddatei gespeichert werden können. Zur Bedienung des Programms
werden Begriffe gebraucht die im Folgendem erörtert sind.

![](vertopal_7043454174a54f35bb26a94c2224a585/media/image2.png)

![](vertopal_7043454174a54f35bb26a94c2224a585/media/image3.png)
Bild oben: **Wichtig** Die Zahlenwerte
haben eine Überschrift und können mit je vier Buttons darüber in
Schritten erhöht, und vier Buttons darunter gesenkt werden. Änderungen
müssen mit Klick auf „Calc" neu berechnet/abgebildet werden.

**Zoomen**\
Wir „zoomen in das Apfelmännchen in dem wir den kleinen magentafrbenen
„Maus-Quader" des Programms, durch Drücken und Halten der linken
Maustaste den Startpunkt und durch bewegen der Maus zum Endpunkt, diese
Punkte mit einem zusätzlichen rechten Mausklick festlegen. Das Programm
beginnt unmittelbar mit der Neuberechnung\

**X-Res und Y-Res**\
Diese Parameter legen die Auflösung des zu berechnenden Apfelmännchen
fest in der es auch als Bild gespeichert wird, aber nicht unbedingt die
im Programm dargestellte. Das Programm skaliert das Apfelmännchen so das
es vollständig zu sehen ist. Beim Start des Programms sind diese immer
gleich der Monitorauflösung. Um eine eventuell gewünschte mathematisch
genaue Abbildung des Apfelmännchens zu erreichen ist hier ein 3 zu 2
Format zu wählen.

**r-Min/Max und i-Min/Max**\
Diese Parameter sind zur Bedienung des Programms nicht nötig und
überspringbar\
Die Abbildungen der Apfelmännchen-Struktur errechnen sich durch
Zahlenpaare zwischen den r/i-min/max Werten welche den X/Y-Achse des
Apfelmännchen zugeordnet werden. Der Symmetrie des Apfelmännchen und des
Algorithmus wegen, geben die Startwerte dieser Parameter nur Sinn wenn r
zwischen einschließlich -1 und +2 sowie i zwischen -1 und +1 liegen.
Wenn wir diese Werte ändern verschieben wir tatsächlich den Ursprung der
Koordinaten innerhalb des Apfelmännchen, und ändern diese Parameter
besser durch das Zoomen. Wer experimentieren will kann die Asymmetrie
durch ändern der i/r-min/max Werte von Hand, am besten direkt nach dem
Start des Programms berechnen und abbilden lassen

**Depth**\
Das im oberen Bild zu sehende SW-Apfelmännchen entsteht durch
Grauabstufung in Abhängigkeit davon, wie oft ein Algorithmus auf den
Bildpunkt angewendet werden muss bis dessen Ergebnis eine
Abbruchbedingung erfüllt. Diese Anzahl an Wiederholungen bezeichnen wir
hier als Iterationstiefe. Der Algorithmus ist im Anhang detailliert
beschrieben und hier Überflüssiges, denn zum Verständnis genügt es zu
wissen das jedem Bildpunkt eine Iterationstiefe zugeordnet ist, welche
die Farbe oder Intensität des Bildpunktes bestimmt. Den Depth-Wert den
wir im Programm vorgeben, legt den Maximalwert an
Iterationen/Wiederholungen des Algorithmus fest der über die Bildpunkte
ausgeführt wird, und die Farbe der Bildpunkte legen wir anschließend
anhand deren Iterationstiefen fest. Erhöhen wir den Startwert Depth von
100 auf 500 und klicken auf „Calc", haben wir in weniger als einer
Sekunde einen Eindruck der graphischen Wirkung in Schwarzweiß.

**Farbgestaltung**\
Um die Farbgestaltung zu verstehen, stellen wir uns die Iterationstiefen
als Zahlenstrahl von Null bis zu unserem vorgegeben Depth-Wert mit
Lücken vor, dem wir die Menge aller Bildpunkte zuordnen. So sind der
Zahlenstrahl und Iterationstiefe 1 alle Bildpunkte zugeordnet die nach
einmaliger Ausführung die Abbruchbedingung erfüllten, und der zwei alle
Bildpunkte die nach zweimaliger Ausführung die Abbruchbedingung
erfüllten und so fort. Die Iterationstiefe 0 entspricht schwarz und je
höher diese desto heller der Punkt nach dem Start des Programms. Schwarz
bedeutet das der Bildpunkt bis zum Depth-Wert die Abbruchbedingung nicht
erfüllt hat.

**Farbgeber**\
Das Programm bietet für jede Iterationstiefe einen RGB-Farbgeber, mit
dem die Farbe aller Bildpunkte welche der Iterationstiefe entsprechen
vorgegeben werden kann. Wichtig zu wissen das die obere der beiden
türkisfarben markierten Zahlen im Bild die Iterationstiefe, und die
untere die Anzahl an Bildpunkten zeigt welcher dieser Iterationstiefe
entsprechen.

![](vertopal_7043454174a54f35bb26a94c2224a585/media/image4.png)

Bild:\
Drei vollständig und ein teilweise zu sehender Farbgeber. Links im Bild
die Navigations- und Befehlsbuttons der Farbgebung

Da die Menge an Farbgebern meist nicht in einem Satz sichtbar ist, zeigt
die obere Zahl im gelb markierten Bereich den aktuell angezeigten Satz
und die Untere die Anzahl an Farbgeber-Sätzen an.

Die Pfeilbuttons dienen der Navigation innerhalb der Sätze. Die Farbe
kann durch ziehen der Farbbalken im Farbgeber mit der Maus eingestellt
werden und wird im waagerechten Balken unter der Anzahl an Bildpunkten
dargestellt. Ein Klick auf diesen Querbalken färbt die Zahlen weis und
zeigt somit an, das dieser Farbgeber einen Festen Wert innerhalb eines
zu erstellenden\
Farbverlaufes zwischen mindestens zwei solcher markierten Farbgebern
ist. Diesen können wir mit einem Klick auf den linken Button im
magentafarben markierten Bereich des Bildes erstellen. Der rechte Button
dient der Übertragung der in den Farbgebern eingestellten Farben auf
das\
Apfelmännchen. Die Implementierung des Farbverlaufs zwischen den
Farbgeber ist nicht vollständig durchdacht, so das hier Experimentieren
vorausgesetzt wird (der Wert jeder zu ändernden Farbe des Verlaufs
sollte größer null sein im Farbgeber und die Verläufen sollten drei
Sätze nicht überschreiten sowie von links beginnend nach rechts
nacheinander übertragen werden) Mit einem Klick auf die Zahlen des
Farbgebers werden die RGB-Werte dessen zu 0.

Im Anhang „Code-Einsprungpunkt" wird auf eine Zeile im Quelltext
verwiesen, dessen Änderung eine Verschiebung des Schwarz-weißen
Apfelmännchens innerhalb des Spektralbereich ermöglicht, und als
Einsprungpunkt algorithmischen Farbgebungs-Codes dient, dessen\
Spezifikation(Parameter?,Ziel?) durch Umgang mit den Farbgebern des
Programms erdacht werden kann.

**Speichern der Apfelmännchen**\
![](vertopal_7043454174a54f35bb26a94c2224a585/media/image5.png)
Mit einem Klick auf „save" wird
ein BMP-Bild des Männchens mit der Zeit als als Namen im Ordner „bmp"
der Programmdatei abgelegt.
Gleichzeitig wird das
Apfelmännchen mit seinen
Farbgebern in der Leiste wie 
sie im Bild links zu sehen ist,
gespeichert und kann durch klicken auf das kleine Abbild
aufgerufen werden. Ein Klick
auf die rote Fläche neben dem
Abbild löscht das gespeicherte
Apfelmännchen.

**Sortierergebnisse**\
> ![](vertopal_7043454174a54f35bb26a94c2224a585/media/image6.png)

***r und i***\
Wenn wir uns mit dem „Maus-Quader" über dem Apfelmännchen befinden,
zeigen uns r und i das zum Bildpunkt gehörende Zahlenpaar des
Algorithmus an

***Fertig***\
Da Berechnungen mit großen Depth-Werten länger als gewohnt dauern und
das Programm keinen Fortschritt darstellt und die Eingabe blockiert ist,
erscheint während laufender Berechnung hier „Rechne" in rot

***I-Werte***\
Ist die Anzahl der Iterationstiefen die über alle Bildpunkte gefunden
wurde, einschließlich schwarz.

***oneMembers***\
Ist die Anzahl der Iterationstiefen welche nur ein Bildpunkt entspricht

***tenMembers***\
Anzahl der Iterationstiefen welche weniger als elf Bildpunkte
entsprechen (inklusive oneMember).

Im Anhang „Sortierwerte" diesen Dokuments wird der Programmcode
aufgezeigt ,welcher durch Zahlenänderung die Ergebnisse one/tenMember
mit anderer Anzahl an Bildpunkten errechnen lässt.

**Anhang**

***Sortierwerte***\
In den Codezeilen 133 und 135 der /include/\_Apple.cpp können die
Grenzwerte zur Ermittlung zugehöriger Bildpunkte direkt geändert und
anschließend neu kompiliert werden.

133 if(iterMembers\[i\]\[1\] \< 2){\
134 oneMembers.push_back(iterMembers\[i\]\[0\]);} else
if(iterMembers\[i\]\[1\] \< 11){ 135\
135 tenMembers.push_back(iterMembers\[i\]\[0\]);}
