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

void InitStringsEN(const char** txt)
{
  txt[PS_OK] = "OK";
  txt[PS_ENABLE] = "Enable";
  txt[PS_REMOVE] = "Remove";
  txt[PS_DUPLICATE] = "Duplicate";
  txt[PS_CRASHED] = "Damaged";
  txt[PS_NO] = "No";
  txt[PS_YES] = "Yes";
  txt[PS_INVERT] = "Invert";
  txt[PS_AUTO] = "Auto";
  txt[PS_GAMEPADDETECTED] = "Gamepad detected. Enable in the settings to use it.";
  txt[PS_WHATSNEW] = "Welcome to PicaSim!";
  txt[PS_SELECTAEROPLANE] = "Plane";
  txt[PS_SELECTSCENERY] = "Scenery";
  txt[PS_SELECTOPTIONS] = "Please select a suitable set of default options";
  txt[PS_SELECTSCENARIO] = "Scenario";
  txt[PS_USEDEFAULT] = "Use default";
  txt[PS_USEDEFAULTPREVIOUS] = "Use default/previous";
  txt[PS_BACK] = "Back";
  txt[PS_OPTIONS1] = "Options 1";
  txt[PS_OPTIONS2] = "Options 2";
  txt[PS_AEROPLANE] = "Aeroplane";
  txt[PS_SCENERY] = "Scenery";
  txt[PS_OBJECTS] = "Objects";
  txt[PS_OBJECTSSETTINGS] = "Objects settings";
  txt[PS_LIGHTING] = "Lighting";
  txt[PS_AICONTROLLERS] = "AI Controllers";
  txt[PS_CONTROLLER] = "Controller";
  txt[PS_JOYSTICK] = "Joystick";
  txt[PS_LOAD] = "Load...";
  txt[PS_SAVE] = "Save...";
  txt[PS_DELETE] = "Delete...";
  txt[PS_ADVANCED] = "Advanced";
  txt[PS_CLEARALLSAVEDSETTINGSANDEXIT] = "Clear all saved settings and exit";
  txt[PS_CONFIRMCLEARALLSETTINGS] = "This will clear all saved settings and exit PicaSim - are you sure?";
  txt[PS_SIMPLE] = "Simple";
  txt[PS_FILENAME] = "Filename:";
  txt[PS_LANGUAGESETTINGS] = "Language settings";
  txt[PS_CURRENTLANGUAGE] = "Current language";
  txt[PS_CAMERASETTINGS] = "Camera settings";
  txt[PS_ZOOMVIEW] = "Zoom view";
  txt[PS_PLANEONLYINZOOMVIEW] = "Draw plane alone in zoom view";
  txt[PS_SMOKEONLYINMAINVIEW] = "Only draw smoke in main view";
  txt[PS_ZOOMVIEWSIZE] = "Zoom view size";
  txt[PS_GROUNDAUTOZOOM] = "Ground autozoom";
  txt[PS_GROUNDAUTOZOOMSCALE] = "Ground autozoom scale";
  txt[PS_GROUNDFIELDOFVIEW] = "Ground field of view";
  txt[PS_GROUNDHORIZONAMOUNT] = "Ground horizon amount";
  txt[PS_GROUNDLAG] = "Ground camera lag";
  txt[PS_GROUNDVIEWFOLLOW] = "Follow aeroplane in ground view";
  txt[PS_GROUNDVIEWYAWOFFSET] = "Yaw offset in ground view";
  txt[PS_GROUNDVIEWPITCHOFFSET] = "Pitch offset in ground view";
  txt[PS_AEROPLANEFIELDOFVIEW] = "Aeroplane field of view";
  txt[PS_STEREOSCOPY] = "Stereoscopy";
  txt[PS_STEREOSEPARATION] = "Stereo separation";
  txt[PS_STEREOINFO] = "+ve value for normal 3D, -ve for cross-eyes technique";
  txt[PS_WALKABOUTSETTINGS] = "Walkabout settings";
  txt[PS_ENABLEWALKABOUTBUTTON] = "Enable walkabout button";
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
"\n"
"You can configure some additional properties like visibility and shadows in the Settings»Objects tab. From there you can "
"also force them to reset to their initial positions, by reloading the scene.";
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
  txt[PS_SETWINDDIRONWALKABOUT] = "Set wind direction on walkabout";
  txt[PS_SIMULATIONSETTINGS] = "Simulation settings";
  txt[PS_TIMESCALE] = "Time scale";
  txt[PS_PHYSICSACCURACY] = "Physics accuracy";
  txt[PS_CONTROLLERSETTINGS] = "Controller settings";
  txt[PS_MODE1DESCRIPTION] = "Mode 1: Rudder & elevator left, ailerons & throttle/flaps right";
  txt[PS_MODE2DESCRIPTION] = "Mode 2: Rudder & throttle/flaps left, ailerons & elevator right";
  txt[PS_MODE3DESCRIPTION] = "Mode 3: Ailerons & throttle/flaps left, rudder & elevator right";
  txt[PS_MODE4DESCRIPTION] = "Mode 4: Ailerons & elevator left, rudder & throttle/flaps right";
  txt[PS_CONTROLLERSIZE] = "Size";
  txt[PS_BRAKESFORWARD] = "Air brakes with stick forward";
  txt[PS_USEABSOLUTECONTROLLERTOUCHPOSITION] = "Use absolute controller touch position";
  txt[PS_STAGGERCONTROLLER] = "Stagger controller (for partial multitouch)";
  txt[PS_ENABLEELEVATORTRIM] = "Enable elevator trim";
  txt[PS_ELEVATORTRIMSIZE] = "Elevator trim size";
  txt[PS_SQUARECONTROLLERS] = "Square controllers";
  txt[PS_HORIZONTALOFFSETFROMEDGE] = "Horizontal offset from edge";
  txt[PS_VERTICALOFFSETFROMEDGE] = "Vertical offset from edge";
  txt[PS_SHAPEOPACITY] = "Shape opacity";
  txt[PS_STICKOPACITY] = "Stick opacity";
  txt[PS_STICKCROSS] = "Stick cross";
  txt[PS_JOYSTICKID] = "Joystick ID";
  txt[PS_JOYSTICKINFO] = "Note: You may need to restart PicaSim after connecting or disconnecting a joystick/controller.";
  txt[PS_NOJOYSTICK] = "No Joystick detected or available.";
  txt[PS_AUDIOSETTINGS] = "Audio settings";
  txt[PS_OVERALLVOLUME] = "Overall volume";
  txt[PS_VARIOMETERVOLUME] = "Variometer volume";
  txt[PS_WINDVOLUME] = "Wind volume";
  txt[PS_OUTSIDEAEROPLANEVOLUME] = "Outside aeroplane volume";
  txt[PS_INSIDEAEROPLANEVOLUME] = "Inside aeroplane volume";
  txt[PS_ONSCREENDISPLAYSETTINGS] = "On-screen display settings";
  txt[PS_WINDARROWSIZE] = "Wind arrow size";
  txt[PS_WINDARROWOPACITY] = "Wind arrow opacity";
  txt[PS_PAUSEBUTTONSSIZE] = "Pause buttons size";
  txt[PS_PAUSEBUTTONOPACITY] = "Pause button opacity";
  txt[PS_INFORMATIONSETTINGS] = "Information settings";
  txt[PS_MAXMARKERSPERTHERMAL] = "Max markers per thermal";
  txt[PS_THERMALWINDFIELD] = "Draw thermal wind field";
  txt[PS_GRAPHAIRSPEED] = "Show air speed (red)";
  txt[PS_GRAPHDURATION] = "Graph duration";
  txt[PS_GRAPHALTITUDE] = "Show altitude (turquoise)";
  txt[PS_GRAPHGROUNDSPEED] = "Show ground speed (green)";
  txt[PS_GRAPHCLIMBRATE] = "Show climb rate (blue)";
  txt[PS_GRAPHWINDSPEED] = "Show wind speed (yellow)";
  txt[PS_GRAPHWINDVERTICALVELOCITY] = "Show wind vertical velocity (purple)";
  txt[PS_STALLMARKERS] = "Stall markers";
  txt[PS_DRAWAEROPLANECOM] = "Draw aeroplane CoM";
  txt[PS_GRAPHFPS] = "Show FPS";
  txt[PS_NUMWINDSTREAMERS] = "Num wind streamers";
  txt[PS_WINDSTREAMERTIME] = "Wind streamer time";
  txt[PS_WINDSTREAMERDELTAZ] = "Wind streamer deltaZ";
  txt[PS_GRAPHICSSETTINGS] = "Graphics settings";
  txt[PS_GROUNDTERRAINLOD] = "Ground terrain LOD";
  txt[PS_AEROPLANETERRAINLOD] = "Aeroplane terrain LOD";
  txt[PS_UPDATETERRAINLOD] = "Update terrain LOD in ground view";
  txt[PS_COMPONENTS] = "Components";
  txt[PS_3DMODEL] = "3D Model";
  txt[PS_BOTH] = "Both";
  txt[PS_CONTROLLEDPLANESHADOWS] = "Controlled plane shadows";
  txt[PS_OTHERSHADOWS] = "Other shadows";
  txt[PS_PROJECTEDSHADOWDETAIL] = "Projected shadow detail";
  txt[PS_BLOB] = "Blob";
  txt[PS_PROJECTED] = "Projected";
  txt[PS_AEROPLANERENDER] = "Aeroplane render";
  txt[PS_USE16BIT] = "Use 16 bit textures to reduce memory (requires restart)";
  txt[PS_SEPARATESPECULAR] = "Separate specular calculation";
  txt[PS_AMBIENTLIGHTINGSCALE] = "Ambient lighting scale";
  txt[PS_DIFFUSELIGHTINGSCALE] = "Diffuse lighting scale";
  txt[PS_TERRAINTEXTUREDETAIL] = "Terrain texture detail";
  txt[PS_MAXSKYBOXDETAIL] = "Max skybox/panorama detail";
  txt[PS_ANTIALIASING] = "Anti-aliasing (MSAA)";
  txt[PS_ANTIALIASING_2X] = "2x";
  txt[PS_ANTIALIASING_4X] = "4x";
  txt[PS_ANTIALIASING_8X] = "8x";
  txt[PS_REQUIRESRESTART] = "Requires restart";
  txt[PS_MISCSETTINGS] = "Misc settings";
  txt[PS_USEBACKBUTTON] = "Use back button to exit";
  txt[PS_DRAWLAUNCHMARKER] = "Draw launch marker";
  txt[PS_DRAWGROUNDPOSITION] = "Draw ground position";
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
  txt[PS_USEAEROPLANEPREFERREDCONTROLLER] = "Use aeroplane preferred controller";
  txt[PS_TESTINGDEVELOPERSETTINGS] = "Testing/Developer settings";
  txt[PS_WIREFRAMETERRAIN] = "Wireframe terrain";
  txt[PS_DRAWSUNPOSITION] = "Draw sun position";
  txt[PS_FREEFLYSETTINGS] = "Free-fly settings";
  txt[PS_DISPLAYFLIGHTTIME] = "Display flight time";
  txt[PS_DISPLAYSPEED] = "Display speed";
  txt[PS_DISPLAYAIRSPEED] = "Display airspeed";
  txt[PS_DISPLAYMAXSPEED] = "Display max speed";
  txt[PS_DISPLAYASCENTRATE] = "Display ascent rate";
  txt[PS_DISPLAYALTITUDE] = "Display altitude";
  txt[PS_DISPLAYDISTANCE] = "Display distance";
  txt[PS_COLOURTEXT] = "Colour text with ascent rate";
  txt[PS_TEXTATTOP] = "Text at top of screen";
  txt[PS_FREEFLYONSTARTUP] = "Free fly on startup";
  txt[PS_ENABLESOCKETCONTROLLER] = "Enable socket controller";
  txt[PS_TEXTBACKGROUNDOPACITY] = "Text background opacity";
  txt[PS_TEXTBACKGROUNDCOLOUR] = "Text background colour";
  txt[PS_MAXNUMBERAI] = "Max number of AI";
  txt[PS_TOOMANYAI] = "Note: Too many AI will cause crashes & slowdown!";
  txt[PS_UNITS] = "Units";
  txt[PS_RACESETTINGS] = "Race/Limbo settings";
  txt[PS_RACEVIBRATIONAMOUNT] = "Vibration amount";
  txt[PS_RACEBEEPVOLUME] = "Beep volume";
  txt[PS_LIMBODIFFICULTY] = "Limbo difficulty";
  txt[PS_DELETELOCALHIGHSCORES] = "Delete local high scores";
  txt[PS_GENERALSETTINGS] = "General settings";
  txt[PS_COLOURSCHEME] = "Colour scheme";
  txt[PS_COLOUROFFSET] = "Colour shift";
  txt[PS_BALLAST] = "Ballast";
  txt[PS_BALLASTFWD] = "Ballast forward";
  txt[PS_BALLASTLEFT] = "Ballast left";
  txt[PS_BALLASTUP] = "Ballast up";
  txt[PS_DRAGMULTIPLIER] = "Drag multiplier";
  txt[PS_SIZEMULTIPLIER] = "Size multiplier";
  txt[PS_MASSMULTIPLIER] = "Mass multiplier";
  txt[PS_ENGINEMULTIPLIER] = "Engine multiplier";
  txt[PS_HASVARIOMETER] = "Has variometer";
  txt[PS_PREFERREDCONTROLLER] = "Preferred controller";
  txt[PS_SHOWBUTTON1] = "Show button 1";
  txt[PS_SHOWBUTTON2] = "Show button 2";
  txt[PS_LAUNCH] = "Launch";
  txt[PS_HOOKS] = "Tow hooks";
  txt[PS_BUNGEELAUNCH] = "Bungee launch";
  txt[PS_AEROTOWLAUNCH] = "Aerotow launch";
  txt[PS_FLATLAUNCHMETHOD] = "Launch method (flat sceneries)";
  txt[PS_HAND] = "Hand";
  txt[PS_BUNGEE] = "Bungee";
  txt[PS_AEROTOW] = "Aerotow";
  txt[PS_LAUNCHSPEED] = "Launch speed";
  txt[PS_LAUNCHANGLE] = "Launch angle up";
  txt[PS_LAUNCHUP] = "Launch up";
  txt[PS_LAUNCHFORWARD] = "Launch forwards";
  txt[PS_LAUNCHLEFT] = "Launch left";
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
  txt[PS_AIRFRAME] = "Airframe";
  txt[PS_UNDERCARRIAGE] = "Undercarriage";
  txt[PS_PROPELLER] = "Propeller";
  txt[PS_BELLYHOOKOFFSETFORWARD] = "Belly hook offset forwards";
  txt[PS_BELLYHOOKOFFSETUP] = "Belly hook offset up";
  txt[PS_NOSEHOOKOFFSETFORWARD] = "Nose tow hook offset forwards";
  txt[PS_NOSEHOOKOFFSETUP] = "Nose tow hook offset up";
  txt[PS_TAILHOOKOFFSETFORWARD] = "Tail tow hook offset forwards";
  txt[PS_TAILHOOKOFFSETUP] = "Tail tow hook offset up";
  txt[PS_MAXBUNGEELENGTH] = "Max bungee length";
  txt[PS_MAXBUNGEEACCEL] = "Max bungee acceleration";
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
  txt[PS_CHASECAMERA] = "Chase camera";
  txt[PS_CAMERATARGETPOSFWD] = "Target offset fwd";
  txt[PS_CAMERATARGETPOSUP] = "Target offset up";
  txt[PS_DISTANCE] = "Distance";
  txt[PS_HEIGHT] = "Height";
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
  txt[PS_CURRENTPOSITION] = "Current position";
  txt[PS_MASS] = "Mass";
  txt[PS_INERTIA] = "Inertia";
  txt[PS_WINGAREA] = "Wing area";
  txt[PS_EXTENTS] = "Extents";
  txt[PS_WINDSETTINGS] = "Wind settings";
  txt[PS_WINDSPEED] = "Wind speed";
  txt[PS_WINDBEARING] = "Wind bearing";
  txt[PS_CANNOTMODIFYSCENERY] = "Cannot modify scenery during a race";
  txt[PS_ALLOWBUNGEE] = "Allow bungee or aerotow launch";
  txt[PS_WINDGUSTTIME] = "Wind gust timescale";
  txt[PS_WINDGUSTFRACTION] = "Wind gust fraction";
  txt[PS_WINDGUSTANGLE] = "Wind gust angle";
  txt[PS_TURBULENCEAMOUNT] = "Turbulence amount";
  txt[PS_SURFACETURBULENCEAMOUNT] = "Surface turbulence";
  txt[PS_SHEARTURBULENCEAMOUNT] = "Wind shear turbulence";
  txt[PS_WINDLIFTSMOOTHING] = "Wind lift smoothing";
  txt[PS_VERTICALWINDDECAYDISTANCE] = "Vertical wind decay height";
  txt[PS_SEPARATIONTENDENCY] = "Separation tendency";
  txt[PS_ROTORTENDENCY] = "Rotor tendency";
  txt[PS_DEADAIRTURBULENCE] = "Dead air turbulence";
  txt[PS_BOUNDARYLAYERDEPTH] = "Boundary layer depth";
  txt[PS_THERMALSETTINGS] = "Thermal settings (also see lighting)";
  txt[PS_DENSITY] = "Density";
  txt[PS_RANGE] = "Range";
  txt[PS_LIFESPAN] = "Life span";
  txt[PS_DEPTH] = "Depth";
  txt[PS_CORERADIUS] = "Core radius";
  txt[PS_DOWNDRAFTEXTENT] = "Downdraft extent";
  txt[PS_UPDRAFTSPEED] = "Updraft speed";
  txt[PS_ASCENTRATE] = "Ascent rate";
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
  txt[PS_RANDOMTERRAINSETTINGS] = "Random terrain settings";
  txt[PS_RANDOMSEED] = "Random seed";
  txt[PS_DISPLACEMENTHEIGHT] = "Displacement height";
  txt[PS_SMOOTHNESS] = "Smoothness";
  txt[PS_EDGEHEIGHT] = "Edge height";
  txt[PS_UPWARDSBIAS] = "Upwards bias";
  txt[PS_FILTERITERATIONS] = "Filter iterations";
  txt[PS_DRAWPLAIN] = "Draw plain/water";
  txt[PS_COLLIDEWITHPLAIN] = "Collide with plain/water";
  txt[PS_PLAININNERRADIUS] = "Plain inner radius";
  txt[PS_PLAINFOGDISTANCE] = "Plain fog distance";
  txt[PS_PLAINHEIGHT] = "Plain height";
  txt[PS_TERRAINSIZE] = "Terrain size";
  txt[PS_COASTENHANCEMENT] = "Coast LOD enhancement";
  txt[PS_TERRAINDETAIL] = "Terrain detail";
  txt[PS_RIDGETERRAINSETTINGS] = "Ridge terrain settings";
  txt[PS_MAXHEIGHTFRACTION] = "Max height fraction";
  txt[PS_WIDTH] = "Width";
  txt[PS_HEIGHTOFFSET] = "Height offset";
  txt[PS_HORIZONTALVARIATION] = "Horizontal variation";
  txt[PS_HORIZONTALWAVELENGTH] = "Horizontal wavelength";
  txt[PS_VERTICALVARIATION] = "Vertical variation fraction";
  txt[PS_PANORAMA3DSETTINGS] = "Panorama 3D settings";
  txt[PS_HEIGHTFIELDSETTINGS] = "Heightfield settings";
  txt[PS_MINHEIGHT] = "Min height";
  txt[PS_MAXHEIGHT] = "Max height";
  txt[PS_AISCENERY] = "AI Scenery";
  txt[PS_SCENETYPE] = "Scene type";
  txt[PS_FLAT] = "Flat";
  txt[PS_SLOPE] = "Slope";
  txt[PS_CURRENTVIEWPOSITION] = "Current view position";
  txt[PS_NOLIGHTINGSETTINGS] = "No lighting settings as the terrain type is Panorama";
  txt[PS_SUNBEARING] = "Sun bearing";
  txt[PS_THERMALACTIVITY] = "Thermal activity";
  txt[PS_TERRAINDARKNESS] = "Terrain darkness";
  txt[PS_CONTROLLERSEESETTINGS] = "Also see additional controller settings under Options";
  txt[PS_CROSS] = "Cross";
  txt[PS_BOX] = "Box";
  txt[PS_CROSSANDBOX] = "Cross & Box";
  txt[PS_STYLE] = "Style";
  txt[PS_CONTROLLERMODE] = "Controller mode";
  txt[PS_TREATTHROTTLEASBRAKES] = "Treat throttle stick as brakes";
  txt[PS_RESETALTSETTINGONLAUNCH] = "Reset configuration on launch";
  txt[PS_NUMCONFIGURATIONS] = "Number of configurations";
  txt[PS_TILTHORIZONTAL] = "Tilt horizontal";
  txt[PS_TILTVERTICAL] = "Tilt vertical";
  txt[PS_ARROWSHORIZONTAL] = "Arrows horizontal";
  txt[PS_ARROWSVERTICAL] = "Arrows vertical";
  txt[PS_ROLLSTICK] = "Roll stick";
  txt[PS_PITCHSTICK] = "Pitch stick";
  txt[PS_YAWSTICK] = "Yaw stick";
  txt[PS_SPEEDSTICK] = "Throttle stick";
  txt[PS_CONSTANT] = "Constant";
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
  txt[PS_NONE] = "None";
  txt[PS_CONTROLSOURCES] = "Control sources";
  txt[PS_AILERONS] = "Ailerons/rudder";
  txt[PS_ELEVATOR] = "Elevator";
  txt[PS_RUDDER] = "Rudder";
  txt[PS_THROTTLE] = "Throttle/flaps/brakes";
  txt[PS_LOOKYAW] = "Look yaw";
  txt[PS_LOOKPITCH] = "Look pitch";
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
  txt[PS_SETTINGSFORCONTROLLER] = "Settings for controller configuration %d: %s";
  txt[PS_TRIMSETTINGS] = "Trim settings";
  txt[PS_NOSIMPLESETTINGS] = "No simple settings available";
  txt[PS_TRIM] = "Trim";
  txt[PS_SCALE] = "Scale";
  txt[PS_CLAMPING] = "Clamping";
  txt[PS_EXPONENTIAL] = "Exponential";
  txt[PS_SPRING] = "Spring";
  txt[PS_POSITIVE] = "Positive";
  txt[PS_NEGATIVE] = "Negative";
  txt[PS_ROLLSTICKMOVEMENT] = "Roll stick movement";
  txt[PS_PITCHSTICKMOVEMENT] = "Pitch stick movement";
  txt[PS_YAWSTICKMOVEMENT] = "Yaw stick movement";
  txt[PS_SPEEDSTICKMOVEMENT] = "Speed stick movement";
  txt[PS_ACCELEROMETERROLLMOVEMENT] = "Accelerometer: roll movement";
  txt[PS_ACCELEROMETERROLL] = "Accelerometer roll";
  txt[PS_ACCELEROMETERPITCHMOVEMENT] = "Acceleromenter: pitch movement";
  txt[PS_ACCELEROMETERPITCH] = "Accelerometer pitch";
  txt[PS_TILTROLLSENSITIVITY] = "Tilt roll sensitivity";
  txt[PS_TILTPITCHSENSITIVITY] = "Tilt pitch sensitivity";
  txt[PS_TILTNEUTRALANGLE] = "Tilt neutral angle";
  txt[PS_NOJOYSTICKWITHID] = "No Joystick with this ID";
  txt[PS_ENABLEJOYSTICK] = "Enable joystick";
  txt[PS_ADJUSTFORCIRCULARSTICKMOVEMENT] = "Adjust for circular stick movement";
  txt[PS_EXTERNALJOYSTICKSETTINGS] = "External joystick settings";
  txt[PS_JOYSTICKLABEL] = "Joystick %d: Input = %5.2f Output = %5.2f";
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
  txt[PS_LOADOPTIONS] = "Load options";
  txt[PS_SAVEOPTIONS] = "Save options";
  txt[PS_DELETEOPTIONS] = "Delete options";
  txt[PS_LOADSCENERY] = "Load scenery";
  txt[PS_SAVESCENERY] = "Save scenery";
  txt[PS_DELETESCENERY] = "Delete scenery";
  txt[PS_LOADOBJECTS] = "Load objects";
  txt[PS_SAVEOBJECTS] = "Save objects";
  txt[PS_DELETEOBJECTS] = "Delete objects";
  txt[PS_LOADLIGHTING] = "Load lighting";
  txt[PS_SAVELIGHTING] = "Save lighting";
  txt[PS_DELETELIGHTING] = "Delete lighting";
  txt[PS_LOADCONTROLLER] = "Load controller";
  txt[PS_SAVECONTROLLER] = "Save controller";
  txt[PS_DELETECONTROLLER] = "Delete controller";
  txt[PS_LOADJOYSTICK] = "Load joystick";
  txt[PS_SAVEJOYSTICK] = "Save joystick";
  txt[PS_DELETEJOYSTICK] = "Delete joystick";
  txt[PS_LOADAEROPLANE] = "Load aeroplane";
  txt[PS_SAVEAEROPLANE] = "Save aeroplane";
  txt[PS_DELETEAEROPLANE] = "Delete aeroplane";
  txt[PS_LOADAICONTROLLERS] = "Load AI controllers";
  txt[PS_SAVEAICONTROLLERS] = "Save AI controllers";
  txt[PS_DELETEAICONTROLLERS] = "Delete AI controllers";
  txt[PS_SELECTPANORAMA] = "Select panorama";
  txt[PS_SELECTTERRAINFILE] = "Select terrain file";
  txt[PS_SELECTPREFERREDCONTROLLER] = "Select preferred controller";
  txt[PS_SELECTOBJECTSSETTINGS] = "Select objects settings";
  txt[PS_SUMMARY] = "Summary";
  txt[PS_NOOBJECTS] = "There are no objects loaded (other than the terrain etc). You can load a set of static and dynamic objects, "
    "making sure they're suitable for the current scene. By enabling object editing you can "
    "create your own - see help and the PicaSim web page for more info.";
  txt[PS_OBJECTSTOTAL] = "Total number of objects:";
  txt[PS_OBJECTSSTATICVISIBLE] = "Number static visible:";
  txt[PS_OBJECTSSTATICINVISIBLE] = "Number static invisible:";
  txt[PS_OBJECTSDYNAMICVISIBLE] = "Number dynamic visible:";
  txt[PS_ABOUT] = "About";
  txt[PS_WEBSITE] = "Website";
  txt[PS_FLYING] = "Flying";
  txt[PS_SETTINGS] = "Settings";
  txt[PS_RACES] = "Challenges";
  txt[PS_TIPS] = "Tips";
  txt[PS_KEYBOARD] = "Keyboard";
  txt[PS_CREDITS] = "Credits";
  txt[PS_LICENCE] = "Licence";
  txt[PS_VERSIONS] = "Versions";
  txt[PS_ABOUTPAIDTEXT] = "PicaSim is a simulator for radio controlled aircraft. At the moment it is designed to help you learn "
"to fly and improve your skills at slope soaring - flying gliders using the lift from the wind as it "
"flows up and over hills.\n"
"\n"
"If you've never flown a model aeroplane before, you're likely to crash the first few times (or more!) "
"that you try to fly. That's OK - it's better to crash in the simulator than with an expensive model the "
"you've spent a long time building or setting up.\n"
"\n"
"PicaSim can help you learn three skills:\n"
"\n"
"1. Using the transmitter sticks to control the aircraft.\n"
"2. Evaulating where to find lift at slope soaring sites.\n"
"3. Controlling the aircraft even when it is flying directly towards you.\n"
"\n"
"In addition, more experienced users can use PicaSim to practise aerobatics, or "
"just enjoy flying when there's no wind!\n"
"\n"
"I have lots of plans to extend and improve PicaSim - to include powered aircraft, more models for "
"the planes, networked flying, AI controlled aircraft etc. The small purchase price will be used to "
"help me do that, and is much appreciated!\n";
  txt[PS_HOWTOFLYTEXT] = "The first thing to do is to visit the website (link below) and watch the short video \"How to Fly\". \n"
"\n"
"When the simulator starts it will be paused. The button in the top right corner will unpause it, "
"sending the glider off flying into the wind. You need to use the on-screen joystick/controller in "
"the bottom right hand corner of the screen to control it - just like the right-hand stick of a "
"radio-control transmitter (set up in Mode 2). When you crash, either use the back button on your "
"device to reset the glider, or pause the simulator, tap the 'rewind' button, and unpause it again.\n"
"\n"
"Once you've got the hang of the basic controls, you need to manoever the glider so that it stays within "
"the region of lift provided by the wind as it flows up and over the hill. The best way to do this (at "
"first) is to fly straight ahead for a three or four seconds, then turn left and fly parallel to the hill "
"for up to about ten seconds. Then turn round and fly back a similar distance past the viewpoint, keeping "
"parallel to the hill and at about the same height as the viewpoint.\n"
"\n"
"If you find things happen too fast, you can try slowing down time in the Settings»Options panel. Remember "
"to set it back to 1.0 when you've got the hang of things though - there isn't one of these settings in "
"real life!\n"
"\n"
"If you're not using one of the panoramic sceneries, you can also change the viewpoint to inside the aeroplane "
"(the 'eye' button when paused, or the 'search' "
"button on your device). Now you can explore the terrain - gaining lift where you can to try to cross areas "
"where the wind/hill doesn't provide any lift. Use the wind arrow to see which direction the wind is blowing in "
"and therefore where lift is likely to be found.\n"
"\n"
"If the terrain is essentially flat it will have been set up to enable thermals, and gliders will be given "
"a winch/bungee start. You'll need to watch the motion of the glider carefully to detect where the thermals "
"are. If you find this difficult, enable the thermal markers in the options - this will display birds flying "
"around in the core of the thermals. Remember "
"that thermals will be stronger over the land than over water, and also stronger around slopes that face the sun.\n"
"\n"
"Many planes will use the second controller to control the rudder. Up/down movements will control the throttle, flaps "
"or air brakes (as appropriate) - look how the control surfaces move to see.\n"
"\n"
"Visit the website below to see some videos showing how to fly - it takes some practice, just like with a "
"real radio controlled aircraft!";
  txt[PS_HOWTOCONFIGURETEXT] = "You can access PicaSim settings from either the start menu, or using one of the buttons when paused.\n"
"\n"
"The settings are split up into blocks - general options, the aeroplane, the scenery/terrain, the lighting "
"and the controller configuration. By default you can't fiddle with the detailed individual settings for most of these "
"but you can load preset configuration blocks. This lets you quickly switch aeroplane or terrain, for example.\n"
"\n"
"If you enable the 'Advanced settings' in each section you'll get to tweak many more settings, which may be a little "
"daunting! In time there will be some information about these settings, but for now it's safe to tweak the values "
"as you can always reload them with one of the presets.\n"
"\n"
"You can save your presets - they get saved to the memory card/data area in your device. If you encounter problems with "
"PicaSim after changing the settings it may be worth reloading each section in turn immediately after starting PicaSim.\n"
"\n"
"If 'Walkabout' is enabled in the options you'll also have a 'walkabout' button when paused. This allows you to use the "
"controller to walk about the terrain, in order to find a new site to fly from. By default the wind direction is "
"automatically adjusted when in this mode (so it blows directly towards the camera).\n"
"\n"
"With panoramic sceneries it's not possible to change the viewpoint, so the walkabout mode is disabled.";
  txt[PS_HOWTORACETEXT] = "Most challenge/race modes involve flying as fast as possible between checkpoints. The next checkpoint you need to aim for "
"is highlighted in colour, and is also pointed to by the white arrow at the bottom of the screen.\n"
"\n"
"There are different types of checkpoints: In the F3F-style races, where you have to fly multiple laps back and forth, "
"you need to fly past the checkpoint, upwind of it. In the cross-country race you just need to get close to the checkpoint (though "
"you can be as high as you like). In the Flatland race you fly between two gates - the active upwind gate is green, and the "
"downwind one is red.\n"
"\n"
"There are also duration modes where the aim is to keep flying for as long as possible using the thermals. Limbo "
"modes require you to fly below the gate as many times as possible (in the right direction!) in a given time. The gate can be "
"lowered in Settings»Options 2 which makes it harder, but you do get more points!\n";
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
"[z] - Toggles the zoom view\n"
"[Right shift] - Press the \"rates\" button, if available\n"
"[s] - Write screenshot into the user data area\n"
"[t] - Toggle slow motion\n"
"[m] - Enter walkabout mode (if enabled)\n"
"[Arrow keys] - These can be mapped to control the plane\n"
"[Num-pad] - Move the plane (when paused). Use Ctrl to rotate\n"
"[F11]/[Alt+Enter]/[F] - Toggle full-screen\n"
"[=] and [-] - Zoom in and out\n"
"[1] and [2] - Toggle the buttons (typically used for smoke)\n"
"\n"
"There are some additional shortcuts for developers:\n"
"\n"
"[l] - Force the plane to reload\n"
"[u] - Toggle UI\n"
"[k] - Cycle display between normal and physics\n"
"[g] - Display the CG position and equivalent box (for moments of inertia)\n"
"[7] and [8] - Decrease/increase number of wind streamers\n"
"[0] and [9] - Cycle through the aerofoils and plot data\n";
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
  txt[PS_CREDITSTEXT] = "PicaSim was written by Danny Chapman.\n\n\n"
"With thanks to:\n\n"
"Heidi and Hazel for their patience\n\n"
"Heidi, Jenne, André and Sylvain for translations\n\n"
"Bullet physics\n\n"
"TinyXML\n\n"
"Marmalade for the cross-platform libraries\n\n"
"Gleb Lebedev for his gamepad extension\n\n"
"Detlef Jacobi for Litermonthalle Nalbach\n\n"
"Some sounds from www.freesfx.co.uk and soundjay\n\n"
"Textures from seamless-pixels.blogspot.co.uk/\n\n"
"Everybody who has helped with testing and reporting bugs\n\n"
"Aeroplane models provided by:\n\n"
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
"Please note that whilst I've tried to set up the models to fly realistically, I make no claims that "
"they actually achieve this goal. The only way to discover this is to go out and fly the real thing, "
"which I urge you to do!";
  txt[PS_LICENCETEXT] = "PicaSim is available from me in the following versions:\n\n"
"Free: The simulator and assets (including planes and sceneries etc) may be distributed according to the Creative Commons Attribution-NonCommercial-NoDerivs licence.\n\n"
"Paid: This version or its derivatives may only be distributed with my explicit authorisation.\n\n"
"If you wish to share content that you have created then you must do so separately from the PicaSim distribution.\n\n"
"The assets in PicaSim may be distributed according to the Creative Commons Attribution-NonCommercial licence (i.e. you may modify and distribute them, so long as you credit the source).\n\n"
"It is quite easy to distribute content for PicaSim by placing it in the UserData and UserSettings. See the website and ask on the forum for more details.";
  txt[PS_TIPS1] = "Visit the PicaSim website for help, including tips on how to fly!";
  txt[PS_TIPS2] = "Use gentle controls - with small inputs most planes will almost fly on their own";
  txt[PS_TIPS3] = "Use the button in the corner when flying to switch between normal/acrobatic/thermal etc modes (when available)";
  txt[PS_TIPS4] = "The camera can be set to follow the plane etc in 3D scenes - use the 'eye' button when paused";
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
  txt[PS_LOADING] = "Loading...";
  txt[PS_RACETIP] = "%s\n\nThe white arrow points to the next checkpoint.";
  txt[PS_LIMBOTIP] = "%s\n\nAdjust the difficulty/score multipler in Settings»Options 2";
  txt[PS_DURATIONTIP] = "%s\n\nLook for thermals and land close to the launch point.";
  txt[PS_TRAINERGLIDERTIP] = "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower corner to control the elevator and rudder, flying the glider in the lift in front of the hill.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
  txt[PS_TRAINERPOWEREDTIP] = "Press the play button to launch, and the pause/rewind button to reset. "
    "Use the controls in the lower right corner to control elevator and ailerons. The controls on the left are for throttle and rudder.\n"
    "Use the help button above for more info, or the help/website back in the main menu for hints on how to fly if you find it hard to keep airborne.";
  txt[PS_SELECTRACE] = "Select challenge";
  txt[PS_PREV] = "Prev";
  txt[PS_NEXT] = "Next";
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

