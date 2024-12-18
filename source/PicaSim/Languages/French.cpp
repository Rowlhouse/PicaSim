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
  txt[PS_CRASHED] = "Scratché";
  txt[PS_FREEFLY] = "Vol Libre";
  txt[PS_RACE] = "Course";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Version Full"; 

  txt[PS_SELECTSCENARIO] = "Scénario";
  txt[PS_SELECTAEROPLANE] = "Planeur";
  txt[PS_SELECTSCENERY] = "Scène";
  txt[PS_SELECTOPTIONS] = "Veuillez séléctionner les options par défaut";
  txt[PS_USEDEFAULT] = "Usage par defaut";
  txt[PS_USEDEFAULTPREVIOUS] = "Usage par defaut précédent";

  txt[PS_BACK] = "Retour";
  txt[PS_OPTIONS1] = "Options 1";
  txt[PS_OPTIONS2] = "Options 2";
  txt[PS_AEROPLANE] = "Planeurs";
  txt[PS_SCENERY] = "Scènes";
  txt[PS_LIGHTING] = "Ciels";
  txt[PS_CONTROLLER] = "Controles";
  txt[PS_JOYSTICK] = "Joysticks";
  txt[PS_LOAD] = "Charger...";
  txt[PS_SAVE] = "Sauver...";
  txt[PS_DELETE] = "Supprimer...";
  txt[PS_ADVANCED] = "+ d'options";
  txt[PS_SIMPLE] = "- d'options";
  txt[PS_FILENAME] = "NomFichier:";
  txt[PS_INFULLVERSION] = "Dans version Full";

  //txt[PS_] = "";


  txt[PS_ABOUT] = "A propos";
  txt[PS_WEBSITE] = "Notre Site Web";
  txt[PS_FLYING] = "Voler";
  txt[PS_SETTINGS] = "Réglages";
  txt[PS_RACES] = "Courses";
  txt[PS_CREDITS] = "Contributions";
  txt[PS_LICENCE] = "Licence";
  txt[PS_VERSIONS] = "Versions";


  txt[PS_ABOUTFREETEXT] = 
"PicaSim est un simulateur de pilotage de modèles radiocommandés. Pour le moment, il est conçu pour vous aider à apprendre "
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
"sera grandement apprécié !\n"
;

  txt[PS_ABOUTPAIDTEXT] = 
"PicaSim est un simulateur de pilotage de modèles radiocommandés. Pour le moment, il est conçu pour vous aider à apprendre "
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
"sera grandement apprécié !\n"
;

  txt[PS_HOWTOFLYTEXT] = 
"La premiere chose à faire est de visiter notre site (lien ci-dessous) et de regarder la courte vidéo \"How to Fly\". \n"
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
"Des vidéos visualisent cela en montrant 'comment voler ?' sur notre site Web - cela sera un bon entrainement au pilotage des vrais modèles"
;

  txt[PS_HOWTOCONFIGURETEXT] = 
"Vous pouvez accéder aux réglages de Picasim depuis le menu initial, ou en utilisant le bouton 'engrenage' lors du mode pause.\n"
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
"Avec les scenes panoramiques, il n'est pas possible d'agir sur la position du pilote. Le mode 'positionpilote' est désactivé"
;

  txt[PS_HOWTORACETEXT] = 
"Le mode Course consiste à circuler le plus vite possible entre 2 balises. La balise où vous devez vous diriger est visualisée"
"en couleur, et aussi pointée par une fleche blanche dans le bas de l'ecran.\n"
"\n"
"Il y a différents types de balises: Dans les courses du style F3F, vous devez aller et venir plusieurs fois d'une balise à l'autre, "
"vous devez dépasser les balises et tourner face au vent de celles-ci. Dans une course de type Cross-Country, vous devez juste passer au dessus des balises "
" (à la hauteur que vous souhaitez). Dans les course de type 'plaine' vous devez voler entre deux 'portes' ou 'plans' - La balise au vent est verte, celle sous le vent est rouge. \n"
"\n"
"Quand la course est terminée les points et classement sont automatiquement ajoutés au palmares global, malgré vous "
"Ce peut être désactivé dans les options. Vous pouvez selectionner le nom qui s'affiche dans la section profil de l'écran de menu 'classement'. "
"Si vous mettez une adresse email cela vous permettra de faire le lien entre le mobile ou PC utilisé."
;

  txt[PS_CREDITSTEXT] = 
