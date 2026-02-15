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
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Aktivieren";
  txt[PS_REMOVE] = "Entfernen";
  txt[PS_DUPLICATE] = "Duplizieren";
  txt[PS_CRASHED] = "Abgestürzt";
  txt[PS_NO] = "Nein";
  txt[PS_YES] = "Ja";
  txt[PS_INVERT] = "Invertieren";
  txt[PS_AUTO] = "Auto";
  txt[PS_GAMEPADDETECTED] = "Gamepad erkannt. In Einstellungen aktivieren.";
  txt[PS_WHATSNEW] = "Willkommen bei PicaSim!";
  txt[PS_FREEFLY] = "Freies Fliegen";
  txt[PS_CHALLENGE] = "Herausforderung";
  txt[PS_SELECTAEROPLANE] = "Flugzeug";
  txt[PS_SELECTSCENERY] = "Landschaft";
  txt[PS_SELECTOPTIONS] = "Bitte passende Option wählen";
  txt[PS_SELECTSCENARIO] = "Scenario";
  txt[PS_USEDEFAULT] = "Standard benutzen";
  txt[PS_USEDEFAULTPREVIOUS] = "Benutze Standard/Vorherige";
  txt[PS_BACK] = "Zurück";
  txt[PS_OPTIONS1] = "Optionen 1";
  txt[PS_OPTIONS2] = "Optionen 2";
  txt[PS_AEROPLANE] = "Flugzeug";
  txt[PS_SCENERY] = "Landschaft";
  txt[PS_OBJECTS] = "Objekte";
  txt[PS_OBJECTSSETTINGS] = "Objekteinstellungen";
  txt[PS_LIGHTING] = "Lichtverhältnisse";
  txt[PS_AICONTROLLERS] = "KI-Steuerungen";
  txt[PS_CONTROLLER] = "Steuerung";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Laden...";
  txt[PS_SAVE] = "Speichern...";
  txt[PS_DELETE] = "Löschen...";
  txt[PS_ADVANCED] = "Fortgeschritten";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Alle Einstellungen löschen und beenden";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "Dies löscht alle Einstellungen und beendet PicaSim - sind Sie sicher?";
  txt[PS_SIMPLE] = "Einfach";
  txt[PS_FILENAME] = "Dateiname:";
  txt[PS_LANGUAGESETTINGS] = "Spracheinstellungen";
  txt[PS_CURRENTLANGUAGE] = "Aktuelle Sprache";
  txt[PS_CAMERASETTINGS] = "Kameraeinstellungen";
  txt[PS_ZOOMVIEW] = "Zoom Ansicht";
  txt[PS_PLANEONLYINZOOMVIEW] = "Flugzeug allein in Zoomansicht";
  txt[PS_SMOKEONLYINMAINVIEW] = "Rauch nur in Hauptansicht";
  txt[PS_ZOOMVIEWSIZE] = "Zoom Ansicht Skalierung";
  txt[PS_GROUNDAUTOZOOM] = "Boden Autozoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Boden Autozoom Skalierung";
  txt[PS_GROUNDFIELDOFVIEW] = "Boden Blickfeld";
  txt[PS_GROUNDHORIZONAMOUNT] = "Boden Horizont Menge";
  txt[PS_GROUNDLAG] = "Bodenkamera Verzögerung";
  txt[PS_GROUNDVIEWFOLLOW] = "Folge dem Flugzeug in Bodenperspektive";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Gier-Versatz in Bodenansicht";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Nick-Versatz in Bodenansicht";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Flugzeug Blickfeld";
  txt[PS_WALKABOUTSETTINGS] = "Rundgang Einstellungen";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Rundgangaktivierung";
  txt[PS_ENABLEOBJECTEDITING] = "Objektbearbeitung aktivieren";
  txt[PS_FORCEALLVISIBLE] = "Sichtbar beim Bearbeiten";
  txt[PS_RESETOBJECTS] = "Objekte zurücksetzen und Szene neu laden";
  txt[PS_OBJECTEDITING] = "Objektbearbeitung";
  txt[PS_OBJECTEDITINGTEXT] = "PicaSim ermöglicht das Simulieren und Zeichnen zusätzlicher Objekte, abgesehen vom Gelände und dem Flugzeug.\n"
