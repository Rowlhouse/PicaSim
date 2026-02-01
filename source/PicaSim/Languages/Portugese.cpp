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
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Ativar";
  txt[PS_REMOVE] = "Remover";
  txt[PS_DUPLICATE] = "Duplicar";
  txt[PS_CRASHED] = "Danificado";
  txt[PS_NO] = "Não";
  txt[PS_YES] = "Sim";
  txt[PS_INVERT] = "Inverter";
  txt[PS_AUTO] = "Auto";
  txt[PS_GAMEPADDETECTED] = "Gamepad detetado. Ative nas definições para usá-lo.";
  txt[PS_WHATSNEW] = "Welcome to PicaSim!";
  txt[PS_FREEFLY] = "Voo Livre";
  txt[PS_CHALLENGE] = "Desafio";
  txt[PS_SELECTAEROPLANE] = "Modelo";
  txt[PS_SELECTSCENERY] = "Cenário";
  txt[PS_SELECTOPTIONS] = "Por favor seleccione um conjunto adequado de opções padrão";
  txt[PS_SELECTSCENARIO] = "Cenário";
  txt[PS_USEDEFAULT] = "Usar padrão";
  txt[PS_USEDEFAULTPREVIOUS] = "Usar padrão/anterior";
  txt[PS_BACK] = "Anterior";
  txt[PS_OPTIONS1] = "Opções";
  txt[PS_OPTIONS2] = "Modo";
  txt[PS_AEROPLANE] = "Modelo";
  txt[PS_SCENERY] = "Cenário";
  txt[PS_OBJECTS] = "Objetos";
  txt[PS_OBJECTSSETTINGS] = "Definições de objetos";
  txt[PS_LIGHTING] = "Iluminação";
  txt[PS_AICONTROLLERS] = "Controladores IA";
  txt[PS_CONTROLLER] = "Controlador";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Carregar...";
  txt[PS_SAVE] = "Salvar...";
  txt[PS_DELETE] = "Apagar...";
  txt[PS_ADVANCED] = "Avançado";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Limpar todas as definições e sair";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "Isto limpará todas as definições guardadas e sairá do PicaSim - tem a certeza?";
  txt[PS_SIMPLE] = "Simples";
  txt[PS_FILENAME] = "Nome do ficheiro:";
  txt[PS_LANGUAGESETTINGS] = "Definições de idioma";
  txt[PS_CURRENTLANGUAGE] = "Idioma atual";
  txt[PS_CAMERASETTINGS] = "Configurações da câmara";
  txt[PS_ZOOMVIEW] = "Zoom";
  txt[PS_PLANEONLYINZOOMVIEW] = "Mostrar avião sozinho no zoom";
  txt[PS_SMOKEONLYINMAINVIEW] = "Mostrar fumo apenas na vista principal";
  txt[PS_ZOOMVIEWSIZE] = "Tamanho do Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Terreno Auto-Zoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Escala do autozoom terreno";
  txt[PS_GROUNDFIELDOFVIEW] = "Campo de visão";
  txt[PS_GROUNDHORIZONAMOUNT] = "Quantidade de terreno no horizonte";
  txt[PS_GROUNDLAG] = "Atraso da câmara de solo";
  txt[PS_GROUNDVIEWFOLLOW] = "Seguir modelo em visão de terreno";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Desvio direcional na vista de solo";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Desvio de inclinação na vista de solo";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Campo de visão do modelo";
  txt[PS_STEREOSCOPY] = "Estereoscopia";
  txt[PS_STEREOSEPARATION] = "Separação estéreo";
  txt[PS_STEREOINFO] = "Valor +ve para 3D normal, -ve para técnica de olhos cruzados";
  txt[PS_WALKABOUTSETTINGS] = "Definições caminhada";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activar botão caminhada";
  txt[PS_ENABLEOBJECTEDITING] = "Ativar edição de objetos";
  txt[PS_FORCEALLVISIBLE] = "Tornar visível ao editar";
  txt[PS_RESETOBJECTS] = "Repor objetos e recarregar cenário";
  txt[PS_OBJECTEDITING] = "Edição de objetos";
  txt[PS_OBJECTEDITINGTEXT] = "O PicaSim permite simular e desenhar objetos adicionais, além do terreno e do avião (etc).\n"
