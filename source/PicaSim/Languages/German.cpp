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
  txt[PS_ENABLE] = "Enable";
  txt[PS_REMOVE] = "Remove";
  txt[PS_DUPLICATE] = "Duplicate";
  txt[PS_CRASHED] = "Abgestürzt";
  txt[PS_NO] = "No";
  txt[PS_YES] = "Yes";
  txt[PS_INVERT] = "Invert";
  txt[PS_AUTO] = "Auto";
  txt[PS_FREEFLY] = "Freies Fliegen";
  txt[PS_RACE] = "Rennen";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Vollversion";
  txt[PS_INTRIALMODE] = "In trial mode - purchase full version to access all planes and sceneries.";
  txt[PS_GAMEPADDETECTED] = "Gamepad detected. Enable in the settings to use it.";
  txt[PS_WHATSNEW] = "Welcome to PicaSim!";
  txt[PS_WHATSNEWTEXT] = "Here's what's new in this version (or recent versions):\n\n"
"• Add the Quark glider.\n"
"• Update various links etc in plane descriptions.\n"
"• Various tweaks/bug fixes.\n"
"\nPlease contact picasimulator@gmail.com with bug reports and questions.\n";
  txt[PS_ONERACE] = "Die gekaufte Version enthaelt weitere Landschaften, Rennen und Flugzeuge und unterstuetzt "
    "zudem die zukuenftige Entwicklung von PicaSim. Bitte zieht daher in Erwaegung, die volle Version zu kaufen.";
  txt[PS_FULLVERSION] = "The full version contains all the planes/sceneries etc - the small purchase price will help future development of PicaSim.";
  txt[PS_SELECTAEROPLANE] = "Flugzeug";
  txt[PS_SELECTSCENERY] = "Landschaft";
  txt[PS_SELECTOPTIONS] = "Bitte passende Option wählen";
  txt[PS_SELECTSCENARIO] = "Scenario";
  txt[PS_USEDEFAULT] = "Standard benutzen";
  txt[PS_USEDEFAULTPREVIOUS] = "Benutze Standard/Vorherige";
  txt[PS_INVALIDRACE] = "Ungueltiges Rennen in der kostenlosen Version von PicaSim";
  txt[PS_BACK] = "Zurück";
  txt[PS_OPTIONS1] = "Optionen 1";
  txt[PS_OPTIONS2] = "Optionen 2";
  txt[PS_AEROPLANE] = "Flugzeug";
  txt[PS_SCENERY] = "Landschaft";
  txt[PS_OBJECTS] = "Objects";
  txt[PS_OBJECTSSETTINGS] = "Objects settings";
  txt[PS_LIGHTING] = "Lichtverhältnisse";
  txt[PS_AICONTROLLERS] = "AI Controllers";
  txt[PS_CONTROLLER] = "Steuerung";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Laden...";
  txt[PS_SAVE] = "Speichern...";
  txt[PS_DELETE] = "Löschen...";
  txt[PS_ADVANCED] = "Fortgeschritten";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Clear all saved settings and exit";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "This will clear all saved settings and exit PicaSim - are you sure?";
  txt[PS_SIMPLE] = "Einfach";
  txt[PS_FILENAME] = "Dateiname:";
  txt[PS_INFULLVERSION] = "In Vollversion";
  txt[PS_PURCHASE] = "Purchase";
  txt[PS_PURCHASESUCCESSFUL] = "Purchase successful - thanks!";
  txt[PS_PURCHASECANCELLED] = "Purchase cancelled.";
  txt[PS_PURCHASEERROR] = "Purchase unsuccessful due to an unknown error.";
  txt[PS_PURCHASEALREADY] = "Purchase unsuccessful since the product was already purchased!";
  txt[PS_PURCHASEFROMSTORE] = "Purchase from the Windows Store?";
  txt[PS_NOTNOW] = "Not now";
  txt[PS_CANCEL] = "Cancel";
  txt[PS_TRIALOVER] = "Trial period expired. Please purchase the full version or use one of the free planes and sceneries.";
  txt[PS_SELECTLANGUAGE] = "Sprache auswählen";
  txt[PS_LANGUAGESETTINGS] = "Spracheinstellungen";
  txt[PS_CURRENTLANGUAGE] = "Aktuelle Sprache";
  txt[PS_INFREEFLYMODE] = "Freiflugmodus";
  txt[PS_CAMERASETTINGS] = "Kameraeinstellungen";
  txt[PS_ZOOMVIEW] = "Zoom Ansicht";
  txt[PS_PLANEONLYINZOOMVIEW] = "Draw plane alone in zoom view";
  txt[PS_SMOKEONLYINMAINVIEW] = "Only draw smoke in main view";
  txt[PS_ZOOMVIEWSIZE] = "Zoom Ansicht Skalierung";
  txt[PS_GROUNDAUTOZOOM] = "Boden Autozoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Boden Autozoom Skalierung";
  txt[PS_GROUNDFIELDOFVIEW] = "Boden Blickfeld";
  txt[PS_GROUNDHORIZONAMOUNT] = "Boden Horizont Menge";
  txt[PS_GROUNDLAG] = "Ground camera lag";
  txt[PS_GROUNDVIEWFOLLOW] = "Folge dem Flugzeug in Bodenperspektive";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Yaw offset in ground view";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Pitch offset in ground view";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Flugzeug Blickfeld";
  txt[PS_STEREOSCOPY] = "Stereoscopy";
  txt[PS_STEREOSEPARATION] = "Stereo separation";
  txt[PS_STEREOINFO] = "+ve value for normal 3D, -ve for cross-eyes technique";
  txt[PS_WALKABOUTSETTINGS] = "Rundgang Einstellungen";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Rundgangaktivierung";
  txt[PS_ENABLEOBJECTEDITING] = "Enable object editing";
  txt[PS_FORCEALLVISIBLE] = "Make visible when editing";
  txt[PS_RESETOBJECTS] = "Reset objects and reload scenery";
  txt[PS_OBJECTEDITING] = "Object editing";
  txt[PS_OBJECTEDITINGTEXT] = "PicaSim allows additional objects to be simulated and drawn, apart from the terrain and the plane (etc).\n"