"\n"
"Im Reiter Einstellungen»Objekte können Sie einen Satz dieser Objekte laden - dies geschieht automatisch beim Laden "
"einer Szene. Die meisten Szenen enthalten keine Objekte. Einige Panoramaszenen haben jedoch Objekte, "
"die nicht Teil des Bodens sind.\n"
"\n"
"Wenn Sie Objekte zur Szene hinzufügen möchten, können Sie die Objektbearbeitung in Einstellungen»Objekte aktivieren. "
"Dies zeigt Bildschirmtasten zum Erstellen/Löschen von Objekten, Ändern der Auswahl und ihrer Eigenschaften. Beim "
"Verschieben können Sie wählen, ob Sie im Weltraum, relativ zur Kamera oder relativ zum Objekt arbeiten möchten. "
"Vergessen Sie nach dem Erstellen nicht, die Objekte zu speichern!\n"
"\n";
  txt[PS_OBJECTNUMBER] = "Objekt %d";
  txt[PS_POSITIONX] = "Position vorwärts";
  txt[PS_POSITIONY] = "Position links";
  txt[PS_POSITIONZ] = "Position oben";
  txt[PS_COLOURR] = "Farbe rot";
  txt[PS_COLOURG] = "Farbe grün";
  txt[PS_COLOURB] = "Farbe blau";
  txt[PS_COLOURH] = "Farbton";
  txt[PS_COLOURS] = "Sättigung";
  txt[PS_COLOURV] = "Helligkeit";
  txt[PS_POSITION] = "Position";
  txt[PS_SIZE] = "Größe";
  txt[PS_COLOUR] = "Farbe";
  txt[PS_VISIBLE] = "Sichtbar";
  txt[PS_SHADOW] = "Schatten";
  txt[PS_SETWINDDIRONWALKABOUT] = "Rundgang Windrichtung festlegen";
  txt[PS_SIMULATIONSETTINGS] = "Simulationseinstellungen";
  txt[PS_TIMESCALE] = "Simulationsgeschwindigkeit";
  txt[PS_PHYSICSACCURACY] = "Physik-Genauigkeit";
  txt[PS_CONTROLLERSETTINGS] = "Steuerungseinstellungen";
  txt[PS_MODE1DESCRIPTION] = "Mode 1: Ruder & Höhenruder links, Querruder & Gashebel/Auftriebshilfe rechts";
  txt[PS_MODE2DESCRIPTION] = "Mode 2: Ruder & Gashebel/Auftriebshilfe links, Querruder & Höhenruder rechts";
  txt[PS_MODE3DESCRIPTION] = "Mode 3: Querruder & Gashebel/Auftriebshilfe links, Ruder & Höhenruder rechts";
  txt[PS_MODE4DESCRIPTION] = "Mode 4: Querruder & Höhenruder links, Ruder & Gashebel/Auftriebshilfe rechts";
  txt[PS_CONTROLLERSIZE] = "Größe";
  txt[PS_BRAKESFORWARD] = "Luftbremsen bei Knüppel vorwärts";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Absolute Touch-Position verwenden";
  txt[PS_STAGGERCONTROLLER] = "Versetzte Steuerung (für teilweisen Multitouch)";
  txt[PS_ENABLEELEVATORTRIM] = "Höhenruder-Trimmung aktivieren";
  txt[PS_ELEVATORTRIMSIZE] = "Höhenruder-Trimmung Größe";
  txt[PS_SQUARECONTROLLERS] = "Quadratische Steuerelemente";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Horizontaler Versatz";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Vertikaler Versatz";
  txt[PS_SHAPEOPACITY] = "Form Hervorhebung";
  txt[PS_STICKOPACITY] = "Knüppel Hervorhebung";
  txt[PS_STICKCROSS] = "Knüppel Kreuz";
  txt[PS_JOYSTICKID] = "Joystick ID";
  txt[PS_JOYSTICKINFO] = "Hinweis: PicaSim muss nach Anschluss/Trennung eines Joysticks neu gestartet werden.";
  txt[PS_NOJOYSTICK] = "Kein Joystick erkannt oder verfügbar.";
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
  txt[PS_THERMALWINDFIELD] = "Thermik-Windfeld zeichnen";
  txt[PS_GRAPHAIRSPEED] = "Zeige Windgeschwindigkeit (rot)";
  txt[PS_GRAPHDURATION] = "Graph-Dauer";
  txt[PS_GRAPHALTITUDE] = "Zeige Höhe (türkis)";
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
  txt[PS_CONTROLLEDPLANESHADOWS] = "Schatten gesteuertes Flugzeug";
  txt[PS_OTHERSHADOWS] = "Andere Schatten";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Projizierter Schatten Detail";
  txt[PS_SHADOWBLURMULTIPLIER] = "Schattenunsch\xC3\xA4rfe-Multiplikator";
  txt[PS_BLOB] = "Fleck";
  txt[PS_PROJECTED] = "Projiziert";
  txt[PS_AEROPLANERENDER] = "Flugzeug Darstellung";
  txt[PS_USE16BIT] = "Benutze 16 Bit Texturen um Speicher zu reduzieren (Neustart erforderlich)";
  txt[PS_SEPARATESPECULAR] = "Separate Glanzlichtberechnung";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Umgebungslicht Skalierung";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Diffuses Licht Skalierung";
  txt[PS_TERRAINTEXTUREDETAIL] = "Geländetextur Detail";
  txt[PS_MAXSKYBOXDETAIL] = "Max Skybox/Panorama Detail";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Neustart erforderlich";
  txt[PS_MISCSETTINGS] = "Verschiedene Einstellungen";
  txt[PS_USEBACKBUTTON] = "Benutze Zurrück-Button zum Schließen";
  txt[PS_DRAWLAUNCHMARKER] = "Zeichne Startmarkierung";
  txt[PS_DRAWGROUNDPOSITION] = "Zeichne bodenmarkierung";
  txt[PS_SKYGRIDOVERLAY] = "Himmelsgitter-Überlagerung";
  txt[PS_SKYGRID_NONE] = "Keines";
  txt[PS_SKYGRID_SPHERE] = "Kugel";
  txt[PS_SKYGRID_BOX] = "Box";
  txt[PS_SKYGRIDALIGNMENT] = "Gitter-Ausrichtung";
  txt[PS_SKYGRIDDISTANCE] = "Gitter-Entfernung";
  txt[PS_SKYGRIDALIGN_ALONGWIND] = "Windrichtung";
  txt[PS_SKYGRIDALIGN_CROSSWIND] = "Quer zum Wind";
  txt[PS_SKYGRIDALIGN_ALONGRUNWAY] = "Längs zur Piste";
  txt[PS_SKYGRIDALIGN_CROSSRUNWAY] = "Quer zur Piste";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Benutze vom Flugzeug bevorzugte Steuerung";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Test/Entwicklereinstellungen";
  txt[PS_WIREFRAMETERRAIN] = "Drahtgelände";
  txt[PS_DRAWSUNPOSITION] = "Zeichne Position der Sonne";
  txt[PS_FREEFLYSETTINGS] = "Freies Fliegen Einstellungen";
  txt[PS_DISPLAYFLIGHTTIME] = "Flugzeit anzeigen";
  txt[PS_DISPLAYSPEED] = "Geschwindigkeit anzeigen";
  txt[PS_DISPLAYAIRSPEED] = "Fluggeschwindigkeit anzeigen";
  txt[PS_DISPLAYMAXSPEED] = "Geschwindigkeit max anzeigen";
  txt[PS_DISPLAYASCENTRATE] = "Steigrate anzeigen";
  txt[PS_DISPLAYALTITUDE] = "Flughöhe anzeigen";
  txt[PS_DISPLAYDISTANCE] = "Abstand anzeigen";
  txt[PS_COLOURTEXT] = "Farbiger Text bei Steigrate";
  txt[PS_TEXTATTOP] = "Text oben auf dem Bildschirm";
  txt[PS_FREEFLYONSTARTUP] = "Freies Fliegen beim Start";
  txt[PS_ENABLESOCKETCONTROLLER] = "Socket-Steuerung aktivieren";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Texthintergrund Deckkraft";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Texthintergrund Farbe";
  txt[PS_MAXNUMBERAI] = "Maximale Anzahl KI";
  txt[PS_TOOMANYAI] = "Hinweis: Zu viele KI verursachen Abstürze & Verlangsamung!";
  txt[PS_UNITS] = "Einheiten";
  txt[PS_RACESETTINGS] = "Rennen/Limbo Einstellungen";
  txt[PS_RACEVIBRATIONAMOUNT] = "Vibration";
  txt[PS_RACEBEEPVOLUME] = "Piep Lautstärke";
  txt[PS_LIMBODIFFICULTY] = "Limbo Schwierigkeiten";
  txt[PS_DELETELOCALHIGHSCORES] = "Lösche lokale Höchstpunktestände";
  txt[PS_GENERALSETTINGS] = "Allgemeine Einstellungen";
  txt[PS_COLOURSCHEME] = "Farbschema";
  txt[PS_COLOUROFFSET] = "Farbverschiebung";
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
  txt[PS_SHOWBUTTON1] = "Zeige Knopf 1";
  txt[PS_SHOWBUTTON2] = "Zeige Knopf 2";
  txt[PS_LAUNCH] = "Start";
  txt[PS_HOOKS] = "Schlepphaken";
  txt[PS_BUNGEELAUNCH] = "Gummistart";
  txt[PS_AEROTOWLAUNCH] = "Flugzeugschlepp";
  txt[PS_FLATLAUNCHMETHOD] = "Startmethode (flaches Gelände)";
  txt[PS_HAND] = "Hand";
  txt[PS_BUNGEE] = "Gummi";
  txt[PS_AEROTOW] = "Flugzeugschlepp";
  txt[PS_LAUNCHSPEED] = "Start Geschwindigkeit";
  txt[PS_LAUNCHANGLE] = "Startwinkel nach oben";
  txt[PS_LAUNCHUP] = "Start nach oben";
  txt[PS_LAUNCHFORWARD] = "Start nach vorne";
  txt[PS_LAUNCHLEFT] = "Start nach links";
  txt[PS_LAUNCHOFFSETUP] = "Start-Versatz nach oben";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Neustart wenn stillstehend";
  txt[PS_CRASHDETECTION] = "Absturzerkennung";
  txt[PS_CRASHDELTAVELX] = "Delta-Geschw. vor/zurück";
  txt[PS_CRASHDELTAVELY] = "Delta-Geschw. seitlich";
  txt[PS_CRASHDELTAVELZ] = "Delta-Geschw. hoch/runter";
  txt[PS_CRASHDELTAANGVELX] = "Delta Rollgeschwindigkeit";
  txt[PS_CRASHDELTAANGVELY] = "Delta Nickgeschwindigkeit";
  txt[PS_CRASHDELTAANGVELZ] = "Delta Giergeschwindigkeit";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Fahrwerk Belastungsskala";
  txt[PS_AIRFRAME] = "Flugzeugzelle";
  txt[PS_UNDERCARRIAGE] = "Fahrwerk";
  txt[PS_PROPELLER] = "Propeller";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Bauchhaken Versatz vorwärts";
  txt[PS_BELLYHOOKOFFSETUP] = "Bauchhaken Versatz oben";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Bugschlepphaken Versatz vorwärts";
  txt[PS_NOSEHOOKOFFSETUP] = "Bugschlepphaken Versatz oben";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Heckschlepphaken Versatz vorwärts";
  txt[PS_TAILHOOKOFFSETUP] = "Heckschlepphaken Versatz oben";
  txt[PS_MAXBUNGEELENGTH] = "Maximale Hochstartgummi-Länge";
  txt[PS_MAXBUNGEEACCEL] = "Maximale Hochstartgummi-Beschleunigung";
  txt[PS_TUGPLANE] = "Schleppflugzeug";
  txt[PS_TUGSIZESCALE] = "Schlepper Größenskala";
  txt[PS_TUGMASSSCALE] = "Schlepper Massenskala";
  txt[PS_TUGENGINESCALE] = "Schlepper Motorskala";
  txt[PS_TUGMAXCLIMBSLOPE] = "Schlepper max Steigung";
  txt[PS_TUGTARGETSPEED] = "Schlepper Zielgeschwindigkeit";
  txt[PS_AEROTOWROPELENGTH] = "Schleppseil Länge";
  txt[PS_AEROTOWROPESTRENGTH] = "Schleppseil Stärke";
  txt[PS_AEROTOWROPEMASSSCALE] = "Schleppseil Massenskala";
  txt[PS_AEROTOWROPEDRAGSCALE] = "Schleppseil Widerstandsskala";
  txt[PS_AEROTOWHEIGHT] = "Schlepp max Höhe";
  txt[PS_AEROTOWCIRCUITSIZE] = "Schlepp Rundkurs Größe";
  txt[PS_TETHERING] = "Fesselflug";
  txt[PS_TETHERLINES] = "Anzahl Leinen";
  txt[PS_TETHERREQUIRESTENSION] = "Spannung für Steuerung erforderlich";
  txt[PS_TETHERPHYSICSOFFSETFORWARD] = "Physik Versatz vorwärts";
  txt[PS_TETHERPHYSICSOFFSETLEFT] = "Physik Versatz links";
  txt[PS_TETHERPHYSICSOFFSETUP] = "Physik Versatz oben";
  txt[PS_TETHERVISUALOFFSETFORWARD] = "Visueller Versatz vorwärts";
  txt[PS_TETHERVISUALOFFSETLEFT] = "Visueller Versatz links";
  txt[PS_TETHERVISUALOFFSETUP] = "Visueller Versatz oben";
  txt[PS_TETHERDISTANCELEFT] = "Leinenabstand links";
  txt[PS_TETHERCOLOURR] = "Leinenfarbe: rot";
  txt[PS_TETHERCOLOURG] = "Leinenfarbe: grün";
  txt[PS_TETHERCOLOURB] = "Leinenfarbe: blau";
  txt[PS_TETHERCOLOURA] = "Leinenfarbe: Deckkraft";
  txt[PS_CHASECAMERA] = "Kamera verfolgen";
  txt[PS_CAMERATARGETPOSFWD] = "Ziel Versatz vorwärts";
  txt[PS_CAMERATARGETPOSUP] = "Ziel Versatz oben";
  txt[PS_DISTANCE] = "Distanz";
  txt[PS_HEIGHT] = "Höhe";
  txt[PS_VERTICALVELFRAC] = "Verticaler Geschwindigkeitsanteil";
  txt[PS_FLEXIBILITY] = "Flexibilität";
  txt[PS_COCKPITCAMERA] = "Cockpitkamera";
  txt[PS_PITCH] = "Neigung";
  txt[PS_ENABLEDEBUGDRAW] = "Debug-Zeichnung aktivieren";
  txt[PS_CANTOW] = "Kann schleppen";
  txt[PS_MAXAI] = "Max KI-Steuerungen: %d (siehe Optionen 2)";
  txt[PS_ADDNEWAICONTROLLER] = "Neue KI-Steuerung hinzufügen";
  txt[PS_REMOVEAICONTROLLERS] = "Alle KI-Steuerungen entfernen";
  txt[PS_LAUNCHSEPARATIONDISTANCE] = "Start-Trennungsabstand";
  txt[PS_INCLUDEINCAMERAVIEWS] = "Kameras aktivieren";
  txt[PS_CREATEMAXNUMCONTROLLERS] = "Max KI-Steuerungen erstellen";
  txt[PS_LAUNCHDIRECTION] = "Startrichtung";
  txt[PS_RANDOMCOLOUROFFSET] = "Zufälliger Farbversatz";
  txt[PS_AICONTROLLER] = "KI-Steuerung";
  txt[PS_AINAVIGATION] = "KI-Navigation";
  txt[PS_PLANETYPE] = "Flugzeugtyp";
  txt[PS_USER] = "Benutzer";
  txt[PS_ALL] = "Alle";
  txt[PS_GLIDER] = "Segler";
  txt[PS_GLIDERS] = "Segler";
  txt[PS_POWERED] = "Motorisiert";
  txt[PS_HELI] = "Hubschrauber";
  txt[PS_CONTROLLINE] = "Fesselflug";
  txt[PS_PANORAMIC] = "Panorama";
  txt[PS_3D] = "3D";
  txt[PS_ALLOWAICONTROL] = "KI-Steuerung erlauben";
  txt[PS_WAYPOINTTOLERANCE] = "Wegpunkt-Toleranz";
  txt[PS_MINSPEED] = "Min Geschwindigkeit";
  txt[PS_CRUISESPEED] = "Reisegeschwindigkeit";
  txt[PS_MAXBANKANGLE] = "Max Schräglage";
  txt[PS_BANKANGLEPERHEADINGCHANGE] = "Schräglage pro Kursänderung";
  txt[PS_SPEEDPERALTITUDECHANGE] = "Geschw. pro Höhenänderung";
  txt[PS_GLIDESLOPEPEREXCESSSPEED] = "Gleitpfad pro Übergeschw.";
  txt[PS_PITCHPERROLLANGLE] = "Neigung pro Rollwinkel";
  txt[PS_HEADINGCHANGEFORNOSLOPE] = "Kursänderung ohne Hang";
  txt[PS_MAXPITCHCONTROL] = "Max Nicksteuerung";
  txt[PS_MAXROLLCONTROL] = "Max Rollsteuerung";
  txt[PS_CONTROLPERROLLANGLE] = "Steuerung pro Rollwinkel";
  txt[PS_PITCHCONTROLPERGLIDESLOPE] = "Nicksteuerung pro Gleitpfad";
  txt[PS_ROLLTIMESCALE] = "Roll-Zeitskala";
  txt[PS_PITCHTIMESCALE] = "Nick-Zeitskala";
  txt[PS_MINALTITUDE] = "Min Höhe";
  txt[PS_SLOPEMINUPWINDDISTANCE] = "Hang min Luv-Abstand";
  txt[PS_SLOPEMAXUPWINDDISTANCE] = "Hang max Luv-Abstand";
  txt[PS_SLOPEMINLEFTDISTANCE] = "Hang min links Abstand";
  txt[PS_SLOPEMAXLEFTDISTANCE] = "Hang max links Abstand";
  txt[PS_SLOPEMINUPDISTANCE] = "Hang min oben Abstand";
  txt[PS_SLOPEMAXUPDISTANCE] = "Hang max oben Abstand";
  txt[PS_SLOPEMAXWAYPOINTTIME] = "Hang max Wegpunktzeit";
  txt[PS_FLATMAXDISTANCE] = "Flach max Wegpunktzeit";
  txt[PS_FLATMAXWAYPOINTTIME] = "Flach max Wegpunktzeit";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Aktuelle Position";
  txt[PS_MASS] = "Gewicht";
  txt[PS_INERTIA] = "Trägheit";
  txt[PS_WINGAREA] = "Flügelfläche";
  txt[PS_EXTENTS] = "Abmessungen";
  txt[PS_WINDSETTINGS] = "Wind Einstellungen";
  txt[PS_WINDSPEED] = "Windgeschwindigkeit";
  txt[PS_WINDBEARING] = "Windrichtung";
  txt[PS_CANNOTMODIFYSCENERY] = "Ihr könnt die Landschaft während eines Rennens nicht verändern";
  txt[PS_ALLOWBUNGEE] = "Start mit Hochstartgummi erlauben";
  txt[PS_WINDGUSTTIME] = "Windböenhäufigkeit";
  txt[PS_WINDGUSTFRACTION] = "Windböenanteil";
  txt[PS_WINDGUSTANGLE] = "Windböenwinkel";
  txt[PS_TURBULENCEAMOUNT] = "Turbulenzmenge";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Bodenturbulenz";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Windscherungsturbulenz";
  txt[PS_WINDLIFTSMOOTHING] = "Windauftriebsglättung";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertikaler Wind Abklinghöhe";
  txt[PS_SEPARATIONTENDENCY] = "Abtrennungstendenz";
  txt[PS_ROTORTENDENCY] = "Rotortendenz";
  txt[PS_DEADAIRTURBULENCE] = "Tote Luft Turbulenz";
  txt[PS_BOUNDARYLAYERDEPTH] = "Grenzschichtdicke";
  txt[PS_THERMALSETTINGS] = "Thermik Einstellungen (siehe auch Licht Einstellungen)";
  txt[PS_DENSITY] = "Dichte";
  txt[PS_RANGE] = "Reichweite";
  txt[PS_LIFESPAN] = "Lebensdauer";
  txt[PS_DEPTH] = "Tiefe";
  txt[PS_CORERADIUS] = "Kern Radius";
  txt[PS_DOWNDRAFTEXTENT] = "Abwind Ausdehnung";
  txt[PS_UPDRAFTSPEED] = "Aufwind Geschwindigkeit";
  txt[PS_ASCENTRATE] = "Steigrate";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Ausdehnung über Lebensdauer";
  txt[PS_RUNWAY] = "Start-/Landebahn";
  txt[PS_RUNWAYTYPE] = "Bahntyp";
  txt[PS_CIRCLE] = "Kreis";
  txt[PS_RUNWAYX] = "Position X";
  txt[PS_RUNWAYY] = "Position Y";
  txt[PS_RUNWAYHEIGHT] = "Höhe";
  txt[PS_RUNWAYANGLE] = "Winkel";
  txt[PS_RUNWAYLENGTH] = "Länge/Radius";
  txt[PS_RUNWAYWIDTH] = "Breite";
  txt[PS_SURFACESETTINGS] = "Oberflächeneinstellungen";
  txt[PS_SURFACEROUGHNESS] = "Rauheit";
  txt[PS_SURFACEFRICTION] = "Reibung";
  txt[PS_RANDOMTERRAINSETTINGS] = "Zufälliges Gelände Einstellung";
  txt[PS_RANDOMSEED] = "Impfnummer";
  txt[PS_DISPLACEMENTHEIGHT] = "Versetzungshöhe";
  txt[PS_SMOOTHNESS] = "Glätte";
  txt[PS_EDGEHEIGHT] = "Geländekantenhöhe";
  txt[PS_UPWARDSBIAS] = "Aufwärtsneigung";
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
  txt[PS_PANORAMA3DSETTINGS] = "Panorama 3D Einstellungen";
  txt[PS_HEIGHTFIELDSETTINGS] = "Höhenfeldeinstellungen";
  txt[PS_MINHEIGHT] = "Minimale Höhe";
  txt[PS_MAXHEIGHT] = "Maximale Höhe";
  txt[PS_AISCENERY] = "KI-Landschaft";
  txt[PS_SCENETYPE] = "Szenentyp";
  txt[PS_FLAT] = "Flach";
  txt[PS_SLOPE] = "Hang";
  txt[PS_CURRENTVIEWPOSITION] = "Momentaner Standpunkt";
  txt[PS_NOLIGHTINGSETTINGS] = "Keine Lichteinstellungen, da Geländetyp Panorama";
  txt[PS_SUNBEARING] = "Sonnenpeilung";
  txt[PS_THERMALACTIVITY] = "Thermikaktivität";
  txt[PS_TERRAINDARKNESS] = "Gelände Dunkelheit";
  txt[PS_CONTROLLERSEESETTINGS] = "Siehe auch zusätzliche Steuerungseinstellungen unter Optionen";
  txt[PS_CROSS] = "Kreuz";
  txt[PS_BOX] = "Box";
  txt[PS_CROSSANDBOX] = "Kreuz & Box";
  txt[PS_STYLE] = "Stil";
  txt[PS_CONTROLLERMODE] = "Steuerungsmodus";
  txt[PS_TREATTHROTTLEASBRAKES] = "Gasknüppel als Bremse verwenden";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Konfiguration beim Start zurücksetzen";
  txt[PS_NUMCONFIGURATIONS] = "Anzahl Konfigurationen";
  txt[PS_TILTHORIZONTAL] = "Kippung horizontal";
  txt[PS_TILTVERTICAL] = "Kippung vertikal";
  txt[PS_ARROWSHORIZONTAL] = "Pfeile horizontal";
  txt[PS_ARROWSVERTICAL] = "Konstant";
  txt[PS_ROLLSTICK] = "Rollknüppel";
  txt[PS_PITCHSTICK] = "Nickknüppel";
  txt[PS_YAWSTICK] = "Gierknüppel";
  txt[PS_SPEEDSTICK] = "Gasknüppel";
  txt[PS_CONSTANT] = "Konstant";
  txt[PS_BUTTON0] = "Taste 1";
  txt[PS_BUTTON1] = "Taste 2";
  txt[PS_BUTTON2] = "Taste 3";
  txt[PS_BUTTON0TOGGLE] = "Taste 1 Umschalten";
  txt[PS_BUTTON1TOGGLE] = "Taste 2 Umschalten";
  txt[PS_BUTTON2TOGGLE] = "Taste 3 Umschalten";
  txt[PS_MIXES] = "Mischungen";
  txt[PS_ELEVATORTOFLAPS] = "Höhenruder zu Klappen";
  txt[PS_AILERONTORUDDER] = "Querruder zu Seitenruder";
  txt[PS_FLAPSTOELEVATOR] = "Klappen zu Höhenruder";
  txt[PS_BRAKESTOELEVATOR] = "Bremse/Gas zu Höhenruder";
  txt[PS_RUDDERTOELEVATOR] = "Seitenruder zu Höhenruder";
  txt[PS_RUDDERTOAILERON] = "Seitenruder zu Querruder";
  txt[PS_RATESBUTTON] = "Raten-Taste";
  txt[PS_RATESCYCLEBUTTON] = "Raten-Zyklus-Taste";
  txt[PS_RELAUNCHBUTTON] = "Neustart-Taste";
  txt[PS_CAMERABUTTON] = "Blickpunkt-Taste";
  txt[PS_PAUSEPLAYBUTTON] = "Pause/Start-Taste";
  txt[PS_NONE] = "Keine";
  txt[PS_CONTROLSOURCES] = "Kontrollursprung";
  txt[PS_AILERONS] = "Querruder/Ruder";
  txt[PS_ELEVATOR] = "Höhenruder";
  txt[PS_RUDDER] = "Ruder";
  txt[PS_THROTTLE] = "Gashebel/Auftriebshilfe/Bremsen";
  txt[PS_LOOKYAW] = "Ansicht Gierachse";
  txt[PS_LOOKPITCH] = "Ansicht Nickwinkel";
  txt[PS_AUX1] = "Aux 1";
  txt[PS_SMOKE1] = "Rauch 1";
  txt[PS_SMOKE2] = "Rauch 2";
  txt[PS_HOOK] = "Haken";
  txt[PS_VELFWD] = "Geschw. vorwärts";
  txt[PS_VELLEFT] = "Geschw. links";
  txt[PS_VELUP] = "Geschw. oben";
  txt[PS_MAXPARTICLES] = "Max Partikelanzahl";
  txt[PS_CHANNELFOROPACITY] = "Kanal für Deckkraft";
  txt[PS_MINOPACITY] = "Min Deckkraft";
  txt[PS_MAXOPACITY] = "Max Deckkraft";
  txt[PS_CHANNELFORRATE] = "Kanal für Rate";
  txt[PS_MINRATE] = "Min Rate";
  txt[PS_MAXRATE] = "Max Rate";
  txt[PS_INITIALSIZE] = "Anfangsgröße";
  txt[PS_FINALSIZE] = "Endgröße";
  txt[PS_DAMPINGTIME] = "Dämpfungszeit";
  txt[PS_JITTER] = "Schwankung";
  txt[PS_ENGINEWASH] = "Motorabgasmenge";
  txt[PS_HUECYCLEFREQ] = "Farbton-Zyklusfrequenz";
  txt[PS_SETTINGSFORCONTROLLER] = "Einstellungen für Steuerungskonfiguration %d: %s";
  txt[PS_TRIMSETTINGS] = "Trimmungseinstellungen";
  txt[PS_NOSIMPLESETTINGS] = "Keine einfachen Einstellungen verfügbar";
  txt[PS_TRIM] = "Trimmung";
  txt[PS_SCALE] = "Maßstab";
  txt[PS_CLAMPING] = "Begrenzung";
  txt[PS_EXPONENTIAL] = "Exponential";
  txt[PS_SPRING] = "Feder";
  txt[PS_POSITIVE] = "Positiv";
  txt[PS_NEGATIVE] = "Negativ";
  txt[PS_ROLLSTICKMOVEMENT] = "Rollknüppel-Bewegung";
  txt[PS_PITCHSTICKMOVEMENT] = "Nickknüppel-Bewegung";
  txt[PS_YAWSTICKMOVEMENT] = "Gierknüppel-Bewegung";
  txt[PS_SPEEDSTICKMOVEMENT] = "Gasknüppel-Bewegung";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Beschleunigungsmesser: Drehbewegung";
  txt[PS_ACCELEROMETERROLL] = "Beschleunigungsmesser Drehbewegung";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Beschleunigungsmesser: Nickwinkelbewegung";
  txt[PS_ACCELEROMETERPITCH] = "Beschleunigungsmesser Nickwinkelbewegung";
  txt[PS_TILTROLLSENSITIVITY] = "Kippung Drehungssensibilität";
  txt[PS_TILTPITCHSENSITIVITY] = "Kippung Nickwinkelsensibilität";
  txt[PS_TILTNEUTRALANGLE] = "Kippung neutraler Winkel";
  txt[PS_NOJOYSTICKWITHID] = "Kein Joystick mit dieser ID";
  txt[PS_ENABLEJOYSTICK] = "Joystick aktivieren";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Anpassung an kreisende Knüppelbewegung";
  txt[PS_EXTERNALJOYSTICKSETTINGS] = "Externe Joystick-Einstellungen";
  txt[PS_JOYSTICKLABEL] = "Joystick %d: Input = %5.2f Output = %5.2f";
  txt[PS_JOYSTICKBUTTONLABEL] = "Joystick button %d: Input = %5.2f Output = %d";
  txt[PS_SCALEPOSITIVE] = "Eingabe +ve Skala";
  txt[PS_SCALENEGATIVE] = "Eingabe -ve Skala";
  txt[PS_OFFSET] = "Eingabe Versatz";
  txt[PS_MAPTO] = "Zuordnen zu";
  txt[PS_PRESSWHENCENTRED] = "Drücken wenn zentriert";
  txt[PS_PRESSWHENLEFTORDOWN] = "Drücken wenn links oder unten";
  txt[PS_PRESSWHENRIGHTORUP] = "Drücken wenn rechts oder oben";
  txt[PS_DEADZONE] = "Totzone";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Joystick-Einstellungen löschen";
  txt[PS_CALIBRATEJOYSTICK] = "Joystick-Kalibrierung starten (Windows)";
  txt[PS_LOADOPTIONS] = "Einstellungen laden";
  txt[PS_SAVEOPTIONS] = "Einstellungen speichern";
  txt[PS_DELETEOPTIONS] = "Einstellungen löschen";
  txt[PS_LOADSCENERY] = "Landschaft laden";
  txt[PS_SAVESCENERY] = "Landschaft speichern";
  txt[PS_DELETESCENERY] = "Landschaft löschen";
  txt[PS_LOADOBJECTS] = "Objekte laden";
  txt[PS_SAVEOBJECTS] = "Objekte speichern";
  txt[PS_DELETEOBJECTS] = "Objekte löschen";
  txt[PS_LOADLIGHTING] = "Lichteinstellungen laden";
  txt[PS_SAVELIGHTING] = "Lichtverhältnisse speichern";
  txt[PS_DELETELIGHTING] = "Lichtverhältnisse löschen";
  txt[PS_LOADCONTROLLER] = "Steuerung laden";
  txt[PS_SAVECONTROLLER] = "Steuerung speichern";
  txt[PS_DELETECONTROLLER] = "Steuerung löschen";
  txt[PS_LOADJOYSTICK] = "Joystick laden";
  txt[PS_SAVEJOYSTICK] = "Joystick speichern";
  txt[PS_DELETEJOYSTICK] = "Joystick löschen";
  txt[PS_LOADAEROPLANE] = "Flugzeug laden";
  txt[PS_SAVEAEROPLANE] = "Flugzeug speichern";
  txt[PS_DELETEAEROPLANE] = "Flugzeug löschen";
  txt[PS_LOADAICONTROLLERS] = "KI-Steuerungen laden";
  txt[PS_SAVEAICONTROLLERS] = "KI-Steuerungen speichern";
  txt[PS_DELETEAICONTROLLERS] = "KI-Steuerungen löschen";
  txt[PS_SELECTPANORAMA] = "Panorama auswählen";
  txt[PS_SELECTTERRAINFILE] = "Gelände auswählen";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Bevorzugte Steuerung auswählen";
  txt[PS_SELECTOBJECTSSETTINGS] = "Objekteinstellungen auswählen";
  txt[PS_SUMMARY] = "Zusammenfassung";
  txt[PS_NOOBJECTS] = "Es sind keine Objekte geladen (außer Gelände usw.). Sie können statische und dynamische Objekte laden, "
    "die für die aktuelle Szene geeignet sind. Durch Aktivieren der Objektbearbeitung können Sie "
    "eigene erstellen - siehe Hilfe und die PicaSim-Webseite für weitere Infos.";
  txt[PS_OBJECTSTOTAL] = "Gesamtzahl Objekte:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Anzahl statisch sichtbar:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Anzahl statisch unsichtbar:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Anzahl dynamisch sichtbar:";
  txt[PS_ABOUT] = "Über";
  txt[PS_WEBSITE] = "Webseite";
  txt[PS_FLYING] = "Fliegen";
  txt[PS_SETTINGS] = "Einstellungen";
  txt[PS_RACES] = "Rennen";
  txt[PS_TIPS] = "Tipps";
  txt[PS_KEYBOARD] = "Tastatur";
  txt[PS_CREDITS] = "Danksagung";
  txt[PS_LICENCE] = "Lizenz";
  txt[PS_VERSIONS] = "Versionen";
  txt[PS_ABOUTPAIDTEXT] = "PicaSim ist ein Simulator für ferngesteuerte Flugzeuge. Gegenwärtig ist er darauf angelegt Euch beim Fliegenlernen zu helfen und Eure Hangflugfähigkeiten zu verbessern- den Segelflieger mit dem Auftrieb des Windes, so wie er an Anhöhen hinauf- und herüberströmt, zu fliegen.\n"
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
    "Ich habe viele Ideen PicaSim zu erweitern und zu verbessern- ich möchte Motorflugzeuge sowie weitere Flugmodelle aufnehmen, vernetztes Fliegen und computergesteuerte Flugzeuge mit einschliessen. Eure kleine Spende hilft mir dabei, dies zu tun und ich bin Euch dafür sehr dankbar.\n";
  txt[PS_HOWTOFLYTEXT] = "Als erstes schaut Ihr Euch am besten die Webseite (siehe Link unten) und das Video (\"Wie man fliegt\") dort an.\n"
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
    "Schaut Euch die Webseite unten an, dort sind einige Vidoes, die zeigen, wie das Fliegen funktioniert- es braucht ein wenig Übung, genau wie mit einem echten ferngesteuerten Modellflugzeug.\n";
  txt[PS_HOWTOCONFIGURETEXT] = "Ihr bekommt entweder über das Startmenü Zugriff auf PicaSims Einstellungen oder über das Klicken irgendeines Symbols, wenn PicaSim stillsteht.\n"
    "\n"
    "Die Einstellungen sind in mehrere Gruppen unterteilt- allgemeine Optionen, das Flugzeug, die Landschaft/das Gelände, Lichtverhältnisse und Steuerungskonfigurationen. Es ist prinzipiell nicht möglich, mit den meisten der detaillierten Einstellungen jeder einzelnen Gruppe zu frickeln, aber Ihr könnt voreingestellte konfigurierte Gruppen laden. Hiermit könnt Ihr beispielsweise schnell Flugzeuge oder das Gelände wechseln.\n"
    "\n"
    "Aktiviert Ihr in jedem Abschnitt die 'Einstellungen für Fortgeschrittene', werdet Ihr sehr viel mehr Feineinstellungen vornehmen können, was allerdings etwas überwältigend sein könnte! Im Laufe der Zeit werde ich detailliertere Informationen über diese Einstellungen liefern, dennoch könnt Ihr bereits einfach risikolos mit den Werten herumfrickeln wie Euch beliebt, da Ihr sie jederzeit wieder mit einem der voreingestellten Gruppen nachladen könnt.\n"
    "\n"
    "Ihr könnt Eure Voreinstellungen speichern- entweder auf der Speicherkarte oder dem Datenbereich Eures Gerätes. Solltet Ihr Probleme mit PicaSim haben, nachdem Ihr die Einstellungen verändert habt, lohnt es sich, sofort nach Start von PicaSim jeden Abschnitt einzeln nacheinander neuzuladen.\n"
    "\n"
    "Ist der 'Rundgang' in den Optionen aktiviert, dann gibt es auch im Stillstand eine 'Rundgangtaste'. Diese ermöglicht Euch mit Hilfe der Steuerung im Gelände umherzulaufen und einen neuen Abflugstandort ausfindig zu machen. Die Windrichtung ist in diesem Modus per Standardeinstellung so eingestellt, dass der Wind immer direkt auf die Kamera zuweht.\n"
    "\n"
    "Bei den Panoramaansichten ist es nicht möglich, den Standpunkt zu verändern, daher ist der Rundgangmodus hier ausgeschaltet.\n";
  txt[PS_HOWTORACETEXT] = "Bei den Rennen geht es darum, so schnell wie möglich zwischen zwei Kontrollpunkten hin- und herzufliegen. Der nächste anzupeilende Kontrollpunkt ist farblich hervorgehoben, zusätzlich zeigt der weisse Pfeil unten im Bildschirm in seine Richtung.\n"
    "\n"
    "Es gibt verschiedene Arten von Kontrollpunkten: Bei den Rennen im F3F- Stil, in denen Ihr immer hin- und herfliegt, müsst Ihr im Luv an den Kontrollpunkten vorbeifliegen. Beim Geländerennen hingegen braucht Ihr nur in die Nähe der Kontrollpunkte zu kommen, könnt aber beliebig hoch sein. "
    "Beim Flachlandrennen fliegt Ihr zwischen zwei Toren - das Tor im Aufwind ist grün, das Tor, das sich im Gegenwind befindet, ist rot.\n";
  txt[PS_TIPSTEXT] = "Hier sind einige Tipps, um das Beste aus PicaSim herauszuholen:\n"
