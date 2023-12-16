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

vector<cshadow *> g_shadows;

vector<entity *> g_entities;

vector<entity *> g_boardableEntities;

vector<entity*> g_ai;

vector<entity *> g_pellets; //for making pellets bounce and squash

vector<entity *> g_solid_entities;

vector<entity *> g_large_entities;

vector<tile *> g_tiles;

vector<door *> g_doors;

vector<vector<box *>> g_boxs;

vector<impliedSlope *> g_impliedSlopes; //slopes which are implied to be behind walls, preventing entities from "hiding" behind them, as in zelda
                                        
vector<impliedSlopeTri *> g_impliedSlopeTris; //same as g_impliedSlopes, but for triangular slopes

vector<textbox *> g_textboxes;

vector<ui *> g_ui;

vector<actor *> g_actors;

vector<mapObject *> g_mapObjects;

vector<mapCollision *> g_mapCollisions;

vector<vector<tri *>> g_triangles;

vector<vector<ramp *>> g_ramps;

vector<heightmap *> g_heightmaps;

vector<navNode *> g_navNodes;

vector<vector<pointOfInterest *>> g_setsOfInterest;

vector<effectIndex *> g_effectIndexes;

struct cmpCoord
{
  bool operator()(const pair<int, int> a, const pair<int, int> b) const
  {
    return a.first + a.second < b.first + b.second;
  }
};

map<pair<int, int>, navNode *, cmpCoord> navNodeMap;

vector<worldsound *> g_worldsounds;

vector<musicNode *> g_musicNodes;

vector<cueSound *> g_cueSounds;

vector<waypoint *> g_waypoints;

vector<trigger *> g_triggers;

vector<listener *> g_listeners;

vector<projectile *> g_projectiles;

vector<attack *> g_attacks;

vector<weapon *> g_weapons;

vector<worldItem *> g_worldItems;

vector<indexItem *> g_indexItems;

vector<particle *> g_particles;

vector<emitter *> g_emitters;

vector<collisionZone *> g_collisionZones;

vector<entity *> g_musicalEntities;
entity *g_currentMusicPlayingEntity = 0;

map<string, int> enemiesMap; // stores (file,cost) for enemies to be spawned procedurally in the map
int g_budget = 0;						 // how many points this map can spend on enemies;

bool boxsenabled = 1; // affects both map editor and full game. Dont edit here
bool g_collisionResolverOn = 1; //also referred to as "jiggling" ents out of solid collisions
bool g_showCRMessages = 0; //collisionResolver messages

bool onionmode = 0; // hide custom graphics


//I worked to make implied slopes and triangular slopes to work realistically, in 
//that they would push the player away like walls but if the player jumped they could
//jump on top of the slope, and that would be best for 3d levels
//since I'm not making 3d levels and they're too demanding for users to play anyways
//I'm simplifying the code which considers collisions with IS and IST
//now they'll just be like regular geometry
//if you ever want to reenable this, it should be easier to toggle this value
const bool g_useSimpleImpliedGeometry = 1;


bool genericmode = 0;
bool freecamera = 0;
bool devMode = 0;
bool canSwitchOffDevMode = 0;
bool inputRefreshCanSwitchOffDevMode = 0;
bool showDevMessages = 1;
bool showErrorMessages = 1;
bool showImportantMessages = 1;

// quick debug info
#define D(a)                                \
  if (canSwitchOffDevMode && showDevMessages)           \
{                                         \
  std::cout << #a << ": " << (a) << endl; \
}

// like D, but dont do newline
#define Dnn(a) \
  if (canSwitchOffDevMode && showDevMessages)           \
{                                         \
  std::cout << #a << ": " << (a); \
}

//print a spacing symbol, no newline
#define Snn() \
  if (canSwitchOffDevMode && showDevMessages)           \
{                                         \
  std::cout << "  "; \
}


#define ID(a)                                \
{                                         \
  std::cout << #a << ": " << (a) << endl; \
}

  template <typename T>
void M(T msg, bool disableNewline = 0)
{
  if (!canSwitchOffDevMode || !showDevMessages || 0)
  {
    return;
  }
  cout << msg;
  if (!disableNewline)
  {
    cout << endl;
  }
}

// particularly for errors
  template <typename T>
void E(T msg, bool disableNewline = 0)
{
  if (!canSwitchOffDevMode || !showErrorMessages || 0)
  {
    return;
  }
  cout << "ERROR: " << msg;
  if (!disableNewline)
  {
    cout << endl;
  }
}

// Particularly important stuff, like loadtimes
  template <typename T>
void I(T msg, bool disableNewline = 0)
{
  if (!showImportantMessages)
  {
    return;
  }
  cout << "I: " << msg;
  if (!disableNewline)
  {
    cout << endl;
  }
}

// Temporary debugging statements- I won't allow myself to block these
#define T(a) std::cout << #a << ": " << (a) << endl;

void breakpoint()
{
  //I("First breakpoint");
  return;
}