"PicaSim est développé par Danny Chapman\n"
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
"Je vous invite à le faire !"
;

  txt[PS_TIPS1] = "Svp, envoyez moi des retours d'expérience que je puisse améliorer PicaSim";
  txt[PS_TIPS2] = "Si vous aimez PicaSim, Svp laissez un commentaire";
  txt[PS_TIPS3] = "Vous pouvez utiliser le gyroscope du mobile pour piloter - Réglage>Contrôle";
  txt[PS_TIPS4] = "Aimez/suivez PicaSim sur Facebook ou Google+ pour être informé des mises à jour et améliorations prévues";
  txt[PS_ALLOWCHOICE] = "Choix d'avion/scène la prochaine fois";
  txt[PS_LOADING] = "Chargement...";

  txt[PS_LIMBOTIP] = "%s\n\nAjuster difficulté/score multiples dans Réglages»Mode";
  txt[PS_TRAINERGLIDERTIP] = 
    "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles pour "
    "commander profondeur et dérive, Faire voler le planeur en avant de la pente. Consulter le site Web "
    "pour des astuces et conseils de vol si vous n'arrivez pas à rester en l'air!";
  txt[PS_TRAINERPOWEREDTIP] = 
    "Bouton play pour décoller, bouton Pause/Retour pour repartir du début. Utiliser les controles droit "
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
  txt[PS_RETRIEVINGLEADERBOARD] = "Classement en cours...";
  txt[PS_SETUSERNAME] = "Pilote ?";
  txt[PS_SETEMAIL] = "Email ?";
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

