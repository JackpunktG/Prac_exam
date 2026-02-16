#set page(margin: (x: 1.5cm, y: 1.5cm))
#set text(size: 9pt, font: "P052")

= Frage: 1
Folgende Sachverhaltsschilderung des Vertriebsmitarbeiters des Unternehmens, dessen
Geschäftsprozesse reorganisiert werden sollen, liegt Ihnen vor:
„Nachdem der Kundenauftrag eingetroffen ist, muss die Verfügbarkeit der Komponenten geprüft und der
Auftrag erfasst werden. Zugleich müssen auch die Kundendaten aktualisiert werden.
Im Ergebnis der Verfügbarkeitsprüfung kann sich herausstellen, dass der angeforderte Artikel nicht
lieferbar ist. In diesem Fall muss ich dem Kunden gegenüber den Auftrag ablehnen.
Ist der Artikel am Lager und sind die Kundendaten vollständig sowie der Auftrag erfasst, erstelle ich für
den Kunden eine Auftragsbestätigung und sorge anschließend dafür, dass der gewünschte Artikel
bereitgestellt wird. Danach erstelle ich die Versandpapiere und gleichzeitig die Rechnung. Damit ist für
mich der Auftrag bearbeitet.“\
Setzen Sie den geschilderten Sachverhalt in einer (einfachen) EPK um.\
#v(20em)


= Frage: 2
Der Zahlungsprozess im Onlineshop der Fahrradfactory GmbH soll wie folgt organisiert werden:
Zur Zahlung der im Warenkorb liegenden Artikel muss der Kunde den "Zahlen“-Button drücken.
Daraufhin stellt das System die Anmeldeseite bereit. Hier erfolgt eine Aufforderung zur Anmeldung.
Dabei wird unterschieden zwischen einer Anmeldung als Neu-oder Stammkunde.
Nach der Anmeldung als Neukunde stehen die Zahlungsarten Vorauszahlung und Lastschrift zur Auswahl.
Bei Stammkunden sind die Zahlungsarten vom Warenwert abhängig. Daher wird zunächst der
Warenwert geprüft. Ist das festgelegte Limit für den Warenwert überschritten, ist als Zahlungsart
Vorauszahlung und Lastschrift möglich. Ist das Limit nicht überschritten, werden Kreditkartenzahlung und Zahlung per Lastschrift angeboten.
Nach der Festlegung der Zahlungsart erfolgt die Zahlung.\
Vervollständigen Sie für den geschilderten Prozess nebenstehende ereignisgesteuerte Prozesskette (EPK). 17 Punkte\nHinweis: Informationsobjekte und Organisationseinheiten sind nicht darzustellen.\
#image("IHK_Fragen/dateien/modellierung_02.png", height: 60%)
Stellen Sie den Zahlungsvorgang als konsolidierte Entscheidungstabelle dar und verkürzen Sie die EPK mithilfe der Entscheidungstabelle und ET-Operatoren. 8 Punkte\
#v(20em)


= Frage: 3
Im Wareneingangsbereich der neuen Lagerhalle sind die Warenengangskontrolle und die
Lagerzuordnung geplant. Der Prozess des Wareneingangs wird wie folgt beschrieben:
„Nach dem Eintreffen der Ware erfolgt eine zunächst eine Bestellkontrolle anhand des Bestell- und
Lieferscheins. Liegt keine Bestellung vor, wird die Ware abgewiesen.Liegt eine Bestellung vor, erfolgt
parallel eine eine quantitative und eine qualitative Prüfung der Lieferscheinpositionen. Liegt eine
Fehlmenge vor, wird dies auf dem Lieferschein vermerkt. Ware, die bei der Qualitätskontrolle Mängel
zeigt, wird zurückgeschickt.Qualitativ einwandfreie Ware wird in der Lagerdatenbank eingebucht, auch
wenn Fahlmengen vorliegen.Anschließend erfolgt eine Einlagerung und Verteilung auf die Lagerorte
oder eine direkte Zuordnung zu den Onlinebestellungen der Kunden anhand der
Kundenauftragsbelege.“
Der beschriebene Geschäftsprozess soll mit einer erweiterten ereignisgesteuerten Prozesskette (eEPK)
beschrieben werden.\
Vervollständigen Sie dazu den nachfolgenden eEPK-Ausschnitt.\
#image("IHK_Fragen/dateien/modellierung_01.png", height: 60%)


#pagebreak()
#align(horizon + center)[#image("IHK_Fragen/dateien/uml_OOP1.pdf", page: 1, width: 100%, fit: "contain")]

#pagebreak()#align(horizon + center)[#image("IHK_Fragen/dateien/uml_OOP1.pdf", page: 2, width: 100%, fit: "contain")]

#pagebreak()#align(horizon + center)[#image("IHK_Fragen/dateien/uml_OOP1.pdf", page: 3, width: 100%, fit: "contain")]

#pagebreak()#align(horizon + center)[#image("IHK_Fragen/dateien/uml_OOP1.pdf", page: 4, width: 100%, fit: "contain")]

#pagebreak()#align(horizon + center)[#image("IHK_Fragen/dateien/uml_OOP1.pdf", page: 5, width: 100%, fit: "contain")]