void breakpoint2()
{
  //I("Second breakpoint");
  return;
}

  template<typename T>
T limit(T input, T min, T max)
{
  if(input > max) {return max;}
  if(input < min) {return min;}
  return input;
}

// for visuals
float p_ratio = 1.151;
float g_brightness_setting = 100;		 // from 0 to 100, 0 is what is normal to me
float g_brightness_map_factor = 0; // this is meant to be set in the mapeditor so some maps can be darker
SDL_Texture* g_shade = 0;  //drawn to darken the frame uniformally
bool g_vsync = true;
float g_background_darkness = 0; // 0 - show bg, 1 - show black
SDL_Texture *background = 0;
bool g_backgroundLoaded = 0;
bool g_useBackgrounds = 1; // a user setting, if the user wishes to see black screens instead of colorful backgrounds
int g_brightness = 100; // brightness of map
// x length times x_z_ratio is proper screen length in z
float XtoZ = 0.496; // 4/2.31, arctan (4/ 3.21) = 60 deg
float XtoY = 0.866;
float YtoX = 1/XtoY;
float g_ratio = 1.618;
bool transition = 0;
int g_walldarkness = 55;			// 65, 75. could be controlled by the map unless you get crafty with recycling textures across maps
bool g_unlit = 0;							// set to 1 if the user has the lowest graphical setting, to disable lighting in maps for performance. Don't eh, don't dev like this
int g_graphicsquality = 3;		// 0 is least, 4 is max
float g_extraShadowSize = 20; // how much bigger are shadows in comparison to their hitboxes.
int g_fogofwarEnabled = 1;
int g_fogofwarRays = 100;
bool g_showHealthbar = 0;
effectIndex *smokeEffect;
effectIndex *littleSmokeEffect;
effectIndex *blackSmokeEffect;
effectIndex *sparksEffect;

particle* g_lastParticleCreated = 0;

// for fow
SDL_Texture *result;
SDL_Texture *result_c;
SDL_Texture *canvas;
SDL_Texture *canvas_fc;
SDL_Texture *light;

SDL_Texture *lighta;
SDL_Texture *lightb;
SDL_Texture *lightc;
SDL_Texture *lightd;

SDL_Texture *lightaro;
SDL_Texture *lightbro;
SDL_Texture *lightcro;
SDL_Texture *lightdro;

SDL_Texture *lightari;
SDL_Texture *lightbri;
SDL_Texture *lightcri;
SDL_Texture *lightdri;

SDL_Texture *TextureA;
SDL_Texture *TextureB;
SDL_Texture *TextureC;
SDL_Texture *TextureD;
SDL_Texture *blackbarTexture;

int g_fogheight = 19;
int g_fogwidth = 21;
int g_lastFunctionalX = 0; // for optimizing the FoW calcs
int g_lastFunctionalY = 0;
int g_fogMiddleX = 10;
int g_fogMiddleY = 9;
float g_viewdist = 310; // 240, 310 is casual, 340 could be from an upgrade.   it was 310 500 is max
//instead of calculating distance every frame, lookup which boxes should be open in a table

std::vector<std::vector<int>>g_fog_window = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};


//for debugging fog-screen alignment
//std::vector<std::vector<int>>g_fog_window = {
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//};



bool g_fogIgnoresLOS = 0;
int g_tile_fade_speed = 90; // 40, 50
int xtileshift = 0;
int ytileshift = 0;
bool g_force_cookies_update = 0;

std::vector<std::vector<int>> g_fogcookies(g_fogwidth, std::vector<int>(g_fogheight));
// this second vector is for storing the cookies that are ontopof walls
// that way, we can draw too layers of shadow before actors and one after
std::vector<std::vector<int>> g_savedcookies(g_fogwidth, std::vector<int>(g_fogheight));

// data for two passes of cookies
std::vector<std::vector<int>> g_fc(g_fogwidth, std::vector<int>(g_fogheight));
std::vector<std::vector<int>> g_sc(g_fogwidth, std::vector<int>(g_fogheight));
std::vector<std::vector<int>> g_shc(g_fogwidth, std::vector<int>(g_fogheight));

// fogslates - 19 x 2 = 38 actors.
std::vector<entity *> g_fogslates;
std::vector<entity *> g_fogslatesA;
std::vector<entity *> g_fogslatesB;

// for having items bounce
vector<float> g_itemsines;

float g_elapsed_accumulator = 0;

// I've bounced around thinking these matter and turning them down
// or deciding that they don't matter and pumping them up
// Here's what I know atm: the first value should be left at 11 prettymuch always
// 2 is okay - g_TiltResolution actually doesn't affect loading
// but it will effect CPU usage, particularly when the triangles are onscreen
// 2 or 4 for large maps, seems okay 1 is more detail than I think anyone needs.
int g_platformResolution = 11; // a factor of 55. 11 is fine. USE 11
float g_TiltResolution = 4;		 // 1, 2, 4, 16 //what size step to use for triangular walls, 2 is almost unnoticable. must be a factor of 64 USE 4

