// Notes:
//
// - The file must be saved using utf-8 without signature (adv save options)
//
// - Translations should generally be a similar or shorter length to the original text (although if there's lots of text - like the help - it doesn't matter). If necessary a shorter but less "correct" phrase may be better.
//
// - Be careful to preserve certain things like:
//     "..." at the end of a phrase
//     Things like %d (which are used to insert numbers into the string)
//     Spaces in multi-line strings
//     The ordering
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

void InitStringsFR(const char** txt)
{
  txt[PS_LANGUAGE_EN] = "French";
  txt[PS_LANGUAGE_NATIVE] = "Francais";
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Enable";
  txt[PS_REMOVE] = "Remove";
  txt[PS_DUPLICATE] = "Duplicate";
  txt[PS_CRASHED] = "Scratché";
  txt[PS_NO] = "No";
  txt[PS_YES] = "Yes";
  txt[PS_INVERT] = "Invert";
  txt[PS_AUTO] = "Auto";
  txt[PS_FREEFLY] = "Vol Libre";
  txt[PS_RACE] = "Course";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Version Full";
  txt[PS_INTRIALMODE] = "In trial mode - purchase full version to access all planes and sceneries.";
  txt[PS_GAMEPADDETECTED] = "Gamepad detected. Enable in the settings to use it.";
  txt[PS_WHATSNEW] = "Welcome to PicaSim!";
  txt[PS_WHATSNEWTEXT] = "Here's what's new in this version (or recent versions):\n\n"
"• Add the Quark glider.\n"
"• Update various links etc in plane descriptions.\n"
"• Various tweaks/bug fixes.\n"
"\nPlease contact picasimulator@gmail.com with bug reports and questions.\n";
  txt[PS_ONERACE] = "La version libre pour mobile de ce simulateur ne contient qu'une seule course. Pour obtenir les autres, achetez la version complete, svp "
    "elle contient plus de courses, aeromodeles, scenes et vous beneficierez d'un support pour les anciens et nouveaux developpements de PicaSim.";
  txt[PS_FULLVERSION] = "The full version contains all the planes/sceneries etc - the small purchase price will help future development of PicaSim.";
  txt[PS_SELECTAEROPLANE] = "Planeur";
  txt[PS_SELECTSCENERY] = "Scène";
  txt[PS_SELECTOPTIONS] = "Veuillez séléctionner les options par défaut";
  txt[PS_SELECTSCENARIO] = "Scénario";
  txt[PS_USEDEFAULT] = "Usage par defaut";
  txt[PS_USEDEFAULTPREVIOUS] = "Usage par defaut précédent";
  txt[PS_INVALIDRACE] = "Course indisponible dans la version libre de PicaSim";
  txt[PS_BACK] = "Retour";
  txt[PS_OPTIONS1] = "Options 1";
  txt[PS_OPTIONS2] = "Options 2";
  txt[PS_AEROPLANE] = "Planeurs";
  txt[PS_SCENERY] = "Scènes";
  txt[PS_OBJECTS] = "Objects";
  txt[PS_OBJECTSSETTINGS] = "Objects settings";
  txt[PS_LIGHTING] = "Ciels";
  txt[PS_AICONTROLLERS] = "AI Controllers";
  txt[PS_CONTROLLER] = "Controles";
  txt[PS_JOYSTICK] = "Joysticks";
  txt[PS_LOAD] = "Charger...";
  txt[PS_SAVE] = "Sauver...";
  txt[PS_DELETE] = "Supprimer...";
  txt[PS_ADVANCED] = "+ d'options";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Clear all saved settings and exit";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "This will clear all saved settings and exit PicaSim - are you sure?";
  txt[PS_SIMPLE] = "- d'options";
  txt[PS_FILENAME] = "NomFichier:";
  txt[PS_INFULLVERSION] = "Dans version Full";
  txt[PS_PURCHASE] = "Purchase";
  txt[PS_PURCHASESUCCESSFUL] = "Purchase successful - thanks!";
  txt[PS_PURCHASECANCELLED] = "Purchase cancelled.";
  txt[PS_PURCHASEERROR] = "Purchase unsuccessful due to an unknown error.";
  txt[PS_PURCHASEALREADY] = "Purchase unsuccessful since the product was already purchased!";
  txt[PS_PURCHASEFROMSTORE] = "Purchase from the Windows Store?";
  txt[PS_NOTNOW] = "Not now";
  txt[PS_CANCEL] = "Cancel";
  txt[PS_TRIALOVER] = "Trial period expired. Please purchase the full version or use one of the free planes and sceneries.";
  txt[PS_SELECTLANGUAGE] = "Selection langue";
  txt[PS_LANGUAGESETTINGS] = "Réglage langue";
  txt[PS_CURRENTLANGUAGE] = "Langue en cours";
  txt[PS_INFREEFLYMODE] = "Mode Vol Libre";
  txt[PS_CAMERASETTINGS] = "Réglage camera";
  txt[PS_ZOOMVIEW] = "Vue Zoom";
  txt[PS_PLANEONLYINZOOMVIEW] = "Draw plane alone in zoom view";
  txt[PS_SMOKEONLYINMAINVIEW] = "Only draw smoke in main view";
  txt[PS_ZOOMVIEWSIZE] = "Taille Vue Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Autozoom depuis le sol";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Echelle autozoom sol";
  txt[PS_GROUNDFIELDOFVIEW] = "Vue vers le sol";
  txt[PS_GROUNDHORIZONAMOUNT] = "Niveau sol/horizon";
  txt[PS_GROUNDLAG] = "Ground camera lag";
  txt[PS_GROUNDVIEWFOLLOW] = "Centrer modèle en vol à l'écran";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Yaw offset in ground view";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Pitch offset in ground view";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Angle/Vue depuis modèle";
  txt[PS_STEREOSCOPY] = "Stereoscopy";
  txt[PS_STEREOSEPARATION] = "Stereo separation";
  txt[PS_STEREOINFO] = "+ve value for normal 3D, -ve for cross-eyes technique";
  txt[PS_WALKABOUTSETTINGS] = "Réglages positionpilote";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activer bouton positionpilote";
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
  txt[PS_SETWINDDIRONWALKABOUT] = "Sens/vent en mode positionpilote";
  txt[PS_SIMULATIONSETTINGS] = "Réglages simulation";
  txt[PS_TIMESCALE] = "Echelle de temps";
  txt[PS_PHYSICSACCURACY] = "Valeurs physiques";
  txt[PS_CONTROLLERSETTINGS] = "Réglages controles";
  txt[PS_MODE1DESCRIPTION] = "Mode 1: Dér/Prof gauche, Ail/Gaz-Flap droite";
  txt[PS_MODE2DESCRIPTION] = "Mode 2: Dér/Gaz-Flap gauche, Ail/Prof droite";
  txt[PS_MODE3DESCRIPTION] = "Mode 3: Ail/Gaz-Flap gauche, Dér/Prof droite";
  txt[PS_MODE4DESCRIPTION] = "Mode 4: Ail/Prof gauche, Dér/Gaz-Flap droite";
  txt[PS_CONTROLLERSIZE] = "Taille";
  txt[PS_BRAKESFORWARD] = "Air brakes with stick forward";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Usage tactile absolu";
  txt[PS_STAGGERCONTROLLER] = "Décaler controle (tactile partiel)";
  txt[PS_ENABLEELEVATORTRIM] = "Enable elevator trim";
  txt[PS_ELEVATORTRIMSIZE] = "Elevator trim size";
  txt[PS_SQUARECONTROLLERS] = "Controles en carré";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Décalage horiz. du bord";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Décalage vert. du bord";
  txt[PS_SHAPEOPACITY] = "Opacité grille controle";
  txt[PS_STICKOPACITY] = "Opacité manche controle";
  txt[PS_STICKCROSS] = "Stick cross";
  txt[PS_JOYSTICKID] = "IDentifiant Joystick";
  txt[PS_JOYSTICKINFO] = "Note: You may need to restart PicaSim after connecting or disconnecting a joystick/controller.";
  txt[PS_NOJOYSTICK] = "No Joystick detected or available.";
  txt[PS_AUDIOSETTINGS] = "Réglages sonores";
  txt[PS_OVERALLVOLUME] = "Volume global";
  txt[PS_VARIOMETERVOLUME] = "Volume Variomètre";
  txt[PS_WINDVOLUME] = "Volume du vent";
  txt[PS_OUTSIDEAEROPLANEVOLUME] = "Volume autour modèle";
  txt[PS_INSIDEAEROPLANEVOLUME] = "Volume dans modèle";
  txt[PS_ONSCREENDISPLAYSETTINGS] = "Réglages de l'affichage écran";
  txt[PS_WINDARROWSIZE] = "Taille flèche de vent";
  txt[PS_WINDARROWOPACITY] = "Opacité flèche de vent";
  txt[PS_PAUSEBUTTONSSIZE] = "Taille boutons mode pause";
  txt[PS_PAUSEBUTTONOPACITY] = "Opacité boutons mode pause";
  txt[PS_INFORMATIONSETTINGS] = "Réglage des informations";
  txt[PS_MAXMARKERSPERTHERMAL] = "Visu. des thermiques";
  txt[PS_THERMALWINDFIELD] = "Draw thermal wind field";
  txt[PS_GRAPHAIRSPEED] = "Visu graph vitesse/air (rouge)";
  txt[PS_GRAPHDURATION] = "Graph duration";
  txt[PS_GRAPHALTITUDE] = "Show altitude (turquoise)";
  txt[PS_GRAPHGROUNDSPEED] = "Visu graph vitesse/sol (vert)";
  txt[PS_GRAPHCLIMBRATE] = "Visu graph taux/montée (bleu)";
  txt[PS_GRAPHWINDSPEED] = "Visu graph vitesse/vent (jaune)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Visu graph vitesse verticale/vent (Violet)";
  txt[PS_STALLMARKERS] = "Marquer décrochages";
  txt[PS_AEROFOILPLOT] = "Plot aerofoil data";
  txt[PS_AEROFOILINFO] = "Infos profil d'aile";
  txt[PS_AEROFOILRANGE] = "Angle range";
  txt[PS_AEROFOILCLMAX] = "Max CL";
  txt[PS_AEROFOILCDMAX] = "Max CD";
  txt[PS_AEROFOILCMMAX] = "Max CM";
  txt[PS_AEROFOILLDMAX] = "Max L/D";
  txt[PS_AEROFOILPLOTREFERENCE] = "Plot reference aerofoil";
  txt[PS_AEROFOILPOLAR] = "Plot polar diagram";
  txt[PS_DRAWAEROPLANECOM] = "Visualiser CoM du modèle";
  txt[PS_GRAPHFPS] = "Visu. fréquence images FPS";
  txt[PS_NUMWINDSTREAMERS] = "Graphique de vent";
  txt[PS_WINDSTREAMERTIME] = "Graphique de temps";
  txt[PS_WINDSTREAMERDELTAZ] = "Graphique vent deltaZ";
  txt[PS_GRAPHICSSETTINGS] = "Réglages Graphiques";
  txt[PS_GROUNDTERRAINLOD] = "Niveau de détail sol";
  txt[PS_AEROPLANETERRAINLOD] = "Niveau de détail modèle";
  txt[PS_UPDATETERRAINLOD] = "Maj détails terrain vue/sol";
  txt[PS_COMPONENTS] = "Composants";
  txt[PS_3DMODEL] = "3D Model";
  txt[PS_BOTH] = "Tout";
  txt[PS_CONTROLLEDPLANESHADOWS] = "Controlled plane shadows";
  txt[PS_OTHERSHADOWS] = "Other shadows";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Projected shadow detail";
  txt[PS_BLOB] = "Blob";
  txt[PS_PROJECTED] = "Projected";
  txt[PS_AEROPLANERENDER] = "Rendu du modèle";
  txt[PS_DRAWSHADOWS] = "Montrer ombres";
  txt[PS_USE16BIT] = "Textures 16 bit (- de mémoire)(redémarrer)";
  txt[PS_SEPARATESPECULAR] = "Separate specular calculation";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Niveau éclairage ambiant";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Niveau éclairage diffus";
  txt[PS_TERRAINTEXTUREDETAIL] = "Détail texture terrain";
  txt[PS_MAXSKYBOXDETAIL] = "Max skybox/panorama detail";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Requires restart";
  txt[PS_REQUIRESVRRESTART] = "Requires VR restart";
  txt[PS_NEARCLIPDISTANCE] = "Distances proches";
  txt[PS_FARCLIPDISTANCE] = "Distances lointaines";
  txt[PS_SHADINGMETHOD] = "Shading method";
  txt[PS_USEMULTILIGHTS] = "Use multiple lights";
  txt[PS_ENABLESMOKE] = "Enable smoke";
  txt[PS_SMOKEQUALITY] = "Smoke quality";
  txt[PS_MISCSETTINGS] = "Réglages divers";
  txt[PS_USEBACKBUTTON] = "Bouton retour pour sortir";
  txt[PS_DRAWLAUNCHMARKER] = "Visu. balise de décollage";
  txt[PS_DRAWGROUNDPOSITION] = "Visu. position sol";
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
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Controle préféré du modèle";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Réglages test/developpement";
  txt[PS_WIREFRAMETERRAIN] = "Maillage terrain";
  txt[PS_DRAWSUNPOSITION] = "Visu. position soleil";
  txt[PS_FREEFLYSETTINGS] = "Réglages Vol Libre";
  txt[PS_DISPLAYFLIGHTTIME] = "Afficher temps de vol";
  txt[PS_DISPLAYSPEED] = "Afficher vitesse";
  txt[PS_DISPLAYAIRSPEED] = "Display airspeed";
  txt[PS_DISPLAYMAXSPEED] = "Afficher max vitesse";
  txt[PS_DISPLAYASCENTRATE] = "Afficher taux de montée";
  txt[PS_DISPLAYALTITUDE] = "Afficher altitude";
  txt[PS_DISPLAYDISTANCE] = "Afficher distance";
  txt[PS_COLOURTEXT] = "Texte couleur/taux de montée";
  txt[PS_TEXTATTOP] = "Text at top of screen";
  txt[PS_FREEFLYONSTARTUP] = "Free fly on startup";
  txt[PS_ENABLESOCKETCONTROLLER] = "Enable socket controller";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Text background opacity";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Text background colour";
  txt[PS_MAXNUMBERAI] = "Max number of AI";
  txt[PS_TOOMANYAI] = "Note: Too many AI will cause crashes & slowdown!";
  txt[PS_UNITS] = "Unités";
  txt[PS_RACESETTINGS] = "Réglages course";
  txt[PS_RACEVIBRATIONAMOUNT] = "Niveau de vibration";
  txt[PS_RACEBEEPVOLUME] = "Volume des Beeps";
  txt[PS_LIMBODIFFICULTY] = "Difficulté du Limbo";
  txt[PS_ENABLESCORELOOP] = "Activer calcul des points";
  txt[PS_DELETELOCALHIGHSCORES] = "Supprimer records locaux";
  txt[PS_GENERALSETTINGS] = "Réglages globaux";
  txt[PS_COLOURSCHEME] = "Gamme de Couleurs";
  txt[PS_COLOUROFFSET] = "Schéma des couleurs";
  txt[PS_BALLAST] = "Ballast";
  txt[PS_BALLASTFWD] = "Ballast avant";
  txt[PS_BALLASTLEFT] = "Ballast gauche";
  txt[PS_BALLASTUP] = "Ballast haut";
  txt[PS_DRAGMULTIPLIER] = "Coeff. de vivacité";
  txt[PS_SIZEMULTIPLIER] = "Coeff. de taille";
  txt[PS_MASSMULTIPLIER] = "Coeff. de masse";
  txt[PS_ENGINEMULTIPLIER] = "Coeff. de propulsion";
  txt[PS_HASVARIOMETER] = "Equipé variomètre";
  txt[PS_PREFERREDCONTROLLER] = "Controle préféré";
  txt[PS_SHOWBUTTON1] = "Show button 1";
  txt[PS_SHOWBUTTON2] = "Show button 2";
  txt[PS_LAUNCH] = "Lancement";
  txt[PS_HOOKS] = "Tow hooks";
  txt[PS_BUNGEELAUNCH] = "Bungee launch";
  txt[PS_AEROTOWLAUNCH] = "Aerotow launch";
  txt[PS_FLATLAUNCHMETHOD] = "Launch method (flat sceneries)";
  txt[PS_HAND] = "Hand";
  txt[PS_BUNGEE] = "Bungee";
  txt[PS_AEROTOW] = "Aerotow";
  txt[PS_LAUNCHSPEED] = "Vitesse lancement";
  txt[PS_LAUNCHANGLE] = "Angle lancement";
  txt[PS_LAUNCHUP] = "Lancement haut";
  txt[PS_LAUNCHFORWARD] = "Lancement avant";
  txt[PS_LAUNCHLEFT] = "Lancement gauche";
  txt[PS_LAUNCHOFFSETUP] = "Launch offset up";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relance auto. si immobile";
  txt[PS_CRASHDETECTION] = "Crash detection";
  txt[PS_CRASHDELTAVELX] = "Delta velocity fwd/back";
  txt[PS_CRASHDELTAVELY] = "Delta velocity sideways";
  txt[PS_CRASHDELTAVELZ] = "Delta velocity up/down";
  txt[PS_CRASHDELTAANGVELX] = "Delta roll velocity";
  txt[PS_CRASHDELTAANGVELY] = "Delta pitch velocity";
  txt[PS_CRASHDELTAANGVELZ] = "Delta yaw velocity";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Suspension resilience scale";
  txt[PS_AIRFRAME] = "Airframe";
  txt[PS_UNDERCARRIAGE] = "Undercarriage";
  txt[PS_PROPELLER] = "Propeller";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Belly hook offset forwards";
  txt[PS_BELLYHOOKOFFSETLEFT] = "Belly hook offset left";
  txt[PS_BELLYHOOKOFFSETUP] = "Belly hook offset up";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Nose tow hook offset forwards";
  txt[PS_NOSEHOOKOFFSETLEFT] = "Nose tow hook offset left";
  txt[PS_NOSEHOOKOFFSETUP] = "Nose tow hook offset up";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Tail tow hook offset forwards";
  txt[PS_TAILHOOKOFFSETLEFT] = "Tail tow hook offset left";
  txt[PS_TAILHOOKOFFSETUP] = "Tail tow hook offset up";
  txt[PS_MAXBUNGEELENGTH] = "Longueur caoutchouc";
  txt[PS_MAXBUNGEEACCEL] = "Puissance caoutchouc";
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
  txt[PS_TETHERING] = "Vol circulaire";
  txt[PS_TETHERLINES] = "Nombre de cables";
  txt[PS_TETHERREQUIRESTENSION] = "Requiert tension pour controle";
  txt[PS_TETHERPHYSICSOFFSETFORWARD] = "Décalage physique avant";
  txt[PS_TETHERPHYSICSOFFSETLEFT] = "Décalage physique gauche";
  txt[PS_TETHERPHYSICSOFFSETUP] = "Décalage physique haut";
  txt[PS_TETHERVISUALOFFSETFORWARD] = "Décalage visuel avant";
  txt[PS_TETHERVISUALOFFSETLEFT] = "Décalage visuel gauche";
  txt[PS_TETHERVISUALOFFSETUP] = "Décalage visuel haut";
  txt[PS_TETHERDISTANCELEFT] = "Distance de cables à gauche";
  txt[PS_TETHERCOLOURR] = "Tether colour: red";
  txt[PS_TETHERCOLOURG] = "Tether colour: green";
  txt[PS_TETHERCOLOURB] = "Tether colour: blue";
  txt[PS_TETHERCOLOURA] = "Tether colour: opacity";
  txt[PS_CHASECAMERA] = "Caméra suiveuse";
  txt[PS_CAMERATARGETPOSFWD] = "Target offset fwd";
  txt[PS_CAMERATARGETPOSUP] = "Target offset up";
  txt[PS_DISTANCE] = "Distance";
  txt[PS_HEIGHT] = "Hauteur";
  txt[PS_VERTICALVELFRAC] = "Angle latéral";
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
  txt[PS_CURRENTPOSITION] = "Position";
  txt[PS_MASS] = "Masse";
  txt[PS_INERTIA] = "Inertia";
  txt[PS_WINGAREA] = "Wing area";
  txt[PS_EXTENTS] = "Extents";
  txt[PS_WINDSETTINGS] = "Réglages de vent";
  txt[PS_WINDSPEED] = "Vitesse";
  txt[PS_WINDBEARING] = "Direction";
  txt[PS_CANNOTMODIFYSCENERY] = "Modification impossible pendant la course";
  txt[PS_ALLOWBUNGEE] = "Usage de la catapulte";
  txt[PS_WINDGUSTTIME] = "Fréquence rafales";
  txt[PS_WINDGUSTFRACTION] = "Volume rafales";
  txt[PS_WINDGUSTANGLE] = "Angle rafales";
  txt[PS_TURBULENCEAMOUNT] = "Niveau Turbulences";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Surface turbulence";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Wind shear turbulence";
  txt[PS_WINDLIFTSMOOTHING] = "Adoucir effet de pente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Tendance cisaillements ";
  txt[PS_ROTORTENDENCY] = "Rotor tendency";
  txt[PS_DEADAIRTURBULENCE] = "Coef. Turbulences";
  txt[PS_BOUNDARYLAYERDEPTH] = "Boundary layer depth";
  txt[PS_THERMALSETTINGS] = "Réglage Thermiques (Option 'Ciels')";
  txt[PS_DENSITY] = "Densité";
  txt[PS_RANGE] = "Etendue";
  txt[PS_LIFESPAN] = "Durée";
  txt[PS_DEPTH] = "Epaisseur";
  txt[PS_CORERADIUS] = "Rayon";
  txt[PS_DOWNDRAFTEXTENT] = "Diminution";
  txt[PS_UPDRAFTSPEED] = "Création";
  txt[PS_ASCENTRATE] = "Taux de montée";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Expansion over lifespan";
  txt[PS_RUNWAY] = "Piste";
  txt[PS_RUNWAYTYPE] = "Runway type";
  txt[PS_CIRCLE] = "Circle";
  txt[PS_RUNWAYX] = "Position en X";
  txt[PS_RUNWAYY] = "Position en Y";
  txt[PS_RUNWAYHEIGHT] = "Hauteur";
  txt[PS_RUNWAYANGLE] = "Angle";
  txt[PS_RUNWAYLENGTH] = "Longueur/Rayon";
  txt[PS_RUNWAYWIDTH] = "Largeur";
  txt[PS_SURFACESETTINGS] = "Paramètres de surface";
  txt[PS_SURFACEROUGHNESS] = "Rugosité";
  txt[PS_SURFACEFRICTION] = "Frottement";
  txt[PS_RANDOMTERRAINSETTINGS] = "Réglages terrains aléatoires";
  txt[PS_RANDOMSEED] = "Végétation aléatoire";
  txt[PS_DISPLACEMENTHEIGHT] = "Variations de hauteur";
  txt[PS_SMOOTHNESS] = "Adoucissements";
  txt[PS_EDGEHEIGHT] = "Hauteur de crêtes";
  txt[PS_UPWARDSBIAS] = "Angle de pente";
  txt[PS_FILTERITERATIONS] = "Filtrage itérations";
  txt[PS_PLAINSETTINGS] = "Plain/water settings";
  txt[PS_DRAWPLAIN] = "Visu plaine/eau";
  txt[PS_COLLIDEWITHPLAIN] = "Gestion Collision sol";
  txt[PS_PLAININNERRADIUS] = "Rayon intérieur plaine";
  txt[PS_PLAINFOGDISTANCE] = "Distance brume en plaine";
  txt[PS_PLAINHEIGHT] = "Hauteur plaine";
  txt[PS_TERRAINSIZE] = "Taille Terrain";
  txt[PS_COASTENHANCEMENT] = "Détails bord de mer";
  txt[PS_TERRAINDETAIL] = "Détails Terrain";
  txt[PS_RIDGETERRAINSETTINGS] = "Réglage crêtes/terrain";
  txt[PS_MAXHEIGHTFRACTION] = "Hauteur maximum fractales";
  txt[PS_WIDTH] = "Largeur";
  txt[PS_HEIGHTOFFSET] = "Hauteur décalages";
  txt[PS_HORIZONTALVARIATION] = "Variation Horizontale";
  txt[PS_HORIZONTALWAVELENGTH] = "Ondulations horizontales";
  txt[PS_VERTICALVARIATION] = "Variation Verticale";
  txt[PS_PANORAMASETTINGS] = "Réglages Panorama";
  txt[PS_PANORAMA3DSETTINGS] = "Réglage Panorama 3D";
  txt[PS_HEIGHTFIELDSETTINGS] = "Réglages hauteur";
  txt[PS_MINHEIGHT] = "Hauteur Minimum";
  txt[PS_MAXHEIGHT] = "Hauteur Maximum";
  txt[PS_AISCENERY] = "AI Scenery";
  txt[PS_SCENETYPE] = "Scene type";
  txt[PS_FLAT] = "Flat";
  txt[PS_SLOPE] = "Slope";
  txt[PS_CURRENTVIEWPOSITION] = "Coordonnées 'positionpilote'";
  txt[PS_NOLIGHTINGSETTINGS] = "Pas de réglage/ciel pour scène 'Panorama'";
  txt[PS_SUNBEARING] = "Niveau d'ensoleillement";
  txt[PS_THERMALACTIVITY] = "Activité Thermique";
  txt[PS_TERRAINDARKNESS] = "Ombrage Terrain";
  txt[PS_CONTROLLERSEESETTINGS] = "Voir aussi réglages controle additionnel sous les options";
  txt[PS_CROSS] = "Croix";
  txt[PS_BOX] = "Boite";
  txt[PS_CROSSANDBOX] = "Croix & Boite";
  txt[PS_CONFIGURATION] = "Configuration";
  txt[PS_STYLE] = "Style";
  txt[PS_CONTROLLERMODE] = "Mode de controle";
  txt[PS_TREATTHROTTLEASBRAKES] = "Treat throttle stick as brakes";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Reset configuration on launch";
  txt[PS_NUMCONFIGURATIONS] = "Numéro configuration";
  txt[PS_RIGHTSTICKHORIZONTAL] = "Manche droit horizontal";
  txt[PS_RIGHTSTICKVERTICAL] = "Manche droit vertical";
  txt[PS_LEFTSTICKHORIZONTAL] = "Manche gauche horizontal";
  txt[PS_LEFTSTICKVERTICAL] = "Manche gauche vertical";
  txt[PS_TILTHORIZONTAL] = "Panoramique horizontal";
  txt[PS_TILTVERTICAL] = "Panoramique vertical";
  txt[PS_ARROWSHORIZONTAL] = "Fleches horizontales";
  txt[PS_ARROWSVERTICAL] = "Fleches verticales";
  txt[PS_ROLLSTICK] = "Manche de roulis";
  txt[PS_PITCHSTICK] = "Manche de profondeur";
  txt[PS_YAWSTICK] = "Manche de lacet";
  txt[PS_SPEEDSTICK] = "Manche de vitesse";
  txt[PS_CONSTANT] = "Constante";
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
  txt[PS_RATESBUTTON] = "Bouton Taux vario";
  txt[PS_RATESCYCLEBUTTON] = "Bouton cycle Taux vario";
  txt[PS_RELAUNCHBUTTON] = "Bouton lancement";
  txt[PS_CAMERABUTTON] = "Bouton point de vue";
  txt[PS_PAUSEPLAYBUTTON] = "Bouton Pause/Démarrer";
  txt[PS_NONE] = "Rien";
  txt[PS_CONTROLSOURCES] = "Sources de Controle";
  txt[PS_AILERONS] = "Ailerons/Dérive";
  txt[PS_ELEVATOR] = "Profondeur";
  txt[PS_RUDDER] = "Dérive";
  txt[PS_THROTTLE] = "Gazs/Volets/Freins";
  txt[PS_LOOKYAW] = "Voir Yaw";
  txt[PS_LOOKPITCH] = "Voir Pitch";
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
  txt[PS_SETTINGSFORCONTROLLER] = "Réglages configuration Controles %d: %s";
  txt[PS_TRIMSETTINGS] = "Réglages de Trim";
  txt[PS_NOSIMPLESETTINGS] = "Pas de réglage simple dispo.";
  txt[PS_TRIM] = "Trim";
  txt[PS_SCALE] = "Echelle";
  txt[PS_CLAMPING] = "Clamping";
  txt[PS_EXPONENTIAL] = "Exponentiel";
  txt[PS_SPRING] = "Rappel neutre";
  txt[PS_POSITIVE] = "Positive";
  txt[PS_NEGATIVE] = "Negative";
  txt[PS_ROLLSTICKMOVEMENT] = "Mouvement du manche de roulis";
  txt[PS_PITCHSTICKMOVEMENT] = "Mouvement du manche de profondeur";
  txt[PS_YAWSTICKMOVEMENT] = "Mouvement du manche de lacet";
  txt[PS_SPEEDSTICKMOVEMENT] = "Mouvement du manche de vitesse";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Gyroscope du mobile : Mouvement en roulis";
  txt[PS_ACCELEROMETERROLL] = "Roulis par le gyro. du mobile";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Gyroscope du mobile : Mouvement montée/descente";
  txt[PS_ACCELEROMETERPITCH] = "Montée/descente par le gyro. du mobile";
  txt[PS_TILTROLLSENSITIVITY] = "Sensibilité roulis";
  txt[PS_TILTPITCHSENSITIVITY] = "Sensibilité montée/descente";
  txt[PS_TILTNEUTRALANGLE] = "Zone neutre";
  txt[PS_NOJOYSTICKWITHID] = "0 Joystick avec cet IDentifiant";
  txt[PS_ENABLEJOYSTICK] = "Activer joystick";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Mouvements de manche courbes";
  txt[PS_EXTERNALJOYSTICKSETTINGS] = "Réglages joystick ext.";
  txt[PS_JOYSTICKLABEL] = "Joystick %d: Entrée = %5.2f Sortie = %5.2f";
  txt[PS_JOYSTICKABUTTONLABEL] = "Joystick %d: Entrée = %5.2f Sortie = %d";
  txt[PS_JOYSTICKBUTTONLABEL] = "Bouton %d: Entrée = %5.2f Sortie = %d";
  txt[PS_SCALEPOSITIVE] = "Input +ve scale";
  txt[PS_SCALENEGATIVE] = "Input -ve scale";
  txt[PS_OFFSET] = "Input Offset";
  txt[PS_MAPTO] = "Appliquer à";
  txt[PS_PRESSWHENCENTRED] = "Press when centred";
  txt[PS_PRESSWHENLEFTORDOWN] = "Press when left or down";
  txt[PS_PRESSWHENRIGHTORUP] = "Press when right or up";
  txt[PS_DEADZONE] = "Zone neutre";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Mise à 0 réglages joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Démarrer calibrage joystick (Windows)";
  txt[PS_LOADOPTIONS] = "Charger options";
  txt[PS_SAVEOPTIONS] = "Sauver options";
  txt[PS_DELETEOPTIONS] = "Supprimer options";
  txt[PS_LOADSCENERY] = "Charger scène";
  txt[PS_SAVESCENERY] = "Sauver scène";
  txt[PS_DELETESCENERY] = "Supprimer scène";
  txt[PS_LOADOBJECTS] = "Load objects";
  txt[PS_SAVEOBJECTS] = "Save objects";
  txt[PS_DELETEOBJECTS] = "Delete objects";
  txt[PS_LOADLIGHTING] = "Charger ciel";
  txt[PS_SAVELIGHTING] = "Sauver ciel";
  txt[PS_DELETELIGHTING] = "Supprimer ciel";
  txt[PS_LOADCONTROLLER] = "Charger boitier de contrôle";
  txt[PS_SAVECONTROLLER] = "Sauver boitier de contrôle";
  txt[PS_DELETECONTROLLER] = "Supprimer boitier de contrôle";
  txt[PS_LOADJOYSTICK] = "Charger joystick";
  txt[PS_SAVEJOYSTICK] = "Sauver joystick";
  txt[PS_DELETEJOYSTICK] = "Supprimer joystick";
  txt[PS_LOADAEROPLANE] = "Charger planeur";
  txt[PS_SAVEAEROPLANE] = "Sauver planeur";
  txt[PS_DELETEAEROPLANE] = "Supprimer planeur";
  txt[PS_LOADAICONTROLLERS] = "Load AI controllers";
  txt[PS_SAVEAICONTROLLERS] = "Save AI controllers";
  txt[PS_DELETEAICONTROLLERS] = "Delete AI controllers";
  txt[PS_SELECTPANORAMA] = "Selection Panorama";
  txt[PS_SELECTTERRAINFILE] = "Selection fichier de terrain";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selection contrôle préféré";
  txt[PS_SELECTOBJECTSSETTINGS] = "Select objects settings";
  txt[PS_SUMMARY] = "Summary";
  txt[PS_NOOBJECTS] = "There are no objects loaded (other than the terrain etc). You can load a set of static and dynamic objects, "
    "making sure they're suitable for the current scene. By enabling object editing you can "
    "create your own - see help and the PicaSim web page for more info.";
  txt[PS_OBJECTSTOTAL] = "Total number of objects:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Number static visible:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Number static invisible:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Number dynamic visible:";
  txt[PS_ABOUT] = "A propos";
  txt[PS_WEBSITE] = "Notre Site Web";
  txt[PS_FLYING] = "Voler";
  txt[PS_SETTINGS] = "Réglages";
  txt[PS_RACES] = "Courses";
  txt[PS_TIPS] = "Tips";
  txt[PS_KEYBOARD] = "Keyboard";
  txt[PS_CREDITS] = "Contributions";
  txt[PS_LICENCE] = "Licence";
  txt[PS_VERSIONS] = "Versions";
  txt[PS_ABOUTFREETEXT] = "PicaSim est un simulateur de pilotage de modèles radiocommandés. Pour le moment, il est conçu pour vous aider à apprendre "
"à voler et améliorer votre pilotage de planeurs purs en utilisant l'effet dynamique du vent en vol de pente qui "
"est dévié vers le haut des collines ou montagnes.\n"
"\n"
"Si vous n'avez jamais fait volé d' aéromodèles avant, vous allez apprécier de scratcher quelques fois au début (ou plus !) "
"en réessayant de voler immédiatement. C'est bien mieux de scratcher dans le simulateur qu' avec un modele réel et couteux que "
"vous avez passé longtemps à construire et régler.\n"
"\n"
"PicaSim peut vous aider à maitriser trois aspects du vol en pente :\n"
"\n"
"1. Bien utiliser les manettes du boitier de controle pour piloter votre planeur.\n"
"2. Cerner où se placer dans une pente pour bénéficier au mieux de l'effet dynamique.\n"
"3. Controler le modèle pour qu'il reste là où vous l'avez décidé en décollant devant vous.\n"
"\n"
"De plus, les utilisateurs expérimentés peuvent utiliser PicaSim pour l'entrainement à la voltige, ou "
"simplement le plaisir de voler lorsque le vol de pente n'est pas possible par manque de vent !\n"
"\n"
"J'ai beaucoup d'idées pour étendre et améliorer PicaSim - des modeles motorisés, d'autres planeurs, "
"vol en réseau, etc. Si vous voulez me supporter et m'encourager "
"dans cela, ou juste me dire merci pour la version en cours, acheter la version Full ou faire un don paypal "
"sera grandement apprécié !\n";
  txt[PS_ABOUTPAIDTEXT] = "PicaSim est un simulateur de pilotage de modèles radiocommandés. Pour le moment, il est conçu pour vous aider à apprendre "
"à voler et améliorer votre pilotage de planeurs purs en utilisant l'effet dynamique du vent en vol de pente qui "
"est dévié vers le haut des collines ou montagnes.\n"
"\n"
"Si vous n'avez jamais fait voler d' aéromodèles avant, vous allez apprécier de scratcher quelques fois au début (ou plus !) "
"en revolant immédiatement. C'est bien mieux de scratcher dans le simulateur qu' avec un modele réel et couteux que "
"vous avez passé longtemps à construire et régler.\n"
"\n"
"PicaSim peut vous aider à maitriser trois aspects du vol en pente :\n"
"\n"
"1. Utiliser les manettes du boitier de controle pour piloter votre aéromodèle.\n"
"2. Cerner où se placer dans une pente pour bénéficier au mieux de l'effet porteur dynamique.\n"
"3. Controler le modèle pour qu'il reste là où vous l'avez décidé en décollant juste devant vous.\n"
"\n"
"De plus, les utilisateurs expérimentés peuvent utiliser PicaSim pour l'entrainement à la voltige, ou "
"simplement le plaisir de voler lorsque le vol de pente n'est pas possible par manque de vent !\n"
"\n"
"J'ai beaucoup d'idées pour étendre et améliorer PicaSim - des modeles motorisés, d'autres planeurs "
", vol en réseau, etc. Si vous voulez me supporter et m'encourager "
"dans cela, ou juste me dire merci pour la version en cours, acheter la version Full ou faire un don paypal "
"sera grandement apprécié !\n";
  txt[PS_HOWTOFLYTEXT] = "La premiere chose à faire est de visiter notre site (lien ci-dessous) et de regarder la courte vidéo \"How to Fly\". \n"
"\n"
"Lors du démarrage du simulateur, il est en mode pause. Le bouton dans le coin haut à droite passe en mode vol, "
"puis lance le planeur dans le vent. Vous devez utiliser le/les joysticks dans le coin en bas "
"à droite de l'écran pour piloter - comme un manche de télécommande (en mode 2)  "
"quand vous plantez le modèle, vous pouvez utiliser le bouton retour de votre emetteur pour "
"relancer le planeur, ou mettre en pause à l'écran le simulateur puis appuyer sur le bouton 'rewind' , et relancer le modèle à nouveau.\n"
"\n"
"Une fois les bases du controle acquises, vous avez besoin de positionner le modele pour qu'il reste "
"dans la zone de la pente où le vent est le plus dévié vers le haut et au-dessus de la pente. La meilleure facon de faire ceci (en "
"premier) est de voler devant soi trois ou quatre secondes, puis de tourner vers la gauche et de voler parallèlement à la pente "
"pendant une dizaine de secondes. Ensuite faire demi-tour pour revenir également parallèle à la pente jusque face au point de décollage, restez "
"toujours parallèle à la pente permet de rester au moins à la hauteur du point de décollage.\n"
"\n"
"Si vous trouvez que les choses vont trop vite, vous pouvez ralentir l'échelle de temps dans Réglages»Panneau options. Retenez que "
"pour revenir aux réglages de base avec la valeur 1.0 quand vous avez la maitrise de ces choses rapides - cela n'existe pas dans "
"la vraie vie !\n"
"\n"
"Vous pouvez changer le point de vue pour etre comme dans le modèle sauf si vous êtes dans une scène panoramique, ce changement peut se faire "
"en cliquant sur le bouton 'oeil' en mode pause, (ou le bouton 'recherche' "
"de votre systeme de controle). Maintenant vous pouvez partir en exploration - prendre plus de hauteur et vous pouvez partir en cross "
"jusqu'à que le vent/pente ne soient plus suffisants pour se maintenir. Utiliser la fleche de vent pour visualiser d'où il vient "
"et retrouver un endroit plus favorable pour se maintenir ou monter.\n"
"\n"
"Si le terrain est essentiellement plat il sera souhaitable d'activer les ascendances thermiques, et les planeurs seront lancés par treuillage. "
"Vous aurez besoin d'observer avec attention les mouvements du planeur pour détecter où sont les thermiques "
"Si vous trouvez cela difficile, il est possible de visualiser les thermiques dans les options - Le centre de chaque thermique sera matérialisé "
"par un disque rouge de la taille approximative du planeur. Cela vous aidera à estimer la distance. Retenez que "
"les thermiques sont plus forts sur la terre que sur l'eau, et également sur les versants exposés au soleil.\n"
"\n"
"Des vidéos visualisent cela en montrant 'comment voler ?' sur notre site Web - cela sera un bon entrainement au pilotage des vrais modèles";
  txt[PS_HOWTOCONFIGURETEXT] = "Vous pouvez accéder aux réglages de Picasim depuis le menu initial, ou en utilisant le bouton 'engrenage' lors du mode pause.\n"
"\n"
"Les réglages sont regroupés par blocs - Options générales, Aéromodèle, Scene/terrain, Ciel/nuages "
"et config controles. Par défaut vous affichez les réglages de base pour chaque bloc mais vous  "
"pouvez charger des configurations préréglées. Cela pour permet de changer rapidement de modèle ou de terrain, par example.\n"
"\n"
"Si vous validez '+ d'options' dans chaque section vous allez avoir de nombreux réglage possibles, cela peut devenir embarrassant "
"Par la suite, vous connaitrez la fonction de chaque réglage mais pour l'instant, gardez des réglages de base commeutilisés dans les préréglages "
"\n"
"Vous pouvez sauvegarder vos propres réglages - ils seront sauvés dans l'espace mémoire de votre mobile ou PC (carte ou disque dur). "
"Si vous rencontrez des difficultés avec vos propres réglages, il est possible de revenir aux réglages initiaux juste après le redémarrage de Picasim.\n"
"\n"
"Si 'positionpilote' est validé dans les options vous aurez aussi un bouton 'positionpilote' en mode pause. Cela vous permettra de changer la position du pilote en agissant sur les joysticks "
"du boitier de controle, possibilité de faire varier à l'infini les lieux de décollage. Par défaut la direction du vent est "
"automatiquement ajustée pour être de face à la position du pilote.\n"
"\n"
"Avec les scenes panoramiques, il n'est pas possible d'agir sur la position du pilote. Le mode 'positionpilote' est désactivé";
  txt[PS_HOWTORACETEXT] = "Le mode Course consiste à circuler le plus vite possible entre 2 balises. La balise où vous devez vous diriger est visualisée"
"en couleur, et aussi pointée par une fleche blanche dans le bas de l'ecran.\n"
"\n"
"Il y a différents types de balises: Dans les courses du style F3F, vous devez aller et venir plusieurs fois d'une balise à l'autre, "
"vous devez dépasser les balises et tourner face au vent de celles-ci. Dans une course de type Cross-Country, vous devez juste passer au dessus des balises "
" (à la hauteur que vous souhaitez). Dans les course de type 'plaine' vous devez voler entre deux 'portes' ou 'plans' - La balise au vent est verte, celle sous le vent est rouge. \n"
"\n"
"Quand la course est terminée les points et classement sont automatiquement ajoutés au palmares global, malgré vous "
"Ce peut être désactivé dans les options. Vous pouvez selectionner le nom qui s'affiche dans la section profil de l'écran de menu 'classement'. "
"Si vous mettez une adresse email cela vous permettra de faire le lien entre le mobile ou PC utilisé.";
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
  txt[PS_CREDITSTEXT] = "PicaSim est développé par Danny Chapman\n"
"Avec l'aide et en remerciant :\n\n"
"Heidi and Hazel pour leur patience\n\n"
"Heidi, Jenne, André et Sylvain pour les traductions\n\n"
"Bullet physics\n\n"
"TinyXML\n\n"
"Marmalade pour la plateforme de distribution\n\n"
"Gleb Lebedev pour son gamepad extension\n\n"
"Detlef Jacobi pour Litermonthalle Nalbach/\n\n"
"Quelques sons depuis www.freesfx.co.uk\n\n"
"Textures depuis seamless-pixels.blogspot.co.uk/\n\n"
"Tous ceux qui m'ont aidé par leur tests et signalements de bugs\n\n"
"Modèles volants réalisés par:\n\n"
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
"Veuillez noter que même si j'ai essayé de concevoir des modèles au vol réaliste, je ne prétends pas avoir "
"parfaitement atteint cet objectif. La seule façon de le découvrir, c'est d'aller sur le terrain faire voler les vrais, "
"Je vous invite à le faire !";
  txt[PS_LICENCETEXT] = "PicaSim is available from me in the following versions:\n\n"
"Free: The simulator and assets (including planes and sceneries etc) may be distributed according to the Creative Commons Attribution-NonCommercial-NoDerivs licence.\n\n"
"Paid: This version or its derivatives may only be distributed with my explicit authorisation.\n\n"
"If you wish to share content that you have created then you must do so separately from the PicaSim distribution.\n\n"
"The assets in PicaSim may be distributed according to the Creative Commons Attribution-NonCommercial licence (i.e. you may modify and distribute them, so long as you credit the source).\n\n"
"It is quite easy to distribute content for PicaSim by placing it in the UserData and UserSettings. See the website and ask on the forum for more details.";
  txt[PS_TIPS1] = "Svp, envoyez moi des retours d'expérience que je puisse améliorer PicaSim";
  txt[PS_TIPS2] = "Si vous aimez PicaSim, Svp laissez un commentaire";
  txt[PS_TIPS3] = "Vous pouvez utiliser le gyroscope du mobile pour piloter - Réglage>Contrôle";
  txt[PS_TIPS4] = "Aimez/suivez PicaSim sur Facebook ou Google+ pour être informé des mises à jour et améliorations prévues";
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
  txt[PS_ALLOWCHOICE] = "Choix d'avion/scène la prochaine fois";
  txt[PS_LOADING] = "Chargement...";
  txt[PS_RACETIP] = "%s\n\nThe white arrow points to the next checkpoint.";
  txt[PS_LIMBOTIP] = "%s\n\nAjuster difficulté/score multiples dans Réglages»Mode";
  txt[PS_DURATIONTIP] = "%s\n\nLook for thermals and land close to the launch point.";
  txt[PS_TRAINERGLIDERTIP] = "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles pour "
    "commander profondeur et dérive, Faire voler le planeur en avant de la pente. Consulter le site Web "
    "pour des astuces et conseils de vol si vous n'arrivez pas à rester en l'air!";
  txt[PS_TRAINERPOWEREDTIP] = "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles droit "
    "pour commander profondeur et ailerons, controles gauches pour commander moteur et dérive. Consulter "
    "le site Web pour des astuces et conseils de vol si vous n'arrivez pas à rester en l'air!";
  txt[PS_MOUSETOCONTINUE] = "Cliquer souris ou taper une touche pour continuer";
  txt[PS_TAPTOCONTINUE] = "Une tape pour continuer";
  txt[PS_ALLRESULTS] = "Tous les résultats";
  txt[PS_LAST24HOURS] = "Sur 24 heures";
  txt[PS_LAST7DAYS] = "Sur 7 jours";
  txt[PS_LAST30DAYS] = "Sur 30 jours";
  txt[PS_TIMEPERIODFORRESULTS] = "Période de temps pour les résultats";
  txt[PS_SCORELINERACE] = "  Score = %.1f Time = %.2f";
  txt[PS_SCORELINELIMBO] = "  Score = %.1f Count = %d";
  txt[PS_NOSCORES] = "Pas de points";
  txt[PS_CURRENTSCORE] = "Points actuels = %.1f Time = %.2f";
  txt[PS_NOSCORESINTIME] = "Pas de points dans la période";
  txt[PS_LEADERBOARDFAILED] = "Calcul Palmares errone - Verifier la connection internet";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Calcul Palmares errone, reessai en cours...";
  txt[PS_RETRIEVINGLEADERBOARD] = "Classement en cours...";
  txt[PS_SCORESUBMISSIONFAILED] = "Soumission du score erronee - Verifier la connection internet";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Soumission du score erronee, reessai en cours...";
  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";
  txt[PS_SETUSERNAME] = "Pilote ?";
  txt[PS_SETEMAIL] = "Email ?";
  txt[PS_USERFAILED] = "Requete utilisateur erronee - Verifier la connection internet";
  txt[PS_USERFAILEDRETRYING] = "Requete utilisateur erronee, reessai en cours...";
  txt[PS_PROFILE] = "Profil ?";
  txt[PS_USERNAME] = "Pilote";
  txt[PS_EMAIL] = "Email";
  txt[PS_LEADERBOARDS] = "Classements";
  txt[PS_UPLOADINGSCORE] = "Calcul des points...";
  txt[PS_REQUESTINGUSER] = "Demande de détails utilisateur...";
  txt[PS_SELECTRACE] = "Selection course";
  txt[PS_SELECTRACETOSEE] = "Choisir course pour voir le classement";
  txt[PS_EDIT] = "Ecrire";
  txt[PS_PROFILESTATUS] = "Etat du Profil";
  txt[PS_SCORELOOPUSERNAME] = "Points du pilote";
  txt[PS_EMAILADDRESS] = "Adresse Email";
  txt[PS_OPTIONALEMAIL] = "Optionnel - Liens croisés pilotes/pilotage";
  txt[PS_FETCHINGEMAIL] = "Recherche email";
  txt[PS_PREV] = "Avant";
  txt[PS_TOP] = "Haut";
  txt[PS_ME] = "Moi";
  txt[PS_NEXT] = "Suivant";
  txt[PS_UPDATINGLEADERBOARD] = "Calcul du classement...";
  txt[PS_NOMORESCORES] = "Pas plus de points";
  txt[PS_SELECTARACE] = "Choisir une course";
  txt[PS_UPLOADINGDATA] = "Mise à jour données...";
  txt[PS_FAILED] = "Erreur: ";
  txt[PS_EMAILTAKEN] = "Email Déjà utilisé. ";
  txt[PS_EMAILINVALID] = "Format email invalide. ";
  txt[PS_USERTAKEN] = "Pilote déjà utilisé. ";
  txt[PS_USERINVALID] = "Format Pilote invalide. ";
  txt[PS_USERSHORT] = "Pilote trop court. ";
  txt[PS_QUITANDHELP] = "Quitter et aider";
  txt[PS_CAMERAANDSETTINGS] = "Caméra et réglages";
  txt[PS_RESETANDPLAY] = "Relance et jouer/pause";
  txt[PS_THROTTLEANDRUDDER] = "Gazs/Volets/Freins et dérive";
  txt[PS_WINDDIRECTION] = "Direction du vent";
  txt[PS_AILERONSANDELEVATOR] = "Ailerons et elevator";
  txt[PS_FLYINGINFO] = "Voler";
  txt[PS_QUITANDHELPTEXT] = "When paused press X to go back to the main menu, and press ? to get this help. \n\nWhen flying, some planes have buttons that are used to switch between controller configurations (e.g. flaps up/down), and tow/winch cable release.";
  txt[PS_CAMERAANDSETTINGSTEXT] = "When paused, the cog allows you to access the settings. \n\nThe eye (only available if it's a 3D environment) allows you to change the camera/viewpoint.";
  txt[PS_RESETANDPLAYTEXT] = "Press the play/pause buttons to start and stop the simulation. \n\nUse the rewind arrow next to it to relaunch. \n\nOptional buttons 1 and 2 are for smoke.";
  txt[PS_THROTTLEANDRUDDERTEXT] = "Depending on the plane type, move the controller left/right to control rudder, and up/down to control either the throttle or brakes. \n\nTwo-channel gliders won't have this control.";
  txt[PS_WINDDIRECTIONTEXT] = "If there's some wind, the blue arrow shows its direction. \n\nThere may be some numbers giving basic flight info like speed and height too.";
  txt[PS_AILERONSANDELEVATORTEXT] = "By default, move the controller left/right to control ailerons (steering), and up/down to control elevator (descend/climb). \n\nR/C pilots: The transmitter mode can changed in the Settings»Options panel.";
  txt[PS_FLYINGINFOTEXT] = "It takes some time and effort to learn to fly - just as it would flying a real R/C plane. \n\nFor more guidance, use the help button back in the main menu.";
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