"\n"
"No separador Definições»Objetos pode carregar um conjunto destes objetos - isto é feito automaticamente ao carregar uma "
"cena. A maioria das cenas não inclui objetos. No entanto, algumas cenas panorâmicas incluem para representar objetos "
"que não fazem parte do solo.\n"
"\n"
"Se quiser adicionar objetos à cena, pode ativar a edição de objetos em Definições»Objetos. Isto fará aparecer um conjunto de "
"botões no ecrã que lhe permitem criar/apagar objetos, alterar a seleção e modificar as suas propriedades. Ao movê-los, "
"tenha em mente que pode escolher trabalhar no espaço mundo, relativo à câmara, ou relativo ao espaço do objeto. "
"Depois de criar um conjunto de objetos, não se esqueça de os guardar!\n"
"\n";
  txt[PS_OBJECTNUMBER] = "Objeto %d";
  txt[PS_POSITIONX] = "Posição frente";
  txt[PS_POSITIONY] = "Posição esquerda";
  txt[PS_POSITIONZ] = "Posição cima";
  txt[PS_COLOURR] = "Cor vermelho";
  txt[PS_COLOURG] = "Cor verde";
  txt[PS_COLOURB] = "Cor azul";
  txt[PS_COLOURH] = "Matiz";
  txt[PS_COLOURS] = "Saturação";
  txt[PS_COLOURV] = "Brilho";
  txt[PS_POSITION] = "Posição";
  txt[PS_SIZE] = "Tamanho";
  txt[PS_COLOUR] = "Cor";
  txt[PS_VISIBLE] = "Visível";
  txt[PS_SHADOW] = "Sombra";
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
  txt[PS_BRAKESFORWARD] = "Freios aéreos com stick para frente";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Usar posição absoluta do toque";
  txt[PS_STAGGERCONTROLLER] = "Controlador Stagger (para multitouch parcial)";
  txt[PS_ENABLEELEVATORTRIM] = "Ativar trim do profundidade";
  txt[PS_ELEVATORTRIMSIZE] = "Tamanho do trim profundidade";
  txt[PS_SQUARECONTROLLERS] = "Controladores quadrados";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Deslocamento horizontal do limite";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Deslocamento vertical do limite";
  txt[PS_SHAPEOPACITY] = "Ajustar a transparência";
  txt[PS_STICKOPACITY] = "Transparência do stick";
  txt[PS_STICKCROSS] = "Cruz do stick";
  txt[PS_JOYSTICKID] = "ID do joystick";
  txt[PS_JOYSTICKINFO] = "Nota: Pode ser necessário reiniciar o PicaSim após ligar ou desligar um joystick.";
  txt[PS_NOJOYSTICK] = "Nenhum joystick detetado ou disponível.";
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
  txt[PS_THERMALWINDFIELD] = "Desenhar campo de vento térmico";
  txt[PS_GRAPHAIRSPEED] = "Mostrar vel. vento (vermelho)";
  txt[PS_GRAPHDURATION] = "Duração do gráfico";
  txt[PS_GRAPHALTITUDE] = "Mostrar altitude (turquesa)";
  txt[PS_GRAPHGROUNDSPEED] = "Mostrar vel. solo (verde)";
  txt[PS_GRAPHCLIMBRATE] = "Mostrar nivel de subida (azul)";
  txt[PS_GRAPHWINDSPEED] = "Mostrar vel. vento (amar.)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Mostrar vel. vert. do vento (roxo)";
  txt[PS_STALLMARKERS] = "Marcas perda sustentação";
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
  txt[PS_CONTROLLEDPLANESHADOWS] = "Sombras do avião controlado";
  txt[PS_OTHERSHADOWS] = "Outras sombras";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Detalhe sombra projetada";
  txt[PS_BLOB] = "Mancha";
  txt[PS_PROJECTED] = "Projetada";
  txt[PS_AEROPLANERENDER] = "Renderização do Modelo";
  txt[PS_USE16BIT] = "Usar text. 16bit reduz memória (reini. Picasim)";
  txt[PS_SEPARATESPECULAR] = "Cálculo especular separado";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Escala ilum. ambiente";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Escala iluminação difusa";
  txt[PS_TERRAINTEXTUREDETAIL] = "Detalhe text. terreno";
  txt[PS_MAXSKYBOXDETAIL] = "Detalhe máx skybox/panorama";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Requer reinício";
  txt[PS_MISCSETTINGS] = "Configurações várias";
  txt[PS_USEBACKBUTTON] = "Utilize o botão anterior para sair";
  txt[PS_DRAWLAUNCHMARKER] = "Mostrar a marca de lançamento";
  txt[PS_DRAWGROUNDPOSITION] = "Mostrar a marca de terreno";
  txt[PS_SKYGRIDOVERLAY] = "Grelha de céu";
  txt[PS_SKYGRID_NONE] = "Nenhuma";
  txt[PS_SKYGRID_SPHERE] = "Esfera";
  txt[PS_SKYGRID_BOX] = "Caixa";
  txt[PS_SKYGRIDALIGNMENT] = "Alinhamento da grelha";
  txt[PS_SKYGRIDDISTANCE] = "Distância da grelha";
  txt[PS_SKYGRIDALIGN_ALONGWIND] = "Com o vento";
  txt[PS_SKYGRIDALIGN_CROSSWIND] = "Através do vento";
  txt[PS_SKYGRIDALIGN_ALONGRUNWAY] = "Ao longo da pista";
  txt[PS_SKYGRIDALIGN_CROSSRUNWAY] = "Através da pista";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Usar o controlador preferido do avião";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Definições teste/desenvol.";
  txt[PS_WIREFRAMETERRAIN] = "Terreno em wireframe";
  txt[PS_DRAWSUNPOSITION] = "Desenhar posição sol";
  txt[PS_FREEFLYSETTINGS] = "Configurações de voo livre";
  txt[PS_DISPLAYFLIGHTTIME] = "Mostrar tempo de voo";
  txt[PS_DISPLAYSPEED] = "Mostrar velocidade";
  txt[PS_DISPLAYAIRSPEED] = "Mostrar velocidade do ar";
  txt[PS_DISPLAYMAXSPEED] = "Mostrar max velocidade";
  txt[PS_DISPLAYASCENTRATE] = "Mostrar taxa de ascenção";
  txt[PS_DISPLAYALTITUDE] = "Mostar altitude";
  txt[PS_DISPLAYDISTANCE] = "Mostar distância";
  txt[PS_COLOURTEXT] = "Colorir texto taxa ascenção";
  txt[PS_TEXTATTOP] = "Texto no topo do ecrã";
  txt[PS_FREEFLYONSTARTUP] = "Voo livre no início";
  txt[PS_ENABLESOCKETCONTROLLER] = "Ativar controlador de rede";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Opacidade do fundo do texto";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Cor do fundo do texto";
  txt[PS_MAXNUMBERAI] = "Número máximo de IA";
  txt[PS_TOOMANYAI] = "Nota: Muitas IA causarão falhas e lentidão!";
  txt[PS_UNITS] = "Unidades";
  txt[PS_RACESETTINGS] = "Configuração da prova/limbo";
  txt[PS_RACEVIBRATIONAMOUNT] = "Intensidade de vibração";
  txt[PS_RACEBEEPVOLUME] = "Volume do aviso";
  txt[PS_LIMBODIFFICULTY] = "Dificuldade do limbo";
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
  txt[PS_SHOWBUTTON1] = "Mostrar botão 1";
  txt[PS_SHOWBUTTON2] = "Mostrar botão 2";
  txt[PS_LAUNCH] = "Lançar";
  txt[PS_HOOKS] = "Ganchos de reboque";
  txt[PS_BUNGEELAUNCH] = "Lançamento por elástico";
  txt[PS_AEROTOWLAUNCH] = "Lançamento por reboque";
  txt[PS_FLATLAUNCHMETHOD] = "Método de lançamento (cenários planos)";
  txt[PS_HAND] = "Mão";
  txt[PS_BUNGEE] = "Elástico";
  txt[PS_AEROTOW] = "Reboque";
  txt[PS_LAUNCHSPEED] = "Vel. lançamento";
  txt[PS_LAUNCHANGLE] = "Âng. de lançamento";
  txt[PS_LAUNCHUP] = "Lançar para cima";
  txt[PS_LAUNCHFORWARD] = "Lançar em frente";
  txt[PS_LAUNCHLEFT] = "Lançar em esquerda";
  txt[PS_LAUNCHOFFSETUP] = "Deslocamento de lançamento para cima";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relançar quando parado";
  txt[PS_CRASHDETECTION] = "Deteção de colisão";
  txt[PS_CRASHDELTAVELX] = "Delta veloc. frente/trás";
  txt[PS_CRASHDELTAVELY] = "Delta veloc. lateral";
  txt[PS_CRASHDELTAVELZ] = "Delta veloc. cima/baixo";
  txt[PS_CRASHDELTAANGVELX] = "Delta veloc. rolamento";
  txt[PS_CRASHDELTAANGVELY] = "Delta veloc. inclinação";
  txt[PS_CRASHDELTAANGVELZ] = "Delta veloc. direcional";
  txt[PS_CRASHSUSPENSIONFORCESCALE] = "Escala resiliência suspensão";
  txt[PS_AIRFRAME] = "Fuselagem";
  txt[PS_UNDERCARRIAGE] = "Chassis";
  txt[PS_PROPELLER] = "Hélice";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Desl. gancho barriga frente";
  txt[PS_BELLYHOOKOFFSETUP] = "Desl. gancho barriga cima";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Desl. gancho nariz frente";
  txt[PS_NOSEHOOKOFFSETUP] = "Desl. gancho nariz cima";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Desl. gancho cauda frente";
  txt[PS_TAILHOOKOFFSETUP] = "Desl. gancho cauda cima";
  txt[PS_MAXBUNGEELENGTH] = "Tam. máx. guincho";
  txt[PS_MAXBUNGEEACCEL] = "Acel. máx. guincho";
  txt[PS_TUGPLANE] = "Avião rebocador";
  txt[PS_TUGSIZESCALE] = "Escala tamanho rebocador";
  txt[PS_TUGMASSSCALE] = "Escala massa rebocador";
  txt[PS_TUGENGINESCALE] = "Escala motor rebocador";
  txt[PS_TUGMAXCLIMBSLOPE] = "Inclinação máx subida rebocador";
  txt[PS_TUGTARGETSPEED] = "Velocidade alvo rebocador";
  txt[PS_AEROTOWROPELENGTH] = "Comprimento cabo reboque";
  txt[PS_AEROTOWROPESTRENGTH] = "Força cabo reboque";
  txt[PS_AEROTOWROPEMASSSCALE] = "Escala massa cabo";
  txt[PS_AEROTOWROPEDRAGSCALE] = "Escala arrasto cabo";
  txt[PS_AEROTOWHEIGHT] = "Altura máx reboque";
  txt[PS_AEROTOWCIRCUITSIZE] = "Tamanho circuito reboque";
  txt[PS_TETHERING] = "No limite (linha de controle)";
  txt[PS_TETHERLINES] = "Número de linhas de limitação";
  txt[PS_TETHERREQUIRESTENSION] = "Requer tensão para controlo";
  txt[PS_TETHERPHYSICSOFFSETFORWARD] = "Ângulo físico para a frente";
  txt[PS_TETHERPHYSICSOFFSETLEFT] = "Ângulo físico para a esquerda";
  txt[PS_TETHERPHYSICSOFFSETUP] = "Ângulo físico para cima";
  txt[PS_TETHERVISUALOFFSETFORWARD] = "Ângulo visual para a frente";
  txt[PS_TETHERVISUALOFFSETLEFT] = "Ângulo visual para a esquerda";
  txt[PS_TETHERVISUALOFFSETUP] = "Ângulo visual para cima";
  txt[PS_TETHERDISTANCELEFT] = "Distância cabo esquerda";
  txt[PS_TETHERCOLOURR] = "Cor cabo: vermelho";
  txt[PS_TETHERCOLOURG] = "Cor cabo: verde";
  txt[PS_TETHERCOLOURB] = "Cor cabo: azul";
  txt[PS_TETHERCOLOURA] = "Cor cabo: opacidade";
  txt[PS_CHASECAMERA] = "Câmara de perseguição";
  txt[PS_CAMERATARGETPOSFWD] = "Desl. alvo frente";
  txt[PS_CAMERATARGETPOSUP] = "Desl. alvo cima";
  txt[PS_DISTANCE] = "Distância";
  txt[PS_HEIGHT] = "Altitude";
  txt[PS_VERTICALVELFRAC] = "Vertical vel frac";
  txt[PS_FLEXIBILITY] = "Flexibilidade";
  txt[PS_COCKPITCAMERA] = "Câmara do cockpit";
  txt[PS_PITCH] = "Inclinação";
  txt[PS_ENABLEDEBUGDRAW] = "Ativar desenho de debug";
  txt[PS_CANTOW] = "Pode rebocar";
  txt[PS_MAXAI] = "Número máx controladores IA: %d (ver Opções 2)";
  txt[PS_ADDNEWAICONTROLLER] = "Adicionar controlador IA";
  txt[PS_REMOVEAICONTROLLERS] = "Remover todos controladores IA";
  txt[PS_LAUNCHSEPARATIONDISTANCE] = "Distância separação lançamento";
  txt[PS_AVAILABLEINFULLVERSION] = "Disponível apenas na versão completa";
  txt[PS_INCLUDEINCAMERAVIEWS] = "Ativar câmaras";
  txt[PS_CREATEMAXNUMCONTROLLERS] = "Criar máx controladores IA";
  txt[PS_LAUNCHDIRECTION] = "Direção de lançamento";
  txt[PS_RANDOMCOLOUROFFSET] = "Desvio de cor aleatório";
  txt[PS_AICONTROLLER] = "Controlador IA";
  txt[PS_AINAVIGATION] = "Navegação IA";
  txt[PS_PLANETYPE] = "Tipo de avião";
  txt[PS_USER] = "Utilizador";
  txt[PS_ALL] = "Todos";
  txt[PS_GLIDER] = "Planador";
  txt[PS_GLIDERS] = "Planadores";
  txt[PS_POWERED] = "Motorizado";
  txt[PS_HELI] = "Helicóptero";
  txt[PS_CONTROLLINE] = "Voo circular";
  txt[PS_PANORAMIC] = "Panorâmico";
  txt[PS_3D] = "3D";
  txt[PS_ALLOWAICONTROL] = "Permitir controlo IA";
  txt[PS_WAYPOINTTOLERANCE] = "Tolerância waypoint";
  txt[PS_MINSPEED] = "Velocidade mín";
  txt[PS_CRUISESPEED] = "Velocidade cruzeiro";
  txt[PS_MAXBANKANGLE] = "Ângulo inclinação máx";
  txt[PS_BANKANGLEPERHEADINGCHANGE] = "Inclinação por mudança de rumo";
  txt[PS_SPEEDPERALTITUDECHANGE] = "Veloc. por mudança altitude";
  txt[PS_GLIDESLOPEPEREXCESSSPEED] = "Rampa planeio por excesso veloc.";
  txt[PS_PITCHPERROLLANGLE] = "Inclinação por ângulo rolamento";
  txt[PS_HEADINGCHANGEFORNOSLOPE] = "Mudança rumo sem encosta";
  txt[PS_MAXPITCHCONTROL] = "Controlo inclinação máx";
  txt[PS_MAXROLLCONTROL] = "Controlo rolamento máx";
  txt[PS_CONTROLPERROLLANGLE] = "Controlo por ângulo rolamento";
  txt[PS_PITCHCONTROLPERGLIDESLOPE] = "Ctrl inclinação por rampa planeio";
  txt[PS_ROLLTIMESCALE] = "Escala tempo rolamento";
  txt[PS_PITCHTIMESCALE] = "Escala tempo inclinação";
  txt[PS_MINALTITUDE] = "Altitude mín";
  txt[PS_SLOPEMINUPWINDDISTANCE] = "Encosta dist. mín contra vento";
  txt[PS_SLOPEMAXUPWINDDISTANCE] = "Encosta dist. máx contra vento";
  txt[PS_SLOPEMINLEFTDISTANCE] = "Encosta dist. mín esquerda";
  txt[PS_SLOPEMAXLEFTDISTANCE] = "Encosta dist. máx esquerda";
  txt[PS_SLOPEMINUPDISTANCE] = "Encosta dist. mín cima";
  txt[PS_SLOPEMAXUPDISTANCE] = "Encosta dist. máx cima";
  txt[PS_SLOPEMAXWAYPOINTTIME] = "Encosta tempo máx waypoint";
  txt[PS_FLATMAXDISTANCE] = "Plano tempo máx waypoint";
  txt[PS_FLATMAXWAYPOINTTIME] = "Plano tempo máx waypoint";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Posição actual";
  txt[PS_MASS] = "Peso";
  txt[PS_INERTIA] = "Inércia";
  txt[PS_WINGAREA] = "Área da asa";
  txt[PS_EXTENTS] = "Dimensões";
  txt[PS_WINDSETTINGS] = "Configurações do vento";
  txt[PS_WINDSPEED] = "Velocidade do vento";
  txt[PS_WINDBEARING] = "Direção do vento";
  txt[PS_CANNOTMODIFYSCENERY] = "Não se pode modificar a cenário durante a prova";
  txt[PS_ALLOWBUNGEE] = "Permitir o lançamento por guincho";
  txt[PS_WINDGUSTTIME] = "Periodicidade rajadas";
  txt[PS_WINDGUSTFRACTION] = "Fracionar rajadas";
  txt[PS_WINDGUSTANGLE] = "Ângulo rajadas";
  txt[PS_TURBULENCEAMOUNT] = "Intens. turbulência";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Turbulência de superfície";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Turbulência de cisalhamento";
  txt[PS_WINDLIFTSMOOTHING] = "Suavização do vento ascendente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Altura decaimento vento vertical";
  txt[PS_SEPARATIONTENDENCY] = "Tendência de separação";
  txt[PS_ROTORTENDENCY] = "Tendência do rotor";
  txt[PS_DEADAIRTURBULENCE] = "Dead air turbulence";
  txt[PS_BOUNDARYLAYERDEPTH] = "Espessura camada limite";
  txt[PS_THERMALSETTINGS] = "Configurações térmicas (veja também a iluminação)";
  txt[PS_DENSITY] = "Densidade";
  txt[PS_RANGE] = "Alcance";
  txt[PS_LIFESPAN] = "Tempo de vida";
  txt[PS_DEPTH] = "Profundidade";
  txt[PS_CORERADIUS] = "Core radius";
  txt[PS_DOWNDRAFTEXTENT] = "Downdraft extent";
  txt[PS_UPDRAFTSPEED] = "Updraft speed";
  txt[PS_ASCENTRATE] = "Taxa de súbida";
  txt[PS_THERMALEXPANSIONOVERLIFESPAN] = "Expansão ao longo da vida";
  txt[PS_RUNWAY] = "Pista";
  txt[PS_RUNWAYTYPE] = "Tipo de pista";
  txt[PS_CIRCLE] = "Círculo";
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
  txt[PS_UPWARDSBIAS] = "Tendência ascendente";
  txt[PS_FILTERITERATIONS] = "Interacções do filtro";
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
  txt[PS_PANORAMA3DSETTINGS] = "Configurações do panorama 3D";
  txt[PS_HEIGHTFIELDSETTINGS] = "Ajustes da altura do campo";
  txt[PS_MINHEIGHT] = "Min altitude";
  txt[PS_MAXHEIGHT] = "Max altitude";
  txt[PS_AISCENERY] = "Cenário IA";
  txt[PS_SCENETYPE] = "Tipo de cena";
  txt[PS_FLAT] = "Plano";
  txt[PS_SLOPE] = "Encosta";
  txt[PS_CURRENTVIEWPOSITION] = "Posição da vista actual";
  txt[PS_NOLIGHTINGSETTINGS] = "Não é possível configurar a iluminação quando o tipo de terreno é Panorama";
  txt[PS_SUNBEARING] = "Posição Sol";
  txt[PS_THERMALACTIVITY] = "Actividade térmica";
  txt[PS_TERRAINDARKNESS] = "Escurecer o terreno";
  txt[PS_CONTROLLERSEESETTINGS] = "Verificar também definições adicionais do controlador em “Opções”";
  txt[PS_CROSS] = "Cruz";
  txt[PS_BOX] = "Caixa";
  txt[PS_CROSSANDBOX] = "Cruz e Caixa";
  txt[PS_STYLE] = "Estilo";
  txt[PS_CONTROLLERMODE] = "Modo do controlador";
  txt[PS_TREATTHROTTLEASBRAKES] = "Usar stick motor como travões";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Repor configuração no lançamento";
  txt[PS_NUMCONFIGURATIONS] = "Nº configurações";
  txt[PS_TILTHORIZONTAL] = "Inclinação horizontal";
  txt[PS_TILTVERTICAL] = "Inclinação vertical";
  txt[PS_ARROWSHORIZONTAL] = "Setas horizontais";
  txt[PS_ARROWSVERTICAL] = "Setas verticais";
  txt[PS_ROLLSTICK] = "Sitck Ailerons";
  txt[PS_PITCHSTICK] = "Stick Profundidade";
  txt[PS_YAWSTICK] = "Stick direcção";
  txt[PS_SPEEDSTICK] = "Stick motor";
  txt[PS_CONSTANT] = "Constante";
  txt[PS_BUTTON0] = "Botão 1";
  txt[PS_BUTTON1] = "Botão 2";
  txt[PS_BUTTON2] = "Botão 3";
  txt[PS_BUTTON0TOGGLE] = "Botão 1 alternar";
  txt[PS_BUTTON1TOGGLE] = "Botão 2 alternar";
  txt[PS_BUTTON2TOGGLE] = "Botão 3 alternar";
  txt[PS_MIXES] = "Misturas";
  txt[PS_ELEVATORTOFLAPS] = "Profundidade para flaps";
  txt[PS_AILERONTORUDDER] = "Ailerons para leme";
  txt[PS_FLAPSTOELEVATOR] = "Flaps para profundidade";
  txt[PS_BRAKESTOELEVATOR] = "Travões/motor para profundidade";
  txt[PS_RUDDERTOELEVATOR] = "Leme para profundidade";
  txt[PS_RUDDERTOAILERON] = "Leme para ailerons";
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
  txt[PS_SMOKE1] = "Fumo 1";
  txt[PS_SMOKE2] = "Fumo 2";
  txt[PS_HOOK] = "Gancho";
  txt[PS_VELFWD] = "Veloc. frente";
  txt[PS_VELLEFT] = "Veloc. esquerda";
  txt[PS_VELUP] = "Veloc. cima";
  txt[PS_MAXPARTICLES] = "Núm. máx partículas";
  txt[PS_CHANNELFOROPACITY] = "Canal para opacidade";
  txt[PS_MINOPACITY] = "Opacidade mín";
  txt[PS_MAXOPACITY] = "Opacidade máx";
  txt[PS_CHANNELFORRATE] = "Canal para taxa";
  txt[PS_MINRATE] = "Taxa mín";
  txt[PS_MAXRATE] = "Taxa máx";
  txt[PS_INITIALSIZE] = "Tamanho inicial";
  txt[PS_FINALSIZE] = "Tamanho final";
  txt[PS_DAMPINGTIME] = "Tempo de amortecimento";
  txt[PS_JITTER] = "Tremor";
  txt[PS_ENGINEWASH] = "Quantidade sopro motor";
  txt[PS_HUECYCLEFREQ] = "Frequência ciclo matiz";
  txt[PS_SETTINGSFORCONTROLLER] = "Definições para a configuração do controlador %d: %s";
  txt[PS_TRIMSETTINGS] = "Definições de ajuste";
  txt[PS_NOSIMPLESETTINGS] = "Sem definições simples disponíveis";
  txt[PS_TRIM] = "Ajuste";
  txt[PS_SCALE] = "Escala";
  txt[PS_CLAMPING] = "Limitação";
  txt[PS_EXPONENTIAL] = "Exponencial";
  txt[PS_SPRING] = "Mola";
  txt[PS_POSITIVE] = "Positivo";
  txt[PS_NEGATIVE] = "Negativo";
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
  txt[PS_JOYSTICKBUTTONLABEL] = "Botão do joystick %d: Input = %5.2f Saída = %d";
  txt[PS_SCALEPOSITIVE] = "Escala entrada +ve";
  txt[PS_SCALENEGATIVE] = "Escala entrada -ve";
  txt[PS_OFFSET] = "Desvio entrada";
  txt[PS_MAPTO] = "Mapear para";
  txt[PS_PRESSWHENCENTRED] = "Premir quando centrado";
  txt[PS_PRESSWHENLEFTORDOWN] = "Premir quando esquerda ou baixo";
  txt[PS_PRESSWHENRIGHTORUP] = "Premir quando direita ou cima";
  txt[PS_DEADZONE] = "Zona neutra";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Limpar as definições do joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Iniciar a calibração do joystick (Windows)";
  txt[PS_LOADOPTIONS] = "Carregar opções";
  txt[PS_SAVEOPTIONS] = "Salvar opções";
  txt[PS_DELETEOPTIONS] = "Apagar opções";
  txt[PS_LOADSCENERY] = "Carregar cenário";
  txt[PS_SAVESCENERY] = "Salvar cenário";
  txt[PS_DELETESCENERY] = "Apagar cenário";
  txt[PS_LOADOBJECTS] = "Carregar objetos";
  txt[PS_SAVEOBJECTS] = "Guardar objetos";
  txt[PS_DELETEOBJECTS] = "Apagar objetos";
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
  txt[PS_LOADAICONTROLLERS] = "Carregar controladores IA";
  txt[PS_SAVEAICONTROLLERS] = "Guardar controladores IA";
  txt[PS_DELETEAICONTROLLERS] = "Apagar controladores IA";
  txt[PS_SELECTPANORAMA] = "Selecionar panorama";
  txt[PS_SELECTTERRAINFILE] = "Selecionar ficheiro de terreno";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selecionar o controlador preferido";
  txt[PS_SELECTOBJECTSSETTINGS] = "Selecionar definições de objetos";
  txt[PS_SUMMARY] = "Resumo";
  txt[PS_NOOBJECTS] = "Não há objetos carregados (além do terreno, etc). Pode carregar um conjunto de objetos estáticos e dinâmicos, "
    "certificando-se de que são adequados para a cena atual. Ao ativar a edição de objetos, pode "
    "criar os seus próprios - consulte a ajuda e a página web do PicaSim para mais informações.";
  txt[PS_OBJECTSTOTAL] = "Número total de objetos:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Número estáticos visíveis:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Número estáticos invisíveis:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Número dinâmicos visíveis:";
  txt[PS_ABOUT] = "Sobre";
  txt[PS_WEBSITE] = "Página de Internet";
  txt[PS_FLYING] = "Para voar";
  txt[PS_SETTINGS] = "Definições";
  txt[PS_RACES] = "Provas";
  txt[PS_TIPS] = "Dicas";
  txt[PS_KEYBOARD] = "Teclado";
  txt[PS_CREDITS] = "Créditos";
  txt[PS_LICENCE] = "Licença";
  txt[PS_VERSIONS] = "Versões";
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
  txt[PS_TIPSTEXT] = "Aqui estão algumas dicas para tirar o máximo partido do PicaSim:\n"
