// Notes:
//
// - The file must be saved using utf-8 without signature (adv save options)
//
// - Translations should generally be a similar or shorter length to the original text (although if there's lots of text - like the help - it doesn't matter). If necessary a shorter but less "correct" phrase may be better.
//
// - Be careful to preserve certain things like:
// "..." at the end of a phrase
// Things like %d (which are used to insert numbers into the string)
// Spaces in multi-line strings
// The ordering
//
// - Use accents etc like ü instead of ue, except in PS_DONATEMSG
//
// - Translate in place - replacing the original English. If you can't think of a good translation, skip it.
//
// - Try to be consistent - so if you use "controller" in one place, don't use "transmitter" in other places.
//
// - It's probably more important to get the first half (down to "Everything below here is for settings") translated - these will be seen by people even if they don't fiddle with the settings.
//
// - Let me know if you see a typo in the English


#include "../PicaStrings.h"


void InitStringsDE(const char** txt)
{
  txt[PS_LANGUAGE_EN] = "German";
  txt[PS_LANGUAGE_NATIVE] = "Deutsch";
  txt[PS_OK] = "OK";
  txt[PS_CRASHED] = "Abgestürzt";
  txt[PS_FREEFLY] = "Freies Fliegen";
  txt[PS_RACE] = "Rennen";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Vollversion"; 

  txt[PS_SELECTAEROPLANE] = "Flugzeug";
  txt[PS_SELECTSCENERY] = "Landschaft";
  txt[PS_SELECTOPTIONS] = "Bitte passende Option wählen";
  txt[PS_USEDEFAULT] = "Standard benutzen";
  txt[PS_USEDEFAULTPREVIOUS] = "Benutze Standard/Vorherige";

  txt[PS_BACK] = "Zurück";
  txt[PS_OPTIONS1] = "Optionen 1";
  txt[PS_OPTIONS2] = "Optionen 2";
  txt[PS_AEROPLANE] = "Flugzeug";
  txt[PS_SCENERY] = "Landschaft";
  txt[PS_LIGHTING] = "Lichtverhältnisse";
  txt[PS_CONTROLLER] = "Steuerung";
  txt[PS_LOAD] = "Laden...";
  txt[PS_SAVE] = "Speichern...";
  txt[PS_DELETE] = "Löschen...";
  txt[PS_ADVANCED] = "Fortgeschritten";
  txt[PS_SIMPLE] = "Einfach";
  txt[PS_FILENAME] = "Dateiname:";
  txt[PS_INFULLVERSION] = "In Vollversion";

  txt[PS_ABOUT] = "Über";
  txt[PS_WEBSITE] = "Webseite";
  txt[PS_FLYING] = "Fliegen";
  txt[PS_SETTINGS] = "Einstellungen";
  txt[PS_RACES] = "Rennen";
  txt[PS_CREDITS] = "Danksagung";




  txt[PS_ABOUTFREETEXT] =
    "PicaSim ist ein Simulator für ferngesteuerte Flugzeuge. Gegenwärtig ist er darauf angelegt, Euch beim Fliegenlernen zu helfen und Eure Hangflugfähigkeiten zu verbessern - den Segelflieger mit dem Auftrieb des Windes, so wie er an Anhöhen hinauf- und herüberströmt, zu fliegen.\n"
    "\n"
    "Wenn Ihr noch nie ein Modellflugzeug geflogen habt, werdet Ihr sehr wahrscheinlich bei Euren ersten Flugversuchen ein paar Mal (oder öfter!) abstürzen. Das ist normal- besser im Simulator als mit einem aufwändigen Modellflieger, in den Ihr viel Zeit zum Bauen und Einrichten investiert habt.\n"
    "\n"
    "PicaSim kann Euch helfen 3 Fähigkeiten zu erlernen:\n"
    "\n"
    "1. Das Flugzeug mit dem Senderknüppel zu steuern.\n"
    "2. Abzuschätzen wo sich an Hangflugstandorten Auftrieb befindet.\n"
    "3. Das Flugzeug selbst dann zu steuern, wenn es direkt auf Euch zufliegt.\n"
    "\n"
    "Zusätzlich können Fortgeschrittene mit PicaSim Luftakrobatik erproben oder sich einfach daran erfreuen, auch bei Windstille fliegen zu können!\n"
    "\n"
    "Ich habe viele Ideen PicaSim zu erweitern und zu verbessern- ich möchte Motorflugzeuge sowie weitere Flugmodelle aufnehmen, vernetztes Fliegen und computeranimierte Flugzeuge mit einschliessen. Falls Ihr mich unterstützen und zum Weitermachen ermuntern möchtet, oder Ihr Euch vielleicht einfach für diese Version bedanken wollt, freue ich mich sehr, wenn Ihr die Spendenversion PicaSimDonate kauft.\n"
    ;


  txt[PS_ABOUTPAIDTEXT] =
    "PicaSim ist ein Simulator für ferngesteuerte Flugzeuge. Gegenwärtig ist er darauf angelegt Euch beim Fliegenlernen zu helfen und Eure Hangflugfähigkeiten zu verbessern- den Segelflieger mit dem Auftrieb des Windes, so wie er an Anhöhen hinauf- und herüberströmt, zu fliegen.\n"
    "\n"
    "Wenn Ihr noch nie ein Modellflugzeug geflogen habt, werdet Ihr sehr wahrscheinlich bei Euren ersten Flugversuchen ein paar Mal (oder öfter!) abstürzen. Das ist normal- besser im Simulator als mit einem aufwändigen Modellflieger, in den Ihr viel Zeit zum Bauen und Einrichten investiert habt.\n"
    "\n"
    "PicaSim kann Euch helfen 3 Fähigkeiten zu erlernen:\n"
    "\n"
    "1. Das Flugzeug mit dem Senderknüppel zu steuern.\n"
    "2. Abzuschätzen wo sich an Hangflugstandorten Auftrieb befindet.\n"
    "3. Das Flugzeug selbst dann zu steuern, wenn es direkt auf Euch zufliegt.\n"
    "\n"
    "Zusätzlich können Fortgeschrittene mit PicaSim Luftakrobatik erproben oder sich einfach daran erfreuen, auch bei Windstille fliegen zu können!\n"
    "\n"
    "Ich habe viele Ideen PicaSim zu erweitern und zu verbessern- ich möchte Motorflugzeuge sowie weitere Flugmodelle aufnehmen, vernetztes Fliegen und computergesteuerte Flugzeuge mit einschliessen. Eure kleine Spende hilft mir dabei, dies zu tun und ich bin Euch dafür sehr dankbar.\n"
    ;


  txt[PS_HOWTOFLYTEXT] =
    "Als erstes schaut Ihr Euch am besten die Webseite (siehe Link unten) und das Video (\"Wie man fliegt\") dort an.\n"
    "\n"
    "Wenn der Simulator gestartet ist, wird der Flieger zunächst stillstehen. Mit dem Knopf in der rechten oberen Ecke schickt Ihr den Segelflieger los in den Wind. Benutzt die Bildschirmsymbole Joystick/Steuerung in der rechten unteren Bildschirmecke genau so wie den rechten Knüppel eines ferngesteuerten Senders (in Modus 2). Im Falle eines Absturzes stoppt Ihr den Simulator, klickt auf den Rückspulknopf und startet den Segelflieger wieder neu. Ihr könnt stattdessen auch, falls vorhanden, den Zurückknopf Eures Gerätes benutzen.\n"
    "\n"
    "Wenn Ihr Euch an die Grundsteuerung gewöhnt habt, solltet Ihr den Segelflieger so manövrieren, dass er innerhalb der Auftriebszone bleibt, die vom Wind geliefert wird und an Anhöhen hinauf- und herüberströmt. Am besten fliegt Ihr dazu zunächst drei bis vier Sekunden geradeaus, biegt dann links ab und fliegt für maximal zehn Sekunden parallel zur Anhöhe. Dreht daraufhin um und fliegt eine ungefähr gleich grosse Entfernung zurück, bis Ihr am Standpunkt vorbeigeflogen seid. Bleibt dabei immer parallel zur Anhöhe und in etwa gleicher Höhe zum Standpunkt.\n"
    "\n"
    "Wenn alles ein wenig zu schnell passiert, könnt Ihr auch in Zeitlupe fliegen- geht zu Einstellungen>Optionen. Denkt nur daran, dies letztendlich wieder rückgängig zu machen und auf 1.0 zu stellen, denn im wirklichen Leben gibt es diese Einstellung schliesslich auch nicht!\n"
    "\n"
    "Möchtet Ihr nicht nur die Panoramaansicht benutzen, könnt Ihr den Standpunkt auch auf das Innere des Flugzeugs verlegen: Augentaste im Stillstand oder Suchtaste auf Eurem Gerät. Nun könnt Ihr das Gelände erkunden- erlangt wo auch immer möglich an Auftrieb, damit Ihr auch Bereiche überfliegen könnt, in denen Wind/Anhöhe keinen Auftrieb liefert. Schaut immer auf den Windpfeil, um zu wissen in welche Richtung der Wind weht und wo sich daher wahrscheinlich Auftrieb befindet.\n"
    "\n"
    "Ist das Gelände fast vollständig flach, habe ich es so eingerichtet, dass Thermik besteht; die Segelflieger werden hier mit einem Hochstartgummi gestartet. Ihr müsst die Bewegung des Segelfliegers sehr aufmerksam verfolgen, um diese Thermik zu erkennen. Bereitet es Euch Schwierigkeiten, aktiviert die Thermikhelfer in den Optionen- sie erscheinen jeweils in der Mitte jeder Thermik und sind etwa so gross wie das Flugzeug. Damit solltet Ihr die Entfernung abschätzen können. Denkt daran, dass Thermik über Land stärker ist als über Wasser, sie ist ebenfalls stärker an Anhöhen, die in der Sonne liegen.\n"
    "\n"
    "Schaut Euch die Webseite unten an, dort sind einige Vidoes, die zeigen, wie das Fliegen funktioniert- es braucht ein wenig Übung, genau wie mit einem echten ferngesteuerten Modellflugzeug.\n"
    ;

  txt[PS_HOWTOCONFIGURETEXT] =
    "Ihr bekommt entweder über das Startmenü Zugriff auf PicaSims Einstellungen oder über das Klicken irgendeines Symbols, wenn PicaSim stillsteht.\n"
    "\n"
    "Die Einstellungen sind in mehrere Gruppen unterteilt- allgemeine Optionen, das Flugzeug, die Landschaft/das Gelände, Lichtverhältnisse und Steuerungskonfigurationen. Es ist prinzipiell nicht möglich, mit den meisten der detaillierten Einstellungen jeder einzelnen Gruppe zu frickeln, aber Ihr könnt voreingestellte konfigurierte Gruppen laden. Hiermit könnt Ihr beispielsweise schnell Flugzeuge oder das Gelände wechseln.\n"
    "\n"
    "Aktiviert Ihr in jedem Abschnitt die 'Einstellungen für Fortgeschrittene', werdet Ihr sehr viel mehr Feineinstellungen vornehmen können, was allerdings etwas überwältigend sein könnte! Im Laufe der Zeit werde ich detailliertere Informationen über diese Einstellungen liefern, dennoch könnt Ihr bereits einfach risikolos mit den Werten herumfrickeln wie Euch beliebt, da Ihr sie jederzeit wieder mit einem der voreingestellten Gruppen nachladen könnt.\n"
    "\n"
    "Ihr könnt Eure Voreinstellungen speichern- entweder auf der Speicherkarte oder dem Datenbereich Eures Gerätes. Solltet Ihr Probleme mit PicaSim haben, nachdem Ihr die Einstellungen verändert habt, lohnt es sich, sofort nach Start von PicaSim jeden Abschnitt einzeln nacheinander neuzuladen.\n"
    "\n"
    "Ist der 'Rundgang' in den Optionen aktiviert, dann gibt es auch im Stillstand eine 'Rundgangtaste'. Diese ermöglicht Euch mit Hilfe der Steuerung im Gelände umherzulaufen und einen neuen Abflugstandort ausfindig zu machen. Die Windrichtung ist in diesem Modus per Standardeinstellung so eingestellt, dass der Wind immer direkt auf die Kamera zuweht.\n"
    "\n"
    "Bei den Panoramaansichten ist es nicht möglich, den Standpunkt zu verändern, daher ist der Rundgangmodus hier ausgeschaltet.\n"
    ;


  txt[PS_HOWTORACETEXT] =
    "Bei den Rennen geht es darum, so schnell wie möglich zwischen zwei Kontrollpunkten hin- und herzufliegen. Der nächste anzupeilende Kontrollpunkt ist farblich hervorgehoben, zusätzlich zeigt der weisse Pfeil unten im Bildschirm in seine Richtung.\n"
    "\n"
    "Es gibt verschiedene Arten von Kontrollpunkten: Bei den Rennen im F3F- Stil, in denen Ihr immer hin- und herfliegt, müsst Ihr im Luv an den Kontrollpunkten vorbeifliegen. Beim Geländerennen hingegen braucht Ihr nur in die Nähe der Kontrollpunkte zu kommen, könnt aber beliebig hoch sein. "
    "Beim Flachlandrennen fliegt Ihr zwischen zwei Toren - das Tor im Aufwind ist grün, das Tor, das sich im Gegenwind befindet, ist rot.\n"
    ;


  txt[PS_CREDITSTEXT] =
"Autor: Danny Chapman.\n\n\n"
"Danke an:\n\n"
"Heidi und Hazel für ihre Geduld\n\n"
"Heidi, Jenne, André und Sylvain für die Übersetzung\n\n"
"Bullet physics\n\n"
"TinyXML\n\n"
"Marmalade für Cross-Platform Bibliotheken\n\n"
"Gleb Lebedev für sein gamepad extension\n\n"
"Detlef Jacobi für Litermonthalle Nalbach/\n\n"
"Sounds von www.freesfx.co.uk\n\n"
"Textures von seamless-pixels.blogspot.co.uk/\n\n"
"Alle, die mir beim Testen und Berichten von Fehlern geholfen haben\n\n"
"Modelle bereitgestellt von:\n\n"
"Mark Cassidy (Phase 6, F18)\n\n"
"Jim Sekol (Banana, Discus, Genie)\n\n"
"rrcdoug2000 (Hang gliders)\n\n"
"Steve Lange (Le Fish, The Plank, Weasel)\n\n"
"Julian Kent (Jart)\n\n"
"Alexander Monell (Kato)\n\n"
"Markus Golec (Spirit26)\n\n"
"Igor Burger & Tatyana Uzunova (Max Bee textures)\n\n"
"Danny Chapman (Magpie, Jet, Canard, Trainer, Extra3D etc)\n\n"
"\n"
"Beachtet bitte, dass trotz grösster Sorgfalt das Flugverhalten der Modelle so realistisch wie möglich zu gestalten, mir dies mitunter nicht immer gelungen sein mag. Am besten findet Ihr dies heraus, wenn Ihr ein wirkliches Modell fliegt!"
;

  txt[PS_TIPS1] = "Bitte schickt mir Rüchmeldungen, damit ich PicaSim verbessern kann";
  txt[PS_TIPS2] = "Gefällt Euch PicaSim, hinterlasst bitte eine Bewertung";
  txt[PS_TIPS3] = " Ihr könnt den Beschleunigungssensor benutzen, um das Modell zu steuern - geht zu Einstellungen»Steuerung";
  txt[PS_TIPS4] = "Folgt PicaSim auf Facebook oder Google+ und erhaltet Benachrichtigungen über Aktualisierungen und zukünftige Entwicklungen";
  txt[PS_ALLOWCHOICE] = "Modell und Landschaftsauswahl beim nächsten Start anzeigen";
  txt[PS_LOADING] = "Lädt...";

  txt[PS_RACETIP] = "%s\n\nDie weissen Pfeile zeigen in die Richtung der nächsten Kontrollpunkte.";
  txt[PS_LIMBOTIP] = "%s\n\nStelle den Schwierigkeitsgrad/Ergebnismultiplikator in Einstellungen»Modus";
  txt[PS_TRAINERGLIDERTIP] = 
    "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
    "Benuzt den Kontrollknüppel um das Ruder und Höhenruder zu steuern, damit der Segelflieger mit dem Auftrieb vor dem Hügel fliegt. "
    "Seht auf der Hilfe/Website nach um Tipps zu erhalten falls es euch schwerfällt in der Luft zu bleiben.";
  txt[PS_TRAINERPOWEREDTIP] = 
    "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
    "Benuzt den Kontrollknüppel rechts um das Höhenruder und Querruder zu steuern, und links um den Gashebel und Ruder zu steuern. "
    "Seht auf der Hilfe/Website nach um Tipps zu erhalten falls es euch schwerfällt in der Luft zu bleiben.";

  txt[PS_MOUSETOCONTINUE] = "Maus oder jede Taste zum Fortfahren";
  txt[PS_TAPTOCONTINUE] = "Berührt zum Fortfahren";

  txt[PS_ALLRESULTS] = "Alle Ergebnisse";
  txt[PS_LAST24HOURS] = "Die letzten 24 Stunden";
  txt[PS_LAST7DAYS] = "Die letzten 7 Tage";
  txt[PS_LAST30DAYS] = "Die letzten 30 Tage";
  txt[PS_TIMEPERIODFORRESULTS] = "Zeitraum für Ergebnisse";
  txt[PS_SCORELINERACE] = " Ergebnis = %.1f Zeit = %.2f";
  txt[PS_SCORELINELIMBO] = "  Ergebnis = %.1f Rechnung = %d";
  txt[PS_NOSCORES] = "Keine Ergebnisse vorhanden";
  txt[PS_CURRENTSCORE] = "Aktuelles Ergebnis = %.1f Zeit = %.2f";
  txt[PS_NOSCORESINTIME] = "Keine Ergebnisse in dem Zeitraum";
  txt[PS_RETRIEVINGLEADERBOARD] = "Rangliste wird abgerufen...";
  txt[PS_SETUSERNAME] = "Benutzername festlegen";
  txt[PS_SETEMAIL] = "Emailadresse eintragen";
  txt[PS_PROFILE] = "Profil";
  txt[PS_USERNAME] = "Benutzername";
  txt[PS_EMAIL] = "Email";
  txt[PS_LEADERBOARDS] = "Rangliste";
  txt[PS_UPLOADINGSCORE] = "Ergebnis wird übertragen...";
  txt[PS_REQUESTINGUSER] = "Benutzer prüfen...";
  txt[PS_SELECTRACE] = "Rennen auswählen";
  txt[PS_SELECTRACETOSEE] = "Wähle ein Rennen um die Rangliste anzuzeigen";
  txt[PS_EDIT] = "Bearbeiten";
  txt[PS_PROFILESTATUS] = "Profilstatus";
  txt[PS_SCORELOOPUSERNAME] = "Scoreloop Benutzername";
  txt[PS_EMAILADDRESS] = "Emailadresse";
  txt[PS_OPTIONALEMAIL] = "Optional - verbindet Scoreloop Benutzernamen mit Geräten";
  txt[PS_FETCHINGEMAIL] = "Erfasse Email";
  txt[PS_PREV] = "Vorher";
  txt[PS_TOP] = "Über";
  txt[PS_ME] = "Ich";
  txt[PS_NEXT] = "Nächster";
  txt[PS_UPDATINGLEADERBOARD] = "Aktualisiere Rangliste...";
  txt[PS_NOMORESCORES] = "Keine weiteren Ergebnisse";
  txt[PS_SELECTARACE] = "Wähle ein Rennen";
  txt[PS_UPLOADINGDATA] = "Daten werden übertragen...";
  txt[PS_FAILED] = "Gescheitert!: ";
  txt[PS_EMAILTAKEN] = "Emailadresse bereits vergeben. ";
  txt[PS_EMAILINVALID] = "Emailformat ungültig. ";
  txt[PS_USERTAKEN] = "Benutzername bereits vergeben. ";
  txt[PS_USERINVALID] = "Benutzerformat ungültig. ";
  txt[PS_USERSHORT] = "Benutzername zu kurz. ";


  // Everything below here is for settings
  txt[PS_SELECTLANGUAGE] = "Sprache auswählen";
  txt[PS_LANGUAGESETTINGS] = "Spracheinstellungen";
  txt[PS_CURRENTLANGUAGE] = "Aktuelle Sprache";
  txt[PS_INFREEFLYMODE] = "Freiflugmodus";
  txt[PS_CAMERASETTINGS] = "Kameraeinstellungen";
  txt[PS_ZOOMVIEW] = "Zoom Ansicht";
  txt[PS_ZOOMVIEWSIZE] = "Zoom Ansicht Skalierung";
  txt[PS_GROUNDAUTOZOOM] = "Boden Autozoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Boden Autozoom Skalierung";
  txt[PS_GROUNDFIELDOFVIEW] = "Boden Blickfeld";
  txt[PS_GROUNDHORIZONAMOUNT] = "Boden Horizont Menge";
  txt[PS_GROUNDVIEWFOLLOW] = "Folge dem Flugzeug in Bodenperspektive";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Flugzeug Blickfeld";
  txt[PS_WALKABOUTSETTINGS] = "Rundgang Einstellungen";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Rundgangaktivierung";
  txt[PS_SETWINDDIRONWALKABOUT] = "Rundgang Windrichtung festlegen";
  txt[PS_SIMULATIONSETTINGS] = "Simulationseinstellungen";
  txt[PS_TIMESCALE] = "Simulationsgeschwindigkeit";
  txt[PS_PHYSICSACCURACY] = "Physik-Genauigkeit";
  txt[PS_CONTROLLERSETTINGS] = "Steuerungseinstellungen";
  txt[PS_MODE1DESCRIPTION] = "Mode 1: Ruder & Höhenruder links, Querruder & Gashebel/Auftriebshilfe rechts";
  txt[PS_MODE2DESCRIPTION] = "Mode 2: Ruder & Gashebel/Auftriebshilfe links, Querruder & Höhenruder rechts";
  txt[PS_MODE3DESCRIPTION] = "Mode 3: Querruder & Gashebel/Auftriebshilfe links, Ruder & Höhenruder rechts";
  txt[PS_MODE4DESCRIPTION] = "Mode 4: Querruder & Höhenruder links, Ruder & Gashebel/Auftriebshilfe rechts";
  txt[PS_ENABLEJOYSTICK] = "Joystick aktivieren";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Anpassung an kreisende Knüppelbewegung";
  txt[PS_CONTROLLERSIZE] = "Größe";
  txt[PS_STAGGERCONTROLLER] = "Versetzte Steuerung (für teilweisen Multitouch)";
  txt[PS_SQUARECONTROLLERS] = "Quadratische Steuerelemente";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Horizontaler Versatz";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Vertikaler Versatz";
  txt[PS_SHAPEOPACITY] = "Form Hervorhebung";
  txt[PS_STICKOPACITY] = "Knüppel Hervorhebung";
  txt[PS_STICKCROSS] = "Knüppel Kreuz";
  txt[PS_AUDIOSETTINGS] = "Audio Einstellungen";
  txt[PS_OVERALLVOLUME] = "Gesamtlautstärke";
  txt[PS_VARIOMETERVOLUME] = "Variometer Lautstärke";
  txt[PS_WINDVOLUME] = "Wind Lautstärke";
  txt[PS_OUTSIDEAEROPLANEVOLUME] = "Lautstärke ausserhalb des Flugzeugs";
  txt[PS_INSIDEAEROPLANEVOLUME] = "Lautstärke im Flugzeug";
  txt[PS_ONSCREENDISPLAYSETTINGS] = "Bildschirmanzeige Einstellungen";
  txt[PS_WINDARROWSIZE] = "Windrichtungspfeil Größe";
  txt[PS_WINDARROWOPACITY] = "Windrichtungspfeil Deckkraft";
  txt[PS_PAUSEBUTTONSSIZE] = "Pause Button Größe";
  txt[PS_PAUSEBUTTONOPACITY] = "Pause Button Deckkraft";
  txt[PS_INFORMATIONSETTINGS] = "Information Einstellungen";
  txt[PS_MAXMARKERSPERTHERMAL] = "Zeichne Thermik";
  txt[PS_GRAPHAIRSPEED] = "Zeige Windgeschwindigkeit (rot)";
  txt[PS_GRAPHGROUNDSPEED] = "Zeige Fluggeschwindigkeit über Grund (grün)";
  txt[PS_GRAPHCLIMBRATE] = "Zeige Steigrate (blau)";
  txt[PS_GRAPHWINDSPEED] = "Zeige Windgeschwindigkeit (gelb)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Zeige Windgeschwindigkeit vertikal (lila)";
  txt[PS_STALLMARKERS] = "Strömungsabriss-Markierungen";
  txt[PS_DRAWAEROPLANECOM] = "Zeichne Schwerpunkt";
  txt[PS_GRAPHFPS] = "Zeige Bilder pro Sekunde (FPS)";
  txt[PS_NUMWINDSTREAMERS] = "Anzahl Windpfeile";
  txt[PS_WINDSTREAMERTIME] = "Windpfeile Dauer";
  txt[PS_WINDSTREAMERDELTAZ] = "Windpfeile Abstand";
  txt[PS_GRAPHICSSETTINGS] = "Graphik Einstellungen";
  txt[PS_GROUNDTERRAINLOD] = "Geländebegrenzung";
  txt[PS_AEROPLANETERRAINLOD] = "Flugzeug Geländebegrenzung";
  txt[PS_UPDATETERRAINLOD] = "Aktualisiere Geländebegrenzung in Bodenansicht";
  txt[PS_COMPONENTS] = "Komponenten";
  txt[PS_3DMODEL] = "3D Model";
  txt[PS_BOTH] = "Beide";
  txt[PS_AEROPLANERENDER] = "Flugzeug Darstellung";
  txt[PS_DRAWSHADOWS] = "Zeichne Schatten";
  txt[PS_USE16BIT] = "Benutze 16 Bit Texturen um Speicher zu reduzieren (Neustart erforderlich)";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Umgebungslicht Skalierung";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Diffuses Licht Skalierung";
  txt[PS_TERRAINTEXTUREDETAIL] = "Geländetextur Detail";
  txt[PS_NEARCLIPDISTANCE] = "Nahkappabstand";
  txt[PS_FARCLIPDISTANCE] = "Fernkappabstand";
  txt[PS_MISCSETTINGS] = "Verschiedene Einstellungen";
  txt[PS_USEBACKBUTTON] = "Benutze Zurrück-Button zum Schließen";
  txt[PS_DRAWLAUNCHMARKER] = "Zeichne Startmarkierung";
  txt[PS_DRAWGROUNDPOSITION] = "Zeichne bodenmarkierung";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Benutze vom Flugzeug bevorzugte Steuerung";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Test/Entwicklereinstellungen";
  txt[PS_WIREFRAMETERRAIN] = "Drahtgelände";
  txt[PS_DRAWSUNPOSITION] = "Zeichne Position der Sonne";
  txt[PS_FREEFLYSETTINGS] = "Freies Fliegen Einstellungen";
  txt[PS_DISPLAYFLIGHTTIME] = "Flugzeit anzeigen";
  txt[PS_DISPLAYSPEED] = "Geschwindigkeit anzeigen";
  txt[PS_DISPLAYMAXSPEED] = "Geschwindigkeit max anzeigen";
  txt[PS_DISPLAYASCENTRATE] = "Steigrate anzeigen";
  txt[PS_DISPLAYALTITUDE] = "Flughöhe anzeigen";
  txt[PS_DISPLAYDISTANCE] = "Abstand anzeigen";
  txt[PS_COLOURTEXT] = "Farbiger Text bei Steigrate";
  txt[PS_RACESETTINGS] = "Rennen/Limbo Einstellungen";
  txt[PS_RACEVIBRATIONAMOUNT] = "Vibration";
  txt[PS_RACEBEEPVOLUME] = "Piep Lautstärke";
  txt[PS_LIMBODIFFICULTY] = "Limbo Schwierigkeiten";
  txt[PS_ENABLESCORELOOP] = "Scoreloop aktivieren";
  txt[PS_DELETELOCALHIGHSCORES] = "Lösche lokale Höchstpunktestände";


  txt[PS_GENERALSETTINGS] = "Allgemeine Einstellungen";
  txt[PS_COLOURSCHEME] = "Farbschema";
  txt[PS_BALLAST] = "Ballast";
  txt[PS_BALLASTFWD] = "Ballast vorne";
  txt[PS_BALLASTLEFT] = "Ballast links";
  txt[PS_BALLASTUP] = "Ballast oben";
  txt[PS_DRAGMULTIPLIER] = "Widerstand Faktor";
  txt[PS_SIZEMULTIPLIER] = "Größe Faktor";
  txt[PS_MASSMULTIPLIER] = "Gewicht Faktor";
  txt[PS_ENGINEMULTIPLIER] = "Motor Faktor";
  txt[PS_HASVARIOMETER] = "Hat Variometer";
  txt[PS_PREFERREDCONTROLLER] = "Bevorzugte Steuerung";
  txt[PS_LAUNCH] = "Start";
  txt[PS_LAUNCHSPEED] = "Start Geschwindigkeit";
  txt[PS_LAUNCHANGLE] = "Startwinkel nach oben";
  txt[PS_LAUNCHUP] = "Start nach oben";
  txt[PS_LAUNCHFORWARD] = "Start nach vorne";
  txt[PS_LAUNCHLEFT] = "Start nach links";
  txt[PS_AIRFRAME] = "Cellule d'avion";
  txt[PS_UNDERCARRIAGE] = "Châssis";
  txt[PS_PROPELLER] = "Hélice";
  txt[PS_MAXBUNGEELENGTH] = "Maximale Hochstartgummi-Länge";
  txt[PS_MAXBUNGEEACCEL] = "Maximale Hochstartgummi-Beschleunigung";
  txt[PS_CHASECAMERA] = "Kamera verfolgen";
  txt[PS_DISTANCE] = "Distanz";
  txt[PS_HEIGHT] = "Höhe";
  txt[PS_VERTICALVELFRAC] = "Verticaler Geschwindigkeitsanteil";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Aktuelle Position";
  txt[PS_MASS] = "Gewicht";

  txt[PS_WINDSETTINGS] = "Wind Einstellungen";
  txt[PS_WINDSPEED] = "Windgeschwindigkeit";
  txt[PS_WINDBEARING] = "Windrichtung";
  txt[PS_CANNOTMODIFYSCENERY] = "Ihr könnt die Landschaft während eines Rennens nicht verändern";
  txt[PS_ALLOWBUNGEE] = "Start mit Hochstartgummi erlauben";
  txt[PS_WINDGUSTTIME] = "Windböenhäufigkeit";
  txt[PS_WINDGUSTFRACTION] = "Windböenanteil";
  txt[PS_WINDGUSTANGLE] = "Windböenwinkel";
  txt[PS_TURBULENCEAMOUNT] = "Turbulenzmenge";
  txt[PS_WINDLIFTSMOOTHING] = "Windauftriebsglättung";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Abtrennungstendenz";
  txt[PS_DEADAIRTURBULENCE] = "Tote Luft Turbulenz";
  txt[PS_THERMALSETTINGS] = "Thermik Einstellungen (siehe auch Licht Einstellungen)";
  txt[PS_DENSITY] = "Dichte";
  txt[PS_RANGE] = "Reichweite";
  txt[PS_LIFESPAN] = "Lebensdauer";
  txt[PS_DEPTH] = "Tiefe";
  txt[PS_CORERADIUS] = "Kern Radius";
  txt[PS_DOWNDRAFTEXTENT] = "Abwind Ausdehnung";
  txt[PS_UPDRAFTSPEED] = "Aufwind Geschwindigkeit";
  txt[PS_ASCENTRATE] = "Steigrate";
  txt[PS_RANDOMTERRAINSETTINGS] = "Zufälliges Gelände Einstellung";
  txt[PS_RANDOMSEED] = "Impfnummer";
  txt[PS_DISPLACEMENTHEIGHT] = "Versetzungshöhe";
  txt[PS_SMOOTHNESS] = "Glätte";
  txt[PS_EDGEHEIGHT] = "Geländekantenhöhe";
  txt[PS_UPWARDSBIAS] = "Upwards bias";
  txt[PS_FILTERITERATIONS] = "Filterwiederholungen";
  txt[PS_DRAWPLAIN] = "Zeichne Ebene/Wasser";
  txt[PS_COLLIDEWITHPLAIN] = "Kollidiere mit Ebene/Wasser";
  txt[PS_PLAININNERRADIUS] = "Ebene Innerer Radius";
  txt[PS_PLAINFOGDISTANCE] = "Ebene Nebelentfernung";
  txt[PS_PLAINHEIGHT] = "Ebenenhöhe";
  txt[PS_TERRAINSIZE] = "Geländegrösse";
  txt[PS_COASTENHANCEMENT] = "Küste LOD Verbesserung";
  txt[PS_TERRAINDETAIL] = "Geländedetail";
  txt[PS_RIDGETERRAINSETTINGS] = "Geländerücken Einstellungen";
  txt[PS_MAXHEIGHTFRACTION] = "Maximaler Höhenanteil";
  txt[PS_WIDTH] = "Breite";
  txt[PS_HEIGHTOFFSET] = "Höhenausgleich";
  txt[PS_HORIZONTALVARIATION] = "Horizontale Variation";
  txt[PS_HORIZONTALWAVELENGTH] = "Horizontale Wellenlänge";
  txt[PS_VERTICALVARIATION] = "Vertikaler Variationsanteil";
  txt[PS_PANORAMASETTINGS] = "Panorama Einstellungen";
  txt[PS_PANORAMA3DSETTINGS] = "Panorama 3D Einstellungen";
  txt[PS_HEIGHTFIELDSETTINGS] = "Höhenfeldeinstellungen";
  txt[PS_MINHEIGHT] = "Minimale Höhe";
  txt[PS_MAXHEIGHT] = "Maximale Höhe";
  txt[PS_CURRENTVIEWPOSITION] = "Momentaner Standpunkt";
  txt[PS_NOLIGHTINGSETTINGS] = "Keine Lichteinstellungen, da Geländetyp Panorama";
  txt[PS_SUNBEARING] = "Sonnenpeilung";
  txt[PS_THERMALACTIVITY] = "Thermikaktivität";
  txt[PS_TERRAINDARKNESS] = "Gelände Dunkelheit";

  txt[PS_CROSS] = "Kreuz";
  txt[PS_BOX] = "Box";
  txt[PS_CROSSANDBOX] = "Kreuz & Box";
  txt[PS_CONFIGURATION] = "Konfiguration";
  txt[PS_STYLE] = "Stil";
  txt[PS_NUMCONFIGURATIONS] = "Anzahl Konfigurationen";
  txt[PS_RIGHTSTICKHORIZONTAL] = "Rechter Knüppel horizontal";
  txt[PS_RIGHTSTICKVERTICAL] = "Rechter Knüppel vertikal";
  txt[PS_LEFTSTICKHORIZONTAL] = "Linker Knüppel horizontal";
  txt[PS_LEFTSTICKVERTICAL] = "Linker Knüppel vertikal";
  txt[PS_TILTHORIZONTAL] = "Kippung horizontal";
  txt[PS_TILTVERTICAL] = "Kippung vertikal";
  txt[PS_ARROWSHORIZONTAL] = "Pfeile horizontal";
  txt[PS_ARROWSVERTICAL] = "Konstant";
  txt[PS_CONSTANT] = "";
  txt[PS_NONE] = "Keine";
  txt[PS_CONTROLSOURCES] = "Kontrollursprung";
  txt[PS_AILERONS] = "Querruder/Ruder";
  txt[PS_ELEVATOR] = "Höhenruder";
  txt[PS_RUDDER] = "Ruder";
  txt[PS_THROTTLE] = "Gashebel/Auftriebshilfe/Bremsen";
  txt[PS_LOOKYAW] = "Ansicht Gierachse";
  txt[PS_LOOKPITCH] = "Ansicht Nickwinkel";
  txt[PS_SETTINGSFORCONTROLLER] = "Einstellungen für Steuerungskonfiguration %d: %s";
  txt[PS_TRIMSETTINGS] = "Trimmungseinstellungen";
  txt[PS_NOSIMPLESETTINGS] = "Keine einfachen Einstellungen verfügbar";
  txt[PS_TRIM] = "Trimmung";
  txt[PS_SCALE] = "Maßstab";
  txt[PS_CLAMPING] = "Clamping";
  txt[PS_EXPONENTIAL] = "Exponential";
  txt[PS_SPRING] = "Feder";
  txt[PS_POSITIVE] = "Positive";
  txt[PS_NEGATIVE] = "Negative";
  //txt[PS_RIGHTSTICKHORIZONTALMOVEMENT] = "Rechter Knüppel: Horizontale Bewegung";
  //txt[PS_RIGHTSTICKVERTICALMOVEMENT] = "Rechter Knüppel: Vertikale Bewegung";
  //txt[PS_LEFTSTICKHORIZONTALMOVEMENT] = "Linker Knüppel: Horizontale Bewegung";
  //txt[PS_LEFTSTICKVERTICALMOVEMENT] = "Linker Knüppel: Vertikal Bewegung";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Beschleunigungsmesser: Drehbewegung";
  txt[PS_ACCELEROMETERROLL] = "Beschleunigungsmesser Drehbewegung";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Beschleunigungsmesser: Nickwinkelbewegung";
  txt[PS_ACCELEROMETERPITCH] = "Beschleunigungsmesser Nickwinkelbewegung";
  txt[PS_TILTROLLSENSITIVITY] = "Kippung Drehungssensibilität";
  txt[PS_TILTPITCHSENSITIVITY] = "Kippung Nickwinkelsensibilität";
  txt[PS_TILTNEUTRALANGLE] = "Kippung neutraler Winkel";


  txt[PS_LOADOPTIONS] = "Einstellungen laden";
  txt[PS_SAVEOPTIONS] = "Einstellungen speichern";
  txt[PS_DELETEOPTIONS] = "Einstellungen löschen";
  txt[PS_LOADSCENERY] = "Landschaft laden";
  txt[PS_SAVESCENERY] = "Landschaft speichern";
  txt[PS_DELETESCENERY] = "Landschaft löschen";
  txt[PS_LOADLIGHTING] = "Lichteinstellungen laden";
  txt[PS_SAVELIGHTING] = "Lichtverhältnisse speichern";
  txt[PS_DELETELIGHTING] = "Lichtverhältnisse löschen";
  txt[PS_LOADCONTROLLER] = "Steuerung laden";
  txt[PS_SAVECONTROLLER] = "Steuerung speichern";
  txt[PS_DELETECONTROLLER] = "Steuerung löschen";
  txt[PS_LOADAEROPLANE] = "Flugzeug laden";
  txt[PS_SAVEAEROPLANE] = "Flugzeug speichern";
  txt[PS_DELETEAEROPLANE] = "Flugzeug löschen";
  txt[PS_SELECTPANORAMA] = "Panorama auswählen";
  txt[PS_SELECTTERRAINFILE] = "Gelände auswählen";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Bevorzugte Steuerung auswählen";


  // These, starting with PM_, must have no accented characters
  txt[PS_ONERACE] =
    "Die gekaufte Version enthaelt weitere Landschaften, Rennen und Flugzeuge und unterstuetzt "
    "zudem die zukuenftige Entwicklung von PicaSim. Bitte zieht daher in Erwaegung, die volle Version zu kaufen.";
  txt[PS_INVALIDRACE] = "Ungueltiges Rennen in der kostenlosen Version von PicaSim";
  txt[PS_LEADERBOARDFAILED] = "Rangliste nicht verfuegbar! - Internetverbindung pruefen";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Rangliste nicht verfuegbar! Neuer Versuch...";
  txt[PS_SCORESUBMISSIONFAILED] = "Ergebnisuebertragung gescheitert! - Internetverbindung pruefen";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Ergebnisuebertragung gescheitert! Neuer Versuch...";
  txt[PS_USERFAILED] = "Benutzeranfrage gescheitert! - Internetverbindung pruefen";
  txt[PS_USERFAILEDRETRYING] = "Benutzeranfrage gescheitert! Neuer Versuch...";

  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";

 txt[PS_QUITANDHELP] = "Spiel verlassen und Hilfe";
 txt[PS_CAMERAANDSETTINGS] = "Kamera und Einstellungen";
 txt[PS_RESETANDPLAY] = "Grundstellung und Spiel/Pause";
 txt[PS_THROTTLEANDRUDDER] = "Gas/Auftrieb und Steuer";
 txt[PS_WINDDIRECTION] = "Windrichtung";
 txt[PS_AILERONSANDELEVATOR] = "Quer- und Hoehenruder";
 txt[PS_FLYINGINFO] = "Fliegen";

 txt[PS_QUITANDHELPTEXT] = "Wenn auf Pause, druecke X um zum Hauptmenue zu kommen, und druecke ? um Hilfe zu bekommen. \n\nWaehrend des Fliegens kann man per Knopfdruck bei einigen Flugzeugen die Kontrolleinstellungen veraendern (zB Auftriebsklappen hoch/runter) oder die Seil-/Zugwinde loesen.";
 txt[PS_CAMERAANDSETTINGSTEXT] = "Wenn auf Pause, kannst du ueber das Zahnrad zu den Einstellungen gelangen. \n\nMit dem Auge kannst du die Kamera/Perspektive wechseln (nur in 3D moeglich) allows you to change the camera/viewpoint.";
 txt[PS_RESETANDPLAYTEXT] = "Drueck auf den Start/Pausenknopf um die Simuation zu starten/unterbrechen. \n\nBenutze den Ruecklaufpfeil um erneut zu starten. \n\nZusatzknoepfe 1 und 2 sind fuer Rauch.";
 txt[PS_THROTTLEANDRUDDERTEXT] = "Abhaengig vom Fugzeugtyp, kannst du entweder mit der Steuerung links/rechts das Querruder oder hoch/runter den Auftrieb oder die Bremse kontrollieren. \n\nZweikanalflieger haben diese Einstellung nicht.";
 txt[PS_WINDDIRECTIONTEXT] = "Besteht Wind, zeigt der blaue Wind dir Richtung an. \n\nGgf warden einige Zahlen angezeigt fuer Basisinformationen wie zB Hoehe und Geschwindigkeit.";
 txt[PS_AILERONSANDELEVATORTEXT] = "Als Standard kontrolliert die rechts/links Funktion das Querruder (steuerung) und die hoch/runter Funktion das Hoehenruder (Auf- und Abstieg)). \n\nR/C Piloten: Der Sendemodus kann under Einstellungen»Optionen veraendert werden.";
 txt[PS_FLYINGINFOTEXT] = "Es braucht Zeit und Muehe das FLiegen zu erlernen - Genau wie in einem realen R/C Flieger.\n\nFuer weitere Anleitung, begebe dich bitte mit dem Hilfeknopf zum Hauptmenue.";
 txt[PS_TRAINERGLIDERTIP] = 
   "Druecke Start um loszufliegen, und Pause oder Rueckstellknopf um neuzustarten. "
   "Benutze die Kontrollen in der unteren Ecke um das Hoehen- und Steuerruder zu beherrschen, damit der Segler im Auftrieb vor dem Berg fliegt.\n"
   "Benutze den Hilfsknopf fuer weitere Informationen, oder die Hilfsseite/website im Hauptmenue fuer Hinweise wie man fliegt, falls du es schwierig findest, in der Luft zu bleiben.";
 txt[PS_TRAINERPOWEREDTIP] = 
   "Druecke Start um loszufliegen, und Pause oder Rueckstellknopf um neuzustarten. "
   "Benutze die Kontrollen in der unteren rechten Ecke um das Hoehen- und Steuerruder zu beherrschen. Die Kontrollen links sind fuer das Gas/Seitenruder.\n"
   "Benutze den Hilfsknopf fuer weitere Informationen, oder die Hilfsseite/website im Hauptmenue fuer Hinweise wie man fliegt, falls du es schwierig findest, in der Luft zu bleiben.";
}

