#ifndef FRAMEWORK_STRINGS_H
#define FRAMEWORK_STRINGS_H

#include "FrameworkSettings.h"

#define TXT(tag) GetPS(tag, language)

enum PicaString
{
  PS_LANGUAGE_EN,
  PS_LANGUAGE_NATIVE,
// General strings
  PS_OK,
  PS_ENABLE,
  PS_REMOVE,
  PS_COPY,
  PS_CRASHED,
  PS_NO,
  PS_YES,
  PS_INVERT,
  PS_AUTO,
// Start menu
  PS_FREEFLY,
  PS_RACE,
  PS_LIMBO,
  PS_DONATEMSG,
  PS_INTRIALMODE,
  PS_GAMEPADDETECTED,
  PS_WHATSNEW,
  PS_WHATSNEWTEXT,
// Prompts
  PS_ONERACE,
  PS_FULLVERSION,
  PS_SELECTAEROPLANE,
  PS_SELECTSCENERY,
  PS_SELECTOPTIONS,
  PS_SELECTSCENARIO,
  PS_USEDEFAULT,
  PS_USEDEFAULTPREVIOUS,
  PS_INVALIDRACE,

// Settings menu
  PS_BACK,
  PS_OPTIONS1,
  PS_OPTIONS2,
  PS_AEROPLANE,
  PS_SCENERY,
  PS_OBJECTS,
  PS_OBJECTSSETTINGS,
  PS_LIGHTING,
  PS_AICONTROLLERS,
  PS_CONTROLLER,
  PS_JOYSTICK,
  PS_LOAD,
  PS_SAVE,
  PS_DELETE,
  PS_ADVANCED,
  PS_CLEARALLSAVEDSETTINGSANDEXIT,
  PS_CONFIRMCLEARALLSETTINGS,
  PS_SIMPLE,
  PS_FILENAME,
  PS_INFULLVERSION,
  PS_PURCHASE,
  PS_PURCHASESUCCESSFUL,
  PS_PURCHASECANCELLED,
  PS_PURCHASEERROR,
  PS_PURCHASEALREADY,
  PS_PURCHASEFROMSTORE,
  PS_NOTNOW,
  PS_CANCEL,
  PS_TRIALOVER,
  // Settings->Options
  PS_SELECTLANGUAGE,
  PS_LANGUAGESETTINGS,
  PS_CURRENTLANGUAGE,
  PS_INFREEFLYMODE,
  PS_CAMERASETTINGS,
  PS_ZOOMVIEW,
  PS_PLANEONLYINZOOMVIEW,
  PS_SMOKEONLYINMAINVIEW,
  PS_ZOOMVIEWSIZE,
  PS_GROUNDAUTOZOOM,
  PS_GROUNDAUTOZOOMSCALE,
  PS_GROUNDFIELDOFVIEW,
  PS_GROUNDHORIZONAMOUNT,
  PS_GROUNDLAG,
  PS_GROUNDVIEWFOLLOW,
  PS_GROUNDVIEWYAWOFFSET,
  PS_GROUNDVIEWPITCHOFFSET,
  PS_AEROPLANEFIELDOFVIEW,
  PS_STEREOSCOPY,
  PS_STEREOSEPARATION,
  PS_STEREOINFO,
  PS_WALKABOUTSETTINGS,
  PS_ENABLEWALKABOUTBUTTON,
  PS_ENABLEOBJECTEDITING,
  PS_FORCEALLVISIBLE,
  PS_RESETOBJECTS,
  PS_OBJECTEDITING,
  PS_OBJECTEDITINGTEXT,
  PS_OBJECTNUMBER,
  PS_POSITIONX,
  PS_POSITIONY,
  PS_POSITIONZ,
  PS_COLOURR,
  PS_COLOURG,
  PS_COLOURB,
  PS_COLOURH,
  PS_COLOURS,
  PS_COLOURV,
  PS_POSITION,
  PS_SIZE,
  PS_COLOUR,
  PS_VISIBLE,
  PS_SHADOW,
  PS_SETWINDDIRONWALKABOUT,
  PS_SIMULATIONSETTINGS,
  PS_TIMESCALE,
  PS_PHYSICSACCURACY,
  PS_CONTROLLERSETTINGS,
  PS_MODE1DESCRIPTION,
  PS_MODE2DESCRIPTION,
  PS_MODE3DESCRIPTION,
  PS_MODE4DESCRIPTION,
  PS_CONTROLLERSIZE,
  PS_BRAKESFORWARD,
  PS_USEABSOLUTECONTROLLERTOUCHPOSITION,
  PS_STAGGERCONTROLLER,
  PS_ENABLEELEVATORTRIM,
  PS_ELEVATORTRIMSIZE,
  PS_SQUARECONTROLLERS,
  PS_HORIZONTALOFFSETFROMEDGE,
  PS_VERTICALOFFSETFROMEDGE,
  PS_SHAPEOPACITY,
  PS_STICKOPACITY,
  PS_STICKCROSS,
  PS_JOYSTICKID,
  PS_JOYSTICKINFO,
  PS_NOJOYSTICK,
  PS_AUDIOSETTINGS,
  PS_OVERALLVOLUME,
  PS_VARIOMETERVOLUME,
  PS_WINDVOLUME,
  PS_OUTSIDEAEROPLANEVOLUME,
  PS_INSIDEAEROPLANEVOLUME,
  PS_ONSCREENDISPLAYSETTINGS,
  PS_WINDARROWSIZE,
  PS_WINDARROWOPACITY,
  PS_PAUSEBUTTONSSIZE,
  PS_PAUSEBUTTONOPACITY,
  PS_INFORMATIONSETTINGS,
  PS_MAXMARKERSPERTHERMAL,
  PS_THERMALWINDFIELD,
  PS_GRAPHAIRSPEED,
  PS_GRAPHDURATION,
  PS_GRAPHALTITUDE,
  PS_GRAPHGROUNDSPEED,
  PS_GRAPHCLIMBRATE,
  PS_GRAPHWINDSPEED,
  PS_GRAPHWINDVERTICALVELOCITY,
  PS_STALLMARKERS,
  PS_AEROFOILPLOT,
  PS_AEROFOILINFO,
  PS_AEROFOILRANGE,
  PS_AEROFOILCLMAX,
  PS_AEROFOILCDMAX,
  PS_AEROFOILCMMAX,
  PS_AEROFOILLDMAX,
  PS_AEROFOILPLOTREFERENCE,
  PS_AEROFOILPOLAR,
  PS_DRAWAEROPLANECOM,
  PS_GRAPHFPS,
  PS_NUMWINDSTREAMERS,
  PS_WINDSTREAMERTIME,
  PS_WINDSTREAMERDELTAZ,
  PS_GRAPHICSSETTINGS,
  PS_GROUNDTERRAINLOD,
  PS_AEROPLANETERRAINLOD,
  PS_UPDATETERRAINLOD,
  PS_COMPONENTS,
  PS_3DMODEL,
  PS_BOTH,
  PS_CONTROLLEDPLANESHADOWS,
  PS_OTHERSHADOWS,
  PS_PROJECTEDSHADOWDETAIL,
  PS_BLOB,
  PS_PROJECTED,
  PS_AEROPLANERENDER,
  PS_DRAWSHADOWS,
  PS_USE16BIT,
  PS_SEPARATESPECULAR,
  PS_AMBIENTLIGHTINGSCALE,
  PS_DIFFUSELIGHTINGSCALE,
  PS_TERRAINTEXTUREDETAIL,
  PS_MAXSKYBOXDETAIL,
  PS_NEARCLIPDISTANCE,
  PS_FARCLIPDISTANCE,
  PS_SHADINGMETHOD,
  PS_USEMULTILIGHTS,
  PS_ENABLESMOKE,
  PS_SMOKEQUALITY,
  PS_MISCSETTINGS,
  PS_USEBACKBUTTON,
  PS_DRAWLAUNCHMARKER,
  PS_DRAWGROUNDPOSITION,
  PS_SKYGRIDOVERLAY,
  PS_SKYGRID_NONE,
  PS_SKYGRID_SPHERE,
  PS_SKYGRID_BOX,
  PS_SKYGRIDALIGNMENT,
  PS_SKYGRIDDISTANCE,
  PS_SKYGRIDALIGN_ALONGWIND,
  PS_SKYGRIDALIGN_CROSSWIND,
  PS_SKYGRIDALIGN_ALONGRUNWAY,
  PS_SKYGRIDALIGN_CROSSRUNWAY,
  PS_USEAEROPLANEPREFERREDCONTROLLER,
  PS_TESTINGDEVELOPERSETTINGS,
  PS_WIREFRAMETERRAIN,
  PS_DRAWSUNPOSITION,
  PS_FREEFLYSETTINGS,
  PS_DISPLAYFLIGHTTIME,
  PS_DISPLAYSPEED,
  PS_DISPLAYAIRSPEED,
  PS_DISPLAYMAXSPEED,
  PS_DISPLAYASCENTRATE,
  PS_DISPLAYALTITUDE,
  PS_DISPLAYDISTANCE,
  PS_COLOURTEXT,
  PS_TEXTATTOP,
  PS_FREEFLYONSTARTUP,
  PS_ENABLESOCKETCONTROLLER,
  PS_TEXTBACKGROUNDOPACITY,
  PS_TEXTBACKGROUNDCOLOUR,
  PS_MAXNUMBERAI,
  PS_TOOMANYAI,
  PS_UNITS,
  PS_RACESETTINGS,
  PS_RACEVIBRATIONAMOUNT,
  PS_RACEBEEPVOLUME,
  PS_LIMBODIFFICULTY,
  PS_ENABLESCORELOOP,
  PS_DELETELOCALHIGHSCORES,
// Settings->aeroplane
  PS_GENERALSETTINGS,
  PS_COLOURSCHEME,
  PS_COLOUROFFSET,
  PS_BALLAST,
  PS_BALLASTFWD,
  PS_BALLASTLEFT,
  PS_BALLASTUP,
  PS_DRAGMULTIPLIER,
  PS_SIZEMULTIPLIER,
  PS_MASSMULTIPLIER,
  PS_ENGINEMULTIPLIER,
  PS_HASVARIOMETER,
  PS_PREFERREDCONTROLLER,
  PS_SHOWBUTTON1,
  PS_SHOWBUTTON2,
  PS_LAUNCH,
  PS_HOOKS,
  PS_BUNGEELAUNCH,
  PS_AEROTOWLAUNCH,
  PS_FLATLAUNCHMETHOD,
  PS_HAND,
  PS_BUNGEE,
  PS_AEROTOW,
  PS_LAUNCHSPEED,
  PS_LAUNCHANGLE,
  PS_LAUNCHUP,
  PS_LAUNCHFORWARD,
  PS_LAUNCHLEFT,
  PS_LAUNCHOFFSETUP,
  PS_RELAUNCHWHENSTATIONARY,
  PS_CRASHDETECTION,
  PS_CRASHDELTAVELX,
  PS_CRASHDELTAVELY,
  PS_CRASHDELTAVELZ,
  PS_CRASHDELTAANGVELX,
  PS_CRASHDELTAANGVELY,
  PS_CRASHDELTAANGVELZ,
  PS_CRASHSUSPENSIONFORCESCALE,
  PS_AIRFRAME,
  PS_UNDERCARRIAGE,
  PS_PROPELLER,
  PS_BELLYHOOKOFFSETFORWARD,
  PS_BELLYHOOKOFFSETLEFT,
  PS_BELLYHOOKOFFSETUP,
  PS_NOSEHOOKOFFSETFORWARD,
  PS_NOSEHOOKOFFSETLEFT,
  PS_NOSEHOOKOFFSETUP,
  PS_TAILHOOKOFFSETFORWARD,
  PS_TAILHOOKOFFSETLEFT,
  PS_TAILHOOKOFFSETUP,
  PS_MAXBUNGEELENGTH,
  PS_MAXBUNGEEACCEL,
  PS_TUGPLANE,
  PS_TUGSIZESCALE,
  PS_TUGMASSSCALE,
  PS_TUGENGINESCALE,
  PS_TUGMAXCLIMBSLOPE,
  PS_TUGTARGETSPEED,
  PS_AEROTOWROPELENGTH,
  PS_AEROTOWROPESTRENGTH,
  PS_AEROTOWROPEMASSSCALE,
  PS_AEROTOWROPEDRAGSCALE,
  PS_AEROTOWHEIGHT,
  PS_AEROTOWCIRCUITSIZE,
  PS_TETHERING,
  PS_TETHERLINES,
  PS_TETHERREQUIRESTENSION,
  PS_TETHERPHYSICSOFFSETFORWARD,
  PS_TETHERPHYSICSOFFSETLEFT,
  PS_TETHERPHYSICSOFFSETUP,
  PS_TETHERVISUALOFFSETFORWARD,
  PS_TETHERVISUALOFFSETLEFT,
  PS_TETHERVISUALOFFSETUP,
  PS_TETHERDISTANCELEFT,
  PS_TETHERCOLOURR,
  PS_TETHERCOLOURG,
  PS_TETHERCOLOURB,
  PS_TETHERCOLOURA,
  PS_CHASECAMERA,
  PS_CAMERATARGETPOSFWD,
  PS_CAMERATARGETPOSUP,
  PS_DISTANCE,
  PS_HEIGHT,
  PS_VERTICALVELFRAC,
  PS_FLEXIBILITY,
  PS_COCKPITCAMERA,
  PS_PITCH,
  PS_ENABLEDEBUGDRAW,
  PS_CANTOW,
  PS_MAXAI,
  PS_ADDNEWAICONTROLLER,
  PS_REMOVEAICONTROLLERS,
  PS_LAUNCHSEPARATIONDISTANCE,
  PS_AVAILABLEINFULLVERSION,
  PS_INCLUDEINCAMERAVIEWS,
  PS_CREATEMAXNUMCONTROLLERS,
  PS_LAUNCHDIRECTION,
  PS_RANDOMCOLOUROFFSET,
  PS_AICONTROLLER,
  PS_AINAVIGATION,
  PS_PLANETYPE,
  PS_USER,
  PS_ALL,
  PS_GLIDER,
  PS_GLIDERS,
  PS_POWERED,
  PS_HELI,
  PS_CONTROLLINE,
  PS_PANORAMIC,
  PS_3D,
  PS_ALLOWAICONTROL,
  PS_WAYPOINTTOLERANCE,
  PS_MINSPEED,
  PS_CRUISESPEED,
  PS_MAXBANKANGLE,
  PS_BANKANGLEPERHEADINGCHANGE,
  PS_SPEEDPERALTITUDECHANGE,
  PS_GLIDESLOPEPEREXCESSSPEED,
  PS_PITCHPERROLLANGLE,
  PS_HEADINGCHANGEFORNOSLOPE,
  PS_MAXPITCHCONTROL,
  PS_MAXROLLCONTROL,
  PS_CONTROLPERROLLANGLE,
  PS_PITCHCONTROLPERGLIDESLOPE,
  PS_ROLLTIMESCALE,
  PS_PITCHTIMESCALE,
  PS_MINALTITUDE,
  PS_SLOPEMINUPWINDDISTANCE,
  PS_SLOPEMAXUPWINDDISTANCE,
  PS_SLOPEMINLEFTDISTANCE,
  PS_SLOPEMAXLEFTDISTANCE,
  PS_SLOPEMINUPDISTANCE,
  PS_SLOPEMAXUPDISTANCE,
  PS_SLOPEMAXWAYPOINTTIME,
  PS_FLATMAXDISTANCE,
  PS_FLATMAXWAYPOINTTIME,
  PS_INFO,
  PS_CURRENTPOSITION,
  PS_MASS,
  PS_INERTIA,
  PS_WINGAREA,
  PS_EXTENTS,
// Settings->scenery
  PS_WINDSETTINGS,
  PS_WINDSPEED,
  PS_WINDBEARING,
  PS_CANNOTMODIFYSCENERY,
  PS_ALLOWBUNGEE,
  PS_WINDGUSTTIME,
  PS_WINDGUSTFRACTION,
  PS_WINDGUSTANGLE,
  PS_TURBULENCEAMOUNT,
  PS_SURFACETURBULENCEAMOUNT,
  PS_SHEARTURBULENCEAMOUNT,
  PS_WINDLIFTSMOOTHING,
  PS_VERTICALWINDDECAYDISTANCE,
  PS_SEPARATIONTENDENCY,
  PS_ROTORTENDENCY,
  PS_DEADAIRTURBULENCE,
  PS_BOUNDARYLAYERDEPTH,
  PS_THERMALSETTINGS,
  PS_DENSITY,
  PS_RANGE,
  PS_LIFESPAN,
  PS_DEPTH,
  PS_CORERADIUS,
  PS_DOWNDRAFTEXTENT,
  PS_UPDRAFTSPEED,
  PS_ASCENTRATE,
  PS_THERMALEXPANSIONOVERLIFESPAN,
  PS_RUNWAY,
  PS_RUNWAYTYPE,
  PS_CIRCLE,
  PS_RUNWAYX,
  PS_RUNWAYY,
  PS_RUNWAYHEIGHT,
  PS_RUNWAYANGLE,
  PS_RUNWAYLENGTH,
  PS_RUNWAYWIDTH,
  PS_SURFACESETTINGS,
  PS_SURFACEROUGHNESS,
  PS_SURFACEFRICTION,
  PS_RANDOMTERRAINSETTINGS,
  PS_RANDOMSEED,
  PS_DISPLACEMENTHEIGHT,
  PS_SMOOTHNESS,
  PS_EDGEHEIGHT,
  PS_UPWARDSBIAS,
  PS_FILTERITERATIONS,
  PS_DRAWPLAIN,
  PS_COLLIDEWITHPLAIN,
  PS_PLAININNERRADIUS,
  PS_PLAINFOGDISTANCE,
  PS_PLAINHEIGHT,
  PS_TERRAINSIZE,
  PS_COASTENHANCEMENT,
  PS_TERRAINDETAIL,
  PS_RIDGETERRAINSETTINGS,
  PS_MAXHEIGHTFRACTION,
  PS_WIDTH,
  PS_HEIGHTOFFSET,
  PS_HORIZONTALVARIATION,
  PS_HORIZONTALWAVELENGTH,
  PS_VERTICALVARIATION,
  PS_PANORAMASETTINGS,
  PS_PANORAMA3DSETTINGS,
  PS_HEIGHTFIELDSETTINGS,
  PS_MINHEIGHT,
  PS_MAXHEIGHT,