"\n"
"• Lembre-se que é um simulador de aviões radiocontrolados - o foco principal é no controlo realista, com o \"piloto\" de pé "
"no solo. Aprender a voar é uma habilidade que requer prática - consulte o site abaixo para ajuda.\n"
"\n"
"• Experimente as definições, especialmente se for piloto R/C e tiver preferências! Se estragar alguma coisa, imediatamente após "
"iniciar o PicaSim, vá às definições e carregue uma predefinição para as secções relevantes.\n"
"\n"
"• Se tiver problemas ou sugestões, envie email para picasimulator@gmail.com\n"
"\n"
"• Os panoramas podem ser exibidos com detalhe \"normal\" e \"máximo\" - vale a pena experimentar o nível de detalhe superior, "
"mas se o PicaSim falhar ao carregar cenários panorâmicos, tente reduzir o detalhe em Definições»Opções 1.\n"
"\n"
"• Os aviões controlados por IA são configurados em dois lugares. Definições»Opções 2 permite definir o número máximo de aviões IA "
"que o seu dispositivo executará. Definições»Controladores IA permite configurar quais aviões serão realmente carregados.\n";
  txt[PS_KEYBOARDTEXT] = "Com um teclado, existem alguns atalhos:\n"
"\n"
"[r] - Relançar\n"
"[p] - Alternar pausa\n"
"[c] - Percorrer as vistas da câmara\n"
"[z] - Alternar vista zoom\n";
  txt[PS_JOYSTICKSETUPTEXT] = "O PicaSim suporta joysticks/gamepads USB externos no Android 3.0 e superior, e no Windows. Em dispositivos Android, "