// triangular walls are the most expensive things to draw, so here s an idea:
// when this variable is set to 1, hide triangular walls which are behind fogofwar.
//  !!! this feature is incomplete
bool g_trifog_optimize = 0;

// these are nonparticular values for drawing the blackbars
SDL_Rect FoWrect;
int px;

bool g_protagHasBeenDrawnThisFrame = 0;
bool g_protagIsBeingDetectedBySmell = 0; //used for when the protag is too close to an enemy and will soon agro that enemy
bool g_protagIsBeingDetectedBySight = 0;
bool g_protagIsInHearingRange = 0;

float oldSummationXVel = 0;
float oldSummationYVel = 0;
float summationXVel = 0;
float summationYVel = 0;
float effectiveSummationXVel = 0;
float effectiveSummationYVel = 0;
float savedPrecedePlayerX = 0;
float savedPrecedePlayerY = 0;
int maxNavCalcMs = 4000;
int navCalcMs = 0; //counts up to maxPrecedeCalcMs, and at that point, resets summationNVel. Old values are passed to oldSummationNVel

//so these ones are quite important, these are for
//precisely how far ahead the monster wishes to plan,
//how far the 

navNode* precedeProtagNode = nullptr; //set in main loop, used by some monsters.
                                      
bool g_loadingATM = 0; // are we loading a new map atm?
SDL_Texture *g_shadowTexture;
SDL_Texture *g_shadowTextureAlternate;
int g_flashtime = 300;		 // ms to flash red after taking damage
float g_cameraShove = 150; // factor of the screen to move the camera when shooting.
float g_cameraAimingOffsetX = 0;
float g_cameraAimingOffsetY = 0;
float g_cameraAimingOffsetXTarget = 0;
float g_cameraAimingOffsetYTarget = 0;
float g_cameraAimingOffsetLerpScale = 0.91;

// text
string g_font;
float g_fontsize = 0.031; // 0.021 - 0.04
float g_minifontsize = 0.01;
float g_transitionSpeed = 3; // 3, 9

//backpack - holds usable items on a hotbar
int g_whichUsableSelectedIndex = 0;
vector<usable*> g_backpack;
float g_usableWaitToCycleTime = 0;
float g_maxUsableWaitToCycleTime = 100;
adventureUI* g_backpackScriptCaller = nullptr;
entity *g_backpackNarrarator; //script callers should have thier own ent to not mess up dialogue indexes (dumb!)
const float g_backpackHorizontalOffset = 0.35;
const float g_hotbarX = 0.45; //don't change this, to move it horizontally change g_backpackHorizontalOffset

int g_backpackIndex = 0; //set to 1 if the player is holding down the button
int g_selectingUsable = 0;
//the hotbar widens when the inventory button is held
float g_hotbarWidth = 0.1;
float g_hotbarWidth_inventoryOpen = 0.3;
float g_hotbarWidth_inventoryClosed = 0.1;
float g_hotbarNextPrevOpacity = 25500; //for fading out next/prev icons
float g_hotbarNextPrevOpacityDelta = 250;
//a short press of the inventory button will advance the backpack index, but
//a longer one will let player select with the movement keys
float g_hotbarLongSelectMs = 150;
float g_currentHotbarSelectMs = 0;
float g_hotbarCycleDirection = 0;

// inventory - we're switching things up. This will be the picnic-box, the inventory for consumables
float use_cooldown = 0; // misleading, its not for attacks at all
vector<attack *> AdventureattackSet;
int inPauseMenu = 0;
bool g_firstFrameOfPauseMenu = 0; //true if this is the first frame of the pause menu, for aligning the cursor
bool g_firstFrameOfSettingsMenu = 0; //true if this is the first frame of the pause menu, for aligning the cursor
float g_UiGlideSpeedX = 0.012;
float g_UiGlideSpeedY; //set from g_UiGlideSpeedX based on aspect ratio
int inventoryScroll = 0;		// how many rows in the inventory we've scrolled thru
int inventorySelection = 1; // which item in the inventory is selected
int itemsPerRow = ceil((0.9 - 0.05) / (0.07 + 0.01));
int g_inventoryColumns = ceil((0.74 - 0.05) / 0.07);
int g_itemsInInventory = 0;
int g_inventoryRows = 4;
SDL_Texture* g_locked_level_texture;
textbox *inventoryText; //showing numbers by items
ui *inventoryMarker; //indicates inventory selection
// for not counting extra presses in UI for shooting and moving axisen
int oldUIUp = 1;
int oldUIDown = 1;
int oldUILeft = 1;
int oldUIRight = 1;
int SoldUIUp = 1;
int SoldUIDown = 1;
int SoldUILeft = 1;
int SoldUIRight = 1;
int g_inputDelayFrames = 30;      //For the nice effect where the user can press a button or hold it
int g_inputDelayRepeatFrames = 6;