  PS_AISCENERY,
  PS_SCENETYPE,
  PS_FLAT,
  PS_SLOPE,

  PS_CURRENTVIEWPOSITION,
// Settings->lighting
  PS_NOLIGHTINGSETTINGS,
  PS_SUNBEARING,
  PS_THERMALACTIVITY,
  PS_TERRAINDARKNESS,
// Settings->controller
  PS_CONTROLLERSEESETTINGS,
  PS_CROSS,
  PS_BOX,
  PS_CROSSANDBOX,
  PS_CONFIGURATION,
  PS_STYLE,
  PS_CONTROLLERMODE,
  PS_TREATTHROTTLEASBRAKES,
  PS_RESETALTSETTINGONLAUNCH,
  PS_NUMCONFIGURATIONS,
  PS_RIGHTSTICKHORIZONTAL,
  PS_RIGHTSTICKVERTICAL,
  PS_LEFTSTICKHORIZONTAL,
  PS_LEFTSTICKVERTICAL,
  PS_TILTHORIZONTAL,
  PS_TILTVERTICAL,
  PS_ARROWSHORIZONTAL,
  PS_ARROWSVERTICAL,
  PS_ROLLSTICK,
  PS_PITCHSTICK,
  PS_YAWSTICK,
  PS_SPEEDSTICK,
  PS_CONSTANT,
  PS_BUTTON0,
  PS_BUTTON1,
  PS_BUTTON2,
  PS_BUTTON0TOGGLE,
  PS_BUTTON1TOGGLE,
  PS_BUTTON2TOGGLE,
  PS_MIXES,
  PS_ELEVATORTOFLAPS,
  PS_AILERONTORUDDER,
  PS_FLAPSTOELEVATOR,
  PS_BRAKESTOELEVATOR,
  PS_RUDDERTOELEVATOR,
  PS_RUDDERTOAILERON,
  PS_RATESBUTTON,
  PS_RATESCYCLEBUTTON,
  PS_RELAUNCHBUTTON,
  PS_CAMERABUTTON,
  PS_PAUSEPLAYBUTTON,
  PS_NONE,
  PS_CONTROLSOURCES,
  PS_AILERONS,
  PS_ELEVATOR,
  PS_RUDDER,
  PS_THROTTLE,
  PS_LOOKYAW,
  PS_LOOKPITCH,
  PS_AUX1,
  PS_SMOKE1,
  PS_SMOKE2,
  PS_HOOK,
  PS_VELFWD,
  PS_VELLEFT,
  PS_VELUP,
  PS_MAXPARTICLES,
  PS_CHANNELFOROPACITY,
  PS_MINOPACITY,
  PS_MAXOPACITY,
  PS_CHANNELFORRATE,
  PS_MINRATE,
  PS_MAXRATE,
  PS_INITIALSIZE,
  PS_FINALSIZE,
  PS_DAMPINGTIME,
  PS_JITTER,
  PS_ENGINEWASH,
  PS_HUECYCLEFREQ,
  PS_SETTINGSFORCONTROLLER,
  PS_TRIMSETTINGS,
  PS_NOSIMPLESETTINGS,
  PS_TRIM,
  PS_SCALE,
  PS_CLAMPING,
  PS_EXPONENTIAL,
  PS_SPRING,
  PS_POSITIVE,
  PS_NEGATIVE,
  PS_ROLLSTICKMOVEMENT,
  PS_PITCHSTICKMOVEMENT,
  PS_YAWSTICKMOVEMENT,
  PS_SPEEDSTICKMOVEMENT,
  PS_ACCELEROMETERROLLMOVEMENT,
  PS_ACCELEROMETERROLL,
  PS_ACCELEROMETERPITCHMOVEMENT,
  PS_ACCELEROMETERPITCH,
  PS_TILTROLLSENSITIVITY,
  PS_TILTPITCHSENSITIVITY,
  PS_TILTNEUTRALANGLE,
// Settings->Joystick
  PS_NOJOYSTICKWITHID,
  PS_ENABLEJOYSTICK,
  PS_ADJUSTFORCIRCULARSTICKMOVEMENT,
  PS_EXTERNALJOYSTICKSETTINGS,
  PS_JOYSTICKLABEL,
  PS_JOYSTICKABUTTONLABEL,
  PS_JOYSTICKBUTTONLABEL,
  PS_SCALEPOSITIVE,
  PS_SCALENEGATIVE,
  PS_OFFSET,
  PS_MAPTO,
  PS_PRESSWHENCENTRED,
  PS_PRESSWHENLEFTORDOWN,
  PS_PRESSWHENRIGHTORUP,
  PS_DEADZONE,
  PS_CLEARJOYSTICKSETTINGS,
  PS_CALIBRATEJOYSTICK,
// Load/save
  PS_LOADOPTIONS,
  PS_SAVEOPTIONS,
  PS_DELETEOPTIONS,
  PS_LOADSCENERY,
  PS_SAVESCENERY,
  PS_DELETESCENERY,
  PS_LOADOBJECTS,
  PS_SAVEOBJECTS,
  PS_DELETEOBJECTS,
  PS_LOADLIGHTING,
  PS_SAVELIGHTING,
  PS_DELETELIGHTING,
  PS_LOADCONTROLLER,
  PS_SAVECONTROLLER,
  PS_DELETECONTROLLER,
  PS_LOADJOYSTICK,
  PS_SAVEJOYSTICK,
  PS_DELETEJOYSTICK,
  PS_LOADAEROPLANE,
  PS_SAVEAEROPLANE,
  PS_DELETEAEROPLANE,
  PS_LOADAICONTROLLERS,
  PS_SAVEAICONTROLLERS,
  PS_DELETEAICONTROLLERS,
  PS_SELECTPANORAMA,
  PS_SELECTTERRAINFILE,
  PS_SELECTPREFERREDCONTROLLER,
  PS_SELECTOBJECTSSETTINGS,

