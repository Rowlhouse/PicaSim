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
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Activer";
  txt[PS_REMOVE] = "Supprimer";
  txt[PS_DUPLICATE] = "Dupliquer";
  txt[PS_CRASHED] = "Scratché";
  txt[PS_NO] = "Non";
  txt[PS_YES] = "Oui";
  txt[PS_INVERT] = "Inverser";
  txt[PS_AUTO] = "Auto";
  txt[PS_GAMEPADDETECTED] = "Manette détectée. Activez-la dans les réglages pour l'utiliser.";
  txt[PS_WHATSNEW] = "Bienvenue sur PicaSim!";
  txt[PS_FREEFLY] = "Vol Libre";
  txt[PS_CHALLENGE] = "Défi";
  txt[PS_SELECTAEROPLANE] = "Planeur";
  txt[PS_SELECTSCENERY] = "Scène";
  txt[PS_SELECTOPTIONS] = "Veuillez séléctionner les options par défaut";
  txt[PS_SELECTSCENARIO] = "Scénario";
  txt[PS_USEDEFAULT] = "Usage par defaut";
  txt[PS_USEDEFAULTPREVIOUS] = "Usage par defaut précédent";
  txt[PS_BACK] = "Retour";
  txt[PS_OPTIONS1] = "Options 1";
  txt[PS_OPTIONS2] = "Options 2";
  txt[PS_AEROPLANE] = "Planeurs";
  txt[PS_SCENERY] = "Scènes";
  txt[PS_OBJECTS] = "Objets";
  txt[PS_OBJECTSSETTINGS] = "Paramètres objets";
  txt[PS_LIGHTING] = "Ciels";
  txt[PS_AICONTROLLERS] = "Contrôleurs IA";
  txt[PS_CONTROLLER] = "Controles";
  txt[PS_JOYSTICK] = "Joysticks";
  txt[PS_LOAD] = "Charger...";
  txt[PS_SAVE] = "Sauver...";
  txt[PS_DELETE] = "Supprimer...";
  txt[PS_ADVANCED] = "+ d'options";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Effacer tous les réglages et quitter";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "Cela effacera tous les réglages sauvegardés et quittera PicaSim - êtes-vous sûr?";
  txt[PS_SIMPLE] = "- d'options";
  txt[PS_FILENAME] = "NomFichier:";
  txt[PS_LANGUAGESETTINGS] = "Réglage langue";
  txt[PS_CURRENTLANGUAGE] = "Langue en cours";
  txt[PS_CAMERASETTINGS] = "Réglage camera";
  txt[PS_ZOOMVIEW] = "Vue Zoom";
  txt[PS_PLANEONLYINZOOMVIEW] = "Afficher l'avion seul en vue zoom";
  txt[PS_SMOKEONLYINMAINVIEW] = "Afficher fumée en vue principale";
  txt[PS_ZOOMVIEWSIZE] = "Taille Vue Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Autozoom depuis le sol";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Echelle autozoom sol";
  txt[PS_GROUNDFIELDOFVIEW] = "Vue vers le sol";
  txt[PS_GROUNDHORIZONAMOUNT] = "Niveau sol/horizon";
  txt[PS_GROUNDLAG] = "Retard caméra sol";
  txt[PS_GROUNDVIEWFOLLOW] = "Centrer modèle en vol à l'écran";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Décalage lacet vue sol";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Décalage tangage vue sol";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Angle/Vue depuis modèle";
  txt[PS_WALKABOUTSETTINGS] = "Réglages positionpilote";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activer bouton positionpilote";
  txt[PS_ENABLEOBJECTEDITING] = "Activer édition objets";
  txt[PS_FORCEALLVISIBLE] = "Rendre visible lors de l'édition";
  txt[PS_RESETOBJECTS] = "Réinitialiser objets et recharger scène";
  txt[PS_OBJECTEDITING] = "Édition objets";
  txt[PS_OBJECTEDITINGTEXT] = "PicaSim permet de simuler et d'afficher des objets supplémentaires, en plus du terrain et de l'avion (etc).\n"