//options / settings menu
bool g_inSettingsMenu = 0;
settingsUI* g_settingsUI;
SDL_Scancode g_swallowedKey; //for swallowing a key from a polled event
bool g_swallowAKey = 0;          //set this to one to swallow the next key pressed
bool g_awaitSwallowedKey = 0;
bool g_swallowedAKeyThisFrame = 0;
int g_pollForThisBinding = -1;
int g_whichRebindValue;
const string g_affirmStr = "Yes";
const string g_negStr = "No";
const string g_leaveStr = "Lets Leave";
const vector<string> g_graphicsStrings = {"Very Low", "Low", "Medium", "High"};

//escape menu
bool g_inEscapeMenu = 0;
escapeUI* g_escapeUI;

//blinking text
int g_blinkingMS = 0;
const int g_maxBlinkingMS = 1000;
bool g_blinkHidden = 0; //alternates every maxBlinkingMS


// physics
float g_gravity = 220;
float g_stepHeight = 15;
int g_hasBeenHoldingJump = 0; //for letting the player hold jump to go higher
int g_jumpGaranteedAccelMs = 0; 
int g_maxJumpGaranteedAccelMs = 150; //for x ms protag is garanteed to accelerate, was 150 
// These two variables contain the position of the hit of the last lineTrace()
int lineTraceX, lineTraceY;

// gameplay loop
int g_maxPelletsInLevel = 0; //number of pellets loaded into the level
int g_currentPelletsCollected = 0; //how many they have
int g_pelletsNeededToProgress = 1; //player has beaten the level, just needs to leave
vector <std::pair<int, string>> g_pelletGoalScripts; //each entry contains the path to a script which
bool g_showPellets = 1;

//for the boarding to entities
entity* g_boardedEntity = 0;
entity* g_formerBoardedEntity = 0; //when transfering, this points to the entity were are transfering from
bool g_transferingByBoardable = 0; //set to true when protag entered a transfering boardable ent, and set to false when the protag has reached the other entity
float g_transferingByBoardableTime = 0; //ms left before arriving at the destination boardable.
float g_maxTransferingByBoardableTime = 0; //set at the same time g_transferingByBoardableTime is, and used in comparison to set protag's position (even though they won't see anything, they will see the indicators and it will also affect audio)

int g_protagIsWithinBoardable = 0;
int g_boardingCooldownMs = 0;
const int g_maxBoardingCooldownMs = 2000;
//for animating the boardable
int g_msSinceBoarding = 0; //counts up from 0 after boarding

//I want the objective to be able to be set to fade away if the player is moving and 
//fade in if the player is standing still, to help them if they are lost
int g_objectiveFadeModeOn = 0;
int g_objectiveOpacity = 25500;
int g_objectiveFadeMaxWaitMs = 2000;
int g_objectiveFadeWaitMs = 0;

//for security, scriptcallers need their own entity
//It's kinda dumb but just go with it
adventureUI* g_pelletGoalScriptCaller = nullptr;
entity *g_pelletNarrarator;

                                     

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

    camera(float fx, float fy)
    {
      fx = x;
      fy = y;
    }
    void update_movement(float elapsed, float targetx, float targety)
    {
      if (!isfinite(targetx) || !isfinite(targety))
      {
        return;
      }

      if (lag == 0)
      {
        x = targetx;
        y = targety;
      }
      else
      {
        x += (targetx - oldx) * (elapsed / 256) * lag;
        y += (targety - oldy) * (elapsed / 256) * lag;

        oldx = x;
        oldy = y;
        // if we're there, within a pixel, set the lagResetTimer to nothing
        if (abs(targetx - x) < 1.4 && abs(targety - y) < 1.4)
        {
          lag = 0;
        }
        else
        {
          // if not, consider increasing lag to catch up
          lag += lagaccel;
        }
      }

      if (enforceLimits)
      {
        if (x < lowerLimitX)
        {
          x = lowerLimitX;
        }
        if (y < lowerLimitY)
        {
          y = lowerLimitY;
        }

        if (x + width > upperLimitX)
        {
          x = upperLimitX - width;
        }
        if (y + height > upperLimitY)
        {
          y = upperLimitY - height;
        }
      }
    }

    void resetCamera()
    {
      enforceLimits = 0;
      lowerLimitX = 0;
      lowerLimitY = 0;
      upperLimitX = 0;
      upperLimitY = 0;
    }
};

// zoom is really g_defaultZoom when screenwidth is STANDARD_SCREENWIDTH
int WIN_WIDTH = 640;
int WIN_HEIGHT = 480;
// theres some warping if STANDARD_SCREENWIDTH < WIN_WIDTH but that shouldn't ever happen
// if in the future kids have screens with 10 million pixels across feel free to mod the game
const int STANDARD_SCREENWIDTH = 1080;
//int WIN_WIDTH = 1280; int WIN_HEIGHT = 720;
// int WIN_WIDTH = 640; int WIN_HEIGHT = 360;
int old_WIN_WIDTH = 0; // used for detecting change in window width to recalculate scalex and y
int saved_WIN_WIDTH = WIN_WIDTH;
int saved_WIN_HEIGHT = WIN_HEIGHT;
SDL_Window *window;
SDL_DisplayMode DM;
bool g_fullscreen = false;
camera g_camera(0, 0);
entity *protag;
entity *mainProtag; // for letting other entities use this ones inventory; game ends when this one dies
entity * g_hog=0;
entity* g_behemoth0=0;
entity* g_behemoth1=0;
entity* g_behemoth2=0;
entity* g_behemoth3=0;
vector<entity*> g_behemoths;

