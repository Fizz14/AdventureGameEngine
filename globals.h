#ifndef globals_h
#define globals_h

#include <iostream>
#include <sstream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <cmath>	 //pow
#include <math.h>	 //sin()
#include <fstream> //loading
#include <vector>
#include <cctype> //make input lowercase for map console
#include <ctime>	//debug clock
#include <string>
#include <map> //saves
#include <ctime> //clock display

// this is unique to the windowsport
#include "windowsinclude.h"

#undef M_PI
#define M_PI 3.14159265358979323846

extern SDL_Color g_textcolor;

using namespace std;

class coord;

class cshadow;

class actor;

class mapObject;

class entity;

class tile;

class door;

class rect;

class mapCollision;

class box;

class impliedSlope;

class impliedSlopeTri;

class tri;

class ramp;

class textbox;

class ui;

class fancychar;

class fancyword;

class fancybox;

class adventureUI;

class settingsUI;

class escapeUI;

class attack;

class weapon;

class projectile;

class heightmap;

class navNode;

class worldsound;

class musicNode;

class cueSound;

class waypoint;

class trigger;

class hitbox;

class listener;

class worldItem;

class indexItem;

class effectIndex;

class particle;

class emitter;

class collisionZone;

class pointOfInterest;

class levelNode;

class levelSequence;

class usable;

class camera
{
  public:
    float oldx = 0;
    float oldy = 0;
    int x = 200;
    int y = 200;
    float width = 640;
    float height = 480;
    float lag = 0.0;
    const float DEFAULTLAGACCEL = 0.01;
    float lagaccel = 0.01; // how much faster the camera gets while lagging
    float zoom = 1;
    float zoommod = 1;
    int lowerLimitX = 0;
    int lowerLimitY = 0;
    int upperLimitX = 0;
    int upperLimitY = 0;
    bool enforceLimits = 0;

    camera(float fx, float fy);

    void update_movement(float elapsed, float targetx, float targety);

    void resetCamera();
};

// AI
enum travelstyle
{
  roam,
  patrol
};


extern vector<cshadow *> g_shadows;

extern vector<entity *> g_entities;

extern vector<entity *> g_boardableEntities;

extern vector<entity*> g_ai;

extern vector<entity *> g_pellets; //for making pellets bounce and squash

extern vector<entity *> g_solid_entities;

extern vector<entity *> g_large_entities;

extern vector<tile *> g_tiles;

extern vector<door *> g_doors;

extern vector<vector<box *>> g_boxs;

extern vector<impliedSlope *> g_impliedSlopes; //slopes which are implied to be behind walls, preventing entities from "hiding" behind them, as in zelda
                                        
extern vector<impliedSlopeTri *> g_impliedSlopeTris; //same as g_impliedSlopes, but for triangular slopes

extern vector<textbox *> g_textboxes;

extern vector<ui *> g_ui;

extern vector<actor *> g_actors;

extern vector<mapObject *> g_mapObjects;

extern vector<mapCollision *> g_mapCollisions;

extern vector<vector<tri *>> g_triangles;

extern vector<vector<ramp *>> g_ramps;

extern vector<heightmap *> g_heightmaps;

extern vector<navNode *> g_navNodes;

extern vector<vector<pointOfInterest *>> g_setsOfInterest;

extern vector<effectIndex *> g_effectIndexes;

extern vector<worldsound *> g_worldsounds;

extern vector<musicNode *> g_musicNodes;

extern vector<cueSound *> g_cueSounds;

extern vector<waypoint *> g_waypoints;

extern vector<trigger *> g_triggers;

extern vector<hitbox *> g_hitboxes;

extern vector<listener *> g_listeners;

extern vector<projectile *> g_projectiles;

extern vector<attack *> g_attacks;

extern vector<weapon *> g_weapons;

extern vector<worldItem *> g_worldItems;

extern vector<indexItem *> g_indexItems;

extern vector<particle *> g_particles;

extern vector<emitter *> g_emitters;

extern vector<collisionZone *> g_collisionZones;