"\n"
"In the Settings»Objects tab you can load a set of these objects - this is done automatically when loading a "
"scene. Most scenes will not include any objects. However, some panoramic scenes will to represent objects "
"that are not part of the ground.\n"
"\n"
"If you want to add objects to the scene, you can enable object editing in Settings»Objects. This will make a set of "
"on-screen buttons that let you create/delete objects, change the selection, and change their properties. When moving "
"them around, bear in mind that you can select to work in world space, relative to the camera, or relative to the space of "
"the object. After creating a set of objects, don't forget to save them!\n"
"\n";
  txt[PS_OBJECTNUMBER] = "Object %d";
  txt[PS_POSITIONX] = "Position fwd";
  txt[PS_POSITIONY] = "Position left";
  txt[PS_POSITIONZ] = "Position up";
  txt[PS_COLOURR] = "Colour red";
  txt[PS_COLOURG] = "Colour green";
  txt[PS_COLOURB] = "Colour blue";
  txt[PS_COLOURH] = "Colour hue";
  txt[PS_COLOURS] = "Colour saturation";
  txt[PS_COLOURV] = "Colour brightness";
  txt[PS_POSITION] = "Position";
  txt[PS_SIZE] = "Size";
  txt[PS_COLOUR] = "Colour";
  txt[PS_VISIBLE] = "Visible";
  txt[PS_SHADOW] = "Shadow";
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
  txt[PS_BRAKESFORWARD] = "Air brakes with stick forward";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Use absolute controller touch position";
  txt[PS_STAGGERCONTROLLER] = "Versetzte Steuerung (für teilweisen Multitouch)";
  txt[PS_ENABLEELEVATORTRIM] = "Enable elevator trim";
  txt[PS_ELEVATORTRIMSIZE] = "Elevator trim size";
  txt[PS_SQUARECONTROLLERS] = "Quadratische Steuerelemente";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Horizontaler Versatz";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Vertikaler Versatz";
  txt[PS_SHAPEOPACITY] = "Form Hervorhebung";
  txt[PS_STICKOPACITY] = "Knüppel Hervorhebung";
  txt[PS_STICKCROSS] = "Knüppel Kreuz";
  txt[PS_JOYSTICKID] = "Joystick ID";
  txt[PS_JOYSTICKINFO] = "Note: You may need to restart PicaSim after connecting or disconnecting a joystick/controller.";
  txt[PS_NOJOYSTICK] = "No Joystick detected or available.";
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
  txt[PS_THERMALWINDFIELD] = "Draw thermal wind field";
  txt[PS_GRAPHAIRSPEED] = "Zeige Windgeschwindigkeit (rot)";
  txt[PS_GRAPHDURATION] = "Graph duration";
  txt[PS_GRAPHALTITUDE] = "Show altitude (turquoise)";
  txt[PS_GRAPHGROUNDSPEED] = "Zeige Fluggeschwindigkeit über Grund (grün)";
  txt[PS_GRAPHCLIMBRATE] = "Zeige Steigrate (blau)";
  txt[PS_GRAPHWINDSPEED] = "Zeige Windgeschwindigkeit (gelb)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Zeige Windgeschwindigkeit vertikal (lila)";
  txt[PS_STALLMARKERS] = "Strömungsabriss-Markierungen";
  txt[PS_AEROFOILPLOT] = "Plot aerofoil data";
  txt[PS_AEROFOILINFO] = "Aerofoil index";
  txt[PS_AEROFOILRANGE] = "Angle range";
  txt[PS_AEROFOILCLMAX] = "Max CL";
  txt[PS_AEROFOILCDMAX] = "Max CD";
  txt[PS_AEROFOILCMMAX] = "Max CM";
  txt[PS_AEROFOILLDMAX] = "Max L/D";
  txt[PS_AEROFOILPLOTREFERENCE] = "Plot reference aerofoil";
  txt[PS_AEROFOILPOLAR] = "Plot polar diagram";
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
  txt[PS_CONTROLLEDPLANESHADOWS] = "Controlled plane shadows";
  txt[PS_OTHERSHADOWS] = "Other shadows";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Projected shadow detail";
  txt[PS_BLOB] = "Blob";
  txt[PS_PROJECTED] = "Projected";
  txt[PS_AEROPLANERENDER] = "Flugzeug Darstellung";
  txt[PS_DRAWSHADOWS] = "Zeichne Schatten";
  txt[PS_USE16BIT] = "Benutze 16 Bit Texturen um Speicher zu reduzieren (Neustart erforderlich)";
  txt[PS_SEPARATESPECULAR] = "Separate specular calculation";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Umgebungslicht Skalierung";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Diffuses Licht Skalierung";
  txt[PS_TERRAINTEXTUREDETAIL] = "Geländetextur Detail";
  txt[PS_MAXSKYBOXDETAIL] = "Max skybox/panorama detail";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Requires restart";
  txt[PS_REQUIRESVRRESTART] = "Requires VR restart";
  txt[PS_NEARCLIPDISTANCE] = "Nahkappabstand";
  txt[PS_FARCLIPDISTANCE] = "Fernkappabstand";
  txt[PS_SHADINGMETHOD] = "Shading method";
  txt[PS_USEMULTILIGHTS] = "Use multiple lights";
  txt[PS_ENABLESMOKE] = "Enable smoke";
  txt[PS_SMOKEQUALITY] = "Smoke quality";
  txt[PS_MISCSETTINGS] = "Verschiedene Einstellungen";
  txt[PS_USEBACKBUTTON] = "Benutze Zurrück-Button zum Schließen";
  txt[PS_DRAWLAUNCHMARKER] = "Zeichne Startmarkierung";
  txt[PS_DRAWGROUNDPOSITION] = "Zeichne bodenmarkierung";
  txt[PS_SKYGRIDOVERLAY] = "Sky grid overlay";
  txt[PS_SKYGRID_NONE] = "None";
  txt[PS_SKYGRID_SPHERE] = "Sphere";
  txt[PS_SKYGRID_BOX] = "Box";
  txt[PS_SKYGRIDALIGNMENT] = "Sky grid alignment";
  txt[PS_SKYGRIDDISTANCE] = "Sky grid distance";
  txt[PS_SKYGRIDALIGN_ALONGWIND] = "Along wind";
  txt[PS_SKYGRIDALIGN_CROSSWIND] = "Across wind";
  txt[PS_SKYGRIDALIGN_ALONGRUNWAY] = "Along runway";
  txt[PS_SKYGRIDALIGN_CROSSRUNWAY] = "Across runway";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Benutze vom Flugzeug bevorzugte Steuerung";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Test/Entwicklereinstellungen";
  txt[PS_WIREFRAMETERRAIN] = "Drahtgelände";
  txt[PS_DRAWSUNPOSITION] = "Zeichne Position der Sonne";
  txt[PS_FREEFLYSETTINGS] = "Freies Fliegen Einstellungen";
  txt[PS_DISPLAYFLIGHTTIME] = "Flugzeit anzeigen";
  txt[PS_DISPLAYSPEED] = "Geschwindigkeit anzeigen";
  txt[PS_DISPLAYAIRSPEED] = "Display airspeed";
  txt[PS_DISPLAYMAXSPEED] = "Geschwindigkeit max anzeigen";
  txt[PS_DISPLAYASCENTRATE] = "Steigrate anzeigen";
  txt[PS_DISPLAYALTITUDE] = "Flughöhe anzeigen";
  txt[PS_DISPLAYDISTANCE] = "Abstand anzeigen";
  txt[PS_COLOURTEXT] = "Farbiger Text bei Steigrate";
  txt[PS_TEXTATTOP] = "Text at top of screen";
  txt[PS_FREEFLYONSTARTUP] = "Free fly on startup";
  txt[PS_ENABLESOCKETCONTROLLER] = "Enable socket controller";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Text background opacity";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Text background colour";
  txt[PS_MAXNUMBERAI] = "Max number of AI";
  txt[PS_TOOMANYAI] = "Note: Too many AI will cause crashes & slowdown!";
  txt[PS_UNITS] = "Units";
  txt[PS_RACESETTINGS] = "Rennen/Limbo Einstellungen";
  txt[PS_RACEVIBRATIONAMOUNT] = "Vibration";
  txt[PS_RACEBEEPVOLUME] = "Piep Lautstärke";
  txt[PS_LIMBODIFFICULTY] = "Limbo Schwierigkeiten";
  txt[PS_ENABLESCORELOOP] = "Scoreloop aktivieren";
  txt[PS_DELETELOCALHIGHSCORES] = "Lösche lokale Höchstpunktestände";
  txt[PS_GENERALSETTINGS] = "Allgemeine Einstellungen";
  txt[PS_COLOURSCHEME] = "Farbschema";
  txt[PS_COLOUROFFSET] = "Colour shift";
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
  txt[PS_SHOWBUTTON1] = "Show button 1";
  txt[PS_SHOWBUTTON2] = "Show button 2";
  txt[PS_LAUNCH] = "Start";
  txt[PS_HOOKS] = "Tow hooks";
  txt[PS_BUNGEELAUNCH] = "Bungee launch";
  txt[PS_AEROTOWLAUNCH] = "Aerotow launch";
  txt[PS_FLATLAUNCHMETHOD] = "Launch method (flat sceneries)";
  txt[PS_HAND] = "Hand";
  txt[PS_BUNGEE] = "Bungee";
  txt[PS_AEROTOW] = "Aerotow";
  txt[PS_LAUNCHSPEED] = "Start Geschwindigkeit";
  txt[PS_LAUNCHANGLE] = "Startwinkel nach oben";
  txt[PS_LAUNCHUP] = "Start nach oben";
  txt[PS_LAUNCHFORWARD] = "Start nach vorne";
  txt[PS_LAUNCHLEFT] = "Start nach links";
  txt[PS_LAUNCHOFFSETUP] = "Launch offset up";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relaunch when stationary";
  txt[PS_CRASHDETECTION] = "Crash detection";
  txt[PS_CRASHDELTAVELX] = "Delta velocity fwd/back";
  txt[PS_CRASHDELTAVELY] = "Delta velocity sideways";
  txt[PS_CRASHDELTAVELZ] = "Delta velocity up/down";
  txt[PS_CRASHDELTAANGVELX] = "Delta roll velocity";
  txt[PS_CRASHDELTAANGVELY] = "Delta pitch velocity";
  txt[PS_CRASHDELTAANGVELZ] = "Delta yaw velocity";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Suspension resilience scale";
  txt[PS_AIRFRAME] = "Cellule d'avion";
  txt[PS_UNDERCARRIAGE] = "Châssis";
  txt[PS_PROPELLER] = "Hélice";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Belly hook offset forwards";
  txt[PS_BELLYHOOKOFFSETLEFT] = "Belly hook offset left";
  txt[PS_BELLYHOOKOFFSETUP] = "Belly hook offset up";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Nose tow hook offset forwards";
  txt[PS_NOSEHOOKOFFSETLEFT] = "Nose tow hook offset left";
  txt[PS_NOSEHOOKOFFSETUP] = "Nose tow hook offset up";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Tail tow hook offset forwards";
  txt[PS_TAILHOOKOFFSETLEFT] = "Tail tow hook offset left";
  txt[PS_TAILHOOKOFFSETUP] = "Tail tow hook offset up";
  txt[PS_MAXBUNGEELENGTH] = "Maximale Hochstartgummi-Länge";
  txt[PS_MAXBUNGEEACCEL] = "Maximale Hochstartgummi-Beschleunigung";
  txt[PS_TUGPLANE] = "Tug plane";
  txt[PS_TUGSIZESCALE] = "Tug size scale";
  txt[PS_TUGMASSSCALE] = "Tug mass scale";
  txt[PS_TUGENGINESCALE] = "Tug engine scale";
  txt[PS_TUGMAXCLIMBSLOPE] = "Tug max climb slope";
  txt[PS_TUGTARGETSPEED] = "Tug target speed";
  txt[PS_AEROTOWROPELENGTH] = "Aerotow rope length";
  txt[PS_AEROTOWROPESTRENGTH] = "Aerotow rope strength";
  txt[PS_AEROTOWROPEMASSSCALE] = "Aerotow rope mass scale";
  txt[PS_AEROTOWROPEDRAGSCALE] = "Aerotow rope drag scale";
  txt[PS_AEROTOWHEIGHT] = "Aerotow max height";
  txt[PS_AEROTOWCIRCUITSIZE] = "Aerotow circuit size";
  txt[PS_TETHERING] = "Tethering (Control Line)";
  txt[PS_TETHERLINES] = "Number of tether lines";
  txt[PS_TETHERREQUIRESTENSION] = "Require tension for control";
  txt[PS_TETHERPHYSICSOFFSETFORWARD] = "Physical offset forwards";
  txt[PS_TETHERPHYSICSOFFSETLEFT] = "Physical offset left";
  txt[PS_TETHERPHYSICSOFFSETUP] = "Physical offset up";
  txt[PS_TETHERVISUALOFFSETFORWARD] = "Visual offset forwards";
  txt[PS_TETHERVISUALOFFSETLEFT] = "Visual offset left";
  txt[PS_TETHERVISUALOFFSETUP] = "Visual offset up";
  txt[PS_TETHERDISTANCELEFT] = "Tether distance left";
  txt[PS_TETHERCOLOURR] = "Tether colour: red";
  txt[PS_TETHERCOLOURG] = "Tether colour: green";
  txt[PS_TETHERCOLOURB] = "Tether colour: blue";
  txt[PS_TETHERCOLOURA] = "Tether colour: opacity";
  txt[PS_CHASECAMERA] = "Kamera verfolgen";
  txt[PS_CAMERATARGETPOSFWD] = "Target offset fwd";
  txt[PS_CAMERATARGETPOSUP] = "Target offset up";
  txt[PS_DISTANCE] = "Distanz";
  txt[PS_HEIGHT] = "Höhe";
  txt[PS_VERTICALVELFRAC] = "Verticaler Geschwindigkeitsanteil";
  txt[PS_FLEXIBILITY] = "Flexibility";
  txt[PS_COCKPITCAMERA] = "Cockpit camera";
  txt[PS_PITCH] = "Pitch";
  txt[PS_ENABLEDEBUGDRAW] = "Enable debug draw";
  txt[PS_CANTOW] = "Can tow";
  txt[PS_MAXAI] = "Max number of AI controllers is %d (see Options 2 tab)";
  txt[PS_ADDNEWAICONTROLLER] = "Add new AI controller";
  txt[PS_REMOVEAICONTROLLERS] = "Remove all AI controllers";
  txt[PS_LAUNCHSEPARATIONDISTANCE] = "Launch separation distance";
  txt[PS_AVAILABLEINFULLVERSION] = "Only available in full version";
  txt[PS_INCLUDEINCAMERAVIEWS] = "Enable cameras";
  txt[PS_CREATEMAXNUMCONTROLLERS] = "Create max AI controllers";
  txt[PS_LAUNCHDIRECTION] = "Launch direction";
  txt[PS_RANDOMCOLOUROFFSET] = "Random colour offset";
  txt[PS_AICONTROLLER] = "AI Controller";
  txt[PS_AINAVIGATION] = "AI Navigation";
  txt[PS_PLANETYPE] = "Plane type";
  txt[PS_USER] = "User";
  txt[PS_ALL] = "All";
  txt[PS_GLIDER] = "Glider";
  txt[PS_GLIDERS] = "Gliders";
  txt[PS_POWERED] = "Powered";
  txt[PS_HELI] = "Helicopter";
  txt[PS_CONTROLLINE] = "Control line";
  txt[PS_PANORAMIC] = "Panoramic";
  txt[PS_3D] = "3D";
  txt[PS_ALLOWAICONTROL] = "Allow AI control";
  txt[PS_WAYPOINTTOLERANCE] = "Waypoint tolerance";
  txt[PS_MINSPEED] = "Min speed";
  txt[PS_CRUISESPEED] = "Cruise speed";
  txt[PS_MAXBANKANGLE] = "Max bank angle";
  txt[PS_BANKANGLEPERHEADINGCHANGE] = "Bank angle per heading";
  txt[PS_SPEEDPERALTITUDECHANGE] = "Speed per altitude";
  txt[PS_GLIDESLOPEPEREXCESSSPEED] = "Glide slope per excess speed";
  txt[PS_PITCHPERROLLANGLE] = "Pitch per roll angle";
  txt[PS_HEADINGCHANGEFORNOSLOPE] = "Heading change for no slope";
  txt[PS_MAXPITCHCONTROL] = "Max pitch control";
  txt[PS_MAXROLLCONTROL] = "Max roll control";
  txt[PS_CONTROLPERROLLANGLE] = "Control per roll angle";
  txt[PS_PITCHCONTROLPERGLIDESLOPE] = "Pitch control per glide slope";
  txt[PS_ROLLTIMESCALE] = "Roll time scale";
  txt[PS_PITCHTIMESCALE] = "Pitch time scale";
  txt[PS_MINALTITUDE] = "Min altitude";
  txt[PS_SLOPEMINUPWINDDISTANCE] = "Slope Min upwind distance";
  txt[PS_SLOPEMAXUPWINDDISTANCE] = "Slope Max upwind distance";
  txt[PS_SLOPEMINLEFTDISTANCE] = "Slope Min left distance";
  txt[PS_SLOPEMAXLEFTDISTANCE] = "Slope Max left distance";
  txt[PS_SLOPEMINUPDISTANCE] = "Slope Min up distance";
  txt[PS_SLOPEMAXUPDISTANCE] = "Slope Max up distance";
  txt[PS_SLOPEMAXWAYPOINTTIME] = "Slope Max waypoint time";
  txt[PS_FLATMAXDISTANCE] = "Flat Max waypoint time";
  txt[PS_FLATMAXWAYPOINTTIME] = "Flat Max waypoint time";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Aktuelle Position";
  txt[PS_MASS] = "Gewicht";
  txt[PS_INERTIA] = "Inertia";
  txt[PS_WINGAREA] = "Wing area";
  txt[PS_EXTENTS] = "Extents";
  txt[PS_WINDSETTINGS] = "Wind Einstellungen";
  txt[PS_WINDSPEED] = "Windgeschwindigkeit";
  txt[PS_WINDBEARING] = "Windrichtung";
  txt[PS_CANNOTMODIFYSCENERY] = "Ihr könnt die Landschaft während eines Rennens nicht verändern";
  txt[PS_ALLOWBUNGEE] = "Start mit Hochstartgummi erlauben";
  txt[PS_WINDGUSTTIME] = "Windböenhäufigkeit";
  txt[PS_WINDGUSTFRACTION] = "Windböenanteil";
  txt[PS_WINDGUSTANGLE] = "Windböenwinkel";
  txt[PS_TURBULENCEAMOUNT] = "Turbulenzmenge";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Surface turbulence";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Wind shear turbulence";
  txt[PS_WINDLIFTSMOOTHING] = "Windauftriebsglättung";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Abtrennungstendenz";
  txt[PS_ROTORTENDENCY] = "Rotor tendency";
  txt[PS_DEADAIRTURBULENCE] = "Tote Luft Turbulenz";
  txt[PS_BOUNDARYLAYERDEPTH] = "Boundary layer depth";
  txt[PS_THERMALSETTINGS] = "Thermik Einstellungen (siehe auch Licht Einstellungen)";
  txt[PS_DENSITY] = "Dichte";
  txt[PS_RANGE] = "Reichweite";
  txt[PS_LIFESPAN] = "Lebensdauer";
  txt[PS_DEPTH] = "Tiefe";
  txt[PS_CORERADIUS] = "Kern Radius";
  txt[PS_DOWNDRAFTEXTENT] = "Abwind Ausdehnung";
  txt[PS_UPDRAFTSPEED] = "Aufwind Geschwindigkeit";
  txt[PS_ASCENTRATE] = "Steigrate";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Expansion over lifespan";
  txt[PS_RUNWAY] = "Runway";
  txt[PS_RUNWAYTYPE] = "Runway type";
  txt[PS_CIRCLE] = "Circle";
  txt[PS_RUNWAYX] = "Position X";
  txt[PS_RUNWAYY] = "Position Y";
  txt[PS_RUNWAYHEIGHT] = "Height";
  txt[PS_RUNWAYANGLE] = "Angle";
  txt[PS_RUNWAYLENGTH] = "Length/radius";
  txt[PS_RUNWAYWIDTH] = "Width";
  txt[PS_SURFACESETTINGS] = "Surface settings";
  txt[PS_SURFACEROUGHNESS] = "Roughness";
  txt[PS_SURFACEFRICTION] = "Friction";
  txt[PS_RANDOMTERRAINSETTINGS] = "Zufälliges Gelände Einstellung";
  txt[PS_RANDOMSEED] = "Impfnummer";
  txt[PS_DISPLACEMENTHEIGHT] = "Versetzungshöhe";
  txt[PS_SMOOTHNESS] = "Glätte";
  txt[PS_EDGEHEIGHT] = "Geländekantenhöhe";
  txt[PS_UPWARDSBIAS] = "Upwards bias";
  txt[PS_FILTERITERATIONS] = "Filterwiederholungen";
  txt[PS_PLAINSETTINGS] = "Plain/water settings";
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
  txt[PS_AISCENERY] = "AI Scenery";
  txt[PS_SCENETYPE] = "Scene type";
  txt[PS_FLAT] = "Flat";
  txt[PS_SLOPE] = "Slope";
  txt[PS_CURRENTVIEWPOSITION] = "Momentaner Standpunkt";
  txt[PS_NOLIGHTINGSETTINGS] = "Keine Lichteinstellungen, da Geländetyp Panorama";
  txt[PS_SUNBEARING] = "Sonnenpeilung";
  txt[PS_THERMALACTIVITY] = "Thermikaktivität";
  txt[PS_TERRAINDARKNESS] = "Gelände Dunkelheit";
  txt[PS_CONTROLLERSEESETTINGS] = "Also see additional controller settings under Options";
  txt[PS_CROSS] = "Kreuz";
  txt[PS_BOX] = "Box";
  txt[PS_CROSSANDBOX] = "Kreuz & Box";
  txt[PS_CONFIGURATION] = "Konfiguration";
  txt[PS_STYLE] = "Stil";
  txt[PS_CONTROLLERMODE] = "Controller mode";
  txt[PS_TREATTHROTTLEASBRAKES] = "Treat throttle stick as brakes";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Reset configuration on launch";
  txt[PS_NUMCONFIGURATIONS] = "Anzahl Konfigurationen";
  txt[PS_RIGHTSTICKHORIZONTAL] = "Rechter Knüppel horizontal";
  txt[PS_RIGHTSTICKVERTICAL] = "Rechter Knüppel vertikal";
  txt[PS_LEFTSTICKHORIZONTAL] = "Linker Knüppel horizontal";
  txt[PS_LEFTSTICKVERTICAL] = "Linker Knüppel vertikal";
  txt[PS_TILTHORIZONTAL] = "Kippung horizontal";
  txt[PS_TILTVERTICAL] = "Kippung vertikal";
  txt[PS_ARROWSHORIZONTAL] = "Pfeile horizontal";
  txt[PS_ARROWSVERTICAL] = "Konstant";
  txt[PS_ROLLSTICK] = "Roll stick";
  txt[PS_PITCHSTICK] = "Pitch stick";
  txt[PS_YAWSTICK] = "Yaw stick";
  txt[PS_SPEEDSTICK] = "Throttle stick";
  txt[PS_CONSTANT] = "";
  txt[PS_BUTTON0] = "Button 1";
  txt[PS_BUTTON1] = "Button 2";
  txt[PS_BUTTON2] = "Button 3";
  txt[PS_BUTTON0TOGGLE] = "Button 1 toggle";
  txt[PS_BUTTON1TOGGLE] = "Button 2 toggle";
  txt[PS_BUTTON2TOGGLE] = "Button 3 toggle";
  txt[PS_MIXES] = "Mixes";
  txt[PS_ELEVATORTOFLAPS] = "Elevator to flaps";
  txt[PS_AILERONTORUDDER] = "Aileron to rudder";
  txt[PS_FLAPSTOELEVATOR] = "Flaps to elevator";
  txt[PS_BRAKESTOELEVATOR] = "Brakes/throttle to elevator";
  txt[PS_RUDDERTOELEVATOR] = "Rudder to elevator";
  txt[PS_RUDDERTOAILERON] = "Rudder to aileron";
  txt[PS_RATESBUTTON] = "Rates button";
  txt[PS_RATESCYCLEBUTTON] = "Rates cycle button";
  txt[PS_RELAUNCHBUTTON] = "Relaunch button";
  txt[PS_CAMERABUTTON] = "Viewpoint button";
  txt[PS_PAUSEPLAYBUTTON] = "Pause/play button";
  txt[PS_NONE] = "Keine";
  txt[PS_CONTROLSOURCES] = "Kontrollursprung";
  txt[PS_AILERONS] = "Querruder/Ruder";
  txt[PS_ELEVATOR] = "Höhenruder";
  txt[PS_RUDDER] = "Ruder";
  txt[PS_THROTTLE] = "Gashebel/Auftriebshilfe/Bremsen";
  txt[PS_LOOKYAW] = "Ansicht Gierachse";
  txt[PS_LOOKPITCH] = "Ansicht Nickwinkel";
  txt[PS_AUX1] = "Aux 1";
  txt[PS_SMOKE1] = "Smoke 1";
  txt[PS_SMOKE2] = "Smoke 2";
  txt[PS_HOOK] = "Hook";
  txt[PS_VELFWD] = "Velocity fwd";
  txt[PS_VELLEFT] = "Velocity left";
  txt[PS_VELUP] = "Velocity up";
  txt[PS_MAXPARTICLES] = "Max num particles";
  txt[PS_CHANNELFOROPACITY] = "Channel for opacity";
  txt[PS_MINOPACITY] = "Min opacity";
  txt[PS_MAXOPACITY] = "Max opacity";
  txt[PS_CHANNELFORRATE] = "Channel for rate";
  txt[PS_MINRATE] = "Min rate";
  txt[PS_MAXRATE] = "Max rate";
  txt[PS_INITIALSIZE] = "Initial size";
  txt[PS_FINALSIZE] = "Final size";
  txt[PS_DAMPINGTIME] = "Damping time";
  txt[PS_JITTER] = "Jitter";
  txt[PS_ENGINEWASH] = "Engine wash amount";
  txt[PS_HUECYCLEFREQ] = "Hue cycle frequency";
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
  txt[PS_ROLLSTICKMOVEMENT] = "Roll stick movement";
  txt[PS_PITCHSTICKMOVEMENT] = "Pitch stick movement";
  txt[PS_YAWSTICKMOVEMENT] = "Yaw stick movement";
  txt[PS_SPEEDSTICKMOVEMENT] = "Speed stick movement";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Beschleunigungsmesser: Drehbewegung";
  txt[PS_ACCELEROMETERROLL] = "Beschleunigungsmesser Drehbewegung";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Beschleunigungsmesser: Nickwinkelbewegung";
  txt[PS_ACCELEROMETERPITCH] = "Beschleunigungsmesser Nickwinkelbewegung";
  txt[PS_TILTROLLSENSITIVITY] = "Kippung Drehungssensibilität";
  txt[PS_TILTPITCHSENSITIVITY] = "Kippung Nickwinkelsensibilität";
  txt[PS_TILTNEUTRALANGLE] = "Kippung neutraler Winkel";
  txt[PS_NOJOYSTICKWITHID] = "No Joystick with this ID";
  txt[PS_ENABLEJOYSTICK] = "Joystick aktivieren";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Anpassung an kreisende Knüppelbewegung";
  txt[PS_EXTERNALJOYSTICKSETTINGS] = "External joystick settings";
  txt[PS_JOYSTICKLABEL] = "Joystick %d: Input = %5.2f Output = %5.2f";
  txt[PS_JOYSTICKABUTTONLABEL] = "Joystick %d: Input = %5.2f Output = %d";
  txt[PS_JOYSTICKBUTTONLABEL] = "Joystick button %d: Input = %5.2f Output = %d";
  txt[PS_SCALEPOSITIVE] = "Input +ve scale";
  txt[PS_SCALENEGATIVE] = "Input -ve scale";
  txt[PS_OFFSET] = "Input Offset";
  txt[PS_MAPTO] = "Map to";
  txt[PS_PRESSWHENCENTRED] = "Press when centred";
  txt[PS_PRESSWHENLEFTORDOWN] = "Press when left or down";
  txt[PS_PRESSWHENRIGHTORUP] = "Press when right or up";
  txt[PS_DEADZONE] = "Dead zone";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Clear joystick settings";
  txt[PS_CALIBRATEJOYSTICK] = "Launch joystick calibration (Windows)";
  txt[PS_LOADOPTIONS] = "Einstellungen laden";
  txt[PS_SAVEOPTIONS] = "Einstellungen speichern";
  txt[PS_DELETEOPTIONS] = "Einstellungen löschen";
  txt[PS_LOADSCENERY] = "Landschaft laden";
  txt[PS_SAVESCENERY] = "Landschaft speichern";
  txt[PS_DELETESCENERY] = "Landschaft löschen";
  txt[PS_LOADOBJECTS] = "Load objects";
  txt[PS_SAVEOBJECTS] = "Save objects";
  txt[PS_DELETEOBJECTS] = "Delete objects";
  txt[PS_LOADLIGHTING] = "Lichteinstellungen laden";
  txt[PS_SAVELIGHTING] = "Lichtverhältnisse speichern";
  txt[PS_DELETELIGHTING] = "Lichtverhältnisse löschen";
  txt[PS_LOADCONTROLLER] = "Steuerung laden";
  txt[PS_SAVECONTROLLER] = "Steuerung speichern";
  txt[PS_DELETECONTROLLER] = "Steuerung löschen";
  txt[PS_LOADJOYSTICK] = "Load joystick";
  txt[PS_SAVEJOYSTICK] = "Save joystick";
  txt[PS_DELETEJOYSTICK] = "Delete joystick";
  txt[PS_LOADAEROPLANE] = "Flugzeug laden";
  txt[PS_SAVEAEROPLANE] = "Flugzeug speichern";
  txt[PS_DELETEAEROPLANE] = "Flugzeug löschen";
  txt[PS_LOADAICONTROLLERS] = "Load AI controllers";
  txt[PS_SAVEAICONTROLLERS] = "Save AI controllers";
  txt[PS_DELETEAICONTROLLERS] = "Delete AI controllers";
  txt[PS_SELECTPANORAMA] = "Panorama auswählen";
  txt[PS_SELECTTERRAINFILE] = "Gelände auswählen";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Bevorzugte Steuerung auswählen";
  txt[PS_SELECTOBJECTSSETTINGS] = "Select objects settings";
  txt[PS_SUMMARY] = "Summary";
  txt[PS_NOOBJECTS] = "There are no objects loaded (other than the terrain etc). You can load a set of static and dynamic objects, "
    "making sure they're suitable for the current scene. By enabling object editing you can "
    "create your own - see help and the PicaSim web page for more info.";
  txt[PS_OBJECTSTOTAL] = "Total number of objects:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Number static visible:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Number static invisible:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Number dynamic visible:";
  txt[PS_ABOUT] = "Über";
  txt[PS_WEBSITE] = "Webseite";
  txt[PS_FLYING] = "Fliegen";
  txt[PS_SETTINGS] = "Einstellungen";
  txt[PS_RACES] = "Rennen";
  txt[PS_TIPS] = "Tips";
  txt[PS_KEYBOARD] = "Keyboard";
  txt[PS_CREDITS] = "Danksagung";
  txt[PS_LICENCE] = "Licence";
  txt[PS_VERSIONS] = "Versions";
  txt[PS_ABOUTFREETEXT] = "PicaSim ist ein Simulator für ferngesteuerte Flugzeuge. Gegenwärtig ist er darauf angelegt, Euch beim Fliegenlernen zu helfen und Eure Hangflugfähigkeiten zu verbessern - den Segelflieger mit dem Auftrieb des Windes, so wie er an Anhöhen hinauf- und herüberströmt, zu fliegen.\n"
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
    "Ich habe viele Ideen PicaSim zu erweitern und zu verbessern- ich möchte Motorflugzeuge sowie weitere Flugmodelle aufnehmen, vernetztes Fliegen und computeranimierte Flugzeuge mit einschliessen. Falls Ihr mich unterstützen und zum Weitermachen ermuntern möchtet, oder Ihr Euch vielleicht einfach für diese Version bedanken wollt, freue ich mich sehr, wenn Ihr die Spendenversion PicaSimDonate kauft.\n";
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
  txt[PS_TIPSTEXT] = "Here are some tips to help you get the most out of PicaSim:\n"