"isto requer um conector USB OTG (On The Go), e o seu dispositivo precisa de o suportar - por favor verifique isso primeiro. "
"Se o separador joystick aparecer nos menus de definições, há boas hipóteses de o seu dispositivo suportar controladores USB externos, "
"mas não é garantido!\n"
"\n"
"O controlador externo precisa de estar ligado antes de executar o PicaSim, e no Windows é recomendado calibrar o joystick usando "
"o botão \"calibrar\" em Definições»Joystick.\n"
"\n"
"Se o seu joystick for detetado e selecionado em Definições»Joystick, terá de atribuir entradas físicas aos controlos reconhecidos pelo PicaSim. "
"Isto é feito identificando qual eixo do joystick corresponde a qual entrada de controlo, e depois há botões para premir quando o stick está "
"no centro, à esquerda ou em baixo, e à direita ou em cima. Pode então verificar que os movimentos do stick correspondem às saídas corretas. "
"Este procedimento é o mesmo no Windows e Android - visite o site abaixo para um vídeo que descreve isto em mais detalhe.";
  txt[PS_CREDITSTEXT] = "O PicaSim foi desenvolvido por Danny Chapman.\n\n\n"
"Com agradecimentos a:\n\n"
"Heidi e Hazel pela sua paciência\n\n"
"Heidi, Jenne, André e Sylvain pelas traduções\n\n"
"Bullet pela física\n\n"
"TinyXML\n\n"
"SDL\n\n"
"OpenAL\n\n"
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
  txt[PS_LICENCETEXT] = "O PicaSim está disponível nas seguintes versões:\n\n"