extern vector<entity *> g_musicalEntities;

extern vector<levelNode *> g_levelNodes;

struct cmpCoord
{
  bool operator()(const pair<int, int> a, const pair<int, int> b) const;
};


// quick debug info
#define D(a)                                \
  if (canSwitchOffDevMode && showDevMessages)           \
{                                         \
  std::cout << #a << ": " << (a) << endl; \
}

#define M(a)                                \
  if (canSwitchOffDevMode && showDevMessages)           \
{                                         \
  std::cout << (a) << endl; \
}

#define I(a)                                \
{                                         \
  std::cout << (a) << endl; \
}

#define E(a)                                \
{                                         \
  std::cout << "ERROR:" << #a << ": " << (a) << endl; \
}


// Temporary debugging statements- I won't allow myself to block these
#define T(a) std::cout << #a << ": " << (a) << endl;

extern map<pair<int, int>, navNode*, cmpCoord> navNodeMap;

void breakpoint();

void breakpoint2();

template<typename T>
T limit(T input, T min, T max);


extern map<string, int> enemiesMap;

extern int g_budget;
extern bool boxsenabled;
extern bool g_collisionResolverOn;
extern bool g_showCRMessages;
extern bool onionmode;

extern const bool g_useSimpleImpliedGeometry;
extern bool genericmode;
extern bool freecamera;
extern bool devMode;
extern bool canSwitchOffDevMode;
extern bool inputRefreshCanSwitchOffDevMode;
extern bool showDevMessages;
extern bool showErrorMessages;
extern bool showImportantMessages;


// for visuals
extern float p_ratio;
extern float g_brightness_setting;
extern float g_brightness_map_factor;
extern SDL_Texture* g_shade;
extern bool g_vsync;
extern float g_background_darkness;
extern SDL_Texture *background;
extern bool g_backgroundLoaded;
extern bool g_useBackgrounds;
extern int g_brightness;
extern float XtoZ;
extern float XtoY;
extern float YtoX;
extern float g_ratio;
extern bool transition;
extern int g_walldarkness;
extern bool g_unlit;
extern int g_graphicsquality;
extern float g_extraShadowSize;
extern int g_fogofwarEnabled;
extern int g_fogofwarRays;
extern bool g_showHealthbar;
extern int g_entitySleepDistance;
extern effectIndex *smokeEffect;
extern effectIndex *littleSmokeEffect;
extern effectIndex *blackSmokeEffect;
extern effectIndex *sparksEffect;

extern particle* g_lastParticleCreated;

// for fow
extern SDL_Texture *result;
extern SDL_Texture *result_c;
extern SDL_Texture *canvas;
extern SDL_Texture *canvas_fc;
extern SDL_Texture *light;

extern SDL_Texture *lighta;
extern SDL_Texture *lightb;
extern SDL_Texture *lightc;
extern SDL_Texture *lightd;

extern SDL_Texture *lightaro;
extern SDL_Texture *lightbro;
extern SDL_Texture *lightcro;
extern SDL_Texture *lightdro;

extern SDL_Texture *lightari;
extern SDL_Texture *lightbri;
extern SDL_Texture *lightcri;
extern SDL_Texture *lightdri;

extern SDL_Texture *TextureA;
extern SDL_Texture *TextureB;
extern SDL_Texture *TextureC;
extern SDL_Texture *TextureD;
extern SDL_Texture *blackbarTexture;

extern int g_fogheight;
extern int g_fogwidth;
extern int g_lastFunctionalX;
extern int g_lastFunctionalY;
extern int g_fogMiddleX;
extern int g_fogMiddleY;
extern float g_viewdist;
//instead of calculating distance every frame, lookup which boxes should be open in a table

extern std::vector<std::vector<int>>g_fog_window;

extern bool g_fogIgnoresLOS;
extern int g_tile_fade_speed;
extern int xtileshift;
extern int ytileshift;
extern bool g_force_cookies_update;

extern std::vector<std::vector<int>> g_fogcookies;
extern std::vector<std::vector<int>> g_savedcookies;