"\n"
"• Denken Sie daran, dass es ein Simulator für ferngesteuerte Flugzeuge ist - der Fokus liegt auf realistischer Steuerung, "
"wobei der \"Pilot\" am Boden steht. Fliegen lernen erfordert Übung - besuchen Sie die Website unten für Hilfe.\n"
"\n"
"• Experimentieren Sie mit den Einstellungen, besonders wenn Sie R/C-Pilot sind! Falls etwas schiefgeht, laden Sie "
"direkt nach dem Start von PicaSim eine Voreinstellung für die entsprechenden Abschnitte.\n"
"\n"
"• Bei Problemen oder Vorschlägen, schreiben Sie an picasimulator@gmail.com\n"
"\n"
"• Panoramen können in \"normal\" und \"maximum\" Detail angezeigt werden - probieren Sie die höhere Detailstufe aus. Falls "
"PicaSim beim Laden abstürzt, reduzieren Sie das Detail in Einstellungen»Optionen 1.\n"
"\n"
"• KI-Flugzeuge werden an zwei Stellen konfiguriert. Einstellungen»Optionen 2 legt die maximale Anzahl fest. "
"Einstellungen»KI-Steuerungen bestimmt, welche Flugzeuge tatsächlich geladen werden.\n";
  txt[PS_KEYBOARDTEXT] = "Mit einer Tastatur gibt es folgende Tastenkürzel:\n"