// zoom is planned to be 1.0 for a resolution of 1920 pixels across
float g_defaultZoom = 0.85;
float g_zoom_mod = 1;		// for devmode
bool g_update_zoom = 0; // update the zoom this frame

float scalex = 1;
float scaley = scalex;
float min_scale = 0.000000000001;
float max_scale = 2.4;

entity *g_focus;
entity *g_objective = 0;
bool g_usingPelletsAsObjective = 0; //can be set by scripts, is unset when another objective is set. Makes is so that the objective is set to a pellet and when the player collects it, it is set to another pellet
entity *narrarator;
vector<entity *> party;
float g_max_framerate = 120;
float g_min_frametime = 1 / g_max_framerate * 1000;
SDL_Event event;
float ticks, lastticks, elapsed = 0, halfsecondtimer;
float camx = 0;
float camy = 0;
SDL_Renderer *renderer;

// g_map specifies the name of the map, g_mapdir specifies the folder with in maps the map is in.
// so its maps/{g_mapdir}/{g_map}.map
string g_map = "sp-title";
string g_mapdir = "sp-title";
string g_waypoint;
string g_mapOfLastSave = "sp-title";
string g_waypointOfLastSave = "a";

// input
const Uint8 *keystate = SDL_GetKeyboardState(NULL);
bool devinput[60] = {false};
bool g_ignoreInput = 1;

//these are meant to make it easy to port bindable inputs
bool input[16] = {false};
bool oldinput[16] = {false};

//these are for UNbindable inputs, which are for using the settings menu
//that way, the player can't be stuck with impossible binds
bool staticInput[5] = {false};
bool oldStaticInput[5] = {false};

SDL_Scancode bindings[16];
bool left_ui_refresh = false; // used to detect when arrows is pressed and then released
bool right_ui_refresh = false;
bool fullscreen_refresh = true;
bool quit = false;
string g_config = "default";
bool g_holdingCTRL = 0;
bool g_holdingTAB = 0;
// this is most noticable with a rifle, but sometimes when you try to shoot
// diagonally, you press one button (e.g. up) a frame or so early then the other (e.g. left)
// as a result, the game instantly shoots up and its unnacceptable.
// this is variable is the amount of frames to wait between getting input and shooting
const int g_diagonalHelpFrames = 4;
int g_cur_diagonalHelpFrames = 0;

// sounds and music
float g_volume = 0;
float g_music_volume = 0.8;
float g_sfx_volume = 1;
bool g_mute = 0;

vector<std::pair<Mix_Chunk*,string>> g_preloadedSounds;
Mix_Chunk *g_ui_voice;
Mix_Chunk *g_menu_open_sound;
Mix_Chunk *g_menu_close_sound;
Mix_Chunk *g_menu_manip_sound;
Mix_Chunk *g_pelletCollectSound;
Mix_Chunk *g_spiketrapSound;
Mix_Chunk *g_bladetrapSound;
Mix_Chunk *g_smarttrapSound;
Mix_Chunk *g_trainReadySound;

Mix_Chunk *g_land;
Mix_Chunk *g_footstep_a;
Mix_Chunk *g_footstep_b;
Mix_Chunk *g_bonk;

Mix_Chunk *g_deathsound;
musicNode *g_closestMusicNode;
musicNode *newClosest;

int g_musicSilenceMs = 0; //this is set by scripts to fade music out for x ms
int g_currentMusicSilenceMs = 0;
int g_musicSilenceFadeMs = 200; // if g_musicSilenceMs == 5000, than we spend 1000ms fading out and then 1000ms fading in

int musicFadeTimer = 0;
bool fadeFlag = 0; // for waiting between fading music in and out
bool entFadeFlag = 0;
int musicUpdateTimer = 0;
Mix_Chunk *g_bulletdestroySound;
Mix_Chunk *g_cannonfireSound;
Mix_Chunk *g_playerdamage;
Mix_Chunk *g_enemydamage;
Mix_Chunk *g_npcdamage;
Mix_Chunk *g_s_playerdeath;

std::map<string, Mix_Chunk> g_static_sounds = {};

// ui
int g_textDropShadowColor = 100;
float g_textDropShadowDist = 0.04; //this is the pixels of the texture, for better or worse
bool protag_can_move = true;
int protag_is_talking = 0; // 0 - not talking 1 - talking 2 - about to stop talking
adventureUI *adventureUIManager;
float textWait = 30;		 // seconds to wait between typing characters of text
float text_speed_up = 1; // speed up text if player holds button. set to 1 if the player isn't pressing it, or 1.5 if she is
float curTextWait = 0;
bool old_z_value = 1; // the last value of the z key. used for advancing dialogue, i.e. z key was down and is now up or was up and is now down if old_z_value != SDL[SDL_SCANCODE_Z]
float g_healthbarBorderSize = 0;
bool g_showHUD = 1;

//for hungersystem
int g_foodpoints = 100;
int g_maxFoodpoints = 150; //150 foodpoints means the player can't possibly eat more, but they won't realize they're hungry until they get down to 100 points (they're wellfed, which is good)
int g_maxVisibleFoodpoints = 100;
int g_foodpointsDecreaseIntervalMs = 1000;
int g_foodpointsDecreaseAmount = 1;
int g_currentFoodpointsDecreaseIntervalMs = 0;
int g_starvingFoodpoints = 20; //need 20 or more foodpoints to use items

bool g_inventoryUiIsLevelSelect = 0; //set to 1 to repurpose to inventory UI for level select UI

bool g_inventoryUiIsKeyboard = 0; //set to 1 to repurpose to inventory UI for player string input
string g_keyboardInput = "";

string g_alphabet = "abcdefghijklmnopqrstuvwxyz<^;";
string g_alphabet_lower = "abcdefghijklmnopqrstuvwxyz<^;";
string g_alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ<^;";

string g_fancyAlphabetChars = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz !@#$%^&*()+-_;,\"\'";

//the pair contains a texture and a width
std::map<int, std::pair<SDL_Texture*, float>> g_fancyAlphabet{};
//here's a map of chars to ints, not containing special characters which are handled later
std::map<char, int> g_fancyCharLookup;

fancybox* g_fancybox;

vector<SDL_Texture*>* g_alphabet_textures;
vector<SDL_Texture*> g_alphabetLower_textures;
vector<float> g_alphabet_widths;
vector<SDL_Texture*> g_alphabetUpper_textures;
vector<float> g_alphabet_upper_widths;

int g_keyboardInputLength = 12;
string g_keyboardSaveToField = ""; //the save-field to write keyboard input to, when ready
SDL_Color g_textcolor = { 155, 115, 115 };
SDL_Color g_healthtextcolor = { 220, 203, 25 };
SDL_Color g_healthtextlowcolor = { 25, 25, 220 };

string g_levelSequenceName = "default"; //use the default level sequence for the base game by default
levelSequence* g_levelSequence;
int g_score; //generally used for unlocking levels, has checks/sets in scripting system and its own hud element

// scripts
float dialogue_cooldown = 0; // seconds until he can have dialogue again.
entity *g_talker = 0;				 // drives scripts, must be referenced before deleting an entity to avoid crashes
bool g_forceEndDialogue = 0; // used to end dialogue when the talking entity has been destroyed.

// debuging
SDL_Texture *nodeDebug;
clock_t debugClock;
string g_lifecycle = "Alpha";
ui* g_dijkstraDebugRed;
ui* g_dijkstraDebugBlue;
ui* g_dijkstraDebugYellow;
entity* g_dijkstraEntity;
bool g_ninja = 0;

//temporary debug stuff
int fdebug = -1;

// world
int g_layers = 12;							 // max blocks in world
int g_numberOfInterestSets = 50; // number of individual sets of pointsOfInterest available for entities to use
string g_first_map = "maps/first/1.map";

// map editing, mapeditor, map-editor
bool g_mousemode = 1;
bool keyboard_marker_vertical_modifier_refresh = 0;
bool keyboard_marker_vertical_modifier_refresh_b = 0;

entity *nudge = 0;												// for nudging entities while map-editing
bool adjusting = 0;												// wether to move selected entity or change its hitbox/shadow position
bool g_autoSetThemesFromMapDirectory = 0; // if 1, loading a map will also set the texturedirectory/theme to the mapdir

// userdata - will be set on some file-select-screen
string g_saveName = "a";

std::map<string, int> g_save = {};
std::map<string, string> g_saveStrings = {};

// AI
enum travelstyle
{
  roam,
  patrol
};

// movement

float g_bhoppingBoost = 0; // the factor applied to friction whilst airborn
float g_defaultBhoppingBoost = 1;
float g_maxBhoppingBoost = 1;
float g_deltaBhopBoost = 1;
int protagConsecutiveBhops = 0; //increased for every successive bhop

float g_jump_afterslow = 0;
float g_jump_afterslow_seconds = 0; //make it so that the longer you bhop the longer you are slowed

bool g_spin_enabled = 1;
entity* g_spin_entity = nullptr;
float g_spin_cooldown = 400;
float g_spin_max_cooldown = 100; //100, for spinning at will
float g_spinning_duration = 0;
float g_spinning_duration_max = 400; //400, for spinning at will
float g_afterspin_duration = 0;
float g_afterspin_duration_max = 200; //200, for spinning at will duration of afterspin imobility
float g_spinning_xvel = 0; //x and y velocities are locked upon starting a spin
float g_spinning_yvel = 0;
float g_spinning_boost = 1.8; //2.6 is pretty fast
float g_doubleSpinHelpMs = 16; //a spin can cancel another spin in the last x ms (double spin)
float g_spinJumpHelpMs = 0; //if you spin a frame after jumping you will jump and spin (spinjump)
float g_currentSpinJumpHelpMs = g_spinJumpHelpMs;
bool g_protag_jumped_this_frame = 0;

bool storedJump = 0;
bool storedSpin = 0;

bool fileExists(const std::string &name)
{
  if (FILE *file = fopen(name.c_str(), "r"))
  {
    fclose(file);
    return true;
  }
  else
  {
    return false;
  }
}

float g_earshot = 4 * 64; // how close do entities need to be to join their friends in battle

void playSound(int channel, Mix_Chunk *sound, int loops)
{
  // M("play sound");
  if (!g_mute && sound != NULL)
  {
    Mix_Volume(channel, g_sfx_volume * 128);
    Mix_PlayChannel(channel, sound, loops);
  }
}


SDL_Texture *MaskTexture(SDL_Renderer *renderer, SDL_Texture *mask, SDL_Texture *diffuse)
{
  int w, h;
  SDL_QueryTexture(diffuse, NULL, NULL, &w, &h);

  SDL_Texture *result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

  SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, result);

  SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_MOD);
  SDL_SetTextureBlendMode(diffuse, SDL_BLENDMODE_NONE);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  SDL_RenderCopy(renderer, diffuse, NULL, NULL);
  SDL_RenderCopy(renderer, mask, NULL, NULL);

  SDL_SetRenderTarget(renderer, NULL);
  return result;
}

float Distance(int x1, int y1, int x2, int y2)
{
  return pow(pow((x1 - x2), 2) + pow((y1 - y2), 2), 0.5);
}

float XYDistance(int x1, int y1, int x2, int y2)
{
  return pow(pow((x1 - x2), 2) + pow((y1 - y2), 2) * XtoY, 0.5);
}

// old crappy code
//  template<class T>
//  navNode* getNodeByPosition(vector<T*> array, int fx, int fy) {
//  	//this is a placeholder solution for testing AI
//  	//this requires a binary search and sorted nodes to work reasonably for larger maps
//  	float min_dist = 0;
//  		navNode* ret;
//  		bool flag = 1;

// 		//just pitifully slow
// 		for (int i = 0; i < array.size(); i++) {
// 			float dist = Distance(fx, fy, array[i]->x, array[i]->y);
// 			if(dist < min_dist || flag) {
// 				min_dist = dist;
// 				ret = array[i];
// 				flag = 0;
// 			}
// 		}
// 		return ret;
// }

// get cardinal points about a position
//  0 is 12oclock, and 2 is 3oclock and so on
vector<int> getCardinalPoint(int x, int y, float range, int index)
{
  float angle = 0;
  switch (index)
  {
    case 6:
      angle = 0;
      break;
    case 7:
      angle = M_PI / 4;
      break;
    case 0:
      angle = M_PI / 2;
      break;
    case 1:
      angle = M_PI * (3.0 / 4.0);
      break;
    case 2:
      angle = M_PI;
      break;
    case 3:
      angle = M_PI * (5.0 / 4.0);
      break;
    case 4:
      angle = M_PI * (3.0 / 2.0);
      break;
    case 5:
      angle = M_PI * (7.0 / 4.0);
      break;
  }
  vector<int> ret;
  ret.push_back(x + (range * cos(angle)));
  ret.push_back(y + (range * sin(angle) * XtoY));
  return ret;
}

// convert frame of sprite to angle
float convertFrameToAngle(int frame, bool flipped)
{
  if (flipped)
  {
    if (frame == 0)
    {
      return M_PI / 2;
    }
    if (frame == 1)
    {
      return (M_PI * 3) / 4;
    }
    if (frame == 2)
    {
      return M_PI;
    }
    if (frame == 3)
    {
      return (M_PI * 5) / 4;
    }
    if (frame == 4)
    {
      return (M_PI * 6) / 4;
    }
  }
  else
  {
    if (frame == 0)
    {
      return M_PI / 2;
    }
    if (frame == 1)
    {
      return (M_PI * 1) / 4;
    }
    if (frame == 2)
    {
      return 0;
    }
    if (frame == 3)
    {
      return (M_PI * 7) / 4;
    }
    if (frame == 4)
    {
      return (M_PI * 6) / 4;
    }
  }

  return 0;
}


//idk why i have two
//this is for steeringAngle
//the only difference is that "flipped" is treated oppositely
float convertFrameToAngleNew(int frame, bool flipped)
{
  if (!flipped)
  {
    if (frame == 0)
    {
      return M_PI / 2;
    }
    if (frame == 1)
    {
      return (M_PI * 3) / 4;
    }
    if (frame == 2)
    {
      return M_PI;
    }
    if (frame == 3)
    {
      return (M_PI * 5) / 4;
    }
    if (frame == 4)
    {
      return (M_PI * 6) / 4;
    }
  }
  else
  {
    if (frame == 0)
    {
      return M_PI / 2;
    }
    if (frame == 1)
    {
      return (M_PI * 1) / 4;
    }
    if (frame == 2)
    {
      return 0;
    }
    if (frame == 3)
    {
      return (M_PI * 7) / 4;
    }
    if (frame == 4)
    {
      return (M_PI * 6) / 4;
    }
  }

  return 0;
}