extern std::vector<std::vector<int>> g_fc;
extern std::vector<std::vector<int>> g_sc;
extern std::vector<std::vector<int>> g_shc;

extern std::vector<entity *> g_fogslates;
extern std::vector<entity *> g_fogslatesA;
extern std::vector<entity *> g_fogslatesB;

// for having items bounce
extern vector<float> g_itemsines;

extern float g_elapsed_accumulator;

extern int g_platformResolution;
extern float g_TiltResolution;

extern bool g_trifog_optimize;

extern SDL_Rect FoWrect;
extern int px;

extern bool g_protagHasBeenDrawnThisFrame;
extern bool g_protagIsBeingDetectedBySmell;
extern bool g_protagIsBeingDetectedBySight;
extern bool g_protagIsInHearingRange;

extern float oldSummationXVel;
extern float oldSummationYVel;
extern float summationXVel;
extern float summationYVel;
extern float effectiveSummationXVel;
extern float effectiveSummationYVel;
extern float savedPrecedePlayerX;
extern float savedPrecedePlayerY;
extern int maxNavCalcMs;
extern int navCalcMs;

extern navNode* precedeProtagNode;
                                      
extern bool g_loadingATM;
extern SDL_Texture *g_shadowTexture;
extern SDL_Texture *g_shadowTextureAlternate;
extern int g_flashtime;
extern float g_cameraShove;
extern float g_cameraAimingOffsetX;
extern float g_cameraAimingOffsetY;
extern float g_cameraAimingOffsetXTarget;
extern float g_cameraAimingOffsetYTarget;
extern float g_cameraAimingOffsetLerpScale;

extern string g_font;
extern float g_fontsize;
extern float g_minifontsize;
extern float g_transitionSpeed;

extern int g_whichUsableSelectedIndex;
extern vector<usable*> g_backpack;
extern float g_usableWaitToCycleTime;
extern float g_maxUsableWaitToCycleTime;
extern adventureUI* g_backpackScriptCaller;
extern entity *g_backpackNarrarator;
extern entity *g_currentMusicPlayingEntity;
extern const float g_backpackHorizontalOffset;
extern const float g_hotbarX;

extern int g_backpackIndex;
extern int g_selectingUsable;
extern float g_hotbarWidth;
extern float g_hotbarWidth_inventoryOpen;
extern float g_hotbarWidth_inventoryClosed;
extern float g_hotbarNextPrevOpacity;
extern float g_hotbarNextPrevOpacityDelta;
extern float g_hotbarLongSelectMs;
extern float g_currentHotbarSelectMs;
extern float g_hotbarCycleDirection;

extern float use_cooldown;
extern vector<attack *> AdventureattackSet;
extern int inPauseMenu;
extern bool g_firstFrameOfPauseMenu;
extern bool g_firstFrameOfSettingsMenu;
extern float g_UiGlideSpeedX;
extern float g_UiGlideSpeedY;
extern int inventoryScroll;
extern int inventorySelection;
extern int itemsPerRow;
extern int g_inventoryColumns;
extern int g_itemsInInventory;
extern int g_inventoryRows;
extern SDL_Texture* g_locked_level_texture;
extern SDL_Texture* g_levelIconEyeTexture;
extern SDL_Texture* g_levelIconBlinkTexture;
extern SDL_Texture* g_levelIconMouthTexture;
extern SDL_Texture* g_levelIconMouth2Texture;
extern textbox *inventoryText;
extern ui *inventoryMarker;
extern int oldUIUp;
extern int oldUIDown;
extern int oldUILeft;
extern int oldUIRight;
extern int SoldUIUp;
extern int SoldUIDown;
extern int SoldUILeft;
extern int SoldUIRight;
extern int g_inputDelayFrames;
extern int g_inputDelayRepeatFrames;

extern bool g_inSettingsMenu;
extern settingsUI* g_settingsUI;
extern SDL_Scancode g_swallowedKey;
extern bool g_swallowAKey;
extern bool g_awaitSwallowedKey;
extern bool g_swallowedAKeyThisFrame;
extern int g_pollForThisBinding;
extern int g_whichRebindValue;
extern const string g_affirmStr;
extern const string g_negStr;
extern const string g_leaveStr;
extern const vector<string> g_graphicsStrings;

extern bool g_inEscapeMenu;
extern escapeUI* g_escapeUI;

extern int g_blinkingMS;
extern const int g_maxBlinkingMS;
extern bool g_blinkHidden;

extern float g_gravity;
extern float g_stepHeight;
extern int g_hasBeenHoldingJump;
extern int g_jumpGaranteedAccelMs;
extern int g_maxJumpGaranteedAccelMs;
extern int lineTraceX, lineTraceY;

extern int g_maxPelletsInLevel;
extern int g_currentPelletsCollected;
extern int g_pelletsNeededToProgress;
extern vector <std::pair<int, string>> g_pelletGoalScripts;
extern bool g_showPellets;

extern entity* g_boardedEntity;
extern entity* g_formerBoardedEntity;
extern bool g_transferingByBoardable;
extern float g_transferingByBoardableTime;
extern float g_maxTransferingByBoardableTime;

extern int g_protagIsWithinBoardable;
extern int g_boardingCooldownMs;
extern const int g_maxBoardingCooldownMs;
extern int g_msSinceBoarding;

extern int g_objectiveFadeModeOn;
extern int g_objectiveOpacity;
extern int g_objectiveFadeMaxWaitMs;
extern int g_objectiveFadeWaitMs;

extern adventureUI* g_pelletGoalScriptCaller;
extern entity *g_pelletNarrarator;

extern int WIN_WIDTH;
extern int WIN_HEIGHT;
extern const int STANDARD_SCREENWIDTH;
extern int old_WIN_WIDTH;
extern int saved_WIN_WIDTH;
extern int saved_WIN_HEIGHT;
extern SDL_Window *window;
extern SDL_DisplayMode DM;
extern bool g_fullscreen;
extern camera g_camera;
extern entity *protag;
extern entity *mainProtag;
extern entity * g_hog;
extern entity* g_behemoth0;
extern entity* g_behemoth1;
extern entity* g_behemoth2;
extern entity* g_behemoth3;
extern vector<entity*> g_behemoths;

extern float g_defaultZoom;
extern float g_zoom_mod;
extern bool g_update_zoom;
extern float scalex;
extern float scaley;
extern float min_scale;
extern float max_scale;

extern entity *g_focus;
extern entity *g_objective;
extern bool g_usingPelletsAsObjective;
extern entity *narrarator;
extern vector<entity *> party;
extern float g_max_framerate;
extern float g_min_frametime;
extern SDL_Event event;
extern float ticks, lastticks, elapsed, halfsecondtimer;
extern float camx;
extern float camy;
extern SDL_Renderer *renderer;

extern string g_map;
extern string g_mapdir;
extern string g_waypoint;
extern string g_mapOfLastSave;
extern string g_waypointOfLastSave;

extern const Uint8 *keystate;
extern bool devinput[60];
extern bool g_ignoreInput;

extern bool input[16];
extern bool oldinput[16];

extern bool staticInput[5];
extern bool oldStaticInput[5];

extern SDL_Scancode bindings[16];
extern bool left_ui_refresh;
extern bool right_ui_refresh;
extern bool fullscreen_refresh;
extern bool quit;
extern string g_config;
extern bool g_holdingCTRL;
extern bool g_holdingTAB;
extern const int g_diagonalHelpFrames;
extern int g_cur_diagonalHelpFrames;

extern float g_volume;
extern float g_music_volume;
extern float g_sfx_volume;
extern bool g_mute;

extern vector<std::pair<Mix_Chunk*,string>> g_preloadedSounds;
extern Mix_Chunk *g_ui_voice;
extern Mix_Chunk *g_menu_open_sound;
extern Mix_Chunk *g_menu_close_sound;
extern Mix_Chunk *g_menu_manip_sound;
extern Mix_Chunk *g_pelletCollectSound;
extern Mix_Chunk *g_spiketrapSound;
extern Mix_Chunk *g_bladetrapSound;
extern Mix_Chunk *g_smarttrapSound;
extern Mix_Chunk *g_trainReadySound;

extern Mix_Chunk *g_land;
extern Mix_Chunk *g_footstep_a;
extern Mix_Chunk *g_footstep_b;
extern Mix_Chunk *g_bonk;

extern Mix_Chunk *g_deathsound;
extern musicNode *g_closestMusicNode;
extern musicNode *newClosest;

extern int g_musicSilenceMs;
extern int g_currentMusicSilenceMs;
extern int g_musicSilenceFadeMs;

extern int musicFadeTimer;
extern bool fadeFlag;
extern bool entFadeFlag;
extern int musicUpdateTimer;
extern Mix_Chunk *g_bulletdestroySound;
extern Mix_Chunk *g_cannonfireSound;
extern Mix_Chunk *g_playerdamage;
extern Mix_Chunk *g_enemydamage;
extern Mix_Chunk *g_npcdamage;
extern Mix_Chunk *g_s_playerdeath;

extern std::map<string, Mix_Chunk> g_static_sounds;

extern int g_textDropShadowColor;
extern float g_textDropShadowDist;
extern bool protag_can_move;
extern int protag_is_talking;
extern adventureUI *adventureUIManager;
extern float textWait;
extern float text_speed_up;
extern float curTextWait;
extern bool old_z_value;
extern float g_healthbarBorderSize;
extern bool g_showHUD;
extern bool g_inTitleScreen;

extern int g_foodpoints;
extern int g_maxFoodpoints;
extern int g_maxVisibleFoodpoints;
extern int g_foodpointsDecreaseIntervalMs;
extern int g_foodpointsDecreaseAmount;
extern int g_currentFoodpointsDecreaseIntervalMs;
extern int g_starvingFoodpoints;

extern bool g_inventoryUiIsLevelSelect;

extern bool g_inventoryUiIsKeyboard;
extern string g_keyboardInput;

extern string g_alphabet;
extern string g_alphabet_lower;
extern string g_alphabet_upper;

extern string g_fancyAlphabetChars;

extern std::map<int, std::pair<SDL_Texture*, float>> g_fancyAlphabet;
extern std::map<char, int> g_fancyCharLookup;

extern fancybox* g_fancybox;

extern vector<SDL_Texture*>* g_alphabet_textures;
extern vector<SDL_Texture*> g_alphabetLower_textures;
extern vector<float> g_alphabet_widths;
extern vector<SDL_Texture*> g_alphabetUpper_textures;
extern vector<float> g_alphabet_upper_widths;

extern int g_keyboardInputLength;
extern string g_keyboardSaveToField;
extern SDL_Color g_textcolor;
extern SDL_Color g_healthtextcolor;
extern SDL_Color g_healthtextlowcolor;

extern string g_levelSequenceName;
extern levelSequence* g_levelSequence;
extern int g_score;

extern float dialogue_cooldown;
extern entity *g_talker;
extern bool g_forceEndDialogue;

extern SDL_Texture *nodeDebug;
extern clock_t debugClock;
extern string g_lifecycle;
extern ui* g_dijkstraDebugRed;
extern ui* g_dijkstraDebugBlue;
extern ui* g_dijkstraDebugYellow;
extern entity* g_dijkstraEntity;
extern bool g_ninja;

extern int fdebug;

extern int g_layers;
extern int g_numberOfInterestSets;
extern string g_first_map;

extern bool g_mousemode;
extern bool keyboard_marker_vertical_modifier_refresh;
extern bool keyboard_marker_vertical_modifier_refresh_b;

extern entity *nudge;
extern bool adjusting;
extern bool g_autoSetThemesFromMapDirectory;

extern string g_saveName;

extern std::map<string, int> g_save;
extern std::map<string, string> g_saveStrings;

extern float g_bhoppingBoost;
extern float g_defaultBhoppingBoost;
extern float g_maxBhoppingBoost;
extern float g_deltaBhopBoost;
extern int protagConsecutiveBhops;

extern float g_jump_afterslow;
extern float g_jump_afterslow_seconds;

extern bool g_spin_enabled;
extern entity* g_spin_entity;
extern float g_spin_cooldown;
extern float g_spin_max_cooldown;
extern float g_spinning_duration;
extern float g_spinning_duration_max;
extern float g_afterspin_duration;
extern float g_afterspin_duration_max;
extern float g_spinning_xvel;
extern float g_spinning_yvel;
extern float g_spinning_boost;
extern float g_doubleSpinHelpMs;
extern float g_spinJumpHelpMs;
extern float g_currentSpinJumpHelpMs;
extern bool g_protag_jumped_this_frame;

extern bool storedJump;
extern bool storedSpin;

extern ofstream ofile;
extern bool olddevinput[60];
extern bool makingtile, makingbox, makingdoor;
extern int lx, ly, rx, ry;
extern float grid;
extern int width, height;
extern int selectIndex;
extern bool tiling;
extern bool drawhitboxes;
extern bool drawNavMesh;
extern bool drawNavMeshEdges;
                           
extern int debug_r, debug_g, debug_b;
extern int shine;
extern bool occlusion;
extern int wallheight;
extern int wallstart;
extern bool showMarker;
extern int lastAction;
extern float navMeshDensity;
extern int limits[4];
extern int m_enemyPoints;
extern string textureDirectory;
extern float mapeditorNavNodeCullRadius;
extern float mapeditorNavNodeTraceRadius;

extern vector<string> consolehistory;
extern int consolehistoryindex;

extern string captex;
extern string walltex;
extern string floortex;
extern string masktex;
extern vector<string> texstrs;
extern int captexIndex;
extern int walltexIndex;
extern int floortexIndex;
extern ui *captexDisplay;
extern ui *walltexDisplay;
extern ui *floortexDisplay;

extern entity *selectPointer;
extern int layer;
extern bool autoMakeWalls;
extern bool makeboxs;
extern bool autoMakeWallcaps;

extern tile *selection;
extern tile *markerz;
extern tile *marker;
extern tile *navNodeIconBlue;
extern tile *navNodeIconRed;
extern tile *navNodeIconYellow;
extern tile *worldsoundIcon;
extern tile *listenerIcon;
extern tile *musicIcon;
extern tile *cueIcon;
extern tile *waypointIcon;
extern tile *poiIcon;
extern tile *doorIcon;
extern tile *triggerIcon;
extern textbox *nodeInfoText;
extern string entstring;

extern string mapname;
extern string backgroundstr;

extern float g_earshot;

extern vector<int> g_creepyLocks;

bool fileExists(const std::string &name);

void playSound(int channel, Mix_Chunk *sound, int loops);


SDL_Texture *MaskTexture(SDL_Renderer *renderer, SDL_Texture *mask, SDL_Texture *diffuse);

float Distance(int x1, int y1, int x2, int y2);

float XYDistance(int x1, int y1, int x2, int y2);

// get cardinal points about a position
//  0 is 12oclock, and 2 is 3oclock and so on
vector<int> getCardinalPoint(int x, int y, float range, int index);

// convert frame of sprite to angle
float convertFrameToAngle(int frame, bool flipped);

//idk why i have two
//this is for steeringAngle
//the only difference is that "flipped" is treated oppositely
float convertFrameToAngleNew(int frame, bool flipped);

//wrap an angle so that it is within the range of 0 and 2pi radians
float wrapAngle(float input);

// convert an angle to a sprite's frame, for eight-frame sprites (arms)
int convertAngleToFrame(float angle);

// measures distance in the world, not by the screen.
float XYWorldDistance(int x1, int y1, int x2, int y2);

//faster, use this if you can
float XYWorldDistanceSquared(int x1, int y1, int x2, int y2);

vector<string> splitString(string s, char delimiter);

bool replaceString(std::string &str, const std::string &from, const std::string &to);

int yesNoPrompt(string msg);

int rng(int min, int max);


#endif