"\n"
"• Remember it's a simulator for radio-controlled planes - the primary focus is on realistic control, with the \"pilot\" standing on "
"the ground. Remember that learning to fly is a skill that needs practice - look on the website below for help.\n"
"\n"
"• Experiment with the settings, especially if you're a R/C pilot and have preferences! If you mess things up, immediately after "
"starting PicaSim go into settings and load a preset for the relevant sections.\n"
"\n"
"• If you experience any problems or have suggestions, email picasimulator@gmail.com\n"
"\n"
"• Panoramas can be displayed at \"normal\" and \"maximum\" detail - it's certainly worth trying the higher detail level, though if "
"PicaSim crashes when loading panoramic sceneries, try reducing the detail in Settings»Options 1.\n"
"\n"
"• AI controlled planes are configured in two places. Settings»Options 2 allows you to set what the maximum number of AI planes "
"your device will run. Settings»AI Controllers allows you to configure what planes will actually be loaded.\n";
  txt[PS_KEYBOARDTEXT] = "If you have a keyboard, there are some shortcuts:\n"
"\n"
"[r] - Relaunch\n"
"[p] - Toggle pause\n"
"[c] - Cycles through the camera views\n"
"[z] - Toggles the zoom view\n";
  txt[PS_JOYSTICKSETUPTEXT] = "PicaSim supports external USB joysticks/gamepads under Android 3.0 and above, and under Windows. On Android devices "