// Everything below here is for settings
  txt[PS_SELECTLANGUAGE] = "Selection langue";
  txt[PS_LANGUAGESETTINGS] = "Réglage langue";
  txt[PS_CURRENTLANGUAGE] = "Langue en cours";
  txt[PS_INFREEFLYMODE] = "Mode Vol Libre";
  txt[PS_CAMERASETTINGS] = "Réglage camera";
  txt[PS_ZOOMVIEW] = "Vue Zoom";
  txt[PS_ZOOMVIEWSIZE] = "Taille Vue Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Autozoom depuis le sol";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Echelle autozoom sol";
  txt[PS_GROUNDFIELDOFVIEW] = "Vue vers le sol";
  txt[PS_GROUNDHORIZONAMOUNT] = "Niveau sol/horizon";
  txt[PS_GROUNDVIEWFOLLOW] = "Centrer modèle en vol à l'écran";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Angle/Vue depuis modèle";
  txt[PS_WALKABOUTSETTINGS] = "Réglages positionpilote";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activer bouton positionpilote";
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
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Usage tactile absolu";
  txt[PS_STAGGERCONTROLLER] = "Décaler controle (tactile partiel)";
  txt[PS_SQUARECONTROLLERS] = "Controles en carré";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Décalage horiz. du bord";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Décalage vert. du bord";
  txt[PS_SHAPEOPACITY] = "Opacité grille controle";
  txt[PS_STICKOPACITY] = "Opacité manche controle";
  txt[PS_STICKCROSS] = "Stick cross";
  txt[PS_JOYSTICKID] = "IDentifiant Joystick";
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
  txt[PS_GRAPHAIRSPEED] = "Visu graph vitesse/air (rouge)";
  txt[PS_GRAPHGROUNDSPEED] = "Visu graph vitesse/sol (vert)";
  txt[PS_GRAPHCLIMBRATE] = "Visu graph taux/montée (bleu)";
  txt[PS_GRAPHWINDSPEED] = "Visu graph vitesse/vent (jaune)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Visu graph vitesse verticale/vent (Violet)";
  txt[PS_STALLMARKERS] = "Marquer décrochages";
  txt[PS_AEROFOILINFO] = "Infos profil d'aile";
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
  txt[PS_AEROPLANERENDER] = "Rendu du modèle";
  txt[PS_DRAWSHADOWS] = "Montrer ombres";
  txt[PS_USE16BIT] = "Textures 16 bit (- de mémoire)(redémarrer)";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Niveau éclairage ambiant";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Niveau éclairage diffus";
  txt[PS_TERRAINTEXTUREDETAIL] = "Détail texture terrain";
  txt[PS_NEARCLIPDISTANCE] = "Distances proches";
  txt[PS_FARCLIPDISTANCE] = "Distances lointaines";
  txt[PS_MISCSETTINGS] = "Réglages divers";
  txt[PS_USEBACKBUTTON] = "Bouton retour pour sortir";
  txt[PS_DRAWLAUNCHMARKER] = "Visu. balise de décollage";
  txt[PS_DRAWGROUNDPOSITION] = "Visu. position sol";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Controle préféré du modèle";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Réglages test/developpement";
  txt[PS_WIREFRAMETERRAIN] = "Maillage terrain";
  txt[PS_DRAWSUNPOSITION] = "Visu. position soleil";
  txt[PS_FREEFLYSETTINGS] = "Réglages Vol Libre";
  txt[PS_DISPLAYFLIGHTTIME] = "Afficher temps de vol";
  txt[PS_DISPLAYSPEED] = "Afficher vitesse";
  txt[PS_DISPLAYMAXSPEED] = "Afficher max vitesse";
  txt[PS_DISPLAYASCENTRATE] = "Afficher taux de montée";
  txt[PS_DISPLAYALTITUDE] = "Afficher altitude";
  txt[PS_DISPLAYDISTANCE] = "Afficher distance";
  txt[PS_COLOURTEXT] = "Texte couleur/taux de montée";
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
  txt[PS_LAUNCH] = "Lancement";
  txt[PS_LAUNCHSPEED] = "Vitesse lancement";
  txt[PS_LAUNCHANGLE] = "Angle lancement";
  txt[PS_LAUNCHUP] = "Lancement haut";
  txt[PS_LAUNCHFORWARD] = "Lancement avant";
  txt[PS_LAUNCHLEFT] = "Lancement gauche";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relance auto. si immobile";
  txt[PS_AIRFRAME] = "Airframe";
  txt[PS_UNDERCARRIAGE] = "Undercarriage";
  txt[PS_PROPELLER] = "Propeller";
  txt[PS_MAXBUNGEELENGTH] = "Longueur caoutchouc";
  txt[PS_MAXBUNGEEACCEL] = "Puissance caoutchouc";
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
  txt[PS_CHASECAMERA] = "Caméra suiveuse";
  txt[PS_DISTANCE] = "Distance";
  txt[PS_HEIGHT] = "Hauteur";
  txt[PS_VERTICALVELFRAC] = "Angle latéral";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Position";
  txt[PS_MASS] = "Masse";

  txt[PS_WINDSETTINGS] = "Réglages de vent";
  txt[PS_WINDSPEED] = "Vitesse";
  txt[PS_WINDBEARING] = "Direction";
  txt[PS_CANNOTMODIFYSCENERY] = "Modification impossible pendant la course";
  txt[PS_ALLOWBUNGEE] = "Usage de la catapulte";
  txt[PS_WINDGUSTTIME] = "Fréquence rafales";
  txt[PS_WINDGUSTFRACTION] = "Volume rafales";
  txt[PS_WINDGUSTANGLE] = "Angle rafales";
  txt[PS_TURBULENCEAMOUNT] = "Niveau Turbulences";
  txt[PS_WINDLIFTSMOOTHING] = "Adoucir effet de pente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Tendance cisaillements ";
  txt[PS_DEADAIRTURBULENCE] = "Coef. Turbulences";
  txt[PS_THERMALSETTINGS] = "Réglage Thermiques (Option 'Ciels')";
  txt[PS_DENSITY] = "Densité";
  txt[PS_RANGE] = "Etendue";
  txt[PS_LIFESPAN] = "Durée";
  txt[PS_DEPTH] = "Epaisseur";
  txt[PS_CORERADIUS] = "Rayon";
  txt[PS_DOWNDRAFTEXTENT] = "Diminution";
  txt[PS_UPDRAFTSPEED] = "Création";
  txt[PS_ASCENTRATE] = "Taux de montée";
  txt[PS_RUNWAY] = "Piste";
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
  txt[PS_PANORAMASETTINGS] = "Réglages Panorama";
  txt[PS_PANORAMA3DSETTINGS] = "Réglage Panorama 3D";
  txt[PS_HEIGHTFIELDSETTINGS] = "Réglages hauteur";
  txt[PS_MINHEIGHT] = "Hauteur Minimum";
  txt[PS_MAXHEIGHT] = "Hauteur Maximum";
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
  txt[PS_MAPTO] = "Appliquer à";
  txt[PS_DEADZONE] = "Zone neutre";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Mise à 0 réglages joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Démarrer calibrage joystick (Windows)";

  txt[PS_LOADOPTIONS] = "Charger options";
  txt[PS_SAVEOPTIONS] = "Sauver options";
  txt[PS_DELETEOPTIONS] = "Supprimer options";
  txt[PS_LOADSCENERY] = "Charger scène";
  txt[PS_SAVESCENERY] = "Sauver scène";
  txt[PS_DELETESCENERY] = "Supprimer scène";
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
  txt[PS_SELECTPANORAMA] = "Selection Panorama";
  txt[PS_SELECTTERRAINFILE] = "Selection fichier de terrain";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selection contrôle préféré";

  // These, starting with PM_, must have no accented characters
  txt[PS_ONERACE] = 
    "La version libre pour mobile de ce simulateur ne contient qu'une seule course. Pour obtenir les autres, achetez la version complete, svp "
    "elle contient plus de courses, aeromodeles, scenes et vous beneficierez d'un support pour les anciens et nouveaux developpements de PicaSim.";
  txt[PS_INVALIDRACE] = "Course indisponible dans la version libre de PicaSim";
  txt[PS_LEADERBOARDFAILED] = "Calcul Palmares errone - Verifier la connection internet";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Calcul Palmares errone, reessai en cours...";
  txt[PS_SCORESUBMISSIONFAILED] = "Soumission du score erronee - Verifier la connection internet";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Soumission du score erronee, reessai en cours...";
  txt[PS_USERFAILED] = "Requete utilisateur erronee - Verifier la connection internet";
  txt[PS_USERFAILEDRETRYING] = "Requete utilisateur erronee, reessai en cours...";

  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";

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
  txt[PS_TRAINERGLIDERTIP] = 
    "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower corner to control the elevator and rudder, flying the glider in the lift in front of the hill.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
  txt[PS_TRAINERPOWEREDTIP] = 
    "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower right corner to control elevator and ailerons. The controls on the left are for throttle and rudder.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
}