"\n"
"Dans l'onglet Réglages»Objets, vous pouvez charger un ensemble de ces objets - ceci est fait automatiquement lors du "
"chargement d'une scène. La plupart des scènes n'incluent pas d'objets. Cependant, certaines scènes panoramiques en "
"contiennent pour représenter des éléments qui ne font pas partie du sol.\n"
"\n"
"Si vous souhaitez ajouter des objets à la scène, vous pouvez activer l'édition d'objets dans Réglages»Objets. Cela fera "
"apparaître des boutons à l'écran vous permettant de créer/supprimer des objets, de changer la sélection et de modifier "
"leurs propriétés. Lors du déplacement, gardez à l'esprit que vous pouvez choisir de travailler dans l'espace monde, "
"relatif à la caméra, ou relatif à l'espace de l'objet. Après avoir créé un ensemble d'objets, n'oubliez pas de les sauvegarder !\n"
"\n";
  txt[PS_OBJECTNUMBER] = "Objet %d";
  txt[PS_POSITIONX] = "Position avant";
  txt[PS_POSITIONY] = "Position gauche";
  txt[PS_POSITIONZ] = "Position haut";
  txt[PS_COLOURR] = "Couleur rouge";
  txt[PS_COLOURG] = "Couleur vert";
  txt[PS_COLOURB] = "Couleur bleu";
  txt[PS_COLOURH] = "Teinte";
  txt[PS_COLOURS] = "Saturation";
  txt[PS_COLOURV] = "Luminosité";
  txt[PS_POSITION] = "Position";
  txt[PS_SIZE] = "Taille";
  txt[PS_COLOUR] = "Couleur";
  txt[PS_VISIBLE] = "Visible";
  txt[PS_SHADOW] = "Ombre";
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
  txt[PS_BRAKESFORWARD] = "Aérofreins avec manche en avant";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Usage tactile absolu";
  txt[PS_STAGGERCONTROLLER] = "Décaler controle (tactile partiel)";
  txt[PS_ENABLEELEVATORTRIM] = "Activer trim profondeur";
  txt[PS_ELEVATORTRIMSIZE] = "Taille trim profondeur";
  txt[PS_SQUARECONTROLLERS] = "Controles en carré";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Décalage horiz. du bord";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Décalage vert. du bord";
  txt[PS_SHAPEOPACITY] = "Opacité grille controle";
  txt[PS_STICKOPACITY] = "Opacité manche controle";
  txt[PS_STICKCROSS] = "Croix du manche";
  txt[PS_JOYSTICKID] = "IDentifiant Joystick";
  txt[PS_JOYSTICKINFO] = "Note: Vous devrez peut-être redémarrer PicaSim après connexion ou déconnexion d'un joystick.";
  txt[PS_NOJOYSTICK] = "Aucun joystick détecté ou disponible.";
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
  txt[PS_THERMALWINDFIELD] = "Afficher champ de vent thermique";
  txt[PS_GRAPHAIRSPEED] = "Visu graph vitesse/air (rouge)";
  txt[PS_GRAPHDURATION] = "Durée du graphique";
  txt[PS_GRAPHALTITUDE] = "Afficher altitude (turquoise)";
  txt[PS_GRAPHGROUNDSPEED] = "Visu graph vitesse/sol (vert)";
  txt[PS_GRAPHCLIMBRATE] = "Visu graph taux/montée (bleu)";
  txt[PS_GRAPHWINDSPEED] = "Visu graph vitesse/vent (jaune)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Visu graph vitesse verticale/vent (Violet)";
  txt[PS_STALLMARKERS] = "Marquer décrochages";
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
  txt[PS_CONTROLLEDPLANESHADOWS] = "Ombres avion contrôlé";
  txt[PS_OTHERSHADOWS] = "Autres ombres";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Détail ombre projetée";
  txt[PS_SHADOWBLURMULTIPLIER] = "Multiplicateur flou d'ombre";
  txt[PS_BLOB] = "Tache";
  txt[PS_PROJECTED] = "Projetée";
  txt[PS_AEROPLANERENDER] = "Rendu du modèle";
  txt[PS_USE16BIT] = "Textures 16 bit (- de mémoire)(redémarrer)";
  txt[PS_SEPARATESPECULAR] = "Calcul spéculaire séparé";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Niveau éclairage ambiant";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Niveau éclairage diffus";
  txt[PS_TERRAINTEXTUREDETAIL] = "Détail texture terrain";
  txt[PS_MAXSKYBOXDETAIL] = "Détail max skybox/panorama";
  txt[PS_ANTIALIASING] = "Anticrénelage (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Redémarrage requis";
  txt[PS_MISCSETTINGS] = "Réglages divers";
  txt[PS_USEBACKBUTTON] = "Bouton retour pour sortir";
  txt[PS_DRAWLAUNCHMARKER] = "Visu. balise de décollage";
  txt[PS_DRAWGROUNDPOSITION] = "Visu. position sol";
  txt[PS_SKYGRIDOVERLAY] = "Grille de ciel";
  txt[PS_SKYGRID_NONE] = "Aucune";
  txt[PS_SKYGRID_SPHERE] = "Sphère";
  txt[PS_SKYGRID_BOX] = "Boîte";
  txt[PS_SKYGRIDALIGNMENT] = "Alignement grille";
  txt[PS_SKYGRIDDISTANCE] = "Distance grille";
  txt[PS_SKYGRIDALIGN_ALONGWIND] = "Dans le vent";
  txt[PS_SKYGRIDALIGN_CROSSWIND] = "Travers vent";
  txt[PS_SKYGRIDALIGN_ALONGRUNWAY] = "Axe de piste";
  txt[PS_SKYGRIDALIGN_CROSSRUNWAY] = "Travers piste";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Controle préféré du modèle";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Réglages test/developpement";
  txt[PS_WIREFRAMETERRAIN] = "Maillage terrain";
  txt[PS_DRAWSUNPOSITION] = "Visu. position soleil";
  txt[PS_FREEFLYSETTINGS] = "Réglages Vol Libre";
  txt[PS_DISPLAYFLIGHTTIME] = "Afficher temps de vol";
  txt[PS_DISPLAYSPEED] = "Afficher vitesse";
  txt[PS_DISPLAYAIRSPEED] = "Afficher vitesse air";
  txt[PS_DISPLAYMAXSPEED] = "Afficher max vitesse";
  txt[PS_DISPLAYASCENTRATE] = "Afficher taux de montée";
  txt[PS_DISPLAYALTITUDE] = "Afficher altitude";
  txt[PS_DISPLAYDISTANCE] = "Afficher distance";
  txt[PS_COLOURTEXT] = "Texte couleur/taux de montée";
  txt[PS_TEXTATTOP] = "Texte en haut de l'écran";
  txt[PS_FREEFLYONSTARTUP] = "Vol libre au démarrage";
  txt[PS_ENABLESOCKETCONTROLLER] = "Activer contrôleur réseau";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Opacité fond du texte";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Couleur fond du texte";
  txt[PS_MAXNUMBERAI] = "Nombre max d'IA";
  txt[PS_TOOMANYAI] = "Note: Trop d'IA causera des ralentissements!";
  txt[PS_UNITS] = "Unités";
  txt[PS_RACESETTINGS] = "Réglages course";
  txt[PS_RACEVIBRATIONAMOUNT] = "Niveau de vibration";
  txt[PS_RACEBEEPVOLUME] = "Volume des Beeps";
  txt[PS_LIMBODIFFICULTY] = "Difficulté du Limbo";
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
  txt[PS_SHOWBUTTON1] = "Afficher bouton 1";
  txt[PS_SHOWBUTTON2] = "Afficher bouton 2";
  txt[PS_LAUNCH] = "Lancement";
  txt[PS_HOOKS] = "Crochets de remorquage";
  txt[PS_BUNGEELAUNCH] = "Lancement au sandow";
  txt[PS_AEROTOWLAUNCH] = "Lancement au remorqueur";
  txt[PS_FLATLAUNCHMETHOD] = "Méthode de lancement (terrain plat)";
  txt[PS_HAND] = "Main";
  txt[PS_BUNGEE] = "Sandow";
  txt[PS_AEROTOW] = "Remorqueur";
  txt[PS_LAUNCHSPEED] = "Vitesse lancement";
  txt[PS_LAUNCHANGLE] = "Angle lancement";
  txt[PS_LAUNCHUP] = "Lancement haut";
  txt[PS_LAUNCHFORWARD] = "Lancement avant";
  txt[PS_LAUNCHLEFT] = "Lancement gauche";
  txt[PS_LAUNCHOFFSETUP] = "Décalage lancement haut";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relance auto. si immobile";
  txt[PS_CRASHDETECTION] = "Détection de crash";
  txt[PS_CRASHDELTAVELX] = "Delta vélocité avant/arrière";
  txt[PS_CRASHDELTAVELY] = "Delta vélocité latérale";
  txt[PS_CRASHDELTAVELZ] = "Delta vélocité haut/bas";
  txt[PS_CRASHDELTAANGVELX] = "Delta vélocité roulis";
  txt[PS_CRASHDELTAANGVELY] = "Delta vélocité tangage";
  txt[PS_CRASHDELTAANGVELZ] = "Delta vélocité lacet";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Échelle résilience suspension";
  txt[PS_AIRFRAME] = "Cellule";
  txt[PS_UNDERCARRIAGE] = "Train d'atterrissage";
  txt[PS_PROPELLER] = "Hélice";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Décalage crochet ventre avant";
  txt[PS_BELLYHOOKOFFSETUP] = "Décalage crochet ventre haut";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Décalage crochet nez avant";
  txt[PS_NOSEHOOKOFFSETUP] = "Décalage crochet nez haut";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Décalage crochet queue avant";
  txt[PS_TAILHOOKOFFSETUP] = "Décalage crochet queue haut";
  txt[PS_MAXBUNGEELENGTH] = "Longueur caoutchouc";
  txt[PS_MAXBUNGEEACCEL] = "Puissance caoutchouc";
  txt[PS_TUGPLANE] = "Avion remorqueur";
  txt[PS_TUGSIZESCALE] = "Échelle taille remorqueur";
  txt[PS_TUGMASSSCALE] = "Échelle masse remorqueur";
  txt[PS_TUGENGINESCALE] = "Échelle moteur remorqueur";
  txt[PS_TUGMAXCLIMBSLOPE] = "Pente max montée remorqueur";
  txt[PS_TUGTARGETSPEED] = "Vitesse cible remorqueur";
  txt[PS_AEROTOWROPELENGTH] = "Longueur câble remorquage";
  txt[PS_AEROTOWROPESTRENGTH] = "Résistance câble remorquage";
  txt[PS_AEROTOWROPEMASSSCALE] = "Échelle masse câble";
  txt[PS_AEROTOWROPEDRAGSCALE] = "Échelle traînée câble";
  txt[PS_AEROTOWHEIGHT] = "Altitude max remorquage";
  txt[PS_AEROTOWCIRCUITSIZE] = "Taille du circuit remorquage";
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
  txt[PS_TETHERCOLOURR] = "Couleur câble: rouge";
  txt[PS_TETHERCOLOURG] = "Couleur câble: vert";
  txt[PS_TETHERCOLOURB] = "Couleur câble: bleu";
  txt[PS_TETHERCOLOURA] = "Couleur câble: opacité";
  txt[PS_CHASECAMERA] = "Caméra suiveuse";
  txt[PS_CAMERATARGETPOSFWD] = "Décalage cible avant";
  txt[PS_CAMERATARGETPOSUP] = "Décalage cible haut";
  txt[PS_DISTANCE] = "Distance";
  txt[PS_HEIGHT] = "Hauteur";
  txt[PS_VERTICALVELFRAC] = "Angle latéral";
  txt[PS_FLEXIBILITY] = "Souplesse";
  txt[PS_COCKPITCAMERA] = "Caméra cockpit";
  txt[PS_PITCH] = "Tangage";
  txt[PS_ENABLEDEBUGDRAW] = "Activer dessin debug";
  txt[PS_CANTOW] = "Peut remorquer";
  txt[PS_MAXAI] = "Nombre max de contrôleurs IA: %d (voir onglet Options 2)";
  txt[PS_ADDNEWAICONTROLLER] = "Ajouter contrôleur IA";
  txt[PS_REMOVEAICONTROLLERS] = "Supprimer tous les contrôleurs IA";
  txt[PS_LAUNCHSEPARATIONDISTANCE] = "Distance de séparation au lancement";
  txt[PS_INCLUDEINCAMERAVIEWS] = "Activer caméras";
  txt[PS_CREATEMAXNUMCONTROLLERS] = "Créer max contrôleurs IA";
  txt[PS_LAUNCHDIRECTION] = "Direction de lancement";
  txt[PS_RANDOMCOLOUROFFSET] = "Décalage couleur aléatoire";
  txt[PS_AICONTROLLER] = "Contrôleur IA";
  txt[PS_AINAVIGATION] = "Navigation IA";
  txt[PS_PLANETYPE] = "Type d'avion";
  txt[PS_USER] = "Utilisateur";
  txt[PS_ALL] = "Tous";
  txt[PS_GLIDER] = "Planeur";
  txt[PS_GLIDERS] = "Planeurs";
  txt[PS_POWERED] = "Motorisé";
  txt[PS_HELI] = "Hélicoptère";
  txt[PS_CONTROLLINE] = "Vol circulaire";
  txt[PS_PANORAMIC] = "Panoramique";
  txt[PS_3D] = "3D";
  txt[PS_ALLOWAICONTROL] = "Autoriser contrôle IA";
  txt[PS_WAYPOINTTOLERANCE] = "Tolérance waypoint";
  txt[PS_MINSPEED] = "Vitesse min";
  txt[PS_CRUISESPEED] = "Vitesse de croisière";
  txt[PS_MAXBANKANGLE] = "Angle d'inclinaison max";
  txt[PS_BANKANGLEPERHEADINGCHANGE] = "Inclinaison par changement de cap";
  txt[PS_SPEEDPERALTITUDECHANGE] = "Vitesse par changement d'altitude";
  txt[PS_GLIDESLOPEPEREXCESSSPEED] = "Pente de plané par excès de vitesse";
  txt[PS_PITCHPERROLLANGLE] = "Tangage par angle de roulis";
  txt[PS_HEADINGCHANGEFORNOSLOPE] = "Changement de cap sans pente";
  txt[PS_MAXPITCHCONTROL] = "Contrôle tangage max";
  txt[PS_MAXROLLCONTROL] = "Contrôle roulis max";
  txt[PS_CONTROLPERROLLANGLE] = "Contrôle par angle de roulis";
  txt[PS_PITCHCONTROLPERGLIDESLOPE] = "Contrôle tangage par pente";
  txt[PS_ROLLTIMESCALE] = "Échelle temps roulis";
  txt[PS_PITCHTIMESCALE] = "Échelle temps tangage";
  txt[PS_MINALTITUDE] = "Altitude min";
  txt[PS_SLOPEMINUPWINDDISTANCE] = "Pente dist. min vent arrière";
  txt[PS_SLOPEMAXUPWINDDISTANCE] = "Pente dist. max vent arrière";
  txt[PS_SLOPEMINLEFTDISTANCE] = "Pente dist. min gauche";
  txt[PS_SLOPEMAXLEFTDISTANCE] = "Pente dist. max gauche";
  txt[PS_SLOPEMINUPDISTANCE] = "Pente dist. min haut";
  txt[PS_SLOPEMAXUPDISTANCE] = "Pente dist. max haut";
  txt[PS_SLOPEMAXWAYPOINTTIME] = "Pente temps max waypoint";
  txt[PS_FLATMAXDISTANCE] = "Plat temps max waypoint";
  txt[PS_FLATMAXWAYPOINTTIME] = "Plat temps max waypoint";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Position";
  txt[PS_MASS] = "Masse";
  txt[PS_INERTIA] = "Inertie";
  txt[PS_WINGAREA] = "Surface alaire";
  txt[PS_EXTENTS] = "Dimensions";
  txt[PS_WINDSETTINGS] = "Réglages de vent";
  txt[PS_WINDSPEED] = "Vitesse";
  txt[PS_WINDBEARING] = "Direction";
  txt[PS_CANNOTMODIFYSCENERY] = "Modification impossible pendant la course";
  txt[PS_ALLOWBUNGEE] = "Usage de la catapulte";
  txt[PS_WINDGUSTTIME] = "Fréquence rafales";
  txt[PS_WINDGUSTFRACTION] = "Volume rafales";
  txt[PS_WINDGUSTANGLE] = "Angle rafales";
  txt[PS_TURBULENCEAMOUNT] = "Niveau Turbulences";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Turbulence de surface";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Turbulence cisaillement";
  txt[PS_WINDLIFTSMOOTHING] = "Adoucir effet de pente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Hauteur décroissance vent vertical";
  txt[PS_SEPARATIONTENDENCY] = "Tendance cisaillements ";
  txt[PS_ROTORTENDENCY] = "Tendance rotor";
  txt[PS_DEADAIRTURBULENCE] = "Coef. Turbulences";
  txt[PS_BOUNDARYLAYERDEPTH] = "Épaisseur couche limite";
  txt[PS_THERMALSETTINGS] = "Réglage Thermiques (Option 'Ciels')";
  txt[PS_DENSITY] = "Densité";
  txt[PS_RANGE] = "Etendue";
  txt[PS_LIFESPAN] = "Durée";
  txt[PS_DEPTH] = "Epaisseur";
  txt[PS_CORERADIUS] = "Rayon";
  txt[PS_DOWNDRAFTEXTENT] = "Diminution";
  txt[PS_UPDRAFTSPEED] = "Création";
  txt[PS_ASCENTRATE] = "Taux de montée";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Expansion sur durée de vie";
  txt[PS_RUNWAY] = "Piste";
  txt[PS_RUNWAYTYPE] = "Type de piste";
  txt[PS_CIRCLE] = "Cercle";
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
  txt[PS_PANORAMA3DSETTINGS] = "Réglage Panorama 3D";
  txt[PS_HEIGHTFIELDSETTINGS] = "Réglages hauteur";
  txt[PS_MINHEIGHT] = "Hauteur Minimum";
  txt[PS_MAXHEIGHT] = "Hauteur Maximum";
  txt[PS_AISCENERY] = "Scène IA";
  txt[PS_SCENETYPE] = "Type de scène";
  txt[PS_FLAT] = "Plat";
  txt[PS_SLOPE] = "Pente";
  txt[PS_CURRENTVIEWPOSITION] = "Coordonnées 'positionpilote'";
  txt[PS_NOLIGHTINGSETTINGS] = "Pas de réglage/ciel pour scène 'Panorama'";
  txt[PS_SUNBEARING] = "Niveau d'ensoleillement";
  txt[PS_THERMALACTIVITY] = "Activité Thermique";
  txt[PS_TERRAINDARKNESS] = "Ombrage Terrain";
  txt[PS_CONTROLLERSEESETTINGS] = "Voir aussi réglages controle additionnel sous les options";
  txt[PS_CROSS] = "Croix";
  txt[PS_BOX] = "Boite";
  txt[PS_CROSSANDBOX] = "Croix & Boite";
  txt[PS_STYLE] = "Style";
  txt[PS_CONTROLLERMODE] = "Mode de controle";
  txt[PS_TREATTHROTTLEASBRAKES] = "Manche gaz comme freins";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Réinitialiser config au lancement";
  txt[PS_NUMCONFIGURATIONS] = "Numéro configuration";
  txt[PS_TILTHORIZONTAL] = "Panoramique horizontal";
  txt[PS_TILTVERTICAL] = "Panoramique vertical";
  txt[PS_ARROWSHORIZONTAL] = "Fleches horizontales";
  txt[PS_ARROWSVERTICAL] = "Fleches verticales";
  txt[PS_ROLLSTICK] = "Manche de roulis";
  txt[PS_PITCHSTICK] = "Manche de profondeur";
  txt[PS_YAWSTICK] = "Manche de lacet";
  txt[PS_SPEEDSTICK] = "Manche de vitesse";
  txt[PS_CONSTANT] = "Constante";
  txt[PS_BUTTON0] = "Bouton 1";
  txt[PS_BUTTON1] = "Bouton 2";
  txt[PS_BUTTON2] = "Bouton 3";
  txt[PS_BUTTON0TOGGLE] = "Bouton 1 bascule";
  txt[PS_BUTTON1TOGGLE] = "Bouton 2 bascule";
  txt[PS_BUTTON2TOGGLE] = "Bouton 3 bascule";
  txt[PS_MIXES] = "Mixages";
  txt[PS_ELEVATORTOFLAPS] = "Profondeur vers volets";
  txt[PS_AILERONTORUDDER] = "Ailerons vers dérive";
  txt[PS_FLAPSTOELEVATOR] = "Volets vers profondeur";
  txt[PS_BRAKESTOELEVATOR] = "Freins/gaz vers profondeur";
  txt[PS_RUDDERTOELEVATOR] = "Dérive vers profondeur";
  txt[PS_RUDDERTOAILERON] = "Dérive vers ailerons";
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
  txt[PS_SMOKE1] = "Fumée 1";
  txt[PS_SMOKE2] = "Fumée 2";
  txt[PS_HOOK] = "Crochet";
  txt[PS_VELFWD] = "Vélocité avant";
  txt[PS_VELLEFT] = "Vélocité gauche";
  txt[PS_VELUP] = "Vélocité haut";
  txt[PS_MAXPARTICLES] = "Nombre max particules";
  txt[PS_CHANNELFOROPACITY] = "Canal pour opacité";
  txt[PS_MINOPACITY] = "Opacité min";
  txt[PS_MAXOPACITY] = "Opacité max";
  txt[PS_CHANNELFORRATE] = "Canal pour taux";
  txt[PS_MINRATE] = "Taux min";
  txt[PS_MAXRATE] = "Taux max";
  txt[PS_INITIALSIZE] = "Taille initiale";
  txt[PS_FINALSIZE] = "Taille finale";
  txt[PS_DAMPINGTIME] = "Temps d'amortissement";
  txt[PS_JITTER] = "Instabilité";
  txt[PS_ENGINEWASH] = "Souffle moteur";
  txt[PS_HUECYCLEFREQ] = "Fréquence cycle teinte";
  txt[PS_SETTINGSFORCONTROLLER] = "Réglages configuration Controles %d: %s";
  txt[PS_TRIMSETTINGS] = "Réglages de Trim";
  txt[PS_NOSIMPLESETTINGS] = "Pas de réglage simple dispo.";
  txt[PS_TRIM] = "Trim";
  txt[PS_SCALE] = "Echelle";
  txt[PS_CLAMPING] = "Limitation";
  txt[PS_EXPONENTIAL] = "Exponentiel";
  txt[PS_SPRING] = "Rappel neutre";
  txt[PS_POSITIVE] = "Positif";
  txt[PS_NEGATIVE] = "Négatif";
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
  txt[PS_JOYSTICKBUTTONLABEL] = "Bouton %d: Entrée = %5.2f Sortie = %d";
  txt[PS_SCALEPOSITIVE] = "Échelle entrée +ve";
  txt[PS_SCALENEGATIVE] = "Échelle entrée -ve";
  txt[PS_OFFSET] = "Décalage entrée";
  txt[PS_MAPTO] = "Appliquer à";
  txt[PS_PRESSWHENCENTRED] = "Appuyer si centré";
  txt[PS_PRESSWHENLEFTORDOWN] = "Appuyer si gauche ou bas";
  txt[PS_PRESSWHENRIGHTORUP] = "Appuyer si droite ou haut";
  txt[PS_DEADZONE] = "Zone neutre";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Mise à 0 réglages joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Démarrer calibrage joystick (Windows)";
  txt[PS_LOADOPTIONS] = "Charger options";
  txt[PS_SAVEOPTIONS] = "Sauver options";
  txt[PS_DELETEOPTIONS] = "Supprimer options";
  txt[PS_LOADSCENERY] = "Charger scène";
  txt[PS_SAVESCENERY] = "Sauver scène";
  txt[PS_DELETESCENERY] = "Supprimer scène";
  txt[PS_LOADOBJECTS] = "Charger objets";
  txt[PS_SAVEOBJECTS] = "Sauver objets";
  txt[PS_DELETEOBJECTS] = "Supprimer objets";
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
  txt[PS_LOADAICONTROLLERS] = "Charger contrôleurs IA";
  txt[PS_SAVEAICONTROLLERS] = "Sauver contrôleurs IA";
  txt[PS_DELETEAICONTROLLERS] = "Supprimer contrôleurs IA";
  txt[PS_SELECTPANORAMA] = "Selection Panorama";
  txt[PS_SELECTTERRAINFILE] = "Selection fichier de terrain";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selection contrôle préféré";
  txt[PS_SELECTOBJECTSSETTINGS] = "Sélection paramètres objets";
  txt[PS_SUMMARY] = "Résumé";
  txt[PS_NOOBJECTS] = "Aucun objet n'est chargé (à part le terrain, etc). Vous pouvez charger un ensemble d'objets statiques et dynamiques, "
    "en vous assurant qu'ils conviennent à la scène actuelle. En activant l'édition d'objets, vous pouvez "
    "créer les vôtres - consultez l'aide et le site Web PicaSim pour plus d'informations.";
  txt[PS_OBJECTSTOTAL] = "Nombre total d'objets:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Nombre statiques visibles:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Nombre statiques invisibles:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Nombre dynamiques visibles:";
  txt[PS_ABOUT] = "A propos";
  txt[PS_WEBSITE] = "Notre Site Web";
  txt[PS_FLYING] = "Voler";
  txt[PS_SETTINGS] = "Réglages";
  txt[PS_RACES] = "Courses";
  txt[PS_TIPS] = "Conseils";
  txt[PS_KEYBOARD] = "Clavier";
  txt[PS_CREDITS] = "Contributions";
  txt[PS_LICENCE] = "Licence";
  txt[PS_VERSIONS] = "Versions";
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
  txt[PS_TIPSTEXT] = "Voici quelques conseils pour tirer le meilleur parti de PicaSim :\n"