"this will require a USB OTG (On The Go) connector, and your device will need to support it - so please check that first. "
"If the joystick tab appears in the settings menus there's a good chance your device will support external USB controllers, "
"but it's not guaranteed!\n"
"\n"
"The external controller needs to be connected before running PicaSim, and under Windows it is recommended to calibrate the joystick using "
"the \"calibrate\" button in Settings»Joystick.\n"
"\n"
"If your joystick is detected and selected in Settings»Joystick you will need to assign physical inputs to the controls recognised by PicaSim. "
"This is done by identifying which joystick axis corresponds to which control input, and then there are buttons to press when the stick is "
"in the centre, to the left or down, and to the right or up. You can then check that the stick movements maps to the correct outputs. "
"This procedure is the same under Windows and Android - visit the website below for a video describing this in more detail.";
  txt[PS_CREDITSTEXT] = "Autor: Danny Chapman.\n\n\n"
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
"Beachtet bitte, dass trotz grösster Sorgfalt das Flugverhalten der Modelle so realistisch wie möglich zu gestalten, mir dies mitunter nicht immer gelungen sein mag. Am besten findet Ihr dies heraus, wenn Ihr ein wirkliches Modell fliegt!";
  txt[PS_LICENCETEXT] = "PicaSim is available from me in the following versions:\n\n"