"Gratuita: O simulador e recursos (incluindo aviões e cenários, etc.) podem ser distribuídos de acordo com a licença Creative Commons Attribution-NonCommercial-NoDerivs.\n\n"
"Paga: Esta versão ou os seus derivados só podem ser distribuídos com a minha autorização explícita.\n\n"
"Se desejar partilhar conteúdo que criou, deve fazê-lo separadamente da distribuição do PicaSim.\n\n"
"Os recursos do PicaSim podem ser distribuídos de acordo com a licença Creative Commons Attribution-NonCommercial (ou seja, pode modificá-los e distribuí-los, desde que credite a fonte).\n\n"
"É bastante fácil distribuir conteúdo para o PicaSim colocando-o em UserData e UserSettings. Consulte o site e pergunte no fórum para mais detalhes.";
  txt[PS_TIPS1] = "Por favor, envie-me a sua opinião para que possa melhorar o PicaSim";
  txt[PS_TIPS2] = "Se gosta do PicaSim, por favor, deixe um comentário/classificação";
  txt[PS_TIPS3] = "Pode usar o acelerómetro para controlar o modelo - em Definições»Controlador";
  txt[PS_TIPS4] = "Gosto/seguir PicaSim no Facebook ou no Google+ para ser notificado das atualizações e planos";
  txt[PS_TIPS5] = "Se gosta do PicaSim, por favor deixe uma avaliação nas versões móveis";
  txt[PS_TIPS6] = "Alterne entre modo 1 e 2 etc nas opções";
  txt[PS_TIPS7] = "Por favor envie-me feedback para que eu possa melhorar o PicaSim";
  txt[PS_TIPS8] = "Se a taxa de frames estiver baixa, tente carregar uma definição de qualidade inferior nas opções, e/ou reduza a definição de qualidade física";
  txt[PS_TIPS9] = "Em dispositivos móveis, tente usar o acelerómetro carregando uma predefinição adequada em Definições»Controlador";
  txt[PS_TIPS10] = "Adicionar lastro aos planadores aumenta a sua capacidade de penetrar no vento, ao custo de uma descida mais rápida";
  txt[PS_TIPS11] = "Ative o modo 'caminhada' em cenas 3D, e use o botão de caminhada quando em pausa para mover o ponto de lançamento e direção do vento";
  txt[PS_TIPS12] = "Falcões a circular indicam áreas de ascendência térmica, mas na verdade é melhor observar a ponta da asa ou o nariz do planador a levantar";
  txt[PS_TIPS13] = "Pilotar aviões R/C reais é ainda mais divertido!";
  txt[PS_TIPS14] = "Siga o PicaSim no Facebook ou Google+ para ser notificado de atualizações e planos";
  txt[PS_TIPS15] = "A versão completa do PicaSim contém aviões, cenários e desafios adicionais";
  txt[PS_TIPS16] = "Adicione aviões controlados por IA em Definições»Controladores IA. Limite o número máximo em Definições»Opções 2";
  txt[PS_LOADING] = "A carregar...";
  txt[PS_RACETIP] = "%s\n\nThe white arrow points to the next checkpoint.";
  txt[PS_LIMBOTIP] = "%s\n\nAdjust the difficulty/score multipler in Settings»Options 2";
  txt[PS_DURATIONTIP] = "%s\n\nLook for thermals and land close to the launch point.";
  txt[PS_TRAINERGLIDERTIP] = "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador para controlar o leme de profundidade e o leme de direcção, quando o planador estiver a subir em frente a uma encosta. Verificar na página de internet  algumas dicas sobre como voar, no caso de considerar difícil mantê-lo no ar.";
  txt[PS_TRAINERPOWEREDTIP] = "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador à direita para controlar o leme de profundidade e os ailerons, e o da esquerda para controlar o motor e o leme de direcção.";
  txt[PS_SELECTRACE] = "Selecionar prova";
  txt[PS_PREV] = "Anterior";
  txt[PS_NEXT] = "Próximo";
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
  txt[PS_VRHEADSET] = "Headset VR";
  txt[PS_ENABLEVRMODE] = "Ativar modo VR";
  txt[PS_HEADSET] = "Headset";
  txt[PS_VRWORLDSCALE] = "Escala do mundo";
  txt[PS_NOTHING] = "Nada";
  txt[PS_LEFTEYE] = "Olho esquerdo";
  txt[PS_RIGHTEYE] = "Olho direito";
  txt[PS_BOTHEYES] = "Ambos os olhos";
  txt[PS_NORMALVIEW] = "Vista normal";
  txt[PS_VRDESKTOP] = "Desktop VR";
  txt[PS_VRANTIALIASING] = "Anti-Aliasing VR";
  txt[PS_NONEUSEDEFAULT] = "Nenhum (usar padrão)";
  txt[PS_VRAUDIO] = "Áudio VR";
  txt[PS_STATUS] = "Estado";
  txt[PS_VRNOTAVAILABLE] = "VR não disponível - nenhum headset detetado";
  txt[PS_VRPANORAMADEPTH] = "Profundidade panorama VR";
  txt[PS_VRSKYDISTANCE] = "Distância do céu (m)";
  txt[PS_VRPARALLAXSCALE] = "Escala de paralaxe";
  txt[PS_ORIENTATION] = "Orientação";
  txt[PS_SMOKESOURCE] = "Fonte de fumo %d";
  txt[PS_NAME] = "Nome";
  txt[PS_BUTTONMAPPINGS] = "Mapeamento de botões";
  txt[PS_USERFILES] = "Ficheiros do utilizador:";
  txt[PS_NOFILESFOUND] = "Nenhum ficheiro encontrado";
}

