	#include <SDL2/SDL.h>        
	#include <SDL2/SDL_image.h>
	#include <SDL2/SDL_ttf.h>
	#include <SDL2/SDL_mixer.h>
	#include <iostream>
	#include <vector>
	#include <algorithm>
	#include <cmath>
	#include <math.h>
	#include <fstream>
	#include <stdio.h>
	#include <string>
	#include <limits>
	#include <stdlib.h>

	#include "globals.cpp"

	#define PI 3.14159265

	#ifndef CLASSES
	#define CLASSES

	using namespace std;

	class heightmap {
	public:
		SDL_Surface* image = 0;
		string name;
		string binding;
		float magnitude = 0.278; //0.278 was a former value. I'd love to expose this value but I cant think of a good way

		heightmap(string fname, string fbinding, float fmagnitude) {
			image = IMG_Load(fbinding.c_str());
			name = fname;
			binding = fbinding;
			
			magnitude = fmagnitude;
			g_heightmaps.push_back(this);
		}

		~heightmap() {
			SDL_FreeSurface(image);
			g_heightmaps.erase(remove(g_heightmaps.begin(), g_heightmaps.end(), this), g_heightmaps.end());
		}

		Uint32 getpixel(SDL_Surface *surface, int x, int y) {
			int bpp = surface->format->BytesPerPixel;
			/* Here p is the address to the pixel we want to retrieve */
			Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch (bpp)
		{
			case 1:
				return *p;
				break;

			case 2:
				return *(Uint16 *)p;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					return p[0] << 16 | p[1] << 8 | p[2];
				else
					return p[0] | p[1] << 8 | p[2] << 16;
					break;

				case 4:
					return *(Uint32 *)p;
					break;

				default:
					return 0;
			}
		}
	};

	class navNode {
	public:
		int x;
		int y;
		vector<navNode*> friends;
		vector<float> costs;
		float costFromSource = 0; //updated with dijkstras algorithm
		navNode* prev = nullptr; //updated with dijkstras algorithm
		string name = "";

		navNode(int fx, int fy) {
			M("navNode()" );
			x = fx;
			y = fy;
			g_navNodes.push_back(this);
		}

		void Add_Friend(navNode* newFriend) {
			friends.push_back(newFriend);
			float cost = pow(pow((newFriend->x - this->x), 2) + pow((newFriend->y - this->y), 2), 0.5);
			costs.push_back(cost);
		}

		void Update_Costs() {
			for (int i = 0; i < friends.size(); i++) {
				costs[i] = Distance(x, y, friends[i]->x, friends[i]->y);
			}
			
		}

		void Render(int red, int green, int blue) {
			SDL_Rect obj = {(this->x -g_camera.x - 20)* g_camera.zoom , ((this->y - g_camera.y - 20) * g_camera.zoom), (40 * g_camera.zoom), (40 * g_camera.zoom)};
			SDL_SetTextureColorMod(nodeDebug, red, green, blue);
			SDL_RenderCopy(renderer, nodeDebug, NULL, &obj);
		}
		
		~navNode() {
			//M("~navNode()");
			D(this);
			for (auto x : friends) {
				x->friends.erase(remove(x->friends.begin(), x->friends.end(), this), x->friends.end());
			}
		 	//M("got here");
			if(count(g_navNodes.begin(), g_navNodes.end(), this)) {
				g_navNodes.erase(remove(g_navNodes.begin(), g_navNodes.end(), this), g_navNodes.end());
			}
		}
	};

	void RecursiveNavNodeDelete(navNode* a) {
		//copy friends array to new vector
		vector<navNode*> buffer;
		for (auto f : a->friends) {
			buffer.push_back(f);
		}
		
		//navNode* b = a->friends[0];
		delete a;
		for(auto f : buffer) {
			if(count(g_navNodes.begin(), g_navNodes.end(), f)) {
				RecursiveNavNodeDelete(f);
			}
		}
	}

	void Update_NavNode_Costs(vector<navNode*> fnodes) {
		M("Update_NavNode_Costs()" );
		for (int i = 0; i < fnodes.size(); i++) {
			fnodes[i]->Update_Costs();
		}
	}


	class coord {
	public:
		int x;
		int y;
	};

	class rect {
	public:
		int x;
		int y;
		int width;
		int height;

		rect() {
			x=0;
			y=0;
			width=45;
			height=45;
		}

		rect(int a, int b, int c, int d) {
			x=a;
			y=b;
			width=c;
			height=d;
		}

		void render(SDL_Renderer * renderer) {
			SDL_Rect rect = { this->x, this->y, this->width, this->height};
			SDL_RenderFillRect(renderer, &rect);
		}
	};

	class mapCollision {
	public:
		//related to saving/displaying the block
		string walltexture;
		string captexture;
		bool capped = false;

		//tiles created from the mapCollision, to be appropriately deleted
		vector<mapObject*> children;

		//tri and boxes which are part of the map are pushed back on 
		//an array of mapCollisions to be kept track of for deletion/undoing
		mapCollision() {
			M("mapCollision()");
			g_mapCollisions.push_back(this);
		}
		~mapCollision() {
			M("~mapCollision()");
			g_mapCollisions.erase(remove(g_mapCollisions.begin(), g_mapCollisions.end(), this), g_mapCollisions.end());
		}
		
	};

	class tri:public mapCollision {
	public:
		int x1; int y1;
		int x2; int y2;
		int type;
		float m; //slope
		int b; //offset
		int layer = 0;
		bool shaded = 0;

		tri(int fx1, int fy1, int fx2, int fy2, int flayer, string fwallt, string fcapt, bool fcapped, bool fshaded) {
			M("tri()");
			x1=fx1; y1=fy1;
			x2=fx2; y2=fy2;
			layer = flayer;
			shaded = fshaded;
			if(x2 < x1 && y2 > y1) {
				type = 0; //  :'
			}
			if(x2 < x1 && y2 < y1) {
				type = 1; //  :,
			}
			if(x2 > x1 && y2 < y1) {
				type = 2; //  ,:
			}
			if(x2 > x1 && y2 > y1) {
				type = 3; //  ':
			}
			m = float(y1 -y2) / float(x1 - x2);
			b = y1 - (m * x1);
			walltexture = fwallt;
			captexture = fcapt;
			capped = fcapped;
			g_triangles[layer].push_back(this);
		}

		~tri() {
			M("~tri()");
			g_triangles[layer].erase(remove(g_triangles[layer].begin(), g_triangles[layer].end(), this), g_triangles[layer].end());
		}

		void render(SDL_Renderer* renderer) {
			
			int tx1 = g_camera.zoom * (x1-g_camera.x);
			int tx2 = g_camera.zoom * (x2-g_camera.x);
			

			int ty1 = g_camera.zoom * (y1-g_camera.y)- layer * 38;
			int ty2 = g_camera.zoom * (y2-g_camera.y)- layer * 38;
			
			
			SDL_RenderDrawLine(renderer,  tx1, ty1, tx2, ty2);
			SDL_RenderDrawLine(renderer,  tx1, ty1, tx2, ty1);
			SDL_RenderDrawLine(renderer,  tx2, ty2, tx2, ty1);

			
		}
	};


	bool PointInsideRightTriangle(tri* t, int px, int py) {
		switch(t->type) {
			case(0):
				if(px >= t->x2 && py >= t->y1 && py <= (t->m * px)  + t->b) {
					
					
					return true;
				}
				break;

			case(1):
				if(px >= t->x2 && py <= t->y1 && py >= (t->m * px) + t->b) {
					
					
					return true;
				}
				break;

			case(2):
				if(px <= t->x2 && py <= t->y1 && py >= (t->m * px) + t->b) {
					
					
					return true;
				}
				break;

			case(3):
				if(px <= t->x2 && py >= t->y1 && py <= (t->m * px)  + t->b) {
					
					
					return true;
				}
				break;

		}
		return false;
	}

	bool RectOverlap(rect a, rect b) {
		if (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y) {
			return true;
		} else {
			return false;
		}
	}

	bool ElipseOverlap(rect a, rect b) {
		//get midpoints
		coord midpointA = {a.x + a.width/2, a.y + a.height/2};
		coord midpointB = {b.x + b.width/2, b.y + b.height/2};
		
		//"unfold" crumpled y axis by multiplying by 1/XtoY
		midpointA.y *= 1/XtoY;
		midpointB.y *= 1/XtoY;

		//circle collision test using widths of rectangles as radiusen
		return (Distance(midpointA.x, midpointA.y, midpointB.x, midpointB.y) < (a.width + b.width)/2);
	}

	bool RectOverlap(SDL_Rect a, SDL_Rect b) {
		if (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y) {
			return true;
		} else {
			return false;
		}
	}

	bool RectOverlap(SDL_FRect a, SDL_FRect b) {
		if (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y) {
			return true;
		} else {
			return false;
		}
	}

	//is a inside b?
	bool RectWithin(rect a, rect b) {
		if (b.x < a.x && b.x + b.width > a.x + a.width && b.y < a.y && b.y + b.height > a.y + a.height) {
			return true;
		} else {
			return false;
		}
	}

	bool TriRectOverlap(tri* a, int x, int y, int width, int height) {
		if(PointInsideRightTriangle(a, x, y +  height)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x + width, y + height)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x + width, y)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x, y)) {
			return 1;
		}
		//also check if the points of the triangle are inside the rectangle
		//skin usage is possibly redundant here
		if(a->x1 > x + 0 && a->x1 < x + width - 0 && a->y1 > y + 0 && a->y1 < y + height - 0) {
			return 1;
		}
		if(a->x2 > x + 0 && a->x2 < x + width - 0 && a->y2 > y + 0&& a->y2 < y + height - 0) {
			return 1;
		}
		if(a->x2 > x + 0 && a->x2 < x + width - 0 && a->y1 > y + 0 && a->y1 < y + height - 0) {
			return 1;
		}
		return 0;
	}

	bool TriRectOverlap(tri* a, rect r) {
		int x = r.x;
		int y = r.y;
		int width = r.width;
		int height = r.height;
		if(PointInsideRightTriangle(a, x, y +  height)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x + width, y + height)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x + width, y)) {
			return 1;
		}
		if(PointInsideRightTriangle(a, x, y)) {
			return 1;
		}
		//also check if the points of the triangle are inside the rectangle
		//skin usage is possibly redundant here
		if(a->x1 > x + 0 && a->x1 < x + width - 0 && a->y1 > y + 0 && a->y1 < y + height - 0) {
			return 1;
		}
		if(a->x2 > x + 0 && a->x2 < x + width - 0 && a->y2 > y + 0&& a->y2 < y + height - 0) {
			return 1;
		}
		if(a->x2 > x + 0 && a->x2 < x + width - 0 && a->y1 > y + 0 && a->y1 < y + height - 0) {
			return 1;
		}
		return 0;
	}

	SDL_Rect transformRect(SDL_Rect input) {
		SDL_Rect obj;
		obj.x = floor((input.x -g_camera.x) * g_camera.zoom);
		obj.y = floor((input.y - g_camera.y) * g_camera.zoom);
		obj.w = ceil(input.w * g_camera.zoom);
		obj.h = ceil(input.h * g_camera.zoom);		
		return obj;
	}

	SDL_FRect transformRect(SDL_FRect input) {
		SDL_FRect obj;
		obj.x = ((input.x -g_camera.x) * g_camera.zoom);
		obj.y = ((input.y - g_camera.y) * g_camera.zoom);
		obj.w = (input.w * g_camera.zoom);
		obj.h = (input.h * g_camera.zoom);		
		return obj;
	}

	rect transformRect(rect input) {
		rect obj;
		obj.x = floor((input.x -g_camera.x) * g_camera.zoom);
		obj.y = floor((input.y - g_camera.y) * g_camera.zoom);
		obj.width = ceil(input.width * g_camera.zoom);
		obj.height = ceil(input.height * g_camera.zoom);		
		return obj;
	}

	class box:public mapCollision {
	public:
		rect bounds;
		bool active = true;
		int layer = 0;
		bool shineTop = 0;
		bool shineBot = 0;
		
		bool shadeTop = 0;

		//why is this one an int? because different values can affect the front corners.
		int shadeBot = 0; 
		//0 - no shading
		//1 - standard shading (corners will be there if their side is there)
		//2 - just corners, that way if this block is behind a triangle

		bool shadeLeft = 0;
		bool shadeRight = 0;


		box(int x1f, int y1f, int x2f, int y2f, int flayer, string &fwallt, string &fcapt, bool fcapped, bool fshineTop, bool fshineBot, const char* shading) {
			M("box()");
			bounds.x = x1f;
			bounds.y = y1f;
			bounds.width = x2f;
			bounds.height = y2f;
			layer = flayer;
			walltexture = fwallt;
			captexture = fcapt;
			capped = fcapped;
			shineTop = fshineTop;
			shineBot = fshineBot;
			shadeTop = (shading[0] == '1');

			switch (shading[1])
			{
			case '0':
				shadeBot = 0;
				break;
			case '1':
				shadeBot = 1;
				break;
			case '2':
				shadeBot = 2;
				break;
			}
			shadeLeft = (shading[2] == '1');
			shadeRight = (shading[3] == '1');
			g_boxs[layer].push_back(this);
		}
		~box() {
			M("~box()");
			g_boxs[layer].erase(remove(g_boxs[layer].begin(), g_boxs[layer].end(), this), g_boxs[layer].end());
		}
	};

	//returns true if there was no hit
	bool LineTrace(int x1, int y1, int x2, int y2, bool display = 0, int size = 30, int layer = 0) {
		float resolution = 10;
		
		if(display) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
		for (float i = 1; i < resolution; i++) {
			int xsize = size * p_ratio;
			int xpos = (i/resolution) * x1 + (1 - i/resolution) * x2;
			int ypos = (i/resolution) * y1 + (1 - i/resolution) * y2;
			rect a = rect(xpos - xsize/2, ypos - size/2, xsize, size);
			SDL_Rect b = {((xpos- xsize/2) - g_camera.x) * g_camera.zoom, ((ypos- size/2) - g_camera.y) * g_camera.zoom, xsize, size};

			if(display) {
				SDL_RenderDrawRect(renderer, &b);
			}	
			
			
			for (int j = 0; j < g_boxs[layer].size(); j++) {
				if(RectOverlap(a, g_boxs[layer][j]->bounds)) {
					return false;
				}
			}
		}
		if(display) {
			SDL_RenderPresent(renderer);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		}
		return true;
	}

	class door {
	public:
		float x;
		float y;
		float width = 50;
		float height = 50;
		float friction;
		rect bounds;
		string to_map;
		string to_point;
		

		door(SDL_Renderer * renderer, const char* fmap, string fto_point,  int fx, int fy, int fwidth, int fheight) {		
			M("door()");
			this->x = fx;
			this->y = fy;

			this->bounds.x = fx;
			this->bounds.y = fy;
			
			to_map = fmap;
			to_point = fto_point;

			this->width = fwidth;
			this->height = fheight;
			this->bounds.width = width;
			this->bounds.height = height;
			g_doors.push_back(this);
			
		}

		~door() {
			g_doors.erase(remove(g_doors.begin(), g_doors.end(), this), g_doors.end());
		}

	};


	class tile {
	public:
		float x = 0;
		float y = 0;
		int z = 0; //really just layer

		float width = 0;
		float height = 0;
		float xoffset = 0; //for aligning texture across the map, change em to floats later
		float yoffset = 0;
		float dxoffset = 0; //for changing texture coords
		float dyoffset = 0;
		int texwidth = 0;
		int texheight = 0;

		bool wraptexture = 1; //should we tile the image or stretch it?
		bool wall = 0; //darken image if it is used for a wall as opposed to a floor
		bool asset_sharer = 0; //1 if the tile is sharing another tile's texture, and this texture should in this case not be deleted in the destructor
		bool software = 0; //tiles used for the mapeditor/ui/etc are specially marked to avoid saving them or trying to delete them from a map

		SDL_Surface* image = 0;
		SDL_Texture* texture = 0;
		string fileaddress = "df"; //for checking if someone else has already loaded a texture
		string mask_fileaddress = "&"; //unset value

		
		
		tile(SDL_Renderer * renderer, const char* filename, const char* mask_filename, int fx, int fy, int fwidth, int fheight, int flayer, bool fwrap, bool fwall, float fdxoffset, float fdyoffset) {		
			this->x = fx;
			this->y = fy;
			this->z = flayer;
			this->width = fwidth;
			this->height = fheight;
			this->wall = fwall;
			this->wraptexture = fwrap;

			this->dxoffset = fdxoffset;
			this->dyoffset = fdyoffset;
			
			fileaddress = filename;
			bool cached = false;
			//has someone else already made a texture?
			for(unsigned int i=0; i < g_tiles.size(); i++){
				if(g_tiles[i]->fileaddress == this->fileaddress && g_tiles[i]->mask_fileaddress == mask_filename) {
					//make sure both are walls or both aren't
					if(g_tiles[i]->wall == this->wall) {
						//M("sharing a texture" );
						cached = true;
						this->texture = g_tiles[i]->texture;
						SDL_QueryTexture(g_tiles[i]->texture, NULL, NULL, &texwidth, &texheight);
						this->asset_sharer = 1;
						break;
					}
				}
			}
			if(cached) {
							
			} else {
				image = IMG_Load(filename);
				texture = SDL_CreateTextureFromSurface(renderer, image);
				if(wall) {
					//make walls a bit darker
					SDL_SetTextureColorMod(texture, -65, -65, -65);
				} else {
					//SDL_SetTextureColorMod(texture, -20, -20, -20);
				}
				
				SDL_QueryTexture(texture, NULL, NULL, &texwidth, &texheight);
				if(fileaddress.find("OCCLUSION") != string::npos) {
					SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
					
				}
				if(mask_filename[0] != '&') {
					SDL_DestroyTexture(texture);				
			
					SDL_Surface* smask = IMG_Load(mask_filename);
					SDL_Texture* mask = SDL_CreateTextureFromSurface(renderer, smask);
					SDL_Texture* diffuse = SDL_CreateTextureFromSurface(renderer, image);
					\
					SDL_SetTextureColorMod(diffuse, -20, -20, -20);
					texture = MaskTexture(renderer, diffuse, mask);
					SDL_FreeSurface(smask);
					SDL_DestroyTexture(mask);
					SDL_DestroyTexture(diffuse);
					//SDL_SetTextureColorMod(texture, 0, 0, 0);
				}
			}

			
			mask_fileaddress = mask_filename;

			
			this->xoffset = fmod(this->x, this->texwidth);
			this->yoffset = fmod(this->y, this->texheight);

			SDL_FreeSurface(image);
			g_tiles.push_back(this);
		}

		~tile() {
			M("~tile()" );
			g_tiles.erase(remove(g_tiles.begin(), g_tiles.end(), this), g_tiles.end());
			if(!asset_sharer) {
				SDL_DestroyTexture(texture);
			}
			
		}

		rect getMovedBounds() {
			return rect(x, y, width, height);
		}

		void render(SDL_Renderer * renderer, camera fcamera) {
			SDL_FPoint nowt = {0, 0};
			rect obj((x -fcamera.x)* fcamera.zoom, (y-fcamera.y) * fcamera.zoom, width * fcamera.zoom, height * fcamera.zoom);		
			rect cam(0, 0, fcamera.width, fcamera.height);
			
			//movement
			if((dxoffset !=0 || dyoffset != 0 ) && wraptexture) {
				xoffset += dxoffset;
				yoffset += dyoffset;
				if(xoffset < 0) {
					xoffset = texwidth;
				}
				if(xoffset > texwidth) {
					xoffset = 0;
				}
				if(yoffset < 0) {
					yoffset = texheight;
				}
				if(yoffset > texheight) {
					yoffset = 0;
				}
			}

			if(RectOverlap(obj, cam)) {
				
				if(this->wraptexture) {	
					SDL_FRect srcrect;
					SDL_FRect dstrect;
					float ypos = 0;
					float xpos = 0;

					srcrect.x = xoffset;
					srcrect.y = yoffset;
					while(1) {
						if(srcrect.x == xoffset) {
							srcrect.w = (texwidth - xoffset);
							dstrect.w = (texwidth - xoffset);
						} else {
							srcrect.w = texwidth;
							dstrect.w = texwidth;
						}
						if(srcrect.y == yoffset) {
							
							srcrect.h = texheight - yoffset;
							dstrect.h = texheight - yoffset;
						} else {
							dstrect.h = texheight;
							srcrect.h = texheight;
						}
						
						
						
						
						

						dstrect.x = (x + xpos);
						dstrect.y = (y + ypos);
						
						
						
						
						//are we still inbounds?
						if(xpos + dstrect.w > this->width) {
							dstrect.w = this->width - xpos;
							if(dstrect.w + srcrect.x > texwidth) {
								dstrect.w = texwidth - srcrect.x;
							}
							srcrect.w = dstrect.w;
						}
						if(ypos + dstrect.h > this->height) {
							dstrect.h = this->height - ypos;
							if(dstrect.h + srcrect.y > texheight) {
								dstrect.h = texheight - srcrect.y;
							}
							srcrect.h = dstrect.h;
							
						}
						//are we still in the texture
						
						
						
						


						//transform

						dstrect.w = (dstrect.w * fcamera.zoom);
						dstrect.h = (dstrect.h * fcamera.zoom);

						dstrect.x = ((dstrect.x - fcamera.x)* fcamera.zoom);
						dstrect.y = ((dstrect.y - fcamera.y)* fcamera.zoom);
						
						SDL_Rect fsrcrect;
						fsrcrect.x = srcrect.x;
						fsrcrect.y = srcrect.y;
						fsrcrect.w = srcrect.w;
						fsrcrect.h = srcrect.h;
						
						SDL_RenderCopyExF(renderer, texture, &fsrcrect, &dstrect, 0, &nowt, SDL_FLIP_NONE);
						
						
						
						
						xpos += srcrect.w;
						srcrect.x = 0;
						
						if(xpos >= this->width) {
							xpos = 0;
							srcrect.x = xoffset;
							
							ypos += srcrect.h;
							srcrect.y = 0;	
							if(ypos >= this->height)
								break;
						}	
					}
				} else {
					SDL_FRect dstrect = { (x -fcamera.x)* fcamera.zoom, (y-fcamera.y) * fcamera.zoom, width * fcamera.zoom, height * fcamera.zoom};
					SDL_RenderCopyF(renderer, texture, NULL, &dstrect);
				}

			}
		}
	};

	class weapon {
	public:
		
		
		float maxCooldown = 400; //ms
		float cooldown = 0;
		string name = "";
		//box data, data about movement
		int max_combo = 2;
		int combo = 0; //increments
		float maxComboResetSeconds = 1;
		float comboResetSeconds = 0;
		bool canBeHeldDown = 1;
		float shotLifetime = 500; //ms
		int width = 20;
		int height = 20;
		int faction = 1;
		int damage = 1;
		float spread = 0;
		float randomspread = 0;
		int range = 512; // max range, entities will try to be 0.8% of this to hit safely. in worldpixels
		float size = 0.1;
		float speed = 1;
		int numshots = 1;
		SDL_Texture* texture;

		float xoffset = 5;

		//given some float time, return floats for x and y position
		//will be rotated/flipped

		//i should update this with variables, which are expressions from an attack file
		float forward(float time) {
			return speed;
		} 

		float sideways(float time) {
			//return 10 * cos(time / 45);
			return 0;
		}

		// weapon(string spriteadress) {
		// 	string spritefile = "sprites/" + spriteadress + ".png";
		// 	SDL_Surface* image = IMG_Load(spritefile.c_str());
		// 	if(onionmode) {
		// 		image = IMG_Load("sprites/onionmode/onion.png");
		// 	}
		// 	texture = SDL_CreateTextureFromSurface(renderer, image);
		// 	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
		// 	width *= size;
		// 	height *= size;
		// 	SDL_FreeSurface(image);
		// 	g_weapons.push_back(this);
		// }

		weapon(string filename) {
			M("weapon()");
			this->name = filename;
			ifstream file;
			string loadstr;
			//try to open from local map folder first
			
			loadstr = "maps/" + g_map + "/" + filename + ".wep";
			D(loadstr);
			const char* plik = loadstr.c_str();
			
			file.open(plik);
			
			if (!file.is_open()) {
				//load from global folder
				loadstr = "weapons/" + filename + ".wep";
				D(loadstr);
				const char* plik = loadstr.c_str();
				
				file.open(plik);
				
				if (!file.is_open()) {
					//just make a default entity
					string newfile = "weapons/default.wep";
					D(loadstr);
					file.open(newfile);
				}
			}
			
			string temp;
			file >> temp;
			temp = "sprites/" + temp + ".png";

			

			SDL_Surface* image = IMG_Load(temp.c_str());
			if(onionmode) {
				image = IMG_Load("sprites/onionmode/onion.png");
			}
			
			texture = SDL_CreateTextureFromSurface(renderer, image);
			file >> this->maxCooldown;
			float size;
			file >> size;
			
			SDL_QueryTexture(texture, NULL, NULL, &width, &height);
			width *= size;
			height *= size;
			SDL_FreeSurface(image);


			
			file >> this->damage;
			file >> this->speed;
			file >> this->shotLifetime;
			file >> this->spread;
			this->spread *= M_PI;
			file >> this->randomspread;
			randomspread *= M_PI;
			file >> this->numshots;
			


			g_weapons.push_back(this);
		}

		~weapon() {
			SDL_DestroyTexture(texture);
			g_weapons.erase(remove(g_weapons.begin(), g_weapons.end(), this), g_weapons.end());
		}
	};

	//anything that exists in the 3d world
	class actor {
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		float width = 0;
		float height = 0;
		SDL_Texture* texture;
		rect bounds;
		string name;

		//add entities and mapObjects to g_actors with dc
		actor() {
			//M("actor()");
			g_actors.push_back(this);
		}
		
		~actor() {
			//M("~actor()");
			g_actors.erase(remove(g_actors.begin(), g_actors.end(), this), g_actors.end());
		}

		virtual void render(SDL_Renderer * renderer, camera fcamera) {
			
		}
		
		
		int getOriginX() {
			return  x + bounds.x + bounds.width/2;
		}

		int getOriginY() {
			return y + bounds.y + bounds.height/2;
		}
	};

		class cshadow:public actor {
	public:
		float size;
		actor* owner = 0;
		SDL_Surface* image = 0;
		SDL_Texture* texture = 0;
		bool asset_sharer = 0;
		int xoffset = 0;
		int yoffset = 0;

		cshadow(SDL_Renderer * renderer, float fsize) {
			M("cshadow()");
			size = fsize;
			//if there is another shadow steal his texture
			if( g_shadows.size() > 0) {
				texture = g_shadows[0]->texture;
				this->asset_sharer = 1;	
			} else {
				image = IMG_Load("sprites/shadow.png");
				texture = SDL_CreateTextureFromSurface(renderer, image);
				SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
				SDL_FreeSurface(image);
			}
			
			g_shadows.push_back(this);
		}

		~cshadow() {
			M("~cshadow()" );
			if(!asset_sharer) { SDL_DestroyTexture(texture);}
			g_shadows.erase(remove(g_shadows.begin(), g_shadows.end(), this), g_shadows.end());
		}
		void render(SDL_Renderer* renderer, camera fcamera);
	};



	class projectile : public actor {
	public:
		int layer;
		float xvel;
		float yvel;

		bool asset_sharer;

		bool animate = 0;
		int frame = 0;
		int animation = 0;
		int xframes = 0;
		int frameInAnimation = 0;
		
		int curheight = 0;
		int curwidth = 0;

		entity* owner = nullptr;
		float maxLifetime = 0.4;
		float lifetime = 500;
		float angle = 0;
		weapon* gun;
		cshadow * shadow = 0;

		projectile(weapon* fweapon) {
			M("projectile()");
			this->width = fweapon->width;
			this->height = fweapon->height;
			this->bounds.width = this->width;
			this->bounds.height = this->height;
			
			
			this->gun = fweapon;
			this->maxLifetime = fweapon->shotLifetime;
			this->lifetime = fweapon->shotLifetime;
			
			shadow = new cshadow(renderer, fweapon->size);
			this->shadow->owner = this;
			shadow->width = width;
			shadow->height = height;
			

			gun = fweapon;
			texture = gun->texture;
			asset_sharer = 1;
			
			animate = 0;
			curheight = height;
			curwidth = width;
			g_projectiles.push_back(this);
		}

		~projectile() {
			//M("~projectile()");
			g_projectiles.erase(remove(g_projectiles.begin(), g_projectiles.end(), this), g_projectiles.end());
			delete shadow;
			//make recursive projectile
		}

		void update(float elapsed) {
			

			rect bounds = {x, y, width, height};
			layer = max(z /64, 0.0f);
			for(auto n : g_boxs[layer]) {
				if(RectOverlap(bounds, n->bounds)) {
					lifetime = 0;
					return;
				}
			}

			if(lifetime <= 0) {
				return;
			}
			
			x += sin(angle) * gun->sideways(maxLifetime - lifetime) + cos(angle) * gun->forward(maxLifetime - lifetime) + xvel;
			y += XtoY * (sin(angle + M_PI / 2) * gun->sideways(maxLifetime - lifetime) + cos(angle + M_PI / 2) * gun->forward(maxLifetime - lifetime) + yvel);
			lifetime -= elapsed;

			//move shadow to feet
			shadow->x = x;
			shadow->y = y;
			
			float floor = 0;
			int layer;
			layer = max(z /64, 0.0f);
			layer = min(layer, (int)g_boxs.size() - 1);
			if(layer > 0) {

				//this means that if the bullet is above a block that isnt right under it, the shadow won't be seen, but it's not worth it atm to change
				rect thisMovedBounds = rect(x,  y, width, height);
				for (auto n : g_boxs[layer - 1]) {
					if(RectOverlap(n->bounds, thisMovedBounds)) {
						floor = 64 * (layer);
						break;
					}
				}
			}
			shadow->z = floor;
		}

		void render(SDL_Renderer * renderer, camera fcamera) {
			SDL_FRect obj = {(x -fcamera.x)* fcamera.zoom, ((y- z * XtoZ) - fcamera.y) * fcamera.zoom, width * fcamera.zoom, height * fcamera.zoom};		
			SDL_FRect cam = {0, 0, fcamera.width, fcamera.height};
			
			if(RectOverlap(obj, cam)) {

				
				
				if(0/*framespots.size() > 0*/) {
					//frame = animation * xframes + frameInAnimation;
					//SDL_Rect dstrect = { obj.x, obj.y, obj.width, obj.height};
					//SDL_Rect srcrect = {framespots[frame]->x,framespots[frame]->y, framewidth, frameheight};
					//const SDL_Point center = {0 ,0};
					//if(texture != NULL) {
					//	SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, &center, flip);
					//}
				} else {
					if(texture != NULL) {
						SDL_RenderCopyF(renderer, texture, NULL, &obj);
					}
				}
			}
		}
	};



	class mapObject:public actor {
	public:
		
		float xoffset = 0;
		float yoffset = 64;
		bool wall; //to darken entwalls
		float sortingOffset = 0; //to make entites float in the air, or sort as if they are higher or lower than they are first use was for building diagonal walls
		float extraYOffset = 0; //used to encode a permanent y axis offset, for diagonal walls
		//0 - slant left, 1 - slant right, 2 - pillar
		int effect = 0;
		bool asset_sharer = 0;
		int framewidth = 0;
		int frameheight = 0;
		bool diffuse = 1; //is this mapobject used for things such as walls or floors, as opposed to props or lighting
		string mask_fileaddress = "&"; //unset value
		SDL_Texture* alternative = nullptr; //representing the texture tinted as if it were the opposite of the texture in terms of shading

		mapObject(SDL_Renderer * renderer, string imageadress, const char* mask_filename, int fx, int fy, int fz, int fwidth, int fheight, bool fwall = 0, float extrayoffset = 0) {
			//M("mapObject() fake");
			
			name = imageadress;
			mask_fileaddress = mask_filename;
			
			bool cached = false; 	
			wall = fwall;
			
			//this could further be improved by going thru g_boxs and g_triangles
			//as there are always less of those than mapobjects
			//heres an idea: could we copy textures if the wall field arent equal instead of loading them again
			if(g_mapObjects.size()  > 1) {
				for(unsigned int i=g_mapObjects.size() - 1; i > 0; i--){
					if(g_mapObjects[i]->mask_fileaddress == mask_filename && g_mapObjects[i]->name == this->name) {
						if(g_mapObjects[i]->wall == this->wall) {
							cached = true;
							this->texture = g_mapObjects[i]->texture;
							this->alternative =g_mapObjects[i]->alternative;
							this->asset_sharer = 1;
							this->framewidth = g_mapObjects[i]->framewidth;
							this->frameheight = g_mapObjects[i]->frameheight;
							break;
						} else {
							cached = true;
							this->texture = g_mapObjects[i]->alternative;
							this->alternative = g_mapObjects[i]->texture;
							
							this->asset_sharer = 1;
							this->framewidth = g_mapObjects[i]->framewidth;
							this->frameheight = g_mapObjects[i]->frameheight;
							break;
						}
					}
				}
			}
			if(cached) {
							
			} else {
				const char* plik = imageadress.c_str();
				SDL_Surface* image = IMG_Load(plik);
				texture = SDL_CreateTextureFromSurface(renderer, image);
				alternative = SDL_CreateTextureFromSurface(renderer, image);
				

				if(mask_filename[0] != '&') {
					
					//the SDL_SetHint() changes a flag from 3 to 0 to 3 again. 
					//this effects texture interpolation, and for masked entities such as wallcaps, it should
					//be off.

					SDL_DestroyTexture(texture);
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
					SDL_Surface* smask = IMG_Load(mask_filename);
					SDL_Texture* mask = SDL_CreateTextureFromSurface(renderer, smask);
					SDL_Texture* diffuse = SDL_CreateTextureFromSurface(renderer, image);
					//SDL_SetTextureColorMod(diffuse, -65, -65, -65);
					texture = MaskTexture(renderer, diffuse, mask);
					SDL_FreeSurface(smask);
					SDL_DestroyTexture(mask);
					SDL_DestroyTexture(diffuse);
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "3");
				}
				SDL_FreeSurface(image);

				if(fwall) {
					wall = 1;
					SDL_SetTextureColorMod(texture, -g_walldarkness, -g_walldarkness, -g_walldarkness);
				} else {
					SDL_SetTextureColorMod(alternative, -g_walldarkness, -g_walldarkness, -g_walldarkness);
				}
				if(name.find("SHADING") != string::npos) {
					//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
					//SDL_SetTextureAlphaMod(texture, 150);
					//diffuse = 0;
					
				}
				if(name.find("OCCLUSION") != string::npos) {
					//cout << "blended " << name << endl;
					SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
					//SDL_SetTextureAlphaMod(texture, 0);
					//diffuse = 0;
				}
				SDL_QueryTexture(texture, NULL, NULL, &this->framewidth, &this->frameheight);
			}

			
			
			
			//used for tiling
			
			
			this->x = fx;
			this->y = fy;
			this->z = fz;
			this->width = fwidth;
			this->height = fheight;
			this->xoffset = int(this->x) % int(this->framewidth);
			this->bounds.y = -55; //added after the ORIGIN was used for ent sorting rather than the FOOT.
			//this essentially just gives the blocks an invisible hitbox starting from their "head" so that their origin is in the middle
			//of the box

			extraYOffset = extrayoffset;
			this->yoffset = ceil(fmod(this->y - this->height+ extrayoffset, this->frameheight));
			//this->yoffset = 33;
			if(fwall ) {
				//this->yoffset = int(this->y + this->height - (z * XtoZ) + extrayoffset) % int(this->frameheight);
				
				//most favorable approach- everything lines up, even diagonal walls.
				//why is the texture height multiplied by 4? It just has to be a positive number, so multiples of the frameheight
				//are added in because of the two negative terms
				this->yoffset = (int)(4 * this->frameheight - this->height - (z * XtoZ)) % this->frameheight;
				
				//this->yoffset = this->frameheight - this->height;
				//this->yoffset = fmod(((this->y + this->height - (z * XtoZ)) + extrayoffset) , (this->frameheight));
				// if(imageadress == "tiles/diffuse/mapeditor/c.png") {
				// 	this->yoffset = 40;
				// }
			}
			
			g_mapObjects.push_back(this);
			
		}

		~mapObject() {
			if(!asset_sharer) {
				SDL_DestroyTexture(alternative);
			}
			g_mapObjects.erase(remove(g_mapObjects.begin(), g_mapObjects.end(), this), g_mapObjects.end());
		}

		rect getMovedBounds() {
			return rect(bounds.x + x, bounds.y + y, bounds.width, bounds.height);
		}

		void render(SDL_Renderer * renderer, camera fcamera) {
			SDL_FPoint nowt = {0, 0};

			SDL_FRect obj; // = {(floor((x -fcamera.x)* fcamera.zoom) , floor((y-fcamera.y - height - XtoZ * z) * fcamera.zoom), ceil(width * fcamera.zoom), ceil(height * fcamera.zoom))};
			obj.x = (x -fcamera.x)* fcamera.zoom;
			obj.y = (y-fcamera.y - height - XtoZ * z) * fcamera.zoom;
			obj.w = width * fcamera.zoom;
			obj.h = height * fcamera.zoom;

			SDL_FRect cam;
			cam.x = 0;
			cam.y = 0;
			cam.w = fcamera.width;
			cam.h = fcamera.height;
			
			if(RectOverlap(obj, cam)) {
				
				//if its a wall, check if it is covering the player
				if(this->wall && protag != nullptr) {
					
					// //make obj one block higher for the wallcap
					// //obj.height -= 45;
					// //obj.y += 45;
					// bool blocking = 0;
					
					// if(!g_protagHasBeenDrawnThisFrame && RectOverlap( transformRect(protag->getMovedBounds() ), transformRect( this->getMovedBounds() ) ) ) {
					// 	//objects are draw on top of each other
					// 	blocking = 1;
					// 	M("blocking protag");
					// }
					// M("did it block?");
					// if(blocking) {
					// 	SDL_SetTextureAlphaMod(texture, 160);
					// } else {
					// 	SDL_SetTextureAlphaMod(texture, 255);
					// }
				}
				
				
				SDL_Rect srcrect;
				SDL_FRect dstrect;
				int ypos = 0;
				int xpos = 0;
				srcrect.x = xoffset;
				srcrect.y = yoffset;
				
				while(1) {
					if(srcrect.x == xoffset) {
							srcrect.w = framewidth - xoffset;
							dstrect.w = framewidth - xoffset;
						} else {
							srcrect.w = framewidth;
							dstrect.w = framewidth;
						}
						if(srcrect.y == yoffset) {
							
							srcrect.h = frameheight - yoffset;
							dstrect.h = frameheight - yoffset;
						} else {
							dstrect.h = frameheight;
							srcrect.h = frameheight;
						}

					dstrect.x = (x + xpos);
					dstrect.y = (y+ ypos- z * XtoZ );
					
					
					
					
					//are we still inbounds?
					if(xpos + dstrect.w > this->width) {
						dstrect.w = this->width - xpos;
						if(dstrect.w + srcrect.x > framewidth) {
							dstrect.w = framewidth - srcrect.x;
						}
						srcrect.w = dstrect.w;
						//seems to cause seams, seemingly.
						//lets add one to the dstrect.w
						//although ceiling it might be better
						//all of the numbers above are ints
						//so rounding shouldnt be a problem
						//dstrect.w++;
						//fucks with shadows
					}
					if(ypos + dstrect.h > this->height ) {
						
						dstrect.h = this->height - ypos;
						if(dstrect.h + srcrect.y > frameheight) {
							dstrect.h = frameheight - srcrect.y;
						}
						srcrect.h = dstrect.h;
						//dstrect.h ++;
					}

					
					
					
					


					//transform
					//if(diffuse) {
						// dstrect.x = floor((dstrect.x - fcamera.x)* fcamera.zoom);
						// dstrect.y = floor((dstrect.y - fcamera.y - height)* fcamera.zoom);
						// dstrect.w = floor(dstrect.w * fcamera.zoom);
						// dstrect.h = floor(dstrect.h * fcamera.zoom);
					//} else {
						dstrect.x = ((dstrect.x - fcamera.x)* fcamera.zoom);
						dstrect.y = ((dstrect.y - fcamera.y - height)* fcamera.zoom);
						dstrect.w = (dstrect.w * fcamera.zoom);
						dstrect.h = (dstrect.h * fcamera.zoom);
					//}

					
					

					SDL_RenderCopyExF(renderer, texture, &srcrect, &dstrect, 0, &nowt, SDL_FLIP_NONE );
					xpos += srcrect.w;
					srcrect.x = 0;
					
					if(xpos >= this->width) {
						
						xpos = 0;
						srcrect.x = xoffset;
						
						ypos += srcrect.h;
						srcrect.y = 0;	
						if(ypos >= this->height)
							break;
					}	
				}
			}
		}
	};

	class entity:public actor {
	public:
		//dialogue
		vector<string> sayings;
		int dialogue_index = 0;

		//sounds
		Mix_Chunk* footstep;
		Mix_Chunk* footstep2;
		Mix_Chunk* voice;

		int footstep_reset = 0; //used for playing footsteps accurately with anim

		

		//basic movement
		float xagil = 0;
		float yagil = 0;
		float xaccel = 0;
		float yaccel = 0;
		float zaccel = 0;
		float xvel = 0;
		float yvel = 0;
		float zvel = 0;
		int layer = 0; //related to z, used for boxs
		bool grounded = 1; //is standing on ground
		float xmaxspeed = 0;
		float ymaxspeed = 0;
		float friction = 0;

		int update_z_time = 0; 
		int max_update_z_time = 1; //update zpos every x frames
		float oldz = 0;

		//there are navNode* called dest and current, but those 
		//are low-level, and updated fickely.
		//destination will be pursued via pathfinding each frame, if set.
		navNode* Destination = nullptr; 
		
		
		

		//animation
		bool animate = false; //does the squash/stretch animation for walking, talking... everything really
		float animtime = 0; //time since having started animating
		float animspeed;
		float animlimit; // the extent to the animation. 0.5 means halfway
		float curwidth;
		float curheight;
		SDL_RendererFlip flip = SDL_FLIP_NONE; //SDL_FLIP_HORIZONTAL; // SDL_FLIP_NONE

		int frame = 0; //current frame on SPRITESHEET
		int msPerFrame = 0; //how long to wait between frames of animation, 0 being infinite time (no frame animation)
		int msTilNextFrame = 0; //accumulater, when it reaches msPerFrame we advance frame
		int frameInAnimation = 0; //current frame in ANIMATION
		bool loopAnimation = 0; //start over after we finish
		int animation = 0; //current animation, or the column of the spritesheet
		int defaultAnimation = 0;

		int framewidth = 120; //width of single frame
		int frameheight = 120; //height of frame
		int xframes = 1; //number of frames ACROSS the spritesheet
		int yframes = 1; //number of frames DOWN the spritesheet
		vector<coord*> framespots;
		bool up, down, left, right; //for chusing one of 8 animations for facing
		bool hadInput = 0; //had input this frame;
		int shooting = 0; //1 if character is shooting

		//object-related design
		bool dynamic = true; //true for things such as wallcaps. movement/box is not calculated if this is false
		bool inParty = false;
		bool talks = false;
		bool wallcap = false; //used for wallcaps
		cshadow * shadow = 0;
		
		//for textured entities (e.g. wallcap)
		float xoffset = 0;
		float yoffset = 64;
		bool wall; //to darken entwalls
		float sortingOffset = 0; //to make entites float in the air, or sort as if they are higher or lower than they are first use was for building diagonal walls
		float extraYOffset = 0; //used to encode a permanent y axis offset, for diagonal walls
		//0 - slant left, 1 - slant right, 2 - pillar
		int effect = 0;
		bool asset_sharer = 0;

		//self-data
		int data[255] = {0};

		//combat
		weapon* hisWeapon;
		bool canFight = 1;
		bool invincible = 0;
		float invincibleMS = 0; //ms before setting invincible to 0
		bool agrod = 0; //are they fighting a target?
		entity* target = nullptr; //who are they fighting?
		int targetFaction = -1; //what faction are they fighting at the moment? If agrod and they lose their target, they will pick a random visible target having this faction
		coord dcoord; //place where they want to stand
		int faction = 0; //0 is player, 1 is most enemies
		float level = 1; //affects damage
		float maxhp = 2;
		float hp = 2;
		float dhpMS = 0; //how long to apply dhp (dot, healing)
		float dhp = 0;
		string weaponName = "";
		int cost = 1000; //cost to spawn in map and then xp granted to killer

		Status status = none;
		float statusMS = 0; //how long to apply status before it becomes "none"
		float statusMag = 0; //magnitude of status


		//pathfinding
		float updateSpeed = 1500;
		float timeSinceLastUpdate = 0;
		navNode* dest = nullptr;
		navNode* current = nullptr;
		vector<navNode*> path;
		float dijkstraSpeed = 800; //how many updates to wait between calling dijkstra's algorithm
		float timeSinceLastDijkstra = -1;
		bool pathfinding = 0;
		float maxDistanceFromHome = 1400;
		float range = 3;
		int stuckTime = 0; //time since ai is trying to go somewhere but isn't moving
		int maxStuckTime = 10; //time waited before resolving stuckness
		float lastx = 0;
		float lasty = 0;

		//default constructor is called automatically for children
		entity() {
			//M("entity()" );
		};

		entity(SDL_Renderer * renderer, string filename, float sizeForDefaults = 1) {
			M("entity()");
			
			ifstream file;
			bool using_default = 0;
			this->name = filename;

			string loadstr;
			//try to open from local map folder first
			
			loadstr = "maps/" + g_map + "/" + filename + ".ent";
			D(loadstr);
			const char* plik = loadstr.c_str();
			
			file.open(plik);
			
			if (!file.is_open()) {
				//load from global folder
				loadstr = "entities/" + filename + ".ent";
				const char* plik = loadstr.c_str();
				
				file.open(plik);
				
				if (!file.is_open()) {
					//just make a default entity
					using_default = 1;
					string newfile = "entities/default.ent";
					file.open(newfile);
				}
			}
			
			string temp;
			file >> temp;
			string spritefilevar;
			
			if(!using_default) {
				spritefilevar = temp; 
			} else {
				spritefilevar = "sprites/" + filename + ".png";
				M(spritefilevar );
			}
			
			const char* spritefile = spritefilevar.c_str();
			float size;
			string comment;
			file >> comment;
			file >> size;

			file >> comment;
			file >> this->xagil;
			file >> this->yagil;
			
			
			file >> comment;
			file >> this->xmaxspeed;
			file >> this->ymaxspeed;
			
			file >> comment;
			file >> this->friction;
			file >> comment;
			file >> this->bounds.width;
			file >> this->bounds.height;
			
			file >> comment;
			file >> this->bounds.x;
			file >> this->bounds.y;

			file >> comment;
			float fsize;
			file >> fsize;
			shadow = new cshadow(renderer, fsize);
			
			this->shadow->owner = this;


			file >> comment;
			file >> shadow->xoffset;
			file >> shadow->yoffset;		
			
			file >> comment;
			file >> this->animspeed;
			file >> this->animlimit;

			file >> comment;
			file >> this->defaultAnimation;
			animation = defaultAnimation;
			
			file >> comment;
			file >> this->framewidth;
			file >> this->frameheight;
			this->shadow->width = framewidth * fsize;
			this->shadow->height = framewidth * fsize * (1/p_ratio);
			
			file >> comment;
			bool setboxfromshadow;
			file >> setboxfromshadow;
			

			

			file >> comment;
			file >> this->dynamic;
			bool solidifyHim = 0;
			
			file >> comment;
			file >> solidifyHim;
			if(solidifyHim) {
				this->solidify();
			}

			file >> comment;
			file >> this->talks;

			file >> comment;
			file >> this->faction;
			if(faction != 0) {
				canFight = 1;
			}
			

			file >> comment;
			file >> this->weaponName;

			file >> comment;
			file >> this->agrod;

			file >> comment;
			file >> maxhp;
			hp = maxhp;

			file >> comment;
			file >> cost;


			if(canFight) {
				//check if someone else already made the weapon
				bool cached = 0;
				
				if(g_weapons.size() > 0) {
					for (auto g : g_weapons) {
						D(g->name);
						D(weaponName);
						if(g->name == weaponName) {
							hisWeapon = g;
							cached = 1;
							break;
						}
					}
				}
				
				if(!cached) {
					hisWeapon = new weapon(weaponName);
				}

				//either way, make the faction set properly
				hisWeapon->faction = this->faction;
			}

			file.close();
			
			//load dialogue file
			if(talks) {
				string txtfilename = "";
				//open from local folder first
				if (fileExists("maps/" + g_map + "/" + filename + ".txt")) {
					txtfilename = "maps/" + g_map + "/" + filename + ".txt";
					M("Using local script");
				} else {
					M("Using global script");
					txtfilename = "scripts/" + filename + ".txt";
				}
				ifstream file(txtfilename);
				string line;

				//load voice
				getline(file, line);
				line = "sounds/voice-" + line +".wav";
				voice = Mix_LoadWAV(line.c_str());

				while(getline(file, line)) {
					sayings.push_back(line);
				}
				
				
			}
			
			SDL_Surface* image = IMG_Load(spritefile);
			if(onionmode) {
				image = IMG_Load("sprites/onionmode/onion.png");
				
			}
			
			texture = SDL_CreateTextureFromSurface(renderer, image);
			this->width = size * framewidth;
			this->height = size * frameheight;

			//move shadow to feet
			shadow->xoffset += width/2 - shadow->width/2;
			shadow->yoffset -= height - shadow->height;
			

			if(setboxfromshadow) {
				this->bounds.width = this->shadow->width;
				this->bounds.height = this->shadow->height;
				this->bounds.x = this->shadow->xoffset;
				this->bounds.y = this->shadow->yoffset;
				
			}
			
			if(using_default) {
				int w, h;
				SDL_QueryTexture(texture, NULL, NULL, &w, &h);
				this->width = w * sizeForDefaults;
				this->height = h * sizeForDefaults;
				this->shadow->width = 0;
			}

			curwidth = width;
			curheight = height;

			
			if(!using_default) {
				xframes = image->w /framewidth;
				yframes = image->h /frameheight;
			
		

			for (int j = 0; j < image->h; j+=frameheight) {
				for (int i = 0; i < image->w; i+= framewidth) {
					coord* a = new coord(); 
					a->x = i;
					a->y = j;
					framespots.push_back(a);
				}
			}

			}
			SDL_FreeSurface(image);
			g_entities.push_back(this);
		}


		~entity() {
			M("~entity()" );
			if (!wallcap) {
				delete shadow;
			}
			if(!asset_sharer) {
				SDL_DestroyTexture(texture);
			}
			for (auto p : framespots) {
				delete p;
			} 
			framespots.clear();
			if(canFight) {
				//this can create crashes if an entity dies with bullets on screen
				//delete hisWeapon;
			}
			g_entities.erase(remove(g_entities.begin(), g_entities.end(), this), g_entities.end());
			for(auto x : g_entities) {
				D(x->name);
			}
		}

		int getOriginX() {
			return x + bounds.x + bounds.width/2;
		}

		int getOriginY() {
			return y + bounds.y + bounds.height/2;
		}

		rect getMovedBounds() {
			return rect(bounds.x + x, bounds.y + y, bounds.width, bounds.height);
		}

		void solidify() {
			//consider checking member field for solidness, and updating
			//shouldnt cause a crash anyway
			g_solid_entities.push_back(this);
		}

		void unsolidify() {
			g_solid_entities.erase(remove(g_solid_entities.begin(), g_solid_entities.end(), this), g_solid_entities.end());
		}

		void shoot();

		void render(SDL_Renderer * renderer, camera fcamera) {
			if(this == protag) { g_protagHasBeenDrawnThisFrame = 1; }
			//if its a wallcap, tile the image just like a maptile
			if(wallcap && effect == 0) {
				
				

			SDL_FPoint nowt = {0, 0};

			rect obj((x -fcamera.x)* fcamera.zoom, (y-fcamera.y - height - XtoZ * z) * fcamera.zoom, width * fcamera.zoom, height * fcamera.zoom);		
			rect cam(0, 0, fcamera.width, fcamera.height);
			
			if(RectOverlap(obj, cam)) {
				/*
				//if its a wall, check if it is covering the player
				if(this->wall && protagForGlimmer != nullptr) {
					
					//make obj one block higher for the wallcap
					//obj.height -= 45;
					//obj.y += 45;
					if((protagForGlimmer->x - fcamera.x) * fcamera.zoom > obj.x && (protagForGlimmer->x - fcamera.x) * fcamera.zoom < obj.x + obj.width && (protagForGlimmer->y  - fcamera.y) * fcamera.zoom > obj.y && (protagForGlimmer->y  - fcamera.y) * fcamera.zoom < obj.y + obj.height) {
						protagGlimmerA = 1;
					}
					if((protagForGlimmer->x + protagForGlimmer->width - fcamera.x) * fcamera.zoom > obj.x && (protagForGlimmer->x + protagForGlimmer->width - fcamera.x) * fcamera.zoom < obj.x + obj.width && (protagForGlimmer->y  - fcamera.y) * fcamera.zoom > obj.y && (protagForGlimmer->y  - fcamera.y) * fcamera.zoom < obj.y + obj.height) {
						protagGlimmerB = 1;
					}
					if((protagForGlimmer->x - fcamera.x) * fcamera.zoom > obj.x && (protagForGlimmer->x - fcamera.x) * fcamera.zoom < obj.x + obj.width && (protagForGlimmer->y - protagForGlimmer->height - fcamera.y) * fcamera.zoom > obj.y && (protagForGlimmer->y - protagForGlimmer->height - fcamera.y) * fcamera.zoom < obj.y + obj.height) {
						protagGlimmerC = 1;
					}
					if((protagForGlimmer->x + protagForGlimmer->width - fcamera.x) * fcamera.zoom > obj.x && (protagForGlimmer->x + protagForGlimmer->width - fcamera.x) * fcamera.zoom < obj.x + obj.width && (protagForGlimmer->y - protagForGlimmer->height - fcamera.y) * fcamera.zoom > obj.y && (protagForGlimmer->y - protagForGlimmer->height - fcamera.y) * fcamera.zoom < obj.y + obj.height) {
						protagGlimmerD = 1;
					}
					
				}
				*/
				
				SDL_Rect srcrect;
				SDL_FRect dstrect;
				int ypos = 0;
				int xpos = 0;
				srcrect.x = xoffset;
				srcrect.y = yoffset;
				
				while(1) {
					if(srcrect.x == xoffset) {
							srcrect.w = framewidth - xoffset;
							dstrect.w = framewidth - xoffset;
						} else {
							srcrect.w = framewidth;
							dstrect.w = framewidth;
						}
						if(srcrect.y == yoffset) {
							
							srcrect.h = frameheight - yoffset;
							dstrect.h = frameheight - yoffset;
						} else {
							dstrect.h = frameheight;
							srcrect.h = frameheight;
						}

					dstrect.x = (x + xpos);
					dstrect.y = (y+ ypos- z * XtoZ );
					
					
					
					
					//are we still inbounds?
					if(xpos + dstrect.w > this->width) {
						dstrect.w = this->width - xpos;
						if(dstrect.w + srcrect.x > framewidth) {
							dstrect.w = framewidth - srcrect.x;
						}
						srcrect.w = dstrect.w;
					}
					if(ypos + dstrect.h > this->height ) {
						
						dstrect.h = this->height - ypos;
						if(dstrect.h + srcrect.y > frameheight) {
							dstrect.h = frameheight - srcrect.y;
						}
						srcrect.h = dstrect.h;
						
					}

					
					
					
					


					//transform
					dstrect.w = ceil(dstrect.w * fcamera.zoom);
					dstrect.h = ceil(dstrect.h * fcamera.zoom);

					dstrect.x = floor((dstrect.x - fcamera.x)* fcamera.zoom);
					dstrect.y = floor((dstrect.y - fcamera.y - height)* fcamera.zoom);
					

					SDL_RenderCopyExF(renderer, texture, &srcrect, &dstrect, 0, &nowt, SDL_FLIP_NONE );
					xpos += srcrect.w;
					srcrect.x = 0;
					
					if(xpos >= this->width) {
						
						xpos = 0;
						srcrect.x = xoffset;
						
						ypos += srcrect.h;
						srcrect.y = 0;	
						if(ypos >= this->height)
							break;
					}	
				}
			}
		
		} else {

				rect obj(floor((x -fcamera.x + (width-curwidth)/2)* fcamera.zoom) , floor(((y-curheight - z * XtoZ) - fcamera.y) * fcamera.zoom), ceil(curwidth * fcamera.zoom), ceil(curheight * fcamera.zoom));		
				rect cam(0, 0, fcamera.width, fcamera.height);
				
				if(RectOverlap(obj, cam)) {
					//set frame from animation
					// animation is y, frameInAnimation is x
					if(dynamic && hadInput) {
						if(up) {
							if(left) {
								animation = 1;
								flip = SDL_FLIP_NONE;
							} else if (right) {
								animation = 1;
								flip = SDL_FLIP_HORIZONTAL;
							} else {
								animation = 0;
								flip = SDL_FLIP_NONE;
							}
						} else if (down) {
							if(left) {
								animation = 3;
								flip = SDL_FLIP_NONE;
							} else if (right) {
								animation = 3;
								flip = SDL_FLIP_HORIZONTAL;
							} else {
								animation = 4;
								flip = SDL_FLIP_NONE;
							}
						} else {
							if(left) {
								animation = 2;
								flip = SDL_FLIP_NONE;
							} else if (right) {
								animation = 2;
								flip = SDL_FLIP_HORIZONTAL;
							} else {
								//default
								animation = 4;
								flip = SDL_FLIP_NONE;
							}
						}
					}
					hadInput = 0;

					frame = animation * xframes + frameInAnimation;
					SDL_FRect dstrect = { obj.x, obj.y, obj.width, obj.height};
					if(framespots.size() > 0) {
						//SDL_FRect dstrect = { obj.x, obj.y, obj.width, obj.height};
						SDL_Rect srcrect = {framespots[frame]->x,framespots[frame]->y, framewidth, frameheight};
						const SDL_FPoint center = {0 ,0};
						if(texture != NULL) {
							SDL_RenderCopyExF(renderer, texture, &srcrect, &dstrect, 0, &center, flip);
						}
					} else {
						if(texture != NULL) {
							SDL_RenderCopyF(renderer, texture, NULL, &dstrect);
						}
					}
				}
			}
		}

		void move_up() {
			//y-=yagil;
			yaccel = -1* yagil;
			if(shooting) { return;}
			up = true;
			down = false;
			hadInput = 1;
		}

		void stop_verti() {
			yaccel = 0;
			if(shooting) { return;}
			up = false;
			down = false;
		}

		void move_down() {
			//y+=yagil;
			yaccel = yagil;
			if(shooting) { return;}
			down = true;
			up = false;
			hadInput = 1;
		}

		void move_left() {
			//x-=xagil;
			xaccel = -1 * xagil;
			//x -= 3;
			if(shooting) { return;}
			left = true;
			right = false;
			hadInput = 1;
		}

		void stop_hori() {
			xaccel = 0;
			if(shooting) { return;}
			left = false;
			right = false;
		}
		
		void move_right() {
			//x+=xagil;
			xaccel = xagil;
			if(shooting) { return;}
			right = true;
			left = false;
			hadInput = 1;
			
		}

		void shoot_up() {
			shooting = 1;
			up = true;
			down = false;
			hadInput = 1;
		}
		
		void shoot_down() {
			shooting = 1;
			down = true;
			up = false;
			hadInput = 1;
		}
		
		void shoot_left() {
			shooting = 1;
			left = true;
			right = false;
			hadInput = 1;
		}

		void shoot_right() {
			shooting = 1;
			xaccel = xagil;
			right = true;
			left = false;
			hadInput = 1;
		}

		template <class T>
		T* Get_Closest_Node(vector<T*> array) {
			float min_dist = 0;
			T* ret;
			bool flag = 1;

			int cacheX = getOriginX();
			int cacheY = getOriginY();

			//todo check for boxs
			for (int i = 0; i < array.size(); i++) {
				float dist = Distance(cacheX, cacheY, array[i]->x, array[i]->y);
				if(dist < min_dist || flag) {
					min_dist = dist;
					ret = array[i];
					flag = 0;
				}
			}
			return ret;
			
		}



		//returns a pointer to a door that the player used
		virtual door* update(vector<box*> boxs, vector<door*> doors, float elapsed) {
			if(msPerFrame != 0) {
				msTilNextFrame += elapsed;
				if(msTilNextFrame > msPerFrame) {
					msTilNextFrame = 0;
					frameInAnimation++;
					if(frameInAnimation == xframes) {
						if(loopAnimation) {
							frameInAnimation = 0;
						} else {
							frameInAnimation = xframes - 1;
							msPerFrame = 0;
						}
					}
				}
			}
			if(animate && !transition) {
				curwidth = width * ((sin(animtime*animspeed))   + (1/animlimit)) * (animlimit);
				curheight = height * ((sin(animtime*animspeed + PI))+ (1/animlimit)) * (animlimit);
				animtime += elapsed;
				if(this == protag && (abs(xvel) > 50 || abs(yvel) > 50) && (1 - sin(animtime * animspeed) < 0.01 || 1 - sin(animtime * animspeed + PI) < 0.01)) {
					if(footstep_reset) {
						footstep_reset = 0;
						if(1 - sin(animtime * animspeed) < 0.01) {
							//playSound(-1, footstep, 0);
						} else {
							//playSound(-1, footstep2, 0);
						}
						
						
					}
				} else {
					footstep_reset = 1;
				}
			} else {
				animtime = 0;
				curwidth = curwidth * 0.8 + width * 0.2;
				curheight = curheight * 0.8 + height* 0.2;
			}
			
			
			if(!dynamic) { return nullptr; }
			//should we animate?
			if(xaccel != 0 || yaccel != 0) {
				animate = 1;
			} else {  
				animate = 0;
			}
			
			

			//normalize accel vector
			float vectorlen = pow( pow(xaccel, 2) + pow(yaccel, 2), 0.5) / (xmaxspeed);
			if(xaccel != 0) {
				xaccel /=vectorlen;
			}
			if(yaccel != 0) {
				yaccel /=vectorlen;
				yaccel /= p_ratio;
			}
			if(xaccel > 0) {
				xvel += xaccel * ((double) elapsed / 256.0);
			}
			
			if(xaccel < 0) {
				xvel += xaccel * ((double) elapsed / 256.0);
			}

			if(yaccel > 0) {
				yvel += yaccel* ((double) elapsed / 256.0);
			}
			
			if(yaccel < 0) {
				yvel += yaccel* ((double) elapsed / 256.0);
			}

			rect movedbounds;
			bool ycollide = 0;
			bool xcollide = 0;
			

			

			//turn off boxs if using the map-editor
			if(boxsenabled) {
				//..check door
				if(this == protag) {
					for (int i = 0; i < doors.size(); i++) {	
						//update bounds with new posj
						rect movedbounds = rect(bounds.x + x + xvel * ((double) elapsed / 256.0), bounds.y + y  + yvel * ((double) elapsed / 256.0), bounds.width, bounds.height);
						//did we walk into a door?
						if(RectOverlap(movedbounds, doors[i]->bounds)) {
							//take the door.
							return doors[i];
						}
					}
				}
				for (auto n : g_solid_entities) {	
					if(n == this) {continue;}
					//update bounds with new pos
					rect thismovedbounds = rect(bounds.x + x, bounds.y + y  + (yvel * ((double) elapsed / 256.0)), bounds.width, bounds.height);
					rect thatmovedbounds = rect(n->bounds.x + n->x, n->bounds.y + n->y, n->bounds.width, n->bounds.height);
					//uh oh, did we collide with something?
					if(RectOverlap(thismovedbounds, thatmovedbounds)) {
						ycollide = true;
						yvel = 0;
					}
					//update bounds with new pos
					thismovedbounds = rect(bounds.x + x + (xvel * ((double) elapsed / 256.0)), bounds.y + y, bounds.width, bounds.height);
					//uh oh, did we collide with something?
					if(RectOverlap(thismovedbounds, thatmovedbounds)) {
						xcollide = true;
						xvel = 0;
					}
				}

				for (int i = 0; i < boxs.size(); i++) {	
					//update bounds with new pos
					rect movedbounds = rect(bounds.x + x, bounds.y + y  + (yvel * ((double) elapsed / 256.0)), bounds.width, bounds.height);
					//uh oh, did we collide with something?
					if(RectOverlap(movedbounds, boxs[i]->bounds)) {
						ycollide = true;
						yvel = 0;
								


						
					}
					//update bounds with new pos
					movedbounds = rect(bounds.x + x + (xvel * ((double) elapsed / 256.0)), bounds.y + y, bounds.width, bounds.height);
					//uh oh, did we collide with something?
					if(RectOverlap(movedbounds, boxs[i]->bounds)) {
						//box detected
						xcollide = true;
						xvel = 0;
						
					}
				}
				
				//how much to push the player to not overlap with triangles.
				float ypush = 0;
				float xpush = 0;
				float jerk = 2; //how much to push the player aside to slide along walls

				//future me: try counting how many triangles the ent overlaps with per axis and disabling jerking
				//if it is more than 1
				
				//and try setting jerk to a comp of the ent velocity to make the movement faster and better-scaling

				//can still get stuck if we walk diagonally into a triangular wall

				for(auto n : g_triangles[layer]){
					rect movedbounds = rect(bounds.x + x, bounds.y + y  + (yvel * ((double) elapsed / 256.0)), bounds.width, bounds.height);
					if(TriRectOverlap(n, movedbounds.x, movedbounds.y, movedbounds.width, movedbounds.height)) {
						//if we move the player one pixel up will we still overlap?
						if((n->type == 3 || n->type == 2) && !TriRectOverlap(n, movedbounds.x - jerk, movedbounds.y, movedbounds.width, movedbounds.height)) {
							xpush = -jerk;
							continue;
						}

						if((n->type == 0 || n->type == 1) && !TriRectOverlap(n, movedbounds.x + jerk, movedbounds.y, movedbounds.width, movedbounds.height)) {
							xpush = jerk;
							continue;
						}
						xpush = 0;
						
						ycollide = true;
						yvel = 0;
					}

					movedbounds = rect(bounds.x + x + (xvel * ((double) elapsed / 256.0)), bounds.y + y, bounds.width, bounds.height);
					if(TriRectOverlap(n, movedbounds.x, movedbounds.y, movedbounds.width, movedbounds.height)) {
						//if we move the player one pixel up will we still overlap?
						if((n->type == 1 || n->type == 2) && !TriRectOverlap(n, movedbounds.x, movedbounds.y - jerk, movedbounds.width, movedbounds.height)) {	 
							ypush = -jerk;
							continue;
						}

						if((n->type == 0 || n->type == 3) && !TriRectOverlap(n, movedbounds.x, movedbounds.y + jerk, movedbounds.width, movedbounds.height)) {	 
							ypush = jerk;
							continue;
						}
						ypush = 0;
						
						xcollide = true;
						xvel = 0;
					}
					
				}
				if(!xcollide) {
					y += ypush;
				}
				if(!ycollide) {		
					x += xpush;
				}
			}
			


			if(!ycollide && !transition) { 
				y+= yvel * ((double) elapsed / 256.0);
				
			}

			//when coordinates are bungled, it isnt happening here
			if(!xcollide && !transition) { 
				x+= xvel * ((double) elapsed / 256.0);
			}
			
			if(grounded) {
				yvel *= pow(friction, ((double) elapsed / 256.0));
				xvel *= pow(friction, ((double) elapsed / 256.0));
			} else {
				yvel *= pow(friction*g_bhoppingBoost, ((double) elapsed / 256.0));
				xvel *= pow(friction*g_bhoppingBoost, ((double) elapsed / 256.0));
			}

			float heightfloor = 0; //filled with floor z from heightmap
			if(g_heightmaps.size() > 0 /*&& update_z_time < 1*/) {
			bool using_heightmap = 0;
			bool heightmap_is_tiled = 0;
			tile* heighttile = nullptr;
			int heightmap_index = 0;
			
			rect tilerect;
			rect movedbounds;
			//movedbounds = rect(this->x + xvel * ((double) elapsed / 256.0), this->y + yvel * ((double) elapsed / 256.0) - this->bounds.height, this->bounds.width, this->bounds.height);
			movedbounds = rect(this->getOriginX(), this->getOriginY(), 0, 0);
			//get what tile entity is on
			//this is poorly set up. It would be better if heightmaps were assigned to tiles, obviously, with a pointer
			bool breakflag = 0;
			for (int i = g_tiles.size() - 1; i >= 0; i--) {
				if(g_tiles[i]->fileaddress == "tiles/marker.png") {continue; }
				tilerect = rect(g_tiles[i]->x, g_tiles[i]->y, g_tiles[i]->width, g_tiles[i]->height);
				
				if(RectOverlap(tilerect, movedbounds)) {
					for (int j = 0; j < g_heightmaps.size(); j++) {
						//M("looking for a heightmap");
						//D(g_heightmaps[j]->name);
						//D(g_tiles[i]->fileaddress);
						if(g_heightmaps[j]->name == g_tiles[i]->fileaddress) {
							//M("found it");
							heightmap_index = j;
							using_heightmap = 1;
							breakflag = 1;
							heightmap_is_tiled = g_tiles[i]->wraptexture;
							heighttile = g_tiles[i];
							break;
						}
					}
					if(breakflag) {break;} //we found it, leave

					//current texture has no mask, keep looking
				}
			}

				
				update_z_time = max_update_z_time;
				//update z position
				SDL_Color rgb = {0, 0, 0};
				heightmap* thismap = g_heightmaps[heightmap_index];
				Uint8 maxred = 0;
				if(using_heightmap) {
					//try each corner;
					//thismap->image->w;
					//code for middle
					//Uint32 data = thismap->heighttilegetpixel(thismap->image, (int)(this->x + xvel * ((double) elapsed / 256.0) + 0.5 * this->width) % thismap->image->w, (int)(this->y + yvel * ((double) elapsed / 256.0) - 0.5 * this->bounds.height) % thismap->image->h);
					Uint32 data;
					if(heightmap_is_tiled) {
						data = thismap->getpixel(thismap->image, (int)(getOriginX()) % thismap->image->w, (int)(getOriginY()) % thismap->image->h);
					} else {
						//tile is not tiled, so we have to get clever with the heighttile pointer

						data = thismap->getpixel(thismap->image, (int)( ((this->getOriginX() - heighttile->x) /heighttile->width) * thismap->image->w), (int)( ((this->getOriginY() - heighttile->y) /heighttile->height) * thismap->image->h));
					}
					SDL_GetRGB(data, thismap->image->format, &rgb.r, &rgb.g, &rgb.b);
					if(RectOverlap(tilerect, movedbounds)) {
						maxred = rgb.r;
					}
					// //code for each corner:
					// Uint32 data = thismap->getpixel(thismap->image, (int)(this->x + xvel * ((double) elapsed / 256.0)) % thismap->image->w, (int)(this->y + yvel * ((double) elapsed / 256.0)) % thismap->image->h);
					// SDL_GetRGB(data, thismap->image->format, &rgb.r, &rgb.g, &rgb.b);
					
					// movedbounds = rect(this->x + xvel * ((double) elapsed / 256.0), this->y + yvel * ((double) elapsed / 256.0), 1, 1);
					// if(RectOverlap(tilerect, movedbounds)) {
					// 	maxred = max(maxred, rgb.r);
					// }

					// data = thismap->getpixel(thismap->image, (int)(this->x + xvel * ((double) elapsed / 256.0) + this->bounds.width) % thismap->image->w, (int)(this->y + yvel * ((double) elapsed / 256.0)) % thismap->image->h);
					// SDL_GetRGB(data, thismap->image->format, &rgb.r, &rgb.g, &rgb.b);
					
					// movedbounds = rect(this->x + xvel * ((double) elapsed / 256.0) + this->bounds.width, this->y + yvel * ((double) elapsed / 256.0), 1, 1);
					// if(RectOverlap(tilerect, movedbounds)) {
					// 	maxred = max(maxred, rgb.r);
					// }

					// data = thismap->getpixel(thismap->image, (int)(this->x + xvel * ((double) elapsed / 256.0)) % thismap->image->w, (int)(this->y + yvel * ((double) elapsed / 256.0) - this->bounds.height) % thismap->image->h);
					// SDL_GetRGB(data, thismap->image->format, &rgb.r, &rgb.g, &rgb.b);
					
					// movedbounds = rect(this->x + xvel * ((double) elapsed / 256.0), this->y + yvel * ((double) elapsed / 256.0) - this->bounds.height, 1, 1);
					// if(RectOverlap(tilerect, movedbounds)) {
					// 	maxred = max(maxred, rgb.r);
					// }

					// data = thismap->getpixel(thismap->image, (int)(this->x + xvel * ((double) elapsed / 256.0) + this->bounds.width) % thismap->image->w, (int)(this->y + yvel * ((double) elapsed / 256.0) - this->bounds.height) % thismap->image->h);
					// SDL_GetRGB(data, thismap->image->format, &rgb.r, &rgb.g, &rgb.b);
					
					// movedbounds = rect(this->x + xvel * ((double) elapsed / 256.0) + this->bounds.width, this->y + yvel * ((double) elapsed / 256.0) - this->bounds.height, 1, 1);
					// if(RectOverlap(tilerect, movedbounds)) {
					// 	maxred = max(maxred, rgb.r);
					// }
					
				}
				//oldz = this->z;
				if(using_heightmap) {
					heightfloor = ((maxred * thismap->magnitude));
				}
				
			} else {
				update_z_time--;
				//this->z = ((oldz) + this->z) / 2 ;
				
			}
			
			layer = max(z /64, 0.0f);
			layer = min(layer, (int)g_boxs.size() - 1);
			//should we fall?
			bool should_fall = 1;
			float floor = 0;
			if(layer > 0) {
				rect thisMovedBounds = rect(bounds.x + x + xvel * ((double) elapsed / 256.0), bounds.y + y + yvel * ((double) elapsed / 256.0), bounds.width, bounds.height);
				for (auto n : g_boxs[layer - 1]) {
					if(RectOverlap(n->bounds, thisMovedBounds)) {
						floor = 64 * (layer);
						break;
					}
				}
				for (auto n : g_triangles[layer - 1]) {
					if(TriRectOverlap(n, thisMovedBounds.x, thisMovedBounds.y, thisMovedBounds.width, thisMovedBounds.height)) {
						floor = 64 * (layer);
						break;
					}
					
				}
				
				
				float shadowFloor = floor;
				floor = max(floor, heightfloor);
				
				bool breakflag = 0;
				for(int i = layer - 1; i >= 0; i--) {
					for (auto n : g_boxs[i]) {
						if(RectOverlap(n->bounds, thisMovedBounds)) {
							shadowFloor = 64 * (i + 1);
							breakflag = 1;
							break;
						}
					}
					if(breakflag) {break;}
					for (auto n : g_triangles[i]) {
						if(TriRectOverlap(n, thisMovedBounds.x, thisMovedBounds.y, thisMovedBounds.width, thisMovedBounds.height)) {
							shadowFloor = 64 * (i + 1);
							breakflag = 1;
							break;
						}
						
					}
					if(breakflag) {break;}
				}
				if(breakflag == 0) {
					//just use heightmap
					shadowFloor = floor;
				}
				this->shadow->z = shadowFloor;
			} else { 
				this->shadow->z = heightfloor;
				floor = heightfloor;
			}
			
			if(z > floor + 1) {
				zaccel -= g_gravity * ((double) elapsed / 256.0);
				grounded = 0;
			} else {
				grounded = 1;
				zvel = max(zvel, 0.0f);
				zaccel = max(zaccel, 0.0f);
				
			}
			
			
			zvel += zaccel * ((double) elapsed / 256.0);
			zvel *= pow(friction, ((double) elapsed / 256.0));
			z += zvel * ((double) elapsed / 256.0);
			z = max(z, floor + 1);
			
			
			z = max(z, heightfloor);
			layer = max(z /64, 0.0f);
			layer = min(layer, (int)g_boxs.size() - 1);

			shadow->x = x + shadow->xoffset;
			shadow->y = y + shadow->yoffset;

			//update combat
			if(shooting) {
				//spawn shot.
				shoot();
			}

			if(!invincible) {
				//M("I'm not invincible");
				//check for projectile box
				for(auto x : g_projectiles) {
					rect thatMovedBounds = rect(x->x + x->bounds.x, x->y + x->bounds.y, x->bounds.width, x->bounds.height);
					rect thisMovedBounds = rect(this->x + bounds.x, y + bounds.y, bounds.width, bounds.height);
					
					if(x->gun->faction != this->faction && ElipseOverlap(thatMovedBounds, thisMovedBounds)) {
						//take damage
						this->hp -= x->gun->damage;

						//destroy projectile
						delete x;			

						//under certain conditions, agro the entity hit and set his target to the shooter
						if(target == nullptr) {
							target = x->owner;
							targetFaction = x->owner->faction;
							agrod = 1;

							//agro all of the boys on this's team who aren't already agrod, and set their target to a close entity from x's faction
							for (auto y : g_entities) {
								if(y->faction == this->faction && (y->agrod == 0 || y->target == nullptr)) {
									y->targetFaction = x->owner->faction;
									y->agrod = 1;
								}
							}
							
						}
					}
				}
			}
			

			//apply statuseffect

			
			//check if he has died
			if(hp <= 0) {
				if(this == protag) {
					//handle death
					// xagil = 0;
					// yagil = 0;
					// protag_can_move = 0;
				} else {
					delete this;
					return nullptr;
				}
			}

			if( (!canFight) || this == protag) {
				return nullptr;
			}
			//shooting ai
			if(agrod) {
				
				//do we have a target?
				if(target != nullptr) {
					//face towards the target
					
					int xdiff = (this->getOriginX()) - (target->getOriginX());
					int ydiff = (this->getOriginY()) - (target->getOriginY());
					ydiff *= 1/XtoY;
					int axdiff = ( abs(xdiff) - abs(ydiff) );
					
					if(axdiff > 0) {
						//xaxis is more important
						this->animation = 2;
						if(xdiff > 0) {
							this->flip = SDL_FLIP_NONE;
						} else {
							this->flip = SDL_FLIP_HORIZONTAL;
						}
					} else {
						//yaxis is more important
						this->flip = SDL_FLIP_NONE;
						if(ydiff > 0) {
							this->animation = 0;
						} else {
							this->animation = 4;
						}
					}
					if(abs(axdiff) < 0.4*XYWorldDistance(getOriginX(), getOriginY(), target->getOriginX(), target->getOriginY())) {
						if(xdiff > 0) {
							this->flip = SDL_FLIP_NONE;
						} else {
							this->flip = SDL_FLIP_HORIZONTAL;
						}
						if(ydiff > 0) {
							this->animation = 1;
						} else {
							this->animation = 3;
						}
					}
					up = 0; left = 0; right = 0; down = 0;
					switch(animation) {
						case 0:
							up = 1;
							break;
						case 1:
							up = 1;
							left = !flip;
							right = flip;
							break;
						case 2:
							left = !flip;
							right = flip;
							break;
						case 3:
							down = 1;
							left = !flip;
							right = flip;
							break;
						case 4:
							down = 1;
							break;
					}

					//this constant is what factor of the range must be had to the player
					//in these types of games, humans seem to shoot even when they are out 
					// of range, so lets go with that
					if(XYWorldDistance(target->getOriginX(), target->getOriginY(), getOriginX(), getOriginY()) < this->hisWeapon->range * 1.1) {
						//we have a target, we're in range, we're angry, so lets shoot
						shoot();
						left = 1;
					}
				} else {
					//another placeholder - target is protag
					if(faction != 0) {
						extern entity* protag;
						target = protag;
					}
					if(targetFaction != -1) {
						//set target to first visible enemy from target faction
						for(auto x : g_entities) {
							if(x->faction == this->targetFaction ) {
								//found an entity that we have vision to
								this->target = x;
							}
						}
					}
				}
			} else {
				//code for becoming agrod when seeing a hostile entity will go here
			}
		
			//walking ai
			if(agrod) {
				if(timeSinceLastDijkstra - elapsed < 0) {
					//need to update our Destination member variable, dijkstra will
					//be called this frame
					if(target != nullptr) {
						//requirements for a valid place to navigate to
						//must be
						//1 - in range
						//2 - have LOS to player (we can even check if the weapon pierces walls later)
						//3 - not be a wall
						//hopefully thats enuff, because i dont really want to make an algorithm
						//to check if a spot is easy to walk to, but its possible.

						//how will we write the algorithm?
						//check the cardinal points of the player having 2/3 of the range
						//thats eight places to check.
						//start with the closest first, otherwise every dijkstra call the AI
						//would basically walk thru the player between these cardinal points
						//if the closest cardinal place fails the second or third requirements
						//try testing another
						//if none work, (e.g. player is standing in the middle of a small room)
						//repeat with half of 2/3 of the range. it might just be better to have them navigate
						//to the player

						//use frame to get prefered cardinal point
						int index = 0;
						switch(animation) {
							case 0:
								//facing up
								index = 0;
								break;
							case 1:
								//facing upleft or upright
								if(flip == SDL_FLIP_HORIZONTAL) {
									index = 1;
								} else {
									index = 7;
								}
								break;
							case 2:
								//facing upleft or upright
								if(flip == SDL_FLIP_HORIZONTAL) {
									index = 2;
								} else {
									index = 6;
								}
								break;
							case 3:
								//facing upleft or upright
								if(flip == SDL_FLIP_HORIZONTAL) {
									index = 3;
								} else {
									index = 5;
								}
								break;
							case 4:
								//facing upleft or upright
								index = 4;
								break;
						} 

						//do we have line of sicht to this destination? if not, lets just run at the player.
						// for (int i = 0; i < 8; i++) {
						// 	//show all cardinalpoints
						// 	vector<int> ret = getCardinalPoint(target->getOriginX(), target->getOriginY(), 200, i);
						// 	SDL_FRect rend;
						// 	int size = 20; 
						// 	rend.x = ret[0] - size/2;
						// 	rend.y = ret[1] - size/2;
						// 	rend.w = size;
						// 	rend.h = size;
						// 	M(ret[0]);
						// 	M(ret[1]);
						// 	M("thats all");
						// 	//SDL_Delay(600);
						// 	rend = transformRect(rend);
						// 	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						// 	SDL_RenderDrawRectF(renderer, &rend);
						// 	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
							
						// }
						// SDL_RenderPresent(renderer);
						// SDL_Delay(10);

						vector<int> ret = getCardinalPoint(target->getOriginX(), target->getOriginY(), this->hisWeapon->range * (0.8), index);
						
						if(LineTrace(ret[0], ret[1], target->getOriginX(), target->getOriginY())) {
							M("There's a good position, keeping my distance");
							//vector<int> ret = getCardinalPoint(target->x, target->y, 200, index);
							Destination = getNodeByPosition(g_navNodes, ret[0], ret[1]);
						
						} else {
							M("cant get a good shot, closing in on player");
							Destination = getNodeByPosition(g_navNodes, target->getOriginX(), target->getOriginY());
						
						}
					}
				}
				//navigate
				if(Destination != nullptr) {
					BasicNavigate(Destination);
				}
				//detect stuckness- if we're stuck, try heading to a random nearby node for a moment
				if(abs(lastx - x) < 0.1 && abs(lasty - y) < 0.1) {
					stuckTime++;
				} else {
					stuckTime = 0;
				}
				lastx = x;
				lasty = y;
				if(stuckTime > maxStuckTime) {
					M("got stuck");
					current = Get_Closest_Node(g_navNodes);
					int c = rand() % current->friends.size();
					Destination = current->friends[c];
					dest = Destination;
					stuckTime = 0;
				}
			} else {
				//patroling/idling/wandering behavior would go here
				xvel = 0;
				yvel = 0;
			}

			return nullptr;
		}

		//all-purpose pathfinding function
		void BasicNavigate(navNode* ultimateTargetNode) {
			if(g_navNodes.size() < 1) {return;}
			if(current == nullptr) {
				current = Get_Closest_Node(g_navNodes);
				dest = current;
			}
			// current->Render(255,0,0);
			// dest->Render(0,255,0);
			// Destination->Render(0,0,255);

			float ydist = 0;
			float xdist = 0;

			xdist = abs(dest->x - getOriginX());
			ydist = abs(dest->y - getOriginY());

			float vect = pow((pow(xdist,2)  + pow(ydist,2)), 0.5);
			float factor = 1;

			if(vect != 0) {
				factor = 1 / vect;
			}

			xdist *= factor;
			ydist *= factor;
				
			if(dest->x > getOriginX()) {
				xaccel = this->xagil * xdist;
			} else {
				xaccel = -this->xagil * xdist;
			}
			if(dest->y > getOriginY()) {
				yaccel = this->yagil * ydist;
			} else {
				yaccel = -this->yagil * ydist;
				
			}
			int prog = 0;
			if(abs(dest->y - getOriginY() ) < 150) {
				prog ++;
			}
			if(abs(dest->x - getOriginX()) < 150) {
				prog ++;
			}

			if(prog == 2) {
				if( path.size() > 0) {
					//take next node in path
					current = dest;
					dest = path.at(path.size() - 1);
					path.erase(path.begin() + path.size()-1);
				} else {
					//path completed
				}
			}
			
			if(timeSinceLastDijkstra < 0) {
				//M("Starting Dijkstra");
				if(dest != nullptr) {
					current = dest;
				}
				current = dest;
				timeSinceLastDijkstra = dijkstraSpeed;
				navNode* targetNode = ultimateTargetNode;
				vector<navNode*> bag;
				for (int i = 0; i < g_navNodes.size(); i++) {
					bag.push_back(g_navNodes[i]);
					g_navNodes[i]->costFromSource = numeric_limits<float>::max();
				}
				current->costFromSource = 0;
				int overflow = 5000;
				
				while(bag.size() > 0) {
					overflow --;
					if(overflow < 0) { break; }

							
					navNode* u;
					float min_dist = numeric_limits<float>::max();
					for (int i = 0; i < bag.size(); i++) { 	
						
							
						if(bag[i]->costFromSource < min_dist){
							u = bag[i];
							min_dist = u->costFromSource;
						}
					}
					//u is closest node in bag
					bag.erase(remove(bag.begin(), bag.end(), u), bag.end());
					for (int i = 0; i < u->friends.size(); i++) {
							
						float alt = u->costFromSource + u->costs[i];
						if(alt < u->friends[i]->costFromSource) {
							u->friends[i]->costFromSource = alt;
							u->friends[i]->prev = u;
						}
					}
				}
				path.clear();
				int secondoverflow = 650;
				while(targetNode != nullptr) {
					secondoverflow--;
					if(secondoverflow < 0) { M("prevented a PF crash."); break;} //preventing this crash results in pathfinding problems
							
					path.push_back(targetNode);
					if(targetNode == current) {
						break;
					}
					targetNode = targetNode->prev;
				}
				
				dest = path.at(path.size() - 1);
				//M("Finished Dikjstra");
			} else {
				timeSinceLastDijkstra -= elapsed;
			}
		
		}
	};

	//search entity by name
	entity* searchEntities(string fname) {
		if(fname == "protag") {
			return protag;
		}
		for(auto n : g_entities) {
			if(n->name == fname) {
				return n;
			}
		}
		return nullptr;
	}

	void entity::shoot() {
		if(this->hisWeapon->cooldown <= 0) {
			for(float i = 0; i < this->hisWeapon->numshots; i++) {

				//M("shoot()");
				hisWeapon->cooldown = hisWeapon->maxCooldown;
				projectile* p = new projectile(hisWeapon);
				
				p->owner = this;
				p->x = getOriginX() - p->width/2;
				p->y = getOriginY() - p->height/2;
				p->z = z + 32;
				
				//inherit velo from parent
				p->xvel = xvel/15;
				p->yvel = yvel/15;
				//angle
				if(up) {
					if(left) {
						p->angle = 3 * M_PI / 4;
						
					} else if (right) {
						p->angle = M_PI / 4;
					} else {
						p->angle = M_PI / 2;
						
					}
				} else if (down) {
					if(left) {
						p->angle = 5 * M_PI / 4;
						
					} else if (right) {
						p->angle = 7 * M_PI / 4;
						
					} else {
						p->angle = 3 * M_PI / 2;
						
					}
				} else {
					if(left) {
						p->angle = M_PI;
						
					} else if (right) {
						p->angle = 0;
						
					} else {
						//default
						p->angle = 3 * M_PI / 4;
						
					}
				}

				//give it an angle based on weapon spread
				if(hisWeapon->randomspread != 0) {
					float randnum = (((float) rand()/RAND_MAX) * 2) - 1;
					p->angle += (randnum * hisWeapon->randomspread);
				}
				if(hisWeapon->spread != 0) {
					p->angle += ( (i - ( hisWeapon->numshots/2) ) * hisWeapon->spread);
				}

				//move it out of the shooter and infront
				p->x += cos(p->angle) * width/2;
				p->y += cos(p->angle + M_PI / 2) * height/2;

				
				
			}
		}
	}

	void cshadow::render(SDL_Renderer * renderer, camera fcamera) {
		SDL_FRect dstrect = { ((this->x)-fcamera.x) *fcamera.zoom, (( (this->y - (XtoZ * z) ) ) -fcamera.y) *fcamera.zoom, (width * size), (height * size)* (637 /640) * 0.9};
		//SDL_Rect dstrect = {500, 500, 200, 200 };
		//dstrect.y += (owner->height -(height/2)) * fcamera.zoom;
		float temp;
		temp = width;
		temp*= fcamera.zoom;
		dstrect.w = temp;
		temp = height;
		temp*= fcamera.zoom;
		dstrect.h = temp;
		
		rect obj(dstrect.x, dstrect.y, dstrect.w, dstrect.h);

		rect cam(0, 0, fcamera.width, fcamera.height);

		if(RectOverlap(obj, cam)) {
			SDL_RenderCopyF(renderer, texture, NULL, &dstrect);	
		}


	}

	class textbox {
	public:
		SDL_Surface* textsurface = 0;
		SDL_Texture* texttexture = 0;
		SDL_Color textcolor = { 255, 255, 255 };
		SDL_FRect thisrect = {0, 0, 50, 50};
		string content = "Default text.";
		TTF_Font* font = 0;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		bool show = true;

		int errorflag = 0;

		//used for drawing in worldspace
		float boxWidth = 50;
		float boxHeight = 50;
		float boxX = 0;
		float boxY = 0;

		float boxScale = 40;
		bool worldspace = false; //use worldspace or screenspace;

		textbox(SDL_Renderer* renderer, const char* fcontent, float size, float fx, float fy, float fwidth) {
			M("textbox()" );
			if(font != NULL) {
				TTF_CloseFont(font);
			}
			font = TTF_OpenFont(g_font.c_str(), size);
			content = fcontent;

			textsurface =  TTF_RenderText_Blended_Wrapped(font, content.c_str(), textcolor, fwidth * WIN_WIDTH);
			texttexture = SDL_CreateTextureFromSurface(renderer, textsurface);

			int texW = 0;
			int texH = 0;
			x = fx;
			y = fy;
			SDL_QueryTexture(texttexture, NULL, NULL, &texW, &texH);
			this->width = texW;
			this->height = texH;
			thisrect = { fx, fy, (float)texW, (float)texH };

			g_textboxes.push_back(this);
		}
		~textbox() {
			M("~textbox()" );
			g_textboxes.erase(remove(g_textboxes.begin(), g_textboxes.end(), this), g_textboxes.end());
			TTF_CloseFont(font); 
			SDL_DestroyTexture(texttexture);
			SDL_FreeSurface(textsurface);
		}
		void render(SDL_Renderer* renderer, int winwidth, int winheight) {
			if(show) {
				if(worldspace) {
					
					SDL_FRect dstrect = {boxX * winwidth, boxY * winheight, width,  thisrect.h};
					SDL_RenderCopyF(renderer, texttexture, NULL, &dstrect);
					
				} else {
					SDL_RenderCopyF(renderer, texttexture, NULL, &thisrect);

				}
			}
		}
		void updateText(string content, int size, float fwidth) {
			extern entity* nudge;
			if(nudge != nullptr) { return; }
			TTF_CloseFont(font); 
			SDL_DestroyTexture(texttexture);
			SDL_FreeSurface(textsurface);
			font = TTF_OpenFont(g_font.c_str(), size);
			textsurface =  TTF_RenderText_Blended_Wrapped(font, content.c_str(), textcolor, fwidth * WIN_WIDTH);
			texttexture = SDL_CreateTextureFromSurface(renderer, textsurface);
			int texW = 0;
			int texH = 0;
			SDL_QueryTexture(texttexture, NULL, NULL, &texW, &texH);
			width = texW;
			thisrect = { (float)x, (float)y, (float)texW, (float)texH };
			
		}
	};

	class ui {
	public:
		float x;
		float y;

		float xagil;
		float yagil;

		float xaccel;
		float yaccel;

		float xvel;
		float yvel;

		float xmaxspeed;
		float ymaxspeed;

		float width = 0.5;
		float height = 0.5;

		float friction;

		bool show = true;
		SDL_Surface* image;
		SDL_Texture* texture;
		
		//for 9patch
		bool is9patch = 0;
		int patchwidth = 256; //213
		float patchscale = 0.4;

		bool persistent = 0;
		int priority = 0; //for ordering, where the textbox has priority 0 and 1 would put it above

		ui(SDL_Renderer * renderer, const char* filename, float fx, float fy, float fwidth, float fheight, int fpriority) {
			M("ui()" );
			image = IMG_Load(filename);
			width = fwidth;
			height = fheight;
			x = fx;
			y = fy;
			texture = SDL_CreateTextureFromSurface(renderer, image);
			g_ui.push_back(this);
			priority = fpriority;
			
		}

		~ui() {
			M("~ui()" );
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
			g_ui.erase(remove(g_ui.begin(), g_ui.end(), this), g_ui.end());
		}

		void render(SDL_Renderer * renderer, camera fcamera) {
			if(this->show) {
				if(is9patch) {
					if(WIN_WIDTH != 0) {
						patchscale = WIN_WIDTH;
						patchscale /= 4000;
					}
					int ibound = width * WIN_WIDTH;
					int jbound = height * WIN_HEIGHT;
					int scaledpatchwidth = patchwidth * patchscale;
					int i = 0; 
					while (i < ibound) {
						int j = 0;
						while (j < jbound) {
							SDL_FRect dstrect = {i + (x * WIN_WIDTH), j + (y * WIN_HEIGHT), scaledpatchwidth, scaledpatchwidth}; //change patchwidth in this declaration for sprite scale
							SDL_Rect srcrect;
							srcrect.h = patchwidth;
							srcrect.w = patchwidth;
							if(i==0) {
								srcrect.x = 0;

							} else {
							if(i + scaledpatchwidth >= ibound) {
								srcrect.x = 2 * patchwidth;
							}else {
								srcrect.x = patchwidth;
							}}
							if(j==0) {
								srcrect.y = 0;
							} else {
							if(j + scaledpatchwidth >= jbound) {
								srcrect.y = 2 * patchwidth;
							}else {
								srcrect.y = patchwidth;
							}}

							//shrink the last non-border tile to fit well.
							int newheight = jbound - (j + scaledpatchwidth); 
							if(j + (2 * scaledpatchwidth) >= jbound && newheight > 0) {
								
								dstrect.h = newheight;
								j+=  newheight;
							} else {
								j+= scaledpatchwidth;
							}

							int newwidth = ibound - (i + scaledpatchwidth); 
							if(i + (2 * scaledpatchwidth) >= ibound && newwidth > 0) {
								
								dstrect.w = newwidth;
							} else {
							}

							//done to fix occasional 1px gap. not a good fix
							dstrect.h += 1;
							SDL_RenderCopyF(renderer, texture, &srcrect, &dstrect);
							
							
							
						}	
						//increment i based on last shrink
						int newwidth = ibound - (i + scaledpatchwidth); 
						if(i + (2 * scaledpatchwidth) >= ibound && newwidth > 0) {
							i+=  newwidth;
						} else {
							i+= scaledpatchwidth;
						}
					}
					
				} else {
					SDL_FRect dstrect = {x * WIN_WIDTH, y * WIN_HEIGHT, width * WIN_WIDTH, height * WIN_HEIGHT};
					SDL_RenderCopyF(renderer, texture, NULL, &dstrect);
				}
			}
		}

		virtual void update_movement(vector<box*> boxs, float elapsed) {
			if(xaccel > 0 && xvel < xmaxspeed) {
				xvel += xaccel * ((double) elapsed / 256.0);
			}
			
			if(xaccel < 0 && xvel > -1 * xmaxspeed) {
				xvel += xaccel * ((double) elapsed / 256.0);
			}

			if(yaccel > 0 && yvel < ymaxspeed) {
				yvel += yaccel* ((double) elapsed / 256.0);
			}
			
			if(yaccel < 0 && yvel > -1 * ymaxspeed) {
				yvel += yaccel* ((double) elapsed / 256.0);
			}

			rect movedbounds;
			bool ycollide = 0;
			bool xcollide = 0;
			y+= yvel * ((double) elapsed / 256.0);
			x+= xvel * ((double) elapsed / 256.0);
			
		}
	};



	class worldsound {
	public:
		//a playable sound in the world, with a position
		Mix_Chunk* blip;
		float volumeFactor = 1;
		float maxDistance = 1200; //distance at which you can no longer hear the sound
		float minWait = 1;
		float maxWait = 5;

		string name;
		int x = 0;
		int y = 0;
		float cooldown = 0;
		
		worldsound(string filename, int fx, int fy) {
			name = filename;
			M("worldsound()" );
			
			ifstream file;

			string loadstr;
			//try to open from local map folder first
			
			loadstr = "maps/" + g_map + "/" + filename + ".ws";
			D(loadstr);
			const char* plik = loadstr.c_str();
			
			file.open(plik);
			
			if (!file.is_open()) {
				loadstr = "worldsounds/" + filename + ".ws";
				const char* plik = loadstr.c_str();
				
				file.open(plik);
				
				if (!file.is_open()) {
					string newfile = "worldsounds/default.ws";
					file.open(newfile);
				}
			}
			
			string temp;
			file >> temp;
			temp = "sounds/" + temp + ".wav";
			
			blip = Mix_LoadWAV(temp.c_str());
			
			
			x = fx;
			y = fy;

			float tempFloat;
			file >> tempFloat;
			volumeFactor = tempFloat;

			file >> tempFloat;
			maxDistance = tempFloat;

			file >> tempFloat;
			minWait = tempFloat * 1000;
			
			file >> tempFloat;
			maxWait = tempFloat * 1000;
			D(tempFloat);
			g_worldsounds.push_back(this);
		}

		~worldsound() {
			M("~worldsound()" );
			Mix_FreeChunk(blip);
			g_worldsounds.erase(remove(g_worldsounds.begin(), g_worldsounds.end(), this), g_worldsounds.end());
		}

		void update(float elapsed) {
			float dist = Distance(x, y, g_camera.x + g_camera.width/2, g_camera.y + g_camera.height/2);
			
			//linear
			float cur_volume = ((maxDistance - dist)/maxDistance) * volumeFactor * 128;
			
			//logarithmic
			//float cur_volume = volume * 128 * (-log(pow(dist, 1/max_distance) + 2.718));
			//float cur_volume = (volume / (dist / max_distance)) * 128;
			

			if(cur_volume < 0) {
				cur_volume = 0;
			}
			Mix_VolumeChunk(blip, cur_volume);
			
			if(cooldown < 0) {
				//change volume
				
				
				
				playSound(-1, blip, 0);
				cooldown = rand() % (int)maxWait + minWait;
			} else {
				cooldown -= elapsed;
			}
		}
	};

	class musicNode {
	public:
		Mix_Music* blip;
		string name = "empty";
		int x = 0;
		int y = 0;
		float radius = 1200;

		musicNode(string fileaddress, int fx, int fy) {
			name = fileaddress;
			fileaddress = "music/" + fileaddress + ".wav";
			blip = Mix_LoadMUS(fileaddress.c_str());
			x = fx;
			y = fy;
			g_musicNodes.push_back(this);
		}
		~musicNode() {
			Mix_FreeMusic(blip);
			g_musicNodes.erase(remove(g_musicNodes.begin(), g_musicNodes.end(), this), g_musicNodes.end());
		}
	};

	class cueSound {
	public:
		Mix_Chunk* blip;
		string name = "empty";
		int x = 0;
		int y = 0;
		float radius = 1200;
		bool played = 0;
		cueSound(string fileaddress, int fx, int fy, int fradius) {
			name = fileaddress;
			fileaddress = "sounds/" + fileaddress + ".wav";
			blip = Mix_LoadWAV(fileaddress.c_str());
			x = fx;
			y = fy;
			radius = fradius;
			g_cueSounds.push_back(this);
		}
		~cueSound() {
			Mix_FreeChunk(blip);
			g_cueSounds.erase(remove(g_cueSounds.begin(), g_cueSounds.end(), this), g_cueSounds.end());
		}
	};

	void playSoundByName(string fname) {
		Mix_Chunk* sound = 0;
		for (auto s : g_cueSounds) {
			if (s->name == fname) {
				sound = s->blip;
				break;
			}
		}
		if(sound == NULL) {
			E("Soundcue " + fname + " not found in level." + " Not critical.");
		}
		if(!g_mute && sound != NULL) {
			Mix_PlayChannel(0, sound,0);
		}
	}

	class waypoint {
	public:
		float x = 0;
		float y = 0;
		string name;
		waypoint(string fname, float fx, float fy) {
			name = fname;
			x = fx;
			y = fy;
			g_waypoints.push_back(this);
		}
		~waypoint() {
			g_waypoints.erase(remove(g_waypoints.begin(), g_waypoints.end(), this), g_waypoints.end());
		}
	};

	class trigger {
	public:
		int x, y, width, height;
		string binding;
		vector<string> script;
		bool active = 1;

		string targetEntity = "protag"; //what entity will activate the trigger

		trigger(string fbinding, int fx, int fy, int fwidth, int fheight, string ftargetEntity) {
			x = fx;
			y = fy;
			width = fwidth;
			height = fheight;
			binding = fbinding;
			targetEntity = ftargetEntity;
			g_triggers.push_back(this);
			//open and read from the script file
			ifstream stream;
			string loadstr;
			//try to open from local map folder first
			
			loadstr = "maps/" + g_map + "/" + fbinding + ".txt";
			D(loadstr);
			const char* plik = loadstr.c_str();
			
			stream.open(plik);
			
			if (!stream.is_open()) {
				stream.open("events/" + fbinding + ".txt");
			}
			string line;

			getline(stream, line);

			while (getline(stream, line)) {
				script.push_back(line);
			}
			
			for(string x: script) {
				M(x);
			}
		
		}

		~trigger() {
			g_triggers.erase(remove(g_triggers.begin(), g_triggers.end(), this), g_triggers.end());
		}
	};

	class listener {
	public:
		int x, y; //just for debugging
		string binding;
		int block = 0;
		int condition = 1;
		string entityName;
		vector<entity*> listenList;
		bool oneWay = 0; //does this become inactive after "hearing" the entity data once?

		vector<string> script;
		bool active = 1;
		listener(string fname, int fblock, int fcondition, string fbinding, int fx, int fy) {
			x = fx;
			y = fy;
			binding = fbinding;
			entityName = fname;
			block = fblock;
			condition = fcondition;

			g_listeners.push_back(this);
			//open and read from the script file
			ifstream stream;
			string loadstr;
			//try to open from local map folder first
			
			loadstr = "maps/" + g_map + "/" + fbinding + ".event";
			const char* plik = loadstr.c_str();
			
			stream.open(plik);
			
			if (!stream.is_open()) {
				stream.open("events/" + fbinding + ".event");
			}
			string line;
			
			while (getline(stream, line)) {
				script.push_back(line);
			}
			
			//build listenList from current entities
			for(auto x : g_entities) {
				if(x->name == entityName) {
					listenList.push_back(x);
				}
			}
		
		}

		~listener() {
			g_listeners.erase(remove(g_listeners.begin(), g_listeners.end(), this), g_listeners.end());
		}

		int update() {
			if (!active) {return 0;}
			for(auto x : listenList) {
				if(x->data[block] == condition) {
					//do nothing
				} else {
					return 0;
				}
			}
			if(oneWay) {active = 0;}
			return 1;
		}
	};

	void clear_map(camera& cameraToReset) {
		g_budget = 0;
		enemiesMap.clear();
		Mix_FadeOutMusic(1000);
		{
			//turn off VSYNC because otherwise we jitter between this frame and the last throughout the animation
			SDL_GL_SetSwapInterval(0);
			bool cont = false;
			float ticks = 0;
			float lastticks = 0;
			float transitionElapsed = 5;
			float mframes = 60;
			float transitionMinFrametime = 5;
			transitionMinFrametime = 1/mframes * 1000;
			
			
			SDL_Surface* transitionSurface = IMG_Load("tiles/engine/transition.png");

			int imageWidth = transitionSurface->w;
			int imageHeight = transitionSurface->h;

			SDL_Texture* transitionTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, transitionSurface->w, transitionSurface->h );
			SDL_SetTextureBlendMode(transitionTexture, SDL_BLENDMODE_BLEND);


			void* pixelReference;
			int pitch;

			float offset = imageHeight;
			
			while (!cont) {
				
				//onframe things
				SDL_LockTexture(transitionTexture, NULL, &pixelReference, &pitch);
				
				memcpy( pixelReference, transitionSurface->pixels, transitionSurface->pitch * transitionSurface->h);
				Uint32 format = SDL_PIXELFORMAT_ARGB8888;
				SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );
				Uint32* pixels = (Uint32*)pixelReference;
				int numPixels = imageWidth * imageHeight;
				Uint32 transparent = SDL_MapRGBA( mappingFormat, 0, 0, 0, 255);
				//Uint32 halftone = SDL_MapRGBA( mappingFormat, 50, 50, 50, 128);

				offset += g_transitionSpeed + 0.02 * offset;
				
				for(int x = 0;  x < imageWidth; x++) {
					for(int y = 0; y < imageHeight; y++) {


						int dest = (y * imageWidth) + x;
						int src =  (y * imageWidth) + x;
						
						if(pow(pow(imageWidth/2 - x,2) + pow(imageHeight + y,2),0.5) < offset) {
							pixels[dest] = transparent;
						} else {
							// if(pow(pow(imageWidth/2 - x,2) + pow(imageHeight + y,2),0.5) < 10 + offset) {
							// 	pixels[dest] = halftone;
							// } else {
								pixels[dest] = 0;
							// }
						}
						
					}
				}

		
				


				ticks = SDL_GetTicks();
				transitionElapsed = ticks - lastticks;
				//lock framerate
				if(transitionElapsed < transitionMinFrametime) {
					SDL_Delay(transitionMinFrametime - transitionElapsed);
					ticks = SDL_GetTicks();
					transitionElapsed = ticks - lastticks;
					
				}	
				lastticks = ticks;

				SDL_UnlockTexture(transitionTexture);
				SDL_RenderCopy(renderer, transitionTexture, NULL, NULL);

				SDL_RenderPresent(renderer);

				if(offset > imageHeight + pow(pow(imageWidth/2,2) + pow(imageHeight,2),0.5)) {
					cont = 1;
				}
			}
			SDL_FreeSurface(transitionSurface);
			SDL_DestroyTexture(transitionTexture);
			transition = 1;
			SDL_GL_SetSwapInterval(1);
		}


		cameraToReset.resetCamera();
		int size;
		size = g_entities.size();

		g_actors.clear();

		//delete all shadows of projectiles
		for(auto x : g_projectiles) {
			delete x;
		}

		//copy protag to a pointer, clear the array, and re-add protag
		entity* hold_protag;
		D(protag->inParty);
		for(int i=0; i< size; i++) {
			if(g_entities[0]->inParty) {
				//remove from array without deleting
				g_entities.erase(remove(g_entities.begin(), g_entities.end(), g_entities[0]), g_entities.end());
			} else {
				delete g_entities[0];
			}
		}
		//push back any entities that were in the party
		for (int i = 0; i < party.size(); i++) {
			g_entities.push_back(party[i]);
			g_actors.push_back(party[i]);
		}
		//push back any shadows we want to keep
		for(auto n : g_shadows) {
			g_actors.push_back(n);
		}
		
		size = g_tiles.size();
		for(int i = 0; i < size; i++) {
			delete g_tiles[0];
		}

		size = g_navNodes.size();
		for(int i = 0; i < size; i++) {
			delete g_navNodes[0];
		}

		size = g_worldsounds.size();
		for(int i = 0; i < size; i++) {
			delete g_worldsounds[0];
		}

		size = g_musicNodes.size();
		for(int i = 0; i < size; i++) {
			delete g_musicNodes[0];
		}

		size = g_cueSounds.size();
		for(int i = 0; i < size; i++) {
			delete g_cueSounds[0];
		}

		size = g_waypoints.size();
		for(int i = 0; i < size; i++) {
			delete g_waypoints[0];
		}

		size = g_doors.size();
		for(int i = 0; i < size; i++) {
			delete g_doors[0];
		}

		size = g_triggers.size();
		for(int i = 0; i < size; i++) {
			delete g_triggers[0];
		}

		size = g_heightmaps.size();
		for(int i = 0; i < size; i++) {
			delete g_heightmaps[0];
		}

		size = g_listeners.size();
		for(int i = 0; i < size; i++) {
			delete g_listeners[0];
		}

		size = g_weapons.size();
		for(int i = 0; i < size; i++) {
			if(g_weapons[0] == protag->hisWeapon) {
				//save the weapon this way
				swap(g_weapons[0], g_weapons[g_weapons.size()-1]);
				continue;
			}
			delete g_weapons[0];
		}

		vector<ui*> persistentui;
		size = g_ui.size();
		for(int i = 0; i < size; i++) {
			if(g_ui[i]->persistent) {
				persistentui.push_back(g_ui[i]);
				g_ui.erase(remove(g_ui.begin(), g_ui.end(), g_ui[0]), g_ui.end());
			} else {
				delete g_ui[0];
			}
		}

		for(auto x : persistentui) {
			g_ui.push_back(x);
		}
		
		g_solid_entities.clear();

		//unloading takes too long- probably because whenever a mapObject is deleted we search the array
		//lets try clearing the array first, because we should delete everything properly afterwards anyway
		g_mapObjects.clear();

		//new, delete all mc, which will automatycznie delete the others
		size = g_mapCollisions.size();
		for (int i = 0; i < size; i++) {
			//M("Lets delete a mapCol");
			int jsize = g_mapCollisions[0]->children.size();
			for (int j = 0; j < jsize; j ++) {
				//M("Lets delete a mapCol child");
				delete g_mapCollisions[0]->children[j];
			}
			delete g_mapCollisions[0];
		}

		//clear layers of boxes and triangles
		for(int i = 0; i < g_boxs.size(); i++) {
			g_boxs[i].clear();
		}
		for(int i = 0; i < g_triangles.size(); i++) {
			g_triangles[i].clear();
		}
		if(g_backgroundLoaded && background != 0) {
			M("deleted background");
			SDL_DestroyTexture(background);
			extern string backgroundstr;
			backgroundstr = "";
			background = 0;
		}
		newClosest = 0;
	}

	#endif