  PS_SUMMARY,
  PS_NOOBJECTS,
  PS_OBJECTSTOTAL,
  PS_OBJECTSSTATICVISIBLE,
  PS_OBJECTSSTATICINVISIBLE,
  PS_OBJECTSDYNAMICVISIBLE,

// Help menu
  PS_ABOUT,
  PS_WEBSITE,
  PS_FLYING,
  PS_SETTINGS,
  PS_RACES,
  PS_TIPS,
  PS_KEYBOARD,
  PS_CREDITS,
  PS_LICENCE,
  PS_VERSIONS,
  PS_ABOUTFREETEXT,
  PS_ABOUTPAIDTEXT,
  PS_HOWTOFLYTEXT,
  PS_HOWTOCONFIGURETEXT,
  PS_HOWTORACETEXT,
  PS_TIPSTEXT,
  PS_KEYBOARDTEXT,
  PS_JOYSTICKSETUPTEXT,
  PS_CREDITSTEXT,
  PS_LICENCETEXT,
  PS_VERSIONSTEXT,

// Loading
  PS_TIPS1,
  PS_TIPS2,
  PS_TIPS3,
  PS_TIPS4,
  PS_TIPS5,
  PS_TIPS6,
  PS_TIPS7,
  PS_TIPS8,
  PS_TIPS9,
  PS_TIPS10,
  PS_TIPS11,
  PS_TIPS12,
  PS_TIPS13,
  PS_TIPS14,
  PS_TIPS15,
  PS_TIPS16,
  PS_TIPS17,
  PS_TIPS18,
  PS_ALLOWCHOICE,
  PS_LOADING,
  PS_RACETIP,
  PS_LIMBOTIP,
  PS_DURATIONTIP,
  PS_TRAINERGLIDERTIP,
  PS_TRAINERPOWEREDTIP,
  PS_MOUSETOCONTINUE,
  PS_TAPTOCONTINUE,

// Scoreloop
  PS_ALLRESULTS,
  PS_LAST24HOURS,
  PS_LAST7DAYS,
  PS_LAST30DAYS,
  PS_TIMEPERIODFORRESULTS,
  PS_SCORELINERACE,
  PS_SCORELINELIMBO,
  PS_NOSCORES,
  PS_CURRENTSCORE,
  PS_NOSCORESINTIME,
  PS_LEADERBOARDFAILED,
  PS_LEADERBOARDFAILEDRETRYING,
  PS_RETRIEVINGLEADERBOARD,
  PS_SCORESUBMISSIONFAILED,
  PS_SCORESUBMISSIONFAILEDRETRYING,
  PS_RESTARTAFTERCALIBRATION,
  PS_SETUSERNAME,
  PS_SETEMAIL,
  PS_USERFAILED,
  PS_USERFAILEDRETRYING,
  PS_PROFILE,
  PS_USERNAME,
  PS_EMAIL,
  PS_LEADERBOARDS,
  PS_UPLOADINGSCORE,
  PS_REQUESTINGUSER,
  PS_SELECTRACE,
  PS_SELECTRACETOSEE,
  PS_EDIT,
  PS_PROFILESTATUS,
  PS_SCORELOOPUSERNAME,
  PS_EMAILADDRESS,
  PS_OPTIONALEMAIL,
  PS_FETCHINGEMAIL,
  PS_PREV,
  PS_TOP,
  PS_ME,
  PS_NEXT,
  PS_UPDATINGLEADERBOARD,
  PS_NOMORESCORES,
  PS_SELECTARACE,
  PS_UPLOADINGDATA,
  PS_FAILED,
  PS_EMAILTAKEN,
  PS_EMAILINVALID,
  PS_USERTAKEN,
  PS_USERINVALID,
  PS_USERSHORT,
  PS_QUITANDHELP,
  PS_CAMERAANDSETTINGS,
  PS_RESETANDPLAY,
  PS_THROTTLEANDRUDDER,
  PS_WINDDIRECTION,
  PS_AILERONSANDELEVATOR,
  PS_FLYINGINFO,
  PS_QUITANDHELPTEXT,
  PS_CAMERAANDSETTINGSTEXT,
  PS_RESETANDPLAYTEXT,
  PS_THROTTLEANDRUDDERTEXT,
  PS_WINDDIRECTIONTEXT,
  PS_AILERONSANDELEVATORTEXT,
  PS_FLYINGINFOTEXT,

// That's all
  PS_NUM_STRINGS,
};

enum Language
{
  LANG_EN,
  LANG_DE,
  LANG_PT,
  LANG_FR,
  LANG_NUM,
  LANG_NUM_ACTIVE = LANG_FR+1
};

static const char* gLanguageStrings[] = 
{
  "English",
  "Deutsch",
  "Português",
  "Français",
};


const char* GetPS(PicaString fs, Language l);

void InitPicaStrings();

#endif