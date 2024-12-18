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
  txt[PS_CRASHED] = "Danificado";
  txt[PS_FREEFLY] = "Voo Livre";
  txt[PS_RACE] = "Prova";
  txt[PS_LIMBO] = "Limbo";
  txt[PS_DONATEMSG] = "Versão Completa"; 

  txt[PS_SELECTSCENARIO] = "Cenário";
  txt[PS_SELECTAEROPLANE] = "Modelo";
  txt[PS_SELECTSCENERY] = "Cenário";
  txt[PS_SELECTOPTIONS] = "Por favor seleccione um conjunto adequado de opções padrão";
  txt[PS_USEDEFAULT] = "Usar padrão";
  txt[PS_USEDEFAULTPREVIOUS] = "Usar padrão/anterior";

  txt[PS_BACK] = "Anterior";
  txt[PS_OPTIONS1] = "Opções";
  txt[PS_OPTIONS2] = "Modo";
  txt[PS_AEROPLANE] = "Modelo";
  txt[PS_SCENERY] = "Cenário";
  txt[PS_LIGHTING] = "Iluminação";
  txt[PS_CONTROLLER] = "Controlador";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Carregar...";
  txt[PS_SAVE] = "Salvar...";
  txt[PS_DELETE] = "Apagar...";
  txt[PS_ADVANCED] = "Avançado";
  txt[PS_SIMPLE] = "Simples";
  txt[PS_FILENAME] = "Nome do ficheiro:";
  txt[PS_INFULLVERSION] = "Na Versão Completa";

  //txt[PS_] = "";


  txt[PS_ABOUT] = "Sobre";
  txt[PS_WEBSITE] = "Página de Internet";
  txt[PS_FLYING] = "Para voar";
  txt[PS_SETTINGS] = "Definições";
  txt[PS_RACES] = "Provas";
  txt[PS_CREDITS] = "Créditos";
  txt[PS_LICENCE] = "Licença";
  txt[PS_VERSIONS] = "Versões";

  txt[PS_ABOUTFREETEXT] = 
"PicaSim é um simulador de modelos rádio-controlados.  O objectivo é ajudá-lo a aprender a voar "
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
"agradeço profundamente!\n"
;

  txt[PS_ABOUTPAIDTEXT] = 
"PicaSim é um simulador de modelos rádio-controlados.  O objectivo é ajudá-lo a aprender a voar "
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
"O pequeno valor da compra será usado para me ajudar a fazer isso, e será muito apreciado!\n"
;

  txt[PS_HOWTOFLYTEXT] = 
"A primeira coisa a fazer é visitar o site (link abaixo) e ver o vídeo \"How to Fly\". \n"
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
"tal como um modelo real!"
;

  txt[PS_HOWTOCONFIGURETEXT] = 
"Pode aceder às configurações do PicaSim, no menu inicial , ou usando um dos botões, quando em pausa.\n"
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
"Com paisagens panorâmicas não é possível mudar o ponto de vista, por isso, o modo de caminhada está desactivado."
;

  txt[PS_HOWTORACETEXT] = 
"Nos modos de prova o objectivo é voar tão rápido quanto possível entre postos de controle. O próximo ponto para o qual "
"tem de prosseguir, é realçado através da côr, sendo também indicado  pela seta branca na parte inferior do ecrã.\n"
"\n"
"Existem diferentes tipos de postos de controle: nas corridas de estilo F3F, onde tem que concluir várias voltas, "
"tem de voar para além do posto de controle, contra o vento do mesmo. Na corrida de cross-country, só precisa passar perto "
"do posto de controle (embora a altitude seja aquela que desejar). Na corrida Flatland, a ideia é voar entre duas portas "
"- a porta contra o vento ascendente é verde, e a do descendente é vermelha.\n"
;

  txt[PS_CREDITSTEXT] = 
"O PicaSim foi desenvolvido por Danny Chapman.\n\n\n"
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
"Faça isso!"
;

  txt[PS_TIPS1] = "Por favor, envie-me a sua opinião para que possa melhorar o PicaSim";
  txt[PS_TIPS2] = "Se gosta do PicaSim, por favor, deixe um comentário/classificação";
  txt[PS_TIPS3] = "Pode usar o acelerómetro para controlar o modelo - em Definições»Controlador";
  txt[PS_TIPS4] = "Gosto/seguir PicaSim no Facebook ou no Google+ para ser notificado das atualizações e planos";
  txt[PS_ALLOWCHOICE] = "Permitir a escolha do modelo/cenário na próxima vez";
  txt[PS_LOADING] = "A carregar...";

  txt[PS_TRAINERGLIDERTIP] = 
    "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador para controlar o leme de profundidade e o leme de direcção, quando o planador estiver a subir em frente a uma encosta. Verificar na página de internet  algumas dicas sobre como voar, no caso de considerar difícil mantê-lo no ar.";
  txt[PS_TRAINERPOWEREDTIP] = 
    "Carregar no botão “começar” para lançar, ou “pausa/voltar ao início” para recomeçar. Usar o controlador à direita para controlar o leme de profundidade e os ailerons, e o da esquerda para controlar o motor e o leme de direcção.";

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
  txt[PS_RETRIEVINGLEADERBOARD] = "Recuperando tabela de pontos...";
  txt[PS_SETUSERNAME] = "Definir nome de utilizador";
  txt[PS_SETEMAIL] = "Definir email";
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

// Everything below here is for settings
  txt[PS_SELECTLANGUAGE] = "Seleccione o idioma";
  txt[PS_LANGUAGESETTINGS] = "Definições de idioma";
  txt[PS_CURRENTLANGUAGE] = "Idioma atual";
  txt[PS_INFREEFLYMODE] = "Em modo livre";
  txt[PS_CAMERASETTINGS] = "Configurações da câmara";
  txt[PS_ZOOMVIEW] = "Zoom";
  txt[PS_ZOOMVIEWSIZE] = "Tamanho do Zoom";
  txt[PS_GROUNDAUTOZOOM] = "Terreno Auto-Zoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Escala do autozoom terreno";
  txt[PS_GROUNDFIELDOFVIEW] = "Campo de visão";
  txt[PS_GROUNDHORIZONAMOUNT] = "Quantidade de terreno no horizonte";
  txt[PS_GROUNDVIEWFOLLOW] = "Seguir modelo em visão de terreno";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Campo de visão do modelo";
  txt[PS_WALKABOUTSETTINGS] = "Definições caminhada";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Activar botão caminhada";
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
  txt[PS_STAGGERCONTROLLER] = "Controlador Stagger (para multitouch parcial)";
  txt[PS_SQUARECONTROLLERS] = "Controladores quadrados";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Deslocamento horizontal do limite";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Deslocamento vertical do limite";
  txt[PS_SHAPEOPACITY] = "Ajustar a transparência";
  txt[PS_STICKOPACITY] = "Transparência do stick";
  txt[PS_STICKCROSS] = "Stick cross";
  txt[PS_JOYSTICKID] = "ID do joystick";
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
  txt[PS_GRAPHAIRSPEED] = "Mostrar vel. vento (vermelho)";
  txt[PS_GRAPHGROUNDSPEED] = "Mostrar vel. solo (verde)";
  txt[PS_GRAPHCLIMBRATE] = "Mostrar nivel de subida (azul)";
  txt[PS_GRAPHWINDSPEED] = "Mostrar vel. vento (amar.)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Mostrar vel. vert. do vento (roxo)";
  txt[PS_STALLMARKERS] = "Marcas perda sustentação";
  txt[PS_AEROFOILINFO] = "Informações do Perfil de Asa";
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
  txt[PS_AEROPLANERENDER] = "Renderização do Modelo";
  txt[PS_DRAWSHADOWS] = "Desenhar sombras";
  txt[PS_USE16BIT] = "Usar text. 16bit reduz memória (reini. Picasim)";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Escala ilum. ambiente";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Escala iluminação difusa";
  txt[PS_TERRAINTEXTUREDETAIL] = "Detalhe text. terreno";
  txt[PS_NEARCLIPDISTANCE] = "Plano perto";
  txt[PS_FARCLIPDISTANCE] = "Plano distante";
  txt[PS_MISCSETTINGS] = "Configurações várias";
  txt[PS_USEBACKBUTTON] = "Utilize o botão anterior para sair";
  txt[PS_DRAWLAUNCHMARKER] = "Mostrar a marca de lançamento";
  txt[PS_DRAWGROUNDPOSITION] = "Mostrar a marca de terreno";
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Usar o controlador preferido do avião";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Definições teste/desenvol.";
  txt[PS_WIREFRAMETERRAIN] = "Terreno em wireframe";
  txt[PS_DRAWSUNPOSITION] = "Desenhar posição sol";
  txt[PS_FREEFLYSETTINGS] = "Configurações de voo livre";
  txt[PS_DISPLAYFLIGHTTIME] = "Mostrar tempo de voo";
  txt[PS_DISPLAYSPEED] = "Mostrar velocidade";
  txt[PS_DISPLAYMAXSPEED] = "Mostrar max velocidade";
  txt[PS_DISPLAYASCENTRATE] = "Mostrar taxa de ascenção";
  txt[PS_DISPLAYALTITUDE] = "Mostar altitude";
  txt[PS_DISPLAYDISTANCE] = "Mostar distância";
  txt[PS_COLOURTEXT] = "Colorir texto taxa ascenção";
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
  txt[PS_LAUNCH] = "Lançar";
  txt[PS_LAUNCHSPEED] = "Vel. lançamento";
  txt[PS_LAUNCHANGLE] = "Âng. de lançamento";
  txt[PS_LAUNCHUP] = "Lançar para cima";
  txt[PS_LAUNCHFORWARD] = "Lançar em frente";
  txt[PS_LAUNCHLEFT] = "Lançar em esquerda";
  txt[PS_RELAUNCHWHENSTATIONARY] = "Relançar quando parado";
  txt[PS_AIRFRAME] = "Fuselagem";
  txt[PS_UNDERCARRIAGE] = "Chassis";
  txt[PS_PROPELLER] = "Hélice";
  txt[PS_MAXBUNGEELENGTH] = "Tam. máx. guincho";
  txt[PS_MAXBUNGEEACCEL] = "Acel. máx. guincho";
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
  txt[PS_CHASECAMERA] = "Câmara de perseguição";
  txt[PS_DISTANCE] = "Distância";
  txt[PS_HEIGHT] = "Altitude";
  txt[PS_VERTICALVELFRAC] = "Vertical vel frac";
  txt[PS_INFO] = "Info";
  txt[PS_CURRENTPOSITION] = "Posição actual";
  txt[PS_MASS] = "Peso";

  txt[PS_WINDSETTINGS] = "Configurações do vento";
  txt[PS_WINDSPEED] = "Velocidade do vento";
  txt[PS_WINDBEARING] = "Direção do vento";
  txt[PS_CANNOTMODIFYSCENERY] = "Não se pode modificar a cenário durante a prova";
  txt[PS_ALLOWBUNGEE] = "Permitir o lançamento por guincho";
  txt[PS_WINDGUSTTIME] = "Periodicidade rajadas";
  txt[PS_WINDGUSTFRACTION] = "Fracionar rajadas";
  txt[PS_WINDGUSTANGLE] = "Ângulo rajadas";
  txt[PS_TURBULENCEAMOUNT] = "Intens. turbulência";
  txt[PS_WINDLIFTSMOOTHING] = "Suavização do vento ascendente";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Tendência de separação";
  txt[PS_DEADAIRTURBULENCE] = "Dead air turbulence";
  txt[PS_THERMALSETTINGS] = "Configurações térmicas (veja também a iluminação)";
  txt[PS_DENSITY] = "Densidade";
  txt[PS_RANGE] = "Alcance";
  txt[PS_LIFESPAN] = "Tempo de vida";
  txt[PS_DEPTH] = "Profundidade";
  txt[PS_CORERADIUS] = "Core radius";
  txt[PS_DOWNDRAFTEXTENT] = "Downdraft extent";
  txt[PS_UPDRAFTSPEED] = "Updraft speed";
  txt[PS_ASCENTRATE] = "Taxa de súbida";
  txt[PS_RUNWAY] = "Pista";
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
  txt[PS_MAPTO] = "Mapear para";
  txt[PS_DEADZONE] = "Zona neutra";
  txt[PS_CLEARJOYSTICKSETTINGS] = "Limpar as definições do joystick";
  txt[PS_CALIBRATEJOYSTICK] = "Iniciar a calibração do joystick (Windows)";

  txt[PS_LOADOPTIONS] = "Carregar opções";
  txt[PS_SAVEOPTIONS] = "Salvar opções";
  txt[PS_DELETEOPTIONS] = "Apagar opções";
  txt[PS_LOADSCENERY] = "Carregar cenário";
  txt[PS_SAVESCENERY] = "Salvar cenário";
  txt[PS_DELETESCENERY] = "Apagar cenário";
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
  txt[PS_SELECTPANORAMA] = "Selecionar panorama";
  txt[PS_SELECTTERRAINFILE] = "Selecionar ficheiro de terreno";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Selecionar o controlador preferido";

  // These, starting with PM_, must have no accented characters
  txt[PS_ONERACE] = 
    "A versao livre deste simulador so contem uma prova. Considere comprar a versao completa."
    "A mesma tem mais provas, modelos, cenarios e ira suportar o futuro desenvolvimento do PicaSim.";
  txt[PS_INVALIDRACE] = "Prova invalida para a versao livre do PicaSim";
  txt[PS_LEADERBOARDFAILED] = "Falhou a leitura da tabela - verifique a ligacao a internet";
  txt[PS_LEADERBOARDFAILEDRETRYING] = "Falhou a leitura da tabela , vou tentar outra vez...";
  txt[PS_SCORESUBMISSIONFAILED] = "Nao consegui enviar os resultados vou tentar outra vez - verifique a ligacao a internet";
  txt[PS_SCORESUBMISSIONFAILEDRETRYING] = "Nao consegui enviar os resultados vou tentar outra vez...";
  txt[PS_USERFAILED] = "O pedido do utilizador falhou , vou tentar outra vez - verifique a ligacao a internet";
  txt[PS_USERFAILEDRETRYING] = "O pedido do utilizador falhou , vou tentar outra vez...";

  txt[PS_RESTARTAFTERCALIBRATION] = "You may need to restart PicaSim after joystick calibration";

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
  txt[PS_TRAINERGLIDERTIP] = 
    "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower corner to control the elevator and rudder, flying the glider in the lift in front of the hill.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
  txt[PS_TRAINERPOWEREDTIP] = 
    "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower right corner to control elevator and ailerons. The controls on the left are for throttle and rudder.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
}



