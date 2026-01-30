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

void InitStringsPT(const char** txt)
{
  txt[PS_LANGUAGE_EN] = "Portuguese";
  txt[PS_LANGUAGE_NATIVE] = "Português";
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Enable";
  txt[PS_REMOVE] = "Remove";
  txt[PS_DUPLICATE] = "Duplicate";
  txt[PS_CRASHED] = "Danificado";
  txt[PS_NO] = "No";
  txt[PS_YES] = "Yes";
  txt[PS_INVERT] = "Invert";
  txt[PS_AUTO] = "Auto";
  txt[PS_FREEFLY] = "Voo Livre";
  txt[PS_RACE] = "Prova";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Versão Completa";
  txt[PS_INTRIALMODE] = "In trial mode - purchase full version to access all planes and sceneries.";
  txt[PS_GAMEPADDETECTED] = "Gamepad detected. Enable in the settings to use it.";
  txt[PS_WHATSNEW] = "Welcome to PicaSim!";
  txt[PS_WHATSNEWTEXT] = "Here's what's new in this version (or recent versions):\n\n"
"• Add the Quark glider.\n"
"• Update various links etc in plane descriptions.\n"
"• Various tweaks/bug fixes.\n"
"\nPlease contact picasimulator@gmail.com with bug reports and questions.\n";
  txt[PS_ONERACE] = "A versao livre deste simulador so contem uma prova. Considere comprar a versao completa."
    "A mesma tem mais provas, modelos, cenarios e ira suportar o futuro desenvolvimento do PicaSim.";
  txt[PS_FULLVERSION] = "The full version contains all the planes/sceneries etc - the small purchase price will help future development of PicaSim.";
  txt[PS_SELECTAEROPLANE] = "Modelo";
  txt[PS_SELECTSCENERY] = "Cenário";
  txt[PS_SELECTOPTIONS] = "Por favor seleccione um conjunto adequado de opções padrão";
  txt[PS_SELECTSCENARIO] = "Cenário";
  txt[PS_USEDEFAULT] = "Usar padrão";
  txt[PS_USEDEFAULTPREVIOUS] = "Usar padrão/anterior";
  txt[PS_INVALIDRACE] = "Prova invalida para a versao livre do PicaSim";
  txt[PS_BACK] = "Anterior";
  txt[PS_OPTIONS1] = "Opções";
  txt[PS_OPTIONS2] = "Modo";
  txt[PS_AEROPLANE] = "Modelo";
  txt[PS_SCENERY] = "Cenário";
  txt[PS_OBJECTS] = "Objects";
  txt[PS_OBJECTSSETTINGS] = "Objects settings";
  txt[PS_LIGHTING] = "Iluminação";
  txt[PS_AICONTROLLERS] = "AI Controllers";
  txt[PS_CONTROLLER] = "Controlador";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Carregar...";
  txt[PS_SAVE] = "Salvar...";
  txt[PS_DELETE] = "Apagar...";
  txt[PS_ADVANCED] = "Avançado";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Clear all saved settings and exit";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "This will clear all saved settings and exit PicaSim - are you sure?";
  txt[PS_SIMPLE] = "Simples";
  txt[PS_FILENAME] = "Nome do ficheiro:";
  txt[PS_INFULLVERSION] = "Na Versão Completa";
  txt[PS_PURCHASE] = "Purchase";
  txt[PS_PURCHASESUCCESSFUL] = "Purchase successful - thanks!";
  txt[PS_PURCHASECANCELLED] = "Purchase cancelled.";
  txt[PS_PURCHASEERROR] = "Purchase unsuccessful due to an unknown error.";
  txt[PS_PURCHASEALREADY] = "Purchase unsuccessful since the product was already purchased!";
  txt[PS_PURCHASEFROMSTORE] = "Purchase from the Windows Store?";
  txt[PS_NOTNOW] = "Not now";
  txt[PS_CANCEL] = "Cancel";
  txt[PS_TRIALOVER] = "Trial period expired. Please purchase the full version or use one of the free planes and sceneries.";
  txt[PS_SELECTLANGUAGE] = "Seleccione o idioma";
  txt[PS_LANGUAGESETTINGS] = "Definições de idioma";
  txt[PS_CURRENTLANGUAGE] = "Idioma atual";
  txt[PS_INFREEFLYMODE] = "Em modo livre";
  txt[PS_CAMERASETTINGS] = "Configurações da câmara";
  txt[PS_ZOOMVIEW] = "Zoom";
  txt[PS_PLANEONLYINZOOMVIEW] = "Draw plane alone in zoom view";
  txt[PS_SMOKEONLYINMAINVIEW] = "Only draw smoke in main view";
  txt[PS_ZOOMVIEWSIZE] = "Tamanho do Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Terreno Auto-Zoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Escala do autozoom terreno";
  txt[PS_GROUNDFIELDOFVIEW] = "Campo de visão";
  txt[PS_GROUNDHORIZONAMOUNT] = "Quantidade de terreno no horizonte";
  txt[PS_GROUNDLAG] = "Ground camera lag";
  txt[PS_GROUNDVIEWFOLLOW] = "Seguir modelo em visão de terreno";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Yaw offset in ground view";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Pitch offset in ground view";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Campo de visão do modelo";
  txt[PS_STEREOSCOPY] = "Stereoscopy";
  txt[PS_STEREOSEPARATION] = "Stereo separation";
  txt[PS_STEREOINFO] = "+ve value for normal 3D, -ve for cross-eyes technique";
  txt[PS_WALKABOUTSETTINGS] = "Definições caminhada";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activar botão caminhada";
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
  txt[PS_SETWINDDIRONWALKABOUT] = "Definir a direcção do vento da caminhada";
  txt[PS_SIMULATIONSETTINGS] = "Configurações do simulador";
  txt[PS_TIMESCALE] = "Escala do tempo";
  txt[PS_PHYSICSACCURACY] = "Precisão física";
  txt[PS_CONTROLLERSETTINGS] = "Configurações do controlador";
  txt[PS_MODE1DESCRIPTION] = "Modo 1: Leme de direcção e leme de profundidade esquerda, Ailerons e motor/flaps direita";
  txt[PS_MODE2DESCRIPTION] = "Modo 2: Leme de direcção e motor/flaps esquerda, Ailerons e leme de profundidade direita";
  txt[PS_MODE3DESCRIPTION] = "Modo 3: Ailerons e motor/flaps esquerda, Leme de direcção e leme de profundidade direita";
  txt[PS_MODE4DESCRIPTION] = "Modo 4: Ailerons e leme de profundidade esquerda, Leme de direcção e motor/flaps direita";
  txt[PS_CONTROLLERSIZE] = "Tamanho";
  txt[PS_BRAKESFORWARD] = "Air brakes with stick forward";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Use absolute controller touch position";
  txt[PS_STAGGERCONTROLLER] = "Controlador Stagger (para multitouch parcial)";
  txt[PS_ENABLEELEVATORTRIM] = "Enable elevator trim";
  txt[PS_ELEVATORTRIMSIZE] = "Elevator trim size";
  txt[PS_SQUARECONTROLLERS] = "Controladores quadrados";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Deslocamento horizontal do limite";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Deslocamento vertical do limite";
  txt[PS_SHAPEOPACITY] = "Ajustar a transparência";
  txt[PS_STICKOPACITY] = "Transparência do stick";
  txt[PS_STICKCROSS] = "Stick cross";
  txt[PS_JOYSTICKID] = "ID do joystick";
  txt[PS_JOYSTICKINFO] = "Note: You may need to restart PicaSim after connecting or disconnecting a joystick/controller.";
  txt[PS_NOJOYSTICK] = "No Joystick detected or available.";
  txt[PS_AUDIOSETTINGS] = "Configurações de áudio";
  txt[PS_OVERALLVOLUME] = "Volume global";
  txt[PS_VARIOMETERVOLUME] = "Volume do variómetro";
  txt[PS_WINDVOLUME] = "Volume do vento";
  txt[PS_OUTSIDEAEROPLANEVOLUME] = "Volume fora do modelo";
  txt[PS_INSIDEAEROPLANEVOLUME] = "Volume dentro do modelo";
  txt[PS_ONSCREENDISPLAYSETTINGS] = "Ecrã - configurações de exibição";
  txt[PS_WINDARROWSIZE] = "Tamanho seta vento";
  txt[PS_WINDARROWOPACITY] = "Transparência seta vento";
  txt[PS_PAUSEBUTTONSSIZE] = "Tamanho botão pausa";
  txt[PS_PAUSEBUTTONOPACITY] = "Transparência botão pausa";
  txt[PS_INFORMATIONSETTINGS] = "Opções de informação";
  txt[PS_MAXMARKERSPERTHERMAL] = "Mostrar térmicas";
  txt[PS_THERMALWINDFIELD] = "Draw thermal wind field";
  txt[PS_GRAPHAIRSPEED] = "Mostrar vel. vento (vermelho)";
  txt[PS_GRAPHDURATION] = "Graph duration";
  txt[PS_GRAPHALTITUDE] = "Show altitude (turquoise)";
  txt[PS_GRAPHGROUNDSPEED] = "Mostrar vel. solo (verde)";
  txt[PS_GRAPHCLIMBRATE] = "Mostrar nivel de subida (azul)";
  txt[PS_GRAPHWINDSPEED] = "Mostrar vel. vento (amar.)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Mostrar vel. vert. do vento (roxo)";
  txt[PS_STALLMARKERS] = "Marcas perda sustentação";
  txt[PS_AEROFOILPLOT] = "Plot aerofoil data";
  txt[PS_AEROFOILINFO] = "Informações do Perfil de Asa";
  txt[PS_AEROFOILRANGE] = "Angle range";
  txt[PS_AEROFOILCLMAX] = "Max CL";
  txt[PS_AEROFOILCDMAX] = "Max CD";
  txt[PS_AEROFOILCMMAX] = "Max CM";
  txt[PS_AEROFOILLDMAX] = "Max L/D";
  txt[PS_AEROFOILPLOTREFERENCE] = "Plot reference aerofoil";
  txt[PS_AEROFOILPOLAR] = "Plot polar diagram";
  txt[PS_DRAWAEROPLANECOM] = "Desenhar avião CoM";
  txt[PS_GRAPHFPS] = "Mostrar FPS";
  txt[PS_NUMWINDSTREAMERS] = "Num wind streamers";
  txt[PS_WINDSTREAMERTIME] = "Wind streamer time";
  txt[PS_WINDSTREAMERDELTAZ] = "Wind streamer deltaZ";
  txt[PS_GRAPHICSSETTINGS] = "Configurações gráficas";
  txt[PS_GROUNDTERRAINLOD] = "Ground terrain LOD";
  txt[PS_AEROPLANETERRAINLOD] = "Aeroplane terrain LOD";
  txt[PS_UPDATETERRAINLOD] = "Atual. terreno LOD vista de solo";
  txt[PS_COMPONENTS] = "Componentes";
  txt[PS_3DMODEL] = "3D Model";
  txt[PS_BOTH] = "Ambos";
  txt[PS_CONTROLLEDPLANESHADOWS] = "Controlled plane shadows";
  txt[PS_OTHERSHADOWS] = "Other shadows";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Projected shadow detail";
  txt[PS_BLOB] = "Blob";
  txt[PS_PROJECTED] = "Projected";
  txt[PS_AEROPLANERENDER] = "Renderização do Modelo";
  txt[PS_DRAWSHADOWS] = "Desenhar sombras";
  txt[PS_USE16BIT] = "Usar text. 16bit reduz memória (reini. Picasim)";
  txt[PS_SEPARATESPECULAR] = "Separate specular calculation";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Escala ilum. ambiente";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Escala iluminação difusa";
  txt[PS_TERRAINTEXTUREDETAIL] = "Detalhe text. terreno";
  txt[PS_MAXSKYBOXDETAIL] = "Max skybox/panorama detail";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Requires restart";
  txt[PS_REQUIRESVRRESTART] = "Requires VR restart";
  txt[PS_NEARCLIPDISTANCE] = "Plano perto";
  txt[PS_FARCLIPDISTANCE] = "Plano distante";
  txt[PS_SHADINGMETHOD] = "Shading method";
  txt[PS_USEMULTILIGHTS] = "Use multiple lights";
  txt[PS_ENABLESMOKE] = "Enable smoke";
  txt[PS_SMOKEQUALITY] = "Smoke quality";
  txt[PS_MISCSETTINGS] = "Configurações várias";
  txt[PS_USEBACKBUTTON] = "Utilize o botão anterior para sair";
  txt[PS_DRAWLAUNCHMARKER] = "Mostrar a marca de lançamento";
  txt[PS_DRAWGROUNDPOSITION] = "Mostrar a marca de terreno";
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
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Usar o controlador preferido do avião";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Definições teste/desenvol.";
  txt[PS_WIREFRAMETERRAIN] = "Terreno em wireframe";
  txt[PS_DRAWSUNPOSITION] = "Desenhar posição sol";
  txt[PS_FREEFLYSETTINGS] = "Configurações de voo livre";
  txt[PS_DISPLAYFLIGHTTIME] = "Mostrar tempo de voo";
  txt[PS_DISPLAYSPEED] = "Mostrar velocidade";
  txt[PS_DISPLAYAIRSPEED] = "Display airspeed";
  txt[PS_DISPLAYMAXSPEED] = "Mostrar max velocidade";
  txt[PS_DISPLAYASCENTRATE] = "Mostrar taxa de ascenção";
  txt[PS_DISPLAYALTITUDE] = "Mostar altitude";
  txt[PS_DISPLAYDISTANCE] = "Mostar distância";
  txt[PS_COLOURTEXT] = "Colorir texto taxa ascenção";
  txt[PS_TEXTATTOP] = "Text at top of screen";
  txt[PS_FREEFLYONSTARTUP] = "Free fly on startup";
  txt[PS_ENABLESOCKETCONTROLLER] = "Enable socket controller";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Text background opacity";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Text background colour";
  txt[PS_MAXNUMBERAI] = "Max number of AI";
  txt[PS_TOOMANYAI] = "Note: Too many AI will cause crashes & slowdown!";
  txt[PS_UNITS] = "Units";
  txt[PS_RACESETTINGS] = "Configuração da prova/limbo";
  txt[PS_RACEVIBRATIONAMOUNT] = "Intensidade de vibração";
  txt[PS_RACEBEEPVOLUME] = "Volume do aviso";
  txt[PS_LIMBODIFFICULTY] = "Dificuldade do limbo";
  txt[PS_ENABLESCORELOOP] = "Activar Scoreloop";
  txt[PS_DELETELOCALHIGHSCORES] = "Apagar pontuações mais altas";
  txt[PS_GENERALSETTINGS] = "Configurações gerais";
  txt[PS_COLOURSCHEME] = "Esquema de cores";
  txt[PS_COLOUROFFSET] = "Desvio da côr";
  txt[PS_BALLAST] = "Balastro";
  txt[PS_BALLASTFWD] = "Balastro à frente";
  txt[PS_BALLASTLEFT] = "Balastro à esquerda";
  txt[PS_BALLASTUP] = "Balastro em cima";
  txt[PS_DRAGMULTIPLIER] = "Arrasto multiplicador";
  txt[PS_SIZEMULTIPLIER] = "Multiplicador de tamanho";
  txt[PS_MASSMULTIPLIER] = "Multiplicador de peso";
  txt[PS_ENGINEMULTIPLIER] = "Multiplicador de motor";
  txt[PS_HASVARIOMETER] = "Tem variómetro";
  txt[PS_PREFERREDCONTROLLER] = "Controlador preferido";
  txt[PS_SHOWBUTTON1] = "Show button 1";
  txt[PS_SHOWBUTTON2] = "Show button 2";
  txt[PS_LAUNCH] = "Lançar";
  txt[PS_HOOKS] = "Tow hooks";
  txt[PS_BUNGEELAUNCH] = "Bungee launch";
  txt[PS_AEROTOWLAUNCH] = "Aerotow launch";
  txt[PS_FLATLAUNCHMETHOD] = "Launch method (flat sceneries)";
  txt[PS_HAND] = "Hand";
  txt[PS_BUNGEE] = "Bungee";
  txt[PS_AEROTOW] = "Aerotow";
  txt[PS_LAUNCHSPEED] = "Vel. lançamento";
  txt[PS_LAUNCHANGLE] = "Âng. de lançamento";
  txt[PS_LAUNCHUP] = "Lançar para cima";
  txt[PS_LAUNCHFORWARD] = "Lançar em frente";
  txt[PS_LAUNCHLEFT] = "Lançar em esquerda";
  txt[PS_LAUNCHOFFSETUP] = "Launch offset up";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relançar quando parado";
  txt[PS_CRASHDETECTION] = "Crash detection";
  txt[PS_CRASHDELTAVELX] = "Delta velocity fwd/back";
  txt[PS_CRASHDELTAVELY] = "Delta velocity sideways";
  txt[PS_CRASHDELTAVELZ] = "Delta velocity up/down";
  txt[PS_CRASHDELTAANGVELX] = "Delta roll velocity";
  txt[PS_CRASHDELTAANGVELY] = "Delta pitch velocity";
  txt[PS_CRASHDELTAANGVELZ] = "Delta yaw velocity";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Suspension resilience scale";
  txt[PS_AIRFRAME] = "Fuselagem";
  txt[PS_UNDERCARRIAGE] = "Chassis";
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
  txt[PS_MAXBUNGEELENGTH] = "Tam. máx. guincho";
  txt[PS_MAXBUNGEEACCEL] = "Acel. máx. guincho";
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
  txt[PS_TETHERING] = "No limite (linha de controle)";
  txt[PS_TETHERLINES] = "Número de linhas de limitação";
  txt[PS_TETHERREQUIRESTENSION] = "Require tension for control";
  txt[PS_TETHERPHYSICSOFFSETFORWARD] = "Ângulo físico para a frente";
  txt[PS_TETHERPHYSICSOFFSETLEFT] = "Ângulo físico para a esquerda";
  txt[PS_TETHERPHYSICSOFFSETUP] = "Ângulo físico para cima";
  txt[PS_TETHERVISUALOFFSETFORWARD] = "Ângulo visual para a frente";
  txt[PS_TETHERVISUALOFFSETLEFT] = "Ângulo visual para a esquerda";
  txt[PS_TETHERVISUALOFFSETUP] = "Ângulo visual para cima";
  txt[PS_TETHERDISTANCELEFT] = "Tether distance left";
  txt[PS_TETHERCOLOURR] = "Tether colour: red";
  txt[PS_TETHERCOLOURG] = "Tether colour: green";
  txt[PS_TETHERCOLOURB] = "Tether colour: blue";
  txt[PS_TETHERCOLOURA] = "Tether colour: opacity";
  txt[PS_CHASECAMERA] = "Câmara de perseguição";
  txt[PS_CAMERATARGETPOSFWD] = "Target offset fwd";
  txt[PS_CAMERATARGETPOSUP] = "Target offset up";
  txt[PS_DISTANCE] = "Distância";
  txt[PS_HEIGHT] = "Altitude";
  txt[PS_VERTICALVELFRAC] = "Vertical vel frac";
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
  txt[PS_CURRENTPOSITION] = "Posição actual";
  txt[PS_MASS] = "Peso";
  txt[PS_INERTIA] = "Inertia";
  txt[PS_WINGAREA] = "Wing area";
  txt[PS_EXTENTS] = "Extents";
  txt[PS_WINDSETTINGS] = "Configurações do vento";
  txt[PS_WINDSPEED] = "Velocidade do vento";
  txt[PS_WINDBEARING] = "Direção do vento";
  txt[PS_CANNOTMODIFYSCENERY] = "Não se pode modificar a cenário durante a prova";
  txt[PS_ALLOWBUNGEE] = "Permitir o lançamento por guincho";
  txt[PS_WINDGUSTTIME] = "Periodicidade rajadas";
  txt[PS_WINDGUSTFRACTION] = "Fracionar rajadas";
  txt[PS_WINDGUSTANGLE] = "Ângulo rajadas";
  txt[PS_TURBULENCEAMOUNT] = "Intens. turbulência";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Surface turbulence";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Wind shear turbulence";
  txt[PS_WINDLIFTSMOOTHING] = "Suavização do vento ascendente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Tendência de separação";
  txt[PS_ROTORTENDENCY] = "Rotor tendency";
  txt[PS_DEADAIRTURBULENCE] = "Dead air turbulence";
  txt[PS_BOUNDARYLAYERDEPTH] = "Boundary layer depth";
  txt[PS_THERMALSETTINGS] = "Configurações térmicas (veja também a iluminação)";
  txt[PS_DENSITY] = "Densidade";
  txt[PS_RANGE] = "Alcance";
  txt[PS_LIFESPAN] = "Tempo de vida";
  txt[PS_DEPTH] = "Profundidade";
  txt[PS_CORERADIUS] = "Core radius";
  txt[PS_DOWNDRAFTEXTENT] = "Downdraft extent";
  txt[PS_UPDRAFTSPEED] = "Updraft speed";
  txt[PS_ASCENTRATE] = "Taxa de súbida";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Expansion over lifespan";
  txt[PS_RUNWAY] = "Pista";
  txt[PS_RUNWAYTYPE] = "Runway type";
  txt[PS_CIRCLE] = "Circle";
  txt[PS_RUNWAYX] = "Posição X";
  txt[PS_RUNWAYY] = "Posição Y";
  txt[PS_RUNWAYHEIGHT] = "Altura";
  txt[PS_RUNWAYANGLE] = "Ângulo";
  txt[PS_RUNWAYLENGTH] = "Comprimento/Raio";
  txt[PS_RUNWAYWIDTH] = "Largura";
  txt[PS_SURFACESETTINGS] = "Definições da superfície";
  txt[PS_SURFACEROUGHNESS] = "Dureza";
  txt[PS_SURFACEFRICTION] = "Fricção";
  txt[PS_RANDOMTERRAINSETTINGS] = "Configurações de terreno aleatórios";
  txt[PS_RANDOMSEED] = "Semente aleatória";
  txt[PS_DISPLACEMENTHEIGHT] = "Altura de deslocação";
  txt[PS_SMOOTHNESS] = "Suavidade";
  txt[PS_EDGEHEIGHT] = "Limitar a altura";
  txt[PS_UPWARDSBIAS] = "Upwards bias";
  txt[PS_FILTERITERATIONS] = "Interacções do filtro";
  txt[PS_PLAINSETTINGS] = "Plain/water settings";
  txt[PS_DRAWPLAIN] = "Desenhar planície/água";
  txt[PS_COLLIDEWITHPLAIN] = "Colisão planície/água";
  txt[PS_PLAININNERRADIUS] = "Raio interno da planície";
  txt[PS_PLAINFOGDISTANCE] = "Distância do nevoeiro na planície";
  txt[PS_PLAINHEIGHT] = "Altura da planície";
  txt[PS_TERRAINSIZE] = "Tamanho do terreno";
  txt[PS_COASTENHANCEMENT] = "Nível de detalhe da ladeira";
  txt[PS_TERRAINDETAIL] = "Detalhes do terreno";
  txt[PS_RIDGETERRAINSETTINGS] = "Definições do cime do terreno";
  txt[PS_MAXHEIGHTFRACTION] = "Fração máxima de altura";
  txt[PS_WIDTH] = "Largura";
  txt[PS_HEIGHTOFFSET] = "Compensar altura";
  txt[PS_HORIZONTALVARIATION] = "Variação horizontal";
  txt[PS_HORIZONTALWAVELENGTH] = "Comprimento de onda horizontal";
  txt[PS_VERTICALVARIATION] = "Fração de variação vertical";
  txt[PS_PANORAMASETTINGS] = "Configurações do panorama";
  txt[PS_PANORAMA3DSETTINGS] = "Configurações do panorama 3D";
  txt[PS_HEIGHTFIELDSETTINGS] = "Ajustes da altura do campo";
  txt[PS_MINHEIGHT] = "Min altitude";
  txt[PS_MAXHEIGHT] = "Max altitude";
  txt[PS_AISCENERY] = "AI Scenery";
  txt[PS_SCENETYPE] = "Scene type";
  txt[PS_FLAT] = "Flat";
  txt[PS_SLOPE] = "Slope";
  txt[PS_CURRENTVIEWPOSITION] = "Posição da vista actual";
  txt[PS_NOLIGHTINGSETTINGS] = "Não é possível configurar a iluminação quando o tipo de terreno é Panorama";
  txt[PS_SUNBEARING] = "Posição Sol";
  txt[PS_THERMALACTIVITY] = "Actividade térmica";
  txt[PS_TERRAINDARKNESS] = "Escurecer o terreno";
  txt[PS_CONTROLLERSEESETTINGS] = "Verificar também definições adicionais do controlador em “Opções”";
  txt[PS_CROSS] = "Cruz";
  txt[PS_BOX] = "Caixa";
  txt[PS_CROSSANDBOX] = "Cruz e Caixa";
  txt[PS_CONFIGURATION] = "Configuração";
  txt[PS_STYLE] = "Estilo";
  txt[PS_CONTROLLERMODE] = "Modo do controlador";
  txt[PS_TREATTHROTTLEASBRAKES] = "Treat throttle stick as brakes";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Reset configuration on launch";
  txt[PS_NUMCONFIGURATIONS] = "Nº configurações";
  txt[PS_RIGHTSTICKHORIZONTAL] = "Stick direito horizontal";
  txt[PS_RIGHTSTICKVERTICAL] = "Stick direito vertical";
  txt[PS_LEFTSTICKHORIZONTAL] = "Stick esquerdo horizontal";
  txt[PS_LEFTSTICKVERTICAL] = "Stick esquerdo vertical";
  txt[PS_TILTHORIZONTAL] = "Inclinação horizontal";
  txt[PS_TILTVERTICAL] = "Inclinação vertical";
  txt[PS_ARROWSHORIZONTAL] = "Setas horizontais";
  txt[PS_ARROWSVERTICAL] = "Setas verticais";
  txt[PS_ROLLSTICK] = "Sitck Ailerons";
  txt[PS_PITCHSTICK] = "Stick Profundidade";
  txt[PS_YAWSTICK] = "Stick direcção";
  txt[PS_SPEEDSTICK] = "Stick motor";
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
  txt[PS_RATESBUTTON] = "Botão de níveis ciclo";
  txt[PS_RATESCYCLEBUTTON] = "Botão de níveis";
  txt[PS_RELAUNCHBUTTON] = "Botão de relançamento";
  txt[PS_CAMERABUTTON] = "Botão de ponto de vista";
  txt[PS_PAUSEPLAYBUTTON] = "Botão pausa/jogar";
  txt[PS_NONE] = "Nenhum";
  txt[PS_CONTROLSOURCES] = "Fontes de Controle";
  txt[PS_AILERONS] = "Ailerons/leme";
  txt[PS_ELEVATOR] = "Elevador";
  txt[PS_RUDDER] = "Leme";
  txt[PS_THROTTLE] = "Motor/flaps/travões";
  txt[PS_LOOKYAW] = "Visão vertical";
  txt[PS_LOOKPITCH] = "Visão horizontal";
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
  txt[PS_SETTINGSFORCONTROLLER] = "Definições para a configuração do controlador %d: %s";
  txt[PS_TRIMSETTINGS] = "Definições de ajuste";
  txt[PS_NOSIMPLESETTINGS] = "Sem definições simples disponíveis";
  txt[PS_TRIM] = "Ajuste";
  txt[PS_SCALE] = "Escala";
  txt[PS_CLAMPING] = "Clamping";
  txt[PS_EXPONENTIAL] = "Exponencial";
  txt[PS_SPRING] = "Mola";
  txt[PS_POSITIVE] = "Positive";
  txt[PS_NEGATIVE] = "Negative";
  txt[PS_ROLLSTICKMOVEMENT] = "Stick movimento ailerons";
  txt[PS_PITCHSTICKMOVEMENT] = "Stick movimento profundidade";
  txt[PS_YAWSTICKMOVEMENT] = "Stick movimento direcção";
  txt[PS_SPEEDSTICKMOVEMENT] = "Stick velocidade";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Acerelómetro: movimento longitudinal";
  txt[PS_ACCELEROMETERROLL] = "Acerelómetro longitudinal";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Acerelómetro: movimento transversal";
  txt[PS_ACCELEROMETERPITCH] = "Acerelómetro transversal";
  txt[PS_TILTROLLSENSITIVITY] = "Sensiblidade do movimento longitudinal";
  txt[PS_TILTPITCHSENSITIVITY] = "Sensiblidade do movimento de profundidade";
  txt[PS_TILTNEUTRALANGLE] = "Ângulo de inclinação neutro";
  txt[PS_NOJOYSTICKWITHID] = "Nenhum joystick com este ID";
  txt[PS_ENABLEJOYSTICK] = "Activar joystick";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Ajustar para o mov. circular stick";
  txt[PS_EXTERNALJOYSTICKSETTINGS] = "Definições do joystick externo";
  txt[PS_JOYSTICKLABEL] = "Joystick %d: Entrada = %5.2f Saída = %5.2f";
  txt[PS_JOYSTICKABUTTONLABEL] = "Joystick %d: Input = %5.2f Saída = %d";
  txt[PS_JOYSTICKBUTTONLABEL] = "Botão do joystick %d: Input = %5.2f Saída = %d";
  txt[PS_SCALEPOSITIVE] = "Input +ve scale";
  txt[PS_SCALENEGATIVE] = "Input -ve scale";
  txt[PS_OFFSET] = "Input Offset";
  txt[PS_MAPTO] = "Mapear para";
  txt[PS_PRESSWHENCENTRED] = "Press when centred";
  txt[PS_PRESSWHENLEFTORDOWN] = "Press when left or down";
  txt[PS_PRESSWHENRIGHTORUP] = "Press when right or up";
  txt[PS_DEADZONE] = "Zona neutra";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Limpar as definições do joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Iniciar a calibração do joystick (Windows)";
  txt[PS_LOADOPTIONS] = "Carregar opções";
  txt[PS_SAVEOPTIONS] = "Salvar opções";
  txt[PS_DELETEOPTIONS] = "Apagar opções";
  txt[PS_LOADSCENERY] = "Carregar cenário";
  txt[PS_SAVESCENERY] = "Salvar cenário";
  txt[PS_DELETESCENERY] = "Apagar cenário";
  txt[PS_LOADOBJECTS] = "Load objects";
  txt[PS_SAVEOBJECTS] = "Save objects";
  txt[PS_DELETEOBJECTS] = "Delete objects";
  txt[PS_LOADLIGHTING] = "Carregar iluminação";
  txt[PS_SAVELIGHTING] = "Salvar iluminação";
  txt[PS_DELETELIGHTING] = "Apagar iluminação";
  txt[PS_LOADCONTROLLER] = "Carregar controlador";
  txt[PS_SAVECONTROLLER] = "Salvar controlador";
  txt[PS_DELETECONTROLLER] = "Apagar controlador";
  txt[PS_LOADJOYSTICK] = "Carregar joystick";
  txt[PS_SAVEJOYSTICK] = "Salvar joystick";
  txt[PS_DELETEJOYSTICK] = "Apagar joystick";
  txt[PS_LOADAEROPLANE] = "Carregar modelo";
  txt[PS_SAVEAEROPLANE] = "Salvar modelo";
  txt[PS_DELETEAEROPLANE] = "Apagar modelo";
  txt[PS_LOADAICONTROLLERS] = "Load AI controllers";
  txt[PS_SAVEAICONTROLLERS] = "Save AI controllers";
  txt[PS_DELETEAICONTROLLERS] = "Delete AI controllers";
  txt[PS_SELECTPANORAMA] = "Selecionar panorama";
  txt[PS_SELECTTERRAINFILE] = "Selecionar ficheiro de terreno";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selecionar o controlador preferido";
  txt[PS_SELECTOBJECTSSETTINGS] = "Select objects settings";
  txt[PS_SUMMARY] = "Summary";
  txt[PS_NOOBJECTS] = "There are no objects loaded (other than the terrain etc). You can load a set of static and dynamic objects, "
    "making sure they're suitable for the current scene. By enabling object editing you can "
    "create your own - see help and the PicaSim web page for more info.";
  txt[PS_OBJECTSTOTAL] = "Total number of objects:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Number static visible:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Number static invisible:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Number dynamic visible:";
  txt[PS_ABOUT] = "Sobre";
  txt[PS_WEBSITE] = "Página de Internet";
  txt[PS_FLYING] = "Para voar";
  txt[PS_SETTINGS] = "Definições";
  txt[PS_RACES] = "Provas";
  txt[PS_TIPS] = "Tips";
  txt[PS_KEYBOARD] = "Keyboard";
  txt[PS_CREDITS] = "Créditos";
  txt[PS_LICENCE] = "Licença";
  txt[PS_VERSIONS] = "Versões";
  txt[PS_ABOUTFREETEXT] = "PicaSim é um simulador de modelos rádio-controlados.  O objectivo é ajudá-lo a aprender a voar "
"e a melhorar as suas capacidades em vôo de encosta, com planadores que utilizam o vento como elevador, "
"fazendo-os subir a encosta.\n"
"\n"
"Se nunca voou um modelo, é provável que falhe nas primeiras vezes (ou mais!) "
"que tente fazê-lo. Não há qualquer problema: é melhor “bater” no simulador do que com um modelo caro "
"que lhe tomou muitas horas de construção.\n"
"\n"
"PicaSim pode ajudá-lo a aprender três aspectos:\n"
"\n"
"1. Usar os sticks do transmissor para controlar o modelo.\n"
"2. Avaliar onde encontrar ventos ascendentes em encostas.\n"
"3. Controlar o modelo, mesmo quando ele vôa em direcção a si.\n"
"\n"
"Além disso, os utilizadores mais experientes podem usar o PicaSim para praticar acrobacias, "
"ou simplesmente para voar quando não há vento!\n"
"\n"
"Tenho muitos planos para alargar e melhorar o PicaSim, por exemplo, incluir aeronaves com "
"motor, mais modelos de aviões, voar em rede, aviões-robot, etc. Se quiser apoiar ou incentivar-me "
"nesta tarefa, adquirir a versão completa,  ou apenas dizer obrigado pela versão atual, "
"agradeço profundamente!\n";
  txt[PS_ABOUTPAIDTEXT] = "PicaSim é um simulador de modelos rádio-controlados.  O objectivo é ajudá-lo a aprender a voar "
"e a melhorar as suas capacidades em vôo de encosta, com planadores que utilizam o vento como elevador, "
"fazendo-os subir a encosta.\n"
"\n"
"Se nunca voou um modelo, é provável que falhe nas primeiras vezes (ou mais!) "
"que tente fazê-lo. Não há qualquer problema: é melhor “bater” no simulador do que com um modelo caro "
"que lhe tomou muitas horas de construção.\n"
"\n"
"PicaSim pode ajudá-lo a aprender três aspectos:\n"
"\n"
"1. Usar os sticks do transmissor para controlar o modelo.\n"
"2. Avaliar onde encontrar ventos ascendentes em encostas.\n"
"3. Controlar o modelo, mesmo quando ele vôa em direcção a si.\n"
"\n"
"Além disso, os utilizadores mais experientes podem usar o PicaSim para praticar acrobacias, "
"ou simplesmente para voar quando não há vento!\n"
"\n"
"Tenho muitos planos para alargar e melhorar o PicaSim, por exemplo, incluir aeronaves com "
"motor, mais modelos de aviões, voar em rede, aviões-robot, etc."
"O pequeno valor da compra será usado para me ajudar a fazer isso, e será muito apreciado!\n";
  txt[PS_HOWTOFLYTEXT] = "A primeira coisa a fazer é visitar o site (link abaixo) e ver o vídeo \"How to Fly\". \n"
"\n"
"Quando o simulador iniciar, estará em pausa. Se premir o botão no canto superior direito fará "
" com que o modelo inicie o voo. Para controlá-lo, tem de usar um joystick/controlador que existe "
"no canto inferior direito do écran, em conjunto com o stick da mão direita, de um (suposto) controle"
" de rádio-transmissor (criado em Modo 2). Se falhar, utilize o botão “voltar” do dispositivo controlador, "
"para repôr o modelo na posição inicial ou coloque em pausa e prima o botão 'rewind' e desligue a pausa.\n"
"\n"
"Agora que já domina os controlos básicos, precisa de manobrar o planador para que ele se mantenha "
"na área de ascenção do vento da encosta. A melhor forma de consegui-lo, é voar a "
"direito durante 3 ou 4 segundos, virar à esquerda e voar em paralelo à encosta durante cerca de "
"4 a 10 segundos. O vôo de regresso deve ser feito a uma distância e altura semelhantes ao ponto "
"onde você se encontra, e paralelamente à encosta.\n"
"\n"
"Se entender que a velocidade é demasiado rápida, abrande o tempo no painel das Configurações/Opções. "
"Mas não se esqueça de voltar ao 1.0 quando se sentir à-vontade -  na realidade, "
"elas não existem!\n"
"\n"
"Se você não estiver a usar um dos cenários panorâmicos,  pode também alterar o ponto de vista para o interior do avião, "
"(utilizando o botão 'olho', quando em pausa, ou o botão 'pesquisa' "
"do seu dispositivo). De seguida, pode explorar o terreno: ganhar altitude ou atravessar zonas "
"onde não há vento suficiente para subir. Para isso, utilize a seta que indica o vento, para perceber qual a sua direcção e, "
" em consequência, descobrir onde existem condições de ascensão.\n"
"\n"
"Se o terreno é essencialmente plano, isso pode querer dizer que existem térmicas, o que permite ao planador "
"ser lançado com um guincho/elástico. Terá de seguir com atenção o movimento do planador, para detectar onde estão "
"as térmicas. Se tiver dificuldades, active os auxiliares de visualização de térmicas nas Opções - estes auxiliares "
"estão desenhados no centro de cada térmica, com o tamanho aproximado do avião. Isto deve ajudá-lo a calcular a distância. "
"Não esquecer que as térmicas são mais fracas na água do que em terra, e mais fortes nas encostas viradas para o sol.\n"
"\n"
"O site abaixo mostra alguns vídeos que lhe indicam como voar. É preciso um pouco de prática, "
"tal como um modelo real!";
  txt[PS_HOWTOCONFIGURETEXT] = "Pode aceder às configurações do PicaSim, no menu inicial , ou usando um dos botões, quando em pausa.\n"
"\n"
"As configurações estão divididas em blocos - opções gerais, o modelo, o cenário/terreno, a iluminação "
"e a configuração do controlador. Por defeito, não pode mexer nas definições detalhadas individuais para a maioria deles, "
"mas pode usar os blocos de configuração predefinidos. Isso permite uma alteração rápida do  modelo ou terreno, por exemplo.\n"
"\n"
"Se activar as 'configurações avançadas' em cada secção, vai ter que ajustar muitas outras configurações, o que pode ser um pouco "
"assustador! Com o tempo, haverá algumas informações sobre essas configurações, mas por agora é seguro ajustar os valores "
"porque pode sempre recarregá-las com uma das predefinições.\n"
"\n"
"Pode salvar as suas predefinições - elas ficam guardados na memória do cartão/dados no seu dispositivo. Se tiver problemas com o "
"PicaSim após alterar as configurações, pode valer a pena recarregar cada secção, de uma vez, imediatamente após o início PicaSim.\n"
"\n"
"Se a opção 'Caminhada' fôr activada nas opções , terá também essa opção quando fizer uma pausa. Isso permite que use o "
"controlador para caminhar sobre o terreno, a fim de encontrar um novo local para voar. Por defeito, a direcção do vento é "
"ajustada automaticamente quando estiver neste modo (assim que sopra directamente para a câmara).\n"
"\n"
"Com paisagens panorâmicas não é possível mudar o ponto de vista, por isso, o modo de caminhada está desactivado.";
  txt[PS_HOWTORACETEXT] = "Nos modos de prova o objectivo é voar tão rápido quanto possível entre postos de controle. O próximo ponto para o qual "
"tem de prosseguir, é realçado através da côr, sendo também indicado  pela seta branca na parte inferior do ecrã.\n"
"\n"
"Existem diferentes tipos de postos de controle: nas corridas de estilo F3F, onde tem que concluir várias voltas, "
"tem de voar para além do posto de controle, contra o vento do mesmo. Na corrida de cross-country, só precisa passar perto "
"do posto de controle (embora a altitude seja aquela que desejar). Na corrida Flatland, a ideia é voar entre duas portas "
"- a porta contra o vento ascendente é verde, e a do descendente é vermelha.\n";
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
  txt[PS_CREDITSTEXT] = "O PicaSim foi desenvolvido por Danny Chapman.\n\n\n"
"Com agradecimentos a:\n\n"
"Heidi e Hazel pela sua paciência\n\n"
"Heidi, Jenne, André e Sylvain pelas traduções\n\n"
"Bullet pela física\n\n"
"TinyXML\n\n"
"Marmalade para as bibliotecas multi-plataformas\n\n"
"Gleb Lebedev para seu gamepad extension\n\n"
"Detlef Jacobi para Litermonthalle Nalbach/\n\n"
"Alguns sons de www.freesfx.co.uk\n\n"
"Textures de seamless-pixels.blogspot.co.uk/\n\n"
"A todos que tem ajudado com testes e a relatar bugs\n\n"
"Modelos de aviões fornecidos por:\n\n"
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
"Por favor, note que, embora eu tenha tentado configurar os modelos a voar de forma realista, não exijo que "
"realmente atingam esse objetivo. A única maneira de descobrir isso é ir lá para fora e voar no mundo real."
"Faça isso!";
  txt[PS_LICENCETEXT] = "PicaSim is available from me in the following versions:\n\n"
"Free: The simulator and assets (including planes and sceneries etc) may be distributed according to the Creative Commons Attribution-NonCommercial-NoDerivs licence.\n\n"
"Paid: This version or its derivatives may only be distributed with my explicit authorisation.\n\n"
"If you wish to share content that you have created then you must do so separately from the PicaSim distribution.\n\n"
"The assets in PicaSim may be distributed according to the Creative Commons Attribution-NonCommercial licence (i.e. you may modify and distribute them, so long as you credit the source).\n\n"
"It is quite easy to distribute content for PicaSim by placing it in the UserData and UserSettings. See the website and ask on the forum for more details.";
  txt[PS_TIPS1] = "Por favor, envie-me a sua opinião para que possa melhorar o PicaSim";
  txt[PS_TIPS2] = "Se gosta do PicaSim, por favor, deixe um comentário/classificação";
  txt[PS_TIPS3] = "Pode usar o acelerómetro para controlar o modelo - em Definições»Controlador";
  txt[PS_TIPS4] = "Gosto/seguir PicaSim no Facebook ou no Google+ para ser notificado das atualizações e planos";
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
  txt[PS_ALLOWCHOICE] = "Permitir a escolha do modelo/cenário na próxima vez";
  txt[PS_LOADING] = "A carregar...";
  txt[PS_RACETIP] = "%s\n\nThe white arrow points to the next checkpoint.";
  txt[PS_LIMBOTIP] = "%s\n\nAdjust the difficulty/score multipler in Settings»Options 2";
  txt[PS_DURATIONTIP] = "%s\n\nLook for thermals and land close to the launch point.";
  txt[PS_TRAINERGLIDERTIP] = "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador para controlar o leme de profundidade e o leme de direcção, quando o planador estiver a subir em frente a uma encosta. Verificar na página de internet  algumas dicas sobre como voar, no caso de considerar difícil mantê-lo no ar.";
  txt[PS_TRAINERPOWEREDTIP] = "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador à direita para controlar o leme de profundidade e os ailerons, e o da esquerda para controlar o motor e o leme de direcção.";
  txt[PS_MOUSETOCONTINUE] = "Usar o rato ou qualquer tecla para continuar";
  txt[PS_TAPTOCONTINUE] = "Clique aqui para continuar";
  txt[PS_ALLRESULTS] = "Todos os resultados";
  txt[PS_LAST24HOURS] = "Nas últimas 24 horas";
  txt[PS_LAST7DAYS] = "Nos últimos 7 dias";
  txt[PS_LAST30DAYS] = "Nos últimos 30 dias";
  txt[PS_TIMEPERIODFORRESULTS] = "Período de tempo para obter resultados";
  txt[PS_SCORELINERACE] = "  Pontos = %.1f Tempo = %.2f";
  txt[PS_SCORELINELIMBO] = "  Pontos = %.1f Contar = %d";
  txt[PS_NOSCORES] = "Sem pontos ainda";
  txt[PS_CURRENTSCORE] = "Pontos actuais = %.1f Tempo = %.2f";
  txt[PS_NOSCORESINTIME] = "Sem pontuação no período de tempo";
  txt[PS_LEADERBOARDFAILED] = "Falhou a leitura da tabela - verifique a ligacao a internet";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Falhou a leitura da tabela , vou tentar outra vez...";
  txt[PS_RETRIEVINGLEADERBOARD] = "Recuperando tabela de pontos...";
  txt[PS_SCORESUBMISSIONFAILED] = "Nao consegui enviar os resultados vou tentar outra vez - verifique a ligacao a internet";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Nao consegui enviar os resultados vou tentar outra vez...";
  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";
  txt[PS_SETUSERNAME] = "Definir nome de utilizador";
  txt[PS_SETEMAIL] = "Definir email";
  txt[PS_USERFAILED] = "O pedido do utilizador falhou , vou tentar outra vez - verifique a ligacao a internet";
  txt[PS_USERFAILEDRETRYING] = "O pedido do utilizador falhou , vou tentar outra vez...";
  txt[PS_PROFILE] = "Perfil";
  txt[PS_USERNAME] = "Nome de utilizador";
  txt[PS_EMAIL] = "Email";
  txt[PS_LEADERBOARDS] = "Pontuação";
  txt[PS_UPLOADINGSCORE] = "Envio de pontos...";
  txt[PS_REQUESTINGUSER] = "Solicitando detalhes do utilizador...";
  txt[PS_SELECTRACE] = "Selecionar prova";
  txt[PS_SELECTRACETOSEE] = "Selecionar prova para ver os pontos";
  txt[PS_EDIT] = "Editar";
  txt[PS_PROFILESTATUS] = "Estado do perfíl";
  txt[PS_SCORELOOPUSERNAME] = "Scoreloop nome do utilizador";
  txt[PS_EMAILADDRESS] = "Endereço de email";
  txt[PS_OPTIONALEMAIL] = "Opcional - interligar o utilizador Scoreloop pelos diversos dispositivos";
  txt[PS_FETCHINGEMAIL] = "Procurar email";
  txt[PS_PREV] = "Anterior";
  txt[PS_TOP] = "Topo";
  txt[PS_ME] = "Eu";
  txt[PS_NEXT] = "Próximo";
  txt[PS_UPDATINGLEADERBOARD] = "Actualizar pontuação...";
  txt[PS_NOMORESCORES] = "Sem mais pontuações";
  txt[PS_SELECTARACE] = "Selecione a prova";
  txt[PS_UPLOADINGDATA] = "Envio de dados...";
  txt[PS_FAILED] = "Falhou: ";
  txt[PS_EMAILTAKEN] = "Email indisponível. ";
  txt[PS_EMAILINVALID] = "Formato de email inválido. ";
  txt[PS_USERTAKEN] = "Nome de utilizador indisponível. ";
  txt[PS_USERINVALID] = "Nome de utilizador invalido. ";
  txt[PS_USERSHORT] = "Nome de utilizador demasiado curto. ";
  txt[PS_QUITANDHELP] = "Quit and help";
  txt[PS_CAMERAANDSETTINGS] = "Camera and settings";
  txt[PS_RESETANDPLAY] = "Reset and play/pause";
  txt[PS_THROTTLEANDRUDDER] = "Throttle/flaps and rudder";
  txt[PS_WINDDIRECTION] = "Wind direction";
  txt[PS_AILERONSANDELEVATOR] = "Ailerons and elevator";
  txt[PS_FLYINGINFO] = "Flying";
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