"\n"
"[r] - Neustart\n"
"[p] - Pause umschalten\n"
"[c] - Kameraansichten durchschalten\n"
"[z] - Zoomansicht umschalten\n";
  txt[PS_JOYSTICKSETUPTEXT] = "PicaSim unterstützt externe USB-Joysticks/Gamepads unter Android 3.0+ und Windows. Auf Android-Geräten "
"ist ein USB-OTG-Adapter erforderlich, und Ihr Gerät muss dies unterstützen - bitte prüfen Sie das zuerst. "
"Wenn der Joystick-Reiter in den Einstellungen erscheint, besteht eine gute Chance, dass externe USB-Controller unterstützt werden, "
"aber es ist nicht garantiert!\n"
"\n"
"Der externe Controller muss vor dem Starten von PicaSim angeschlossen sein. Unter Windows wird empfohlen, den Joystick mit "
"der \"Kalibrieren\"-Taste in Einstellungen»Joystick zu kalibrieren.\n"
"\n"
"Wenn Ihr Joystick erkannt und in Einstellungen»Joystick ausgewählt ist, müssen Sie die physischen Eingaben den von PicaSim "
"erkannten Steuerelementen zuweisen. Dies geschieht durch Identifizieren, welche Joystick-Achse welcher Steuerungseingabe entspricht. "
"Dann gibt es Tasten zum Drücken, wenn der Stick in der Mitte, links/unten oder rechts/oben ist. "
"Dieses Verfahren ist unter Windows und Android gleich - besuchen Sie die Website für ein ausführliches Video.";
  txt[PS_TIPS1] = "Bitte schickt mir Rüchmeldungen, damit ich PicaSim verbessern kann";
  txt[PS_TIPS2] = "Gefällt Euch PicaSim, hinterlasst bitte eine Bewertung";
  txt[PS_TIPS3] = " Ihr könnt den Beschleunigungssensor benutzen, um das Modell zu steuern - geht zu Einstellungen»Steuerung";
  txt[PS_TIPS4] = "Folgt PicaSim auf Facebook oder Google+ und erhaltet Benachrichtigungen über Aktualisierungen und zukünftige Entwicklungen";
  txt[PS_TIPS5] = "Wenn Ihnen PicaSim gefällt, hinterlassen Sie bitte eine Bewertung bei den mobilen Versionen";
  txt[PS_TIPS6] = "Wechseln Sie zwischen Modus 1 und 2 usw. in den Optionen";
  txt[PS_TIPS7] = "Bitte senden Sie mir Feedback, damit ich PicaSim verbessern kann";
  txt[PS_TIPS8] = "Bei niedriger Bildrate versuchen Sie, eine niedrigere Qualitätseinstellung zu laden und/oder die Physikqualität zu reduzieren";
  txt[PS_TIPS9] = "Auf Mobilgeräten versuchen Sie den Beschleunigungssensor durch Laden einer geeigneten Voreinstellung in Einstellungen»Steuerung";
  txt[PS_TIPS10] = "Ballast bei Seglern erhöht die Fähigkeit, gegen den Wind zu fliegen, auf Kosten schnelleren Sinkens";
  txt[PS_TIPS11] = "Aktivieren Sie den 'Rundgang'-Modus in 3D-Szenen und nutzen Sie die Geh-Taste bei Pause zum Verschieben des Startpunkts";
  txt[PS_TIPS12] = "Kreisende Habichte zeigen Thermikbereiche an, aber achten Sie besser auf das Heben der Flügelspitze oder Nase des Seglers";
  txt[PS_TIPS13] = "Echte R/C-Flugzeuge zu fliegen macht noch mehr Spaß!";
  txt[PS_TIPS14] = "Folgen Sie PicaSim auf Facebook, um über Updates und Pläne informiert zu werden";
  txt[PS_TIPS15] = "Es gibt ein Forum für PicaSim auf Tapatalk";
  txt[PS_TIPS16] = "Fügen Sie KI-gesteuerte Flugzeuge in Einstellungen»KI-Steuerungen hinzu. Begrenzen Sie die Anzahl in Einstellungen»Optionen 2";
  txt[PS_LOADING] = "Lädt...";
  txt[PS_RACETIP] = "%s\n\nDie weissen Pfeile zeigen in die Richtung der nächsten Kontrollpunkte.";
  txt[PS_LIMBOTIP] = "%s\n\nStelle den Schwierigkeitsgrad/Ergebnismultiplikator in Einstellungen»Modus";
  txt[PS_DURATIONTIP] = "%s\n\nSuche nach Thermik und lande nahe dem Startpunkt.";
  txt[PS_TRAINERGLIDERTIP] = "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
    "Benuzt den Kontrollknüppel um das Ruder und Höhenruder zu steuern, damit der Segelflieger mit dem Auftrieb vor dem Hügel fliegt. "
    "Seht auf der Hilfe/Website nach um Tipps zu erhalten falls es euch schwerfällt in der Luft zu bleiben.";
  txt[PS_TRAINERPOWEREDTIP] = "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
    "Benuzt den Kontrollknüppel rechts um das Höhenruder und Querruder zu steuern, und links um den Gashebel und Ruder zu steuern. "
    "Seht auf der Hilfe/Website nach um Tipps zu erhalten falls es euch schwerfällt in der Luft zu bleiben.";
  txt[PS_SELECTRACE] = "Rennen auswählen";
  txt[PS_PREV] = "Vorher";
  txt[PS_NEXT] = "Nächster";
  txt[PS_QUITANDHELP] = "Spiel verlassen und Hilfe";
  txt[PS_CAMERAANDSETTINGS] = "Kamera und Einstellungen";
  txt[PS_RESETANDPLAY] = "Grundstellung und Spiel/Pause";
  txt[PS_THROTTLEANDRUDDER] = "Gas/Auftrieb und Steuer";
  txt[PS_WINDDIRECTION] = "Windrichtung";
  txt[PS_AILERONSANDELEVATOR] = "Quer- und Hoehenruder";
  txt[PS_FLYINGINFO] = "Fliegen";
  txt[PS_QUITANDHELPTEXT] = "Wenn auf Pause, druecke X um zum Hauptmenue zu kommen, und druecke ? um Hilfe zu bekommen. \n\nWaehrend des Fliegens kann man per Knopfdruck bei einigen Flugzeugen die Kontrolleinstellungen veraendern (zB Auftriebsklappen hoch/runter) oder die Seil-/Zugwinde loesen.";
  txt[PS_CAMERAANDSETTINGSTEXT] = "Wenn auf Pause, kannst du ueber das Zahnrad zu den Einstellungen gelangen. \n\nMit dem Auge kannst du die Kamera/Perspektive wechseln (nur in 3D moeglich).";
  txt[PS_RESETANDPLAYTEXT] = "Drueck auf den Start/Pausenknopf um die Simuation zu starten/unterbrechen. \n\nBenutze den Ruecklaufpfeil um erneut zu starten. \n\nZusatzknoepfe 1 und 2 sind fuer Rauch.";
  txt[PS_THROTTLEANDRUDDERTEXT] = "Abhaengig vom Fugzeugtyp, kannst du entweder mit der Steuerung links/rechts das Querruder oder hoch/runter den Auftrieb oder die Bremse kontrollieren. \n\nZweikanalflieger haben diese Einstellung nicht.";
  txt[PS_WINDDIRECTIONTEXT] = "Besteht Wind, zeigt der blaue Wind dir Richtung an. \n\nGgf warden einige Zahlen angezeigt fuer Basisinformationen wie zB Hoehe und Geschwindigkeit.";
  txt[PS_AILERONSANDELEVATORTEXT] = "Als Standard kontrolliert die rechts/links Funktion das Querruder (steuerung) und die hoch/runter Funktion das Hoehenruder (Auf- und Abstieg)). \n\nR/C Piloten: Der Sendemodus kann under Einstellungen»Optionen veraendert werden.";
  txt[PS_FLYINGINFOTEXT] = "Es braucht Zeit und Muehe das FLiegen zu erlernen - Genau wie in einem realen R/C Flieger.\n\nFuer weitere Anleitung, begebe dich bitte mit dem Hilfeknopf zum Hauptmenue.";
  txt[PS_VRHEADSET] = "VR-Headset";
  txt[PS_ENABLEVRMODE] = "VR-Modus aktivieren";
  txt[PS_HEADSET] = "Headset";
  txt[PS_NOTHING] = "Nichts";
  txt[PS_LEFTEYE] = "Linkes Auge";
  txt[PS_RIGHTEYE] = "Rechtes Auge";
  txt[PS_BOTHEYES] = "Beide Augen";
  txt[PS_BOTHEYESCROSSED] = "Kreuzblick";
  txt[PS_NORMALVIEW] = "Normale Ansicht";
  txt[PS_VRDESKTOP] = "VR-Desktop";
  txt[PS_VRANTIALIASING] = "VR-Kantenglättung";
  txt[PS_NONEUSEDEFAULT] = "Keine (Standard)";
  txt[PS_VRAUDIO] = "VR-Audio";
  txt[PS_STATUS] = "Status";
  txt[PS_VRNOTAVAILABLE] = "VR nicht verfügbar - kein Headset erkannt";
  txt[PS_VRPANORAMADEPTH] = "VR-Panoramatiefe";
  txt[PS_VRPANORAMAEXTENSION] = "VR Panorama Erweiterung";
  txt[PS_VROVERLAYDISTANCE] = "VR-Overlay-Abstand";
  txt[PS_VROVERLAYSCALE] = "VR-Overlay-Skalierung";
  txt[PS_MIRROR_TEXT] = "VR-Headset wird verwendet";
  txt[PS_MIRROR_SETTINGS] = "VR-Anzeige in Einstellungen spiegeln";
  txt[PS_ORIENTATION] = "Ausrichtung";
  txt[PS_SMOKESOURCE] = "Rauchquelle %d";
  txt[PS_NAME] = "Name";
  txt[PS_BUTTONMAPPINGS] = "Tastenzuordnungen";
  txt[PS_USERFILES] = "Benutzerdateien:";
  txt[PS_NOFILESFOUND] = "Keine Dateien gefunden";
  txt[PS_FAILEDTOLOADENVIRONMENT] = "Umgebung konnte nicht geladen werden - bitte laden Sie eine der Landschaftsvoreinstellungen und versuchen Sie es erneut";
}