"\n"
"• N'oubliez pas qu'il s'agit d'un simulateur d'avions radiocommandés - l'accent est mis sur un contrôle réaliste, avec le \"pilote\" "
"debout au sol. Apprendre à voler est une compétence qui demande de la pratique - consultez le site Web ci-dessous pour de l'aide.\n"
"\n"
"• Expérimentez avec les réglages, surtout si vous êtes pilote R/C et avez des préférences ! Si vous faites des erreurs, immédiatement "
"après avoir démarré PicaSim, allez dans les réglages et chargez un préréglage pour les sections concernées.\n"
"\n"
"• Si vous rencontrez des problèmes ou avez des suggestions, envoyez un email à picasimulator@gmail.com\n"
"\n"
"• Les panoramas peuvent être affichés en détail \"normal\" et \"maximum\" - cela vaut la peine d'essayer le niveau de détail supérieur, "
"mais si PicaSim plante lors du chargement des scènes panoramiques, essayez de réduire le détail dans Réglages»Options 1.\n"
"\n"
"• Les avions contrôlés par l'IA sont configurés à deux endroits. Réglages»Options 2 vous permet de définir le nombre maximum d'avions IA "
"que votre appareil peut gérer. Réglages»Contrôleurs IA vous permet de configurer quels avions seront réellement chargés.\n";
  txt[PS_KEYBOARDTEXT] = "Avec un clavier, voici les raccourcis disponibles :\n"