//wrap an angle so that it is within the range of 0 and 2pi radians
float wrapAngle(float input) {
  while(input < 0) {
    input+= 2*M_PI;
  }
  while(input > 2*M_PI) {
    input-= 2*M_PI;
  }
  return input;
}

// convert an angle to a sprite's frame, for eight-frame sprites (arms)
int convertAngleToFrame(float angle)
{
  //up is pi/2
  vector<float> angles = {0, (M_PI * 1) / 4, M_PI / 2, (M_PI * 3) / 4, M_PI, (M_PI * 5) / 4, (M_PI * 6) / 4, (M_PI * 7) / 4, (M_PI * 2) }; //gonna guess that that last element is a bit off
  vector<int> frames = {6, 7, 0, 1, 2, 3, 4, 5, 6};

  for (long long unsigned int i = 0; i < angles.size(); i++)
  {
    if ((angles[i] + M_PI / 8) > angle)
    {
      return frames[i];
    }
  }

  //return 5; //this was a crutch
  return 0; //shouldn't happen
}

// measures distance in the world, not by the screen.
float XYWorldDistance(int x1, int y1, int x2, int y2)
{
  y1 *= 1 / XtoY;
  y2 *= 1 / XtoY;
  return pow(pow((x1 - x2), 2) + pow((y1 - y2), 2), 0.5);
}

//faster, use this if you can
float XYWorldDistanceSquared(int x1, int y1, int x2, int y2)
{
  y1 *= 1 / XtoY;
  y2 *= 1 / XtoY;
  return pow((x1 - x2), 2) + pow((y1 - y2), 2);

}

vector<string> splitString(string s, char delimiter)
{
  int start;
  long long unsigned int end;
  start = 0;
  string token;
  vector<string> ret;
  end = s.find(delimiter, start);
  while (end != string::npos)
  {
    token = s.substr(start, end - start);
    start = end + 1;
    ret.push_back(token);
    end = s.find(delimiter, start);
  }

  ret.push_back(s.substr(start));
  return ret;
}

bool replaceString(std::string &str, const std::string &from, const std::string &to)
{
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

int yesNoPrompt(string msg)
{
  // warn for file overwrite
  const SDL_MessageBoxButtonData buttons[] = {
    {/* .flags, .buttonid, .text */ 0, 0, "Yes"},
    {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No"},
  };
  const SDL_MessageBoxColorScheme colorScheme = {
    {/* .colors (.r, .g, .b) */
      /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
      {0, 0, 0},
      /* [SDL_MESSAGEBOX_COLOR_TEXT] */
      {200, 200, 200},
      /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
      {180, 180, 180},
      /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
      {0, 0, 0},
      /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
      {255, 255, 255}}};
  const SDL_MessageBoxData messageboxdata = {
    SDL_MESSAGEBOX_INFORMATION, /* .flags */
    NULL,												/* .window */
    "",													/* .title */
    msg.c_str(),								/* .message */
    SDL_arraysize(buttons),			/* .numbuttons */
    buttons,										/* .buttons */
    &colorScheme								/* .colorScheme */
  };
  int buttonid;
  if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
  {
    SDL_Log("error displaying message box");
  }
  return buttonid;
}

// string stringToHex(string input) {
// 	stringstream ss;
// 	for(auto x : input){
// 		ss << std::hex << int(x);
// 	}
// 	string ret ( ss.str() );

// 	return ret;
// }

// string hexToString(const string& in) {
//     string ret;

//     size_t cnt = in.length() / 2;

//     for (size_t i = 0; cnt > i; ++i) {
//         uint32_t s = 0;
//         stringstream ss;
//         ss << std::hex << in.substr(i * 2, 2);
//         ss >> s;

//         ret.push_back(static_cast<unsigned char>(s));
//     }

//     return ret;
// }

// //these are both shite, fix em later
// string intToHex(int input) {
// 	string s = to_string(input);
// 	return stringToHex(s);
// }

// int hexToInt(string input) {
// 	return stoi(hexToString(input));
// }

// TUDO:
// set minimum window width and height to prevent crashes wenn the window is very small



template <class T>
T* getNodeByPos(vector<T*> array, int x, int y) {
  float min_dist = 0;
  T* ret = nullptr;
  bool flag = 1;

  //todo check for boxs
  if(array.size() == 0) {return nullptr;}
  for (long long unsigned int i = 0; i < array.size(); i++) {
    float dist = Distance(x, y, array[i]->x, array[i]->y);
    if(dist < min_dist || flag) {
      min_dist = dist;
      ret = array[i];
      flag = 0;
    }
  }
  return ret;
}

#endif