"Free: The simulator and assets (including planes and sceneries etc) may be distributed according to the Creative Commons Attribution-NonCommercial-NoDerivs licence.\n\n"
"Paid: This version or its derivatives may only be distributed with my explicit authorisation.\n\n"
"If you wish to share content that you have created then you must do so separately from the PicaSim distribution.\n\n"
"The assets in PicaSim may be distributed according to the Creative Commons Attribution-NonCommercial licence (i.e. you may modify and distribute them, so long as you credit the source).\n\n"
"It is quite easy to distribute content for PicaSim by placing it in the UserData and UserSettings. See the website and ask on the forum for more details.";
  txt[PS_TIPS1] = "Bitte schickt mir Rüchmeldungen, damit ich PicaSim verbessern kann";
  txt[PS_TIPS2] = "Gefällt Euch PicaSim, hinterlasst bitte eine Bewertung";
  txt[PS_TIPS3] = " Ihr könnt den Beschleunigungssensor benutzen, um das Modell zu steuern - geht zu Einstellungen»Steuerung";
  txt[PS_TIPS4] = "Folgt PicaSim auf Facebook oder Google+ und erhaltet Benachrichtigungen über Aktualisierungen und zukünftige Entwicklungen";
  txt[PS_TIPS5] = "If you like PicaSim, please leave a review/rating on the mobile versions";
  txt[PS_TIPS6] = "Switch between mode 1 and 2 etc in the options";
  txt[PS_TIPS7] = "Please send me feedback so I can improve PicaSim";
  txt[PS_TIPS8] = "If the frame rate is low, try loading a lower quality setting in the options, and/or reduce the physics quality setting";
  txt[PS_TIPS9] = "On mobile devices try using the accelerometer by loading a suitable preset in Settings»Controller";
  txt[PS_TIPS10] = "Adding ballast to gliders increases their ability to penetrate into the wind, at the cost of faster descent";
  txt[PS_TIPS11] = "Enable 'walkabout' mode in 3D scenes, and use the walk button when paused to move the launch point and wind direction";
  txt[PS_TIPS12] = "Circling hawks indicate areas of thermal lift, but it is actually better to watch for the glider's wing tip or nose to lift";
  txt[PS_TIPS13] = "Flying real R/C planes is even more fun!";
  txt[PS_TIPS14] = "Like/follow PicaSim on Facebook or Google+ to get notified of updates and plans";
  txt[PS_TIPS15] = "The full version of PicaSim contains additional planes, sceneries and challenges";
  txt[PS_TIPS16] = "Add AI controlled planes in Settings»AI Controllers. Limit the maximum number in Settings»Options 2";
  txt[PS_TIPS17] = "You can enable elevator trim in Settings»Options 1";
  txt[PS_TIPS18] = "Ask questions and get news about PicaSim on Facebook, Google+ or the forum";
  txt[PS_ALLOWCHOICE] = "Modell und Landschaftsauswahl beim nächsten Start anzeigen";
  txt[PS_LOADING] = "Lädt...";
  txt[PS_RACETIP] = "%s\n\nDie weissen Pfeile zeigen in die Richtung der nächsten Kontrollpunkte.";
  txt[PS_LIMBOTIP] = "%s\n\nStelle den Schwierigkeitsgrad/Ergebnismultiplikator in Einstellungen»Modus";
  txt[PS_DURATIONTIP] = "%s\n\nLook for thermals and land close to the launch point.";
  txt[PS_TRAINERGLIDERTIP] = "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
    "Benuzt den Kontrollknüppel um das Ruder und Höhenruder zu steuern, damit der Segelflieger mit dem Auftrieb vor dem Hügel fliegt. "
    "Seht auf der Hilfe/Website nach um Tipps zu erhalten falls es euch schwerfällt in der Luft zu bleiben.";
  txt[PS_TRAINERPOWEREDTIP] = "Drückt den Startknopf zum Abflug, und den Pause/Rückspulknopf für den Neustart."
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
  txt[PS_LEADERBOARDFAILED] = "Rangliste nicht verfuegbar! - Internetverbindung pruefen";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Rangliste nicht verfuegbar! Neuer Versuch...";
  txt[PS_RETRIEVINGLEADERBOARD] = "Rangliste wird abgerufen...";
  txt[PS_SCORESUBMISSIONFAILED] = "Ergebnisuebertragung gescheitert! - Internetverbindung pruefen";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Ergebnisuebertragung gescheitert! Neuer Versuch...";
  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";
  txt[PS_SETUSERNAME] = "Benutzername festlegen";
  txt[PS_SETEMAIL] = "Emailadresse eintragen";
  txt[PS_USERFAILED] = "Benutzeranfrage gescheitert! - Internetverbindung pruefen";
  txt[PS_USERFAILEDRETRYING] = "Benutzeranfrage gescheitert! Neuer Versuch...";
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
  txt[PS_VRHEADSET] = "VR Headset";
  txt[PS_ENABLEVRMODE] = "Enable VR Mode";
  txt[PS_HEADSET] = "Headset";
  txt[PS_VRWORLDSCALE] = "World Scale";
  txt[PS_NOTHING] = "Nothing";
  txt[PS_VRVIEW] = "VR View";
  txt[PS_NORMALVIEW] = "Normal View";
  txt[PS_VRDESKTOP] = "VR Desktop";
  txt[PS_VRANTIALIASING] = "VR Anti-Aliasing";
  txt[PS_NONEUSEDEFAULT] = "None (use default)";
  txt[PS_VRAUDIO] = "VR Audio";
  txt[PS_STATUS] = "Status";
  txt[PS_VRNOTAVAILABLE] = "VR not available - no headset detected";
  txt[PS_ORIENTATION] = "Orientation";
  txt[PS_SMOKESOURCE] = "Smoke source %d";
  txt[PS_NAME] = "Name";
  txt[PS_BUTTONMAPPINGS] = "Button Mappings";
  txt[PS_USERFILES] = "User files:";
  txt[PS_NOFILESFOUND] = "No files found";
}