"\n"
"[r] - Relancer\n"
"[p] - Basculer pause\n"
"[c] - Parcourir les vues caméra\n"
"[z] - Basculer vue zoom\n";
  txt[PS_JOYSTICKSETUPTEXT] = "PicaSim prend en charge les joysticks/manettes USB externes sous Android 3.0 et supérieur, ainsi que sous Windows. Sur les appareils "
"Android, cela nécessite un connecteur USB OTG (On The Go), et votre appareil doit le supporter - veuillez vérifier cela d'abord. "
"Si l'onglet joystick apparaît dans les menus de réglages, il y a de bonnes chances que votre appareil supporte les contrôleurs USB externes, "
"mais ce n'est pas garanti !\n"
"\n"
"Le contrôleur externe doit être connecté avant de lancer PicaSim, et sous Windows il est recommandé de calibrer le joystick en utilisant "
"le bouton \"calibrer\" dans Réglages»Joystick.\n"
"\n"
"Si votre joystick est détecté et sélectionné dans Réglages»Joystick, vous devrez assigner les entrées physiques aux commandes reconnues par PicaSim. "
"Cela se fait en identifiant quel axe du joystick correspond à quelle entrée de commande, puis il y a des boutons à appuyer lorsque le manche est "
"au centre, à gauche ou en bas, et à droite ou en haut. Vous pouvez ensuite vérifier que les mouvements du manche correspondent aux sorties correctes. "
"Cette procédure est la même sous Windows et Android - visitez le site Web ci-dessous pour une vidéo décrivant cela plus en détail.";
  txt[PS_TIPS1] = "Svp, envoyez moi des retours d'expérience que je puisse améliorer PicaSim";
  txt[PS_TIPS2] = "Si vous aimez PicaSim, Svp laissez un commentaire";
  txt[PS_TIPS3] = "Vous pouvez utiliser le gyroscope du mobile pour piloter - Réglage>Contrôle";
  txt[PS_TIPS4] = "Aimez/suivez PicaSim sur Facebook ou Google+ pour être informé des mises à jour et améliorations prévues";
  txt[PS_TIPS5] = "Si vous aimez PicaSim, laissez un avis/note sur les versions mobiles";
  txt[PS_TIPS6] = "Basculez entre mode 1 et 2 etc dans les options";
  txt[PS_TIPS7] = "Envoyez-moi vos commentaires pour que je puisse améliorer PicaSim";
  txt[PS_TIPS8] = "Si le taux d'images est bas, essayez de charger un réglage de qualité inférieure dans les options, et/ou réduisez le réglage de qualité physique";
  txt[PS_TIPS9] = "Sur les appareils mobiles, essayez d'utiliser l'accéléromètre en chargeant un préréglage approprié dans Réglages»Contrôle";
  txt[PS_TIPS10] = "Ajouter du ballast aux planeurs augmente leur capacité à pénétrer dans le vent, au prix d'une descente plus rapide";
  txt[PS_TIPS11] = "Activez le mode 'positionpilote' dans les scènes 3D, et utilisez le bouton marche en pause pour déplacer le point de lancement et la direction du vent";
  txt[PS_TIPS12] = "Les buses en cercle indiquent des zones d'ascendance thermique, mais il est préférable de surveiller le bout d'aile ou le nez du planeur qui se lève";
  txt[PS_TIPS13] = "Piloter de vrais avions R/C est encore plus amusant !";
  txt[PS_TIPS14] = "Suivez PicaSim sur Facebook pour être informé des mises à jour et des projets";
  txt[PS_TIPS15] = "Il existe un forum pour PicaSim sur Tapatalk";
  txt[PS_TIPS16] = "Ajoutez des avions contrôlés par l'IA dans Réglages»Contrôleurs IA. Limitez le nombre maximum dans Réglages»Options 2";
  txt[PS_LOADING] = "Chargement...";
  txt[PS_RACETIP] = "%s\n\nLa flèche blanche indique le prochain point de passage.";
  txt[PS_LIMBOTIP] = "%s\n\nAjuster difficulté/score multiples dans Réglages»Mode";
  txt[PS_DURATIONTIP] = "%s\n\nRecherchez les thermiques et atterrissez près du point de lancement.";
  txt[PS_TRAINERGLIDERTIP] = "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles pour "
    "commander profondeur et dérive, Faire voler le planeur en avant de la pente. Consulter le site Web "
    "pour des astuces et conseils de vol si vous n'arrivez pas à rester en l'air!";
  txt[PS_TRAINERPOWEREDTIP] = "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles droit "
    "pour commander profondeur et ailerons, controles gauches pour commander moteur et dérive. Consulter "
    "le site Web pour des astuces et conseils de vol si vous n'arrivez pas à rester en l'air!";
  txt[PS_SELECTRACE] = "Selection course";
  txt[PS_PREV] = "Avant";
  txt[PS_NEXT] = "Suivant";
  txt[PS_QUITANDHELP] = "Quitter et aider";
  txt[PS_CAMERAANDSETTINGS] = "Caméra et réglages";
  txt[PS_RESETANDPLAY] = "Relance et jouer/pause";
  txt[PS_THROTTLEANDRUDDER] = "Gazs/Volets/Freins et dérive";
  txt[PS_WINDDIRECTION] = "Direction du vent";
  txt[PS_AILERONSANDELEVATOR] = "Ailerons et elevator";
  txt[PS_FLYINGINFO] = "Voler";
  txt[PS_QUITANDHELPTEXT] = "En pause, appuyez sur X pour revenir au menu principal, et sur ? pour obtenir cette aide. \n\nEn vol, certains avions ont des boutons pour basculer entre les configurations de contrôle (ex.: volets haut/bas) et pour larguer le câble de remorquage/treuil.";
  txt[PS_CAMERAANDSETTINGSTEXT] = "En pause, l'engrenage permet d'accéder aux paramètres. \n\nL'œil (disponible uniquement en environnement 3D) permet de changer la caméra/point de vue.";
  txt[PS_RESETANDPLAYTEXT] = "Appuyez sur les boutons lecture/pause pour démarrer et arrêter la simulation. \n\nUtilisez la flèche de retour à côté pour relancer. \n\nLes boutons optionnels 1 et 2 sont pour la fumée.";
  txt[PS_THROTTLEANDRUDDERTEXT] = "Selon le type d'avion, déplacez le contrôleur gauche/droite pour contrôler la dérive, et haut/bas pour contrôler les gaz ou les freins. \n\nLes planeurs à deux voies n'auront pas ce contrôle.";
  txt[PS_WINDDIRECTIONTEXT] = "S'il y a du vent, la flèche bleue indique sa direction. \n\nDes chiffres peuvent aussi afficher des informations de vol de base comme la vitesse et l'altitude.";
  txt[PS_AILERONSANDELEVATORTEXT] = "Par défaut, déplacez le contrôleur gauche/droite pour contrôler les ailerons (direction), et haut/bas pour contrôler la profondeur (descendre/monter). \n\nPilotes R/C: Le mode émetteur peut être changé dans le panneau Paramètres»Options.";
  txt[PS_FLYINGINFOTEXT] = "Apprendre à voler demande du temps et de la pratique - tout comme piloter un vrai avion R/C. \n\nPour plus de conseils, utilisez le bouton d'aide dans le menu principal.";
  txt[PS_VRHEADSET] = "Casque VR";
  txt[PS_ENABLEVRMODE] = "Activer mode VR";
  txt[PS_HEADSET] = "Casque";
  txt[PS_NOTHING] = "Rien";
  txt[PS_LEFTEYE] = "Oeil gauche";
  txt[PS_RIGHTEYE] = "Oeil droit";
  txt[PS_BOTHEYES] = "Les deux yeux";
  txt[PS_BOTHEYESCROSSED] = "Vision croisee";
  txt[PS_NORMALVIEW] = "Vue normale";
  txt[PS_VRDESKTOP] = "Bureau VR";
  txt[PS_VRANTIALIASING] = "Anticrénelage VR";
  txt[PS_NONEUSEDEFAULT] = "Aucun (par défaut)";
  txt[PS_VRAUDIO] = "Audio VR";
  txt[PS_STATUS] = "Statut";
  txt[PS_VRNOTAVAILABLE] = "VR non disponible - aucun casque détecté";
  txt[PS_VRPANORAMADEPTH] = "Profondeur panorama VR";
  txt[PS_VRPANORAMAEXTENSION] = "Extension Panorama VR";
  txt[PS_MIRROR_TEXT] = "Casque VR en cours d'utilisation";
  txt[PS_MIRROR_SETTINGS] = "Afficher le miroir VR dans les Paramètres";
  txt[PS_ORIENTATION] = "Orientation";
  txt[PS_SMOKESOURCE] = "Source fumée %d";
  txt[PS_NAME] = "Nom";
  txt[PS_BUTTONMAPPINGS] = "Assignation des boutons";
  txt[PS_USERFILES] = "Fichiers utilisateur:";
  txt[PS_NOFILESFOUND] = "Aucun fichier trouvé";
  txt[PS_FAILEDTOLOADENVIRONMENT] = "Échec du chargement de l'environnement - veuillez charger un des paramètres de décor prédéfinis et réessayer";
}

