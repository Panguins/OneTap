#pragma once
#include <cmath>
#include <algorithm>
#include "memory.h"

#pragma warning( push, 0 )

typedef unsigned long ulong;
#define  Assert( _exp )										((void)0)
#define DEFAULT_TICK_INTERVAL	(0.015)				// 15 msec is the default
#define MINIMUM_TICK_INTERVAL   (0.001)
#define MAXIMUM_TICK_INTERVAL	(0.1)
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10
// This is the max # of players the engine can handle
#define ABSOLUTE_PLAYER_LIMIT 255  // not 256, so we can send the limit as a byte 
#define ABSOLUTE_PLAYER_LIMIT_DW	( (ABSOLUTE_PLAYER_LIMIT/32) + 1 )

// a player name may have 31 chars + 0 on the PC.
// the 360 only allows 15 char + 0, but stick with the larger PC size for cross-platform communication
#define MAX_PLAYER_NAME_LENGTH		32

#define MAX_PLAYERS_PER_CLIENT		1	// One player per PC

// Max decorated map name, with things like workshop/cp_foo.ugc123456
#define MAX_MAP_NAME				96

// Max name used in save files. Needs to be left at 32 for SourceSDK compatibility.
#define MAX_MAP_NAME_SAVE			32

// Max non-decorated map name for e.g. server browser (just cp_foo)
#define MAX_DISPLAY_MAP_NAME		32

#define	MAX_NETWORKID_LENGTH		64  // num chars for a network (i.e steam) ID

// BUGBUG: Reconcile with or derive this from the engine's internal definition!
// FIXME: I added an extra bit because I needed to make it signed
#define SP_MODEL_INDEX_BITS			13

// How many bits to use to encode an edict.
#define	MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	MAX_EDICTS					(1<<MAX_EDICT_BITS)

// How many bits to use to encode an server class index
#define MAX_SERVER_CLASS_BITS		9
// Max # of networkable server classes
#define MAX_SERVER_CLASSES			(1<<MAX_SERVER_CLASS_BITS)

#define SIGNED_GUID_LEN 32 // Hashed CD Key (32 hex alphabetic chars + 0 terminator )

// Used for networking ehandles.
#define NUM_ENT_ENTRY_BITS		(MAX_EDICT_BITS + 1)
#define NUM_ENT_ENTRIES			(1 << NUM_ENT_ENTRY_BITS)
#define ENT_ENTRY_MASK			(NUM_ENT_ENTRIES - 1)
#define INVALID_EHANDLE_INDEX	0xFFFFFFFF

#define NUM_SERIAL_NUM_BITS		(32 - NUM_ENT_ENTRY_BITS)


// Networked ehandles use less bits to encode the serial number.
#define NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS	10
#define NUM_NETWORKED_EHANDLE_BITS					(MAX_EDICT_BITS + NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS)
#define INVALID_NETWORKED_EHANDLE_VALUE				((1 << NUM_NETWORKED_EHANDLE_BITS) - 1)

// This is the maximum amount of data a PackedEntity can have. Having a limit allows us
// to use static arrays sometimes instead of allocating memory all over the place.
#define MAX_PACKEDENTITY_DATA	(16384)

// This is the maximum number of properties that can be delta'd. Must be evenly divisible by 8.
#define MAX_PACKEDENTITY_PROPS	(4096)

// a client can have up to 4 customization files (logo, sounds, models, txt).
#define MAX_CUSTOM_FILES		4		// max 4 files
#define MAX_CUSTOM_FILE_SIZE	524288	// Half a megabyte

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01	// bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL		0x02	// procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL		0x04	// bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE	0x08	// bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER	0x10	// bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800	
#define BONE_USED_BY_VERTEX_LOD2	0x00001000  
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000	// bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS		0x00200000	// Vector48
#define BONE_HAS_SAVEFRAME_ROT		0x00400000	// Quaternion64
#define IN_ATTACK				(1 << 0)
#define IN_JUMP					(1 << 1)
#define IN_DUCK					(1 << 2)
#define IN_FORWARD				(1 << 3)
#define IN_BACK					(1 << 4)
#define IN_USE					(1 << 5)
#define IN_CANCEL				(1 << 6)
#define IN_LEFT					(1 << 7)
#define IN_RIGHT				(1 << 8)
#define IN_MOVELEFT				(1 << 9)
#define IN_MOVERIGHT			(1 << 10)
#define IN_ATTACK2				(1 << 11)
#define IN_RUN					(1 << 12)
#define IN_RELOAD				(1 << 13)
#define IN_ALT1					(1 << 14)
#define IN_ALT2					(1 << 15)
#define IN_SCORE				(1 << 16)
#define IN_SPEED				(1 << 17)
#define IN_WALK					(1 << 18)
#define IN_ZOOM					(1 << 19)
#define IN_WEAPON1				(1 << 20)
#define IN_WEAPON2				(1 << 21)
#define IN_BULLRUSH				(1 << 22)

#define	FL_ONGROUND				(1 << 0)	
#define FL_DUCKING				(1 << 1)	
#define	FL_WATERJUMP			(1 << 3)	
#define FL_ONTRAIN				(1 << 4) 
#define FL_INRAIN				(1 << 5)	
#define FL_FROZEN				(1 << 6) 
#define FL_ATCONTROLS			(1 << 7) 
#define	FL_CLIENT				(1 << 8)	
#define FL_FAKECLIENT			(1 << 9)	
#define	FL_INWATER				(1 << 10)

#define TEXTURE_GROUP_LIGHTMAP						xors( "Lightmaps" )
#define TEXTURE_GROUP_WORLD							xors( "World textures" )
#define TEXTURE_GROUP_MODEL							xors( "Model textures" )
#define TEXTURE_GROUP_VGUI							xors( "VGUI textures" )
#define TEXTURE_GROUP_PARTICLE						xors( "Particle textures")
#define TEXTURE_GROUP_DECAL							xors( "Decal textures")
#define TEXTURE_GROUP_SKYBOX						xors( "SkyBox textures")
#define TEXTURE_GROUP_CLIENT_EFFECTS				xors( "ClientEffect textures")
#define TEXTURE_GROUP_OTHER							xors( "Other textures")
#define TEXTURE_GROUP_PRECACHED						xors( "Precached"		)		// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						xors( "CubeMap textures")
#define TEXTURE_GROUP_RENDER_TARGET					xors( "RenderTargets")
#define TEXTURE_GROUP_UNACCOUNTED					xors( "Unaccounted textures"	)// Textures that weren't assigned a texture group.
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		    xors( "Static Vertex")
#define TEXTURE_GROUP_STATICPROPS					xors(  "StaticProp Textures")
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			xors( "Static Indices")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		xors( "Displacement Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	xors( "Lighting Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	xors( "World Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	xors( "Model Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	xors( "Other Verts")
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			xors( "Dynamic Indices")
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			xors( "Dynamic Verts")
#define TEXTURE_GROUP_DEPTH_BUFFER					xors( "DepthBuffer")
#define TEXTURE_GROUP_VIEW_MODEL					xors( "ViewModel")
#define TEXTURE_GROUP_PIXEL_SHADERS					xors( "Pixel Shaders")
#define TEXTURE_GROUP_VERTEX_SHADERS				xors( "Vertex Shaders")
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			xors( "RenderTarget Surfaces" )
#define TEXTURE_GROUP_MORPH_TARGETS					xors( "Morph Targets" )

#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	CONTENTS_OPAQUE

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200	

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
// CONTENTS_OPAQUE + SURF_NODRAW count as CONTENTS_OPAQUE (shadow-casting toolsblocklight textures)
#define CONTENTS_BLOCKLIGHT		0x400

#define CONTENTS_TEAM1			0x800	// per team contents used to differentiate collisions 
#define CONTENTS_TEAM2			0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace

#define	MASK_SHOT	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)

// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture

#define	SURF_HINT		0x0100	// make a primary bsp splitter

#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOPAINT	SURF_NODECALS	// the surface can not have paint placed on it
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox

#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks fluid movement
#define	MASK_NPCFLUID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// bullets see these as solid, except monsters (world+brush only)
#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC_FLUID	(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

#define CHAR_TEX_ANTLION			'A'
#define CHAR_TEX_BLOODYFLESH		'B'
#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_GRASS			'J'	// L4D addition
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC			'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE			'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_ASPHALT			'Q'	// L4D addition
#define CHAR_TEX_BRICK			'R'	// L4D addition
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'	// L4D addition
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//// CHAR_TEX_UNUSED		'X' ///< do not use - "fake" materials use this (ladders, wading, clips, etc)
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.
#define CHAR_TEX_CLAY			1 	// L4D addition
#define CHAR_TEX_PLASTER			2	// L4D addition
#define CHAR_TEX_ROCK			3	// L4D addition
#define CHAR_TEX_RUBBER			4	// L4D addition
#define CHAR_TEX_SHEETROCK		5	// L4D addition
#define CHAR_TEX_CLOTH			6	// L4D addition
#define CHAR_TEX_CARPET			7	// L4D addition
#define CHAR_TEX_PAPER			8	// L4D addition
#define CHAR_TEX_UPHOLSTERY		9	// L4D addition
#define CHAR_TEX_PUDDLE			10	// L4D addition
#define CHAR_TEX_MUD				11	// L4D addition
#define CHAR_TEX_SANDBARREL		12	// CSGO addition (no penetration)
struct RecvProp;

class vec2
{
public:
	vec2( )
	{
		x = y = 0.0f;
	}

	template <typename T>
	vec2( T X, T Y )
	{
		x = static_cast< float >( X ); y = static_cast< float >( Y );
	}

	vec2( float* v )
	{
		x = v[0]; y = v[1];
	}

	vec2( const float* v )
	{
		x = v[0]; y = v[1];
	}

	vec2( const vec2& v )
	{
		x = v.x; y = v.y;
	}

	vec2& operator=( const vec2& v )
	{
		x = v.x; y = v.y; return *this;
	}

	float& operator[]( int i )
	{
		return ( ( float* ) this )[i];
	}

	float operator[]( int i ) const
	{
		return ( ( float* ) this )[i];
	}

	vec2& operator+=( const vec2& v )
	{
		x += v.x; y += v.y; return *this;
	}

	vec2& operator-=( const vec2& v )
	{
		x -= v.x; y -= v.y; return *this;
	}

	vec2& operator*=( const vec2& v )
	{
		x *= v.x; y *= v.y; return *this;
	}

	vec2& operator/=( const vec2& v )
	{
		x /= v.x; y /= v.y; return *this;
	}

	vec2& operator+=( float v )
	{
		x += v; y += v; return *this;
	}

	vec2& operator-=( float v )
	{
		x -= v; y -= v; return *this;
	}

	vec2& operator*=( float v )
	{
		x *= v; y *= v; return *this;
	}

	vec2& operator/=( float v )
	{
		x /= v; y /= v; return *this;
	}

	vec2 operator+( const vec2& v ) const
	{
		return vec2( x + v.x, y + v.y );
	}

	vec2 operator-( const vec2& v ) const
	{
		return vec2( x - v.x, y - v.y );
	}

	vec2 operator*( const vec2& v ) const
	{
		return vec2( x * v.x, y * v.y );
	}

	vec2 operator/( const vec2& v ) const
	{
		return vec2( x / v.x, y / v.y );
	}

	vec2 operator+( float v ) const
	{
		return vec2( x + v, y + v );
	}

	vec2 operator-( float v ) const
	{
		return vec2( x - v, y - v );
	}

	vec2 operator*( float v ) const
	{
		return vec2( x * v, y * v );
	}

	vec2 operator/( float v ) const
	{
		return vec2( x / v, y / v );
	}

	void set( float X = 0.0f, float Y = 0.0f )
	{
		x = X; y = Y;
	}

	float length( void ) const
	{
		return sqrtf( x * x + y * y );
	}

	float lengthsqr( void ) const
	{
		return ( x * x + y * y );
	}

	float dist_to( const vec2& v ) const
	{
		return ( *this - v ).length( );
	}

	float dist_to_sqr( const vec2& v ) const
	{
		return ( *this - v ).lengthsqr( );
	}

	float dot( const vec2& v ) const
	{
		return ( x * v.x + y * v.y );
	}

	bool is_zero( void ) const
	{
		return ( x > -FLT_EPSILON && x < FLT_EPSILON &&
				 y > -FLT_EPSILON && y < FLT_EPSILON );
	}

	operator bool( ) const noexcept
	{
		return !is_zero( );
	}

public:
	float x, y;
};

class vec3
{
public:
	vec3( )
	{
		x = y = z = 0.0f;
	}

	vec3( float X, float Y, float Z )
	{
		x = X;
		y = Y;
		z = Z;
	}

	vec3( float* v )
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	vec3( const float* v )
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	vec3( const vec3& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	vec3( const vec2& v )
	{
		x = v.x;
		y = v.y;
		z = 0.0f;
	}

	vec3& operator=( const vec3& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	vec3& operator=( const vec2& v )
	{
		x = v.x;
		y = v.y;
		z = 0.0f;
		return *this;
	}

	float& operator[]( int i )
	{
		return ( ( float* ) this )[i];
	}

	float operator[]( int i ) const
	{
		return ( ( float* ) this )[i];
	}

	vec3& operator+=( const vec3& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator-=( const vec3& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3& operator*=( const vec3& v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3& operator/=( const vec3& v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	vec3& operator+=( float v )
	{
		x += v;
		y += v;
		z += v;
		return *this;
	}

	vec3& operator-=( float v )
	{
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}

	vec3& operator*=( float v )
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3& operator/=( float v )
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	vec3 operator+( const vec3& v ) const
	{
		return vec3( x + v.x, y + v.y, z + v.z );
	}

	vec3 operator-( const vec3& v ) const
	{
		return vec3( x - v.x, y - v.y, z - v.z );
	}

	vec3 operator*( const vec3& v ) const
	{
		return vec3( x * v.x, y * v.y, z * v.z );
	}

	vec3 operator/( const vec3& v ) const
	{
		return vec3( x / v.x, y / v.y, z / v.z );
	}

	vec3 operator+( float v ) const
	{
		return vec3( x + v, y + v, z + v );
	}

	vec3 operator-( float v ) const
	{
		return vec3( x - v, y - v, z - v );
	}

	vec3 operator*( float v ) const
	{
		return vec3( x * v, y * v, z * v );
	}

	vec3 operator/( float v ) const
	{
		return vec3( x / v, y / v, z / v );
	}

	bool operator== ( const vec3& other ) const
	{
		return x == other.x && y == other.y && other.z == other.z;
	}

	bool operator!= ( const vec3& other ) const
	{
		return !( *this == other );
	}

	bool operator!( ) const
	{
		return this->is_zero( );
	}

	void set( float X = 0.0f, float Y = 0.0f, float Z = 0.0f )
	{
		x = X;
		y = Y;
		z = Z;
	}

	vec3 center( ) const
	{
		return *this * 0.5f;
	}

	float length( ) const
	{
		return std::sqrt( x * x + y * y + z * z );
	}

	float length_sqr( ) const
	{
		return ( x * x + y * y + z * z );
	}

	float length2d( ) const
	{
		return std::sqrt( x * x + y * y );
	}

	float length2d_sqr( ) const
	{
		return ( x * x + y * y );
	}

	float dist( const vec3& v ) const
	{
		return ( *this - v ).length( );
	}

	float dist_sqr( const vec3& v ) const
	{
		return ( *this - v ).length_sqr( );
	}

	float dot( const vec3& v ) const
	{
		return ( x * v.x + y * v.y + z * v.z );
	}	
	
	float dot_2d( const vec3& v ) const
	{
		return ( x * v.x + y * v.y );
	}

	vec3 cross_product( const vec3& v ) const
	{
		return vec3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	}

	bool is_zero( ) const
	{
		return ( x > -0.01f && x < 0.01f &&
				 y > -0.01f && y < 0.01f &&
				 z > -0.01f && z < 0.01f );
	}

	bool is_nan( ) const
	{ 
		return isnan( x ) || isnan( y ) || isnan( z );
	}

	void normalize( )
	{
		auto& vec = *this;

		// Proper large epenis normalization, none of that gay while loop shit
		for ( auto i = 0; i <= 2; i++ )
			if ( vec[i] > 180.f || vec[i] < -180.f )
				vec[i] = ( vec[i] < 0.f ) ? vec[i] + 360.f * round( abs( vec[i] / 360.f ) ) : vec[i] - 360.f * round( abs( vec[i] / 360.f ) );
	}

	vec3 normalized( ) const
	{
		auto vec = *this;
		vec.normalize( );
		return vec;
	}

	void clamp( )
	{
		x = std::clamp<float>( x, -89.f, 89.f );
		y = std::clamp<float>( y, -180.0f, 180.0f );
		z = 0.0f;
	}

	vec3 clamped( )
	{
		return vec3(
			std::clamp<float>( x, -89.f, 89.f ),
			std::clamp<float>( y, -180.0f, 180.0f ),
			0.0f
		);
	}

	void sanitize( )
	{
		//safety
		if ( !isfinite( x ) || isnan( x ) || isinf( x ) )
			x = 0.0f;

		if ( !isfinite( y ) || isnan( y ) || isinf( y ) )
			y = 0.0f;

		//normalize
		normalize( );

		//clamp
		clamp( );
	}

	void normalize_in_place( )
	{
		vec3& v = *this;

		float iradius = 1.f / ( this->length( ) + FLT_EPSILON );

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
	}

	bool is_zero( )
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

public:
	float x, y, z;
};

struct VMatrix
{
	float m[4][4];

	inline float* operator[]( int i )
	{
		return m[i];
	}

	inline const float* operator[]( int i ) const
	{
		return m[i];
	}
};


class CRecvProxyData
{
public:
	const RecvProp* m_prop;
	unsigned char pad[0x18];		
	int m_element;
	int m_spawn_id;
};


typedef void( *RecvVarProxyFn )( const CRecvProxyData *pData, void *pStruct, void *pOut );

struct RecvTable
{
	RecvProp		*m_pProps;
	int				m_nProps;
	void			*m_pDecoder;
	char			*m_pNetTableName;
	bool			m_bInitialized;
	bool			m_bInMainList;
};

struct RecvProp
{
	char					*m_pVarName;
	int						m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;
	bool					m_bInsideArray;
	const void				*m_pExtraData;
	RecvProp				*m_pArrayProp;
	void*					m_ArrayLengthProxy;
	void*					m_ProxyFn;
	void*					m_DataTableProxyFn;
	RecvTable				*m_pDataTable;
	int						m_Offset;
	int						m_ElementStride;
	int						m_nElements;
	const char				*m_pParentArrayPropName;
};

class c_client
{
public:
	void* m_create_function;
	void* m_create_event;
	char* m_name;
	RecvTable* m_table;
	c_client* m_next;
	int m_class_id;
};

class c_base_client
{
public:
	c_client* get_all_classes( )
	{
		return memory::get_vfunc< c_client*( __thiscall* )( void* ) >( this, 8 )( this );
	}
};

using matrix3x3_t = float[3][3];

struct matrix3x4_t
{
	matrix3x4_t( ) { }
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const vec3& xAxis, const vec3& yAxis, const vec3& zAxis, const vec3&vecOrigin )
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const vec3& xAxis, const vec3& yAxis, const vec3& zAxis, const vec3 &vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void SetOrigin( vec3 const & p )
	{
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate( void )
	{
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				m_flMatVal[i][j] = NAN;
			}
		}
	}

	float *operator[]( int i ) { Assert( ( i >= 0 ) && ( i < 3 ) ); return m_flMatVal[i]; }
	const float *operator[]( int i ) const { Assert( ( i >= 0 ) && ( i < 3 ) ); return m_flMatVal[i]; }
	float *Base( ) { return &m_flMatVal[0][0]; }
	const float *Base( ) const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};


class ICollideable
{
public:
	virtual void pad( );
	virtual const vec3& OBBMins( ) const;
	virtual const vec3& OBBMaxs( ) const;

	const vec3& collision_origin( )
	{
		return *reinterpret_cast< vec3* >( reinterpret_cast< uintptr_t >( this ) + 0x90 );
	}

	const vec3& collision_angles( )
	{
		return *reinterpret_cast< vec3* >( reinterpret_cast< uintptr_t >( this ) + 0x9C );
	}

	vec3                      get_prescaled_mins( );
	vec3                      get_prescaled_maxs( );
};

class mstudiobbox_t
{
public:
	int32_t m_Bone = 0;   // 0x00
	int32_t m_Group = 0;   // 0x04
	vec3 m_Mins;               // 0x08
	vec3 m_Maxs;               // 0x14
	int32_t m_HitboxNameId = 0;   // 0x20
	vec3 m_Rotation;
	float   m_Radius = 0.f; // 0x30
private:
	uint8_t m_0x34[0x10];       // 0x34
public:
	mstudiobbox_t( ) = default;
	inline size_t get_bone_id( ) const;

	const char* name( )
	{
		if ( m_HitboxNameId == 0 )
			return "";

		return reinterpret_cast< const char* >( reinterpret_cast< uintptr_t >( this ) + m_HitboxNameId );
	}
};

class fclr_t
{
	float R, G, B, A;
public:
	fclr_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

	fclr_t( float r, float g, float b, float a ) : R( r ), G( g ), B( b ), A( a ) { }

	fclr_t( float r, float g, float b ) : R( r ), G( g ), B( b ), A( 255 ) { }

	float& r( ) { return R; }
	float& g( ) { return G; }
	float& b( ) { return B; }
	float& a( ) { return A; }

	fclr_t& operator =( fclr_t& c )
	{
		R = c.r( );
		G = c.g( );
		B = c.b( );
		A = c.a( );
		return *this;
	}

	fclr_t operator+( const fclr_t& v ) const
	{
		return fclr_t( R + v.R, G + v.G, B + v.B, A + v.A );
	}

	explicit operator bool( ) const noexcept
	{
		return ( R > 0 || G > 0 || B > 0 || A > 0 );
	}

	bool operator==( fclr_t& c ) const
	{
		return ( R == c.r( ) && G == c.g( ) && B == c.b( ) );
	}
};

typedef struct InputContextHandle_t__ *InputContextHandle_t;
struct client_textmessage_t;
struct model_t;
class SurfInfo;
class CSentence;
class CAudioSource;
class AudioState_t;
class ISpatialQuery;
class CPhysCollide;
class IAchievementMgr;
class INetChannelInfo;
class ISPSharedMemory;
class CGamestatsData;
class KeyValues;
class CSteamAPIContext;
struct Frustum_t;
class pfnDemoCustomDataCallback;

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = ( 1 << 0 ),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = ( 1 << 1 ),
	MATERIAL_VAR_NO_DRAW = ( 1 << 2 ),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = ( 1 << 3 ),
	MATERIAL_VAR_VERTEXCOLOR = ( 1 << 4 ),
	MATERIAL_VAR_VERTEXALPHA = ( 1 << 5 ),
	MATERIAL_VAR_SELFILLUM = ( 1 << 6 ),
	MATERIAL_VAR_ADDITIVE = ( 1 << 7 ),
	MATERIAL_VAR_ALPHATEST = ( 1 << 8 ),
	MATERIAL_VAR_MULTIPASS = ( 1 << 9 ),
	MATERIAL_VAR_ZNEARER = ( 1 << 10 ),
	MATERIAL_VAR_MODEL = ( 1 << 11 ),
	MATERIAL_VAR_FLAT = ( 1 << 12 ),
	MATERIAL_VAR_NOCULL = ( 1 << 13 ),
	MATERIAL_VAR_NOFOG = ( 1 << 14 ),
	MATERIAL_VAR_IGNOREZ = ( 1 << 15 ),
	MATERIAL_VAR_DECAL = ( 1 << 16 ),
	MATERIAL_VAR_ENVMAPSPHERE = ( 1 << 17 ),
	MATERIAL_VAR_NOALPHAMOD = ( 1 << 18 ),
	MATERIAL_VAR_ENVMAPCAMERASPACE = ( 1 << 19 ),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = ( 1 << 20 ),
	MATERIAL_VAR_TRANSLUCENT = ( 1 << 21 ),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = ( 1 << 22 ),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = ( 1 << 23 ),
	MATERIAL_VAR_OPAQUETEXTURE = ( 1 << 24 ),
	MATERIAL_VAR_ENVMAPMODE = ( 1 << 25 ),
	MATERIAL_VAR_SUPPRESS_DECALS = ( 1 << 26 ),
	MATERIAL_VAR_HALFLAMBERT = ( 1 << 27 ),
	MATERIAL_VAR_WIREFRAME = ( 1 << 28 ),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = ( 1 << 29 ),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};
enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
								IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
								IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

								NUM_IMAGE_FORMATS
};


enum MaterialVarFlags2_t
{
	// NOTE: These are for $flags2!!!!!
	//	UNUSED											= (1 << 0),

	MATERIAL_VAR2_LIGHTING_UNLIT = 0,
	MATERIAL_VAR2_LIGHTING_VERTEX_LIT = ( 1 << 1 ),
	MATERIAL_VAR2_LIGHTING_LIGHTMAP = ( 1 << 2 ),
	MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP = ( 1 << 3 ),
	MATERIAL_VAR2_LIGHTING_MASK =
	( MATERIAL_VAR2_LIGHTING_VERTEX_LIT |
	  MATERIAL_VAR2_LIGHTING_LIGHTMAP |
	  MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP ),

	// FIXME: Should this be a part of the above lighting enums?
	MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL = ( 1 << 4 ),
	MATERIAL_VAR2_USES_ENV_CUBEMAP = ( 1 << 5 ),
	MATERIAL_VAR2_NEEDS_TANGENT_SPACES = ( 1 << 6 ),
	MATERIAL_VAR2_NEEDS_SOFTWARE_LIGHTING = ( 1 << 7 ),
	// GR - HDR path puts lightmap alpha in separate texture...
	MATERIAL_VAR2_BLEND_WITH_LIGHTMAP_ALPHA = ( 1 << 8 ),
	MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS = ( 1 << 9 ),
	MATERIAL_VAR2_USE_FLASHLIGHT = ( 1 << 10 ),
	MATERIAL_VAR2_USE_FIXED_FUNCTION_BAKED_LIGHTING = ( 1 << 11 ),
	MATERIAL_VAR2_NEEDS_FIXED_FUNCTION_FLASHLIGHT = ( 1 << 12 ),
	MATERIAL_VAR2_USE_EDITOR = ( 1 << 13 ),
	MATERIAL_VAR2_NEEDS_POWER_OF_TWO_FRAME_BUFFER_TEXTURE = ( 1 << 14 ),
	MATERIAL_VAR2_NEEDS_FULL_FRAME_BUFFER_TEXTURE = ( 1 << 15 ),
	MATERIAL_VAR2_IS_SPRITECARD = ( 1 << 16 ),
	MATERIAL_VAR2_USES_VERTEXID = ( 1 << 17 ),
	MATERIAL_VAR2_SUPPORTS_HW_SKINNING = ( 1 << 18 ),
	MATERIAL_VAR2_SUPPORTS_FLASHLIGHT = ( 1 << 19 ),
	MATERIAL_VAR2_USE_GBUFFER0 = ( 1 << 20 ),
	MATERIAL_VAR2_USE_GBUFFER1 = ( 1 << 21 ),
	MATERIAL_VAR2_SELFILLUMMASK = ( 1 << 22 ),
	MATERIAL_VAR2_SUPPORTS_TESSELLATION = ( 1 << 23 )
};


enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

typedef unsigned int MorphFormat_t;
class ITexture;
class IMaterialProxy;
class IMaterial;

typedef uint64_t VertexFormat_t;
enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

class IMaterialVar
{
public:
	virtual int		        vpad( void ) = 0;

	virtual ITexture		*GetTextureValue( void ) = 0;

	virtual char const *	GetName( void ) const = 0;
	virtual uint16_t    	GetNameAsSymbol( ) const = 0;

	virtual void			SetFloatValue( float val ) = 0;

	virtual void			SetIntValue( int val ) = 0;

	virtual void			SetStringValue( char const *val ) = 0;
	virtual char const *	GetStringValue( void ) const = 0;

	// Use FourCC values to pass app-defined data structures between
	// the proxy and the shader. The shader should ignore the data if
	// its FourCC type not correct.	
	virtual void			SetFourCCValue( unsigned long type, void *pData ) = 0;
	virtual void			GetFourCCValue( unsigned long *type, void **ppData ) = 0;

	// Vec (dim 2-4)
	virtual void			SetVecValue( float const* val, int numcomps ) = 0;
	virtual void			SetVecValue( float x, float y ) = 0;
	virtual void			SetVecValue( float x, float y, float z ) = 0;
	virtual void			SetVecValue( float x, float y, float z, float w ) = 0;
	virtual void			GetLinearVecValue( float *val, int numcomps ) const = 0;

	// revisit: is this a good interface for textures?
	virtual void			SetTextureValue( ITexture * ) = 0;

	virtual IMaterial *		GetMaterialValue( void ) = 0;
	virtual void			SetMaterialValue( IMaterial * ) = 0;

	virtual bool			IsDefined( ) const = 0;
	virtual void			SetUndefined( ) = 0;

	// Matrix
	virtual void			SetMatrixValue( VMatrix const& matrix ) = 0;
	virtual const VMatrix  &GetMatrixValue( ) = 0;
	virtual bool			MatrixIsIdentity( ) const = 0;

	// Copy....
	virtual void			CopyFrom( IMaterialVar *pMaterialVar ) = 0;

	virtual void			SetValueAutodetectType( char const *val ) = 0;

	virtual IMaterial *		GetOwningMaterial( ) = 0;

	//set just 1 component
	virtual void			SetVecComponentValue( float fVal, int nComponent ) = 0;

	virtual int				GetIntValueInternal( void ) const = 0;
	virtual float			GetFloatValueInternal( void ) const = 0;
	virtual float const*	GetVecValueInternal( ) const = 0;
	virtual void			GetVecValueInternal( float *val, int numcomps ) const = 0;
	virtual int				VectorSizeInternal( ) const = 0;
};

class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName( ) const = 0;
	virtual const char *	GetTextureGroupName( ) const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties( int *width, int *height,
															ImageFormat *imageFormat, bool* isTranslucent ) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage( unsigned char *data,
												  int width, int height,
												  ImageFormat imageFormat ) const = 0;
	// 
	virtual int				GetMappingWidth( ) = 0;
	virtual int				GetMappingHeight( ) = 0;

	virtual int				GetNumAnimationFrames( ) = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage( void ) = 0;
	virtual	void			GetMaterialOffset( float *pOffset ) = 0;
	virtual void			GetMaterialScale( float *pScale ) = 0;
	virtual IMaterial		*GetMaterialPage( void ) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar*	FindVar( const char *varName, bool *found, bool complain = true ) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount( void ) = 0;
	virtual void			DecrementReferenceCount( void ) = 0;

	inline void AddRef( ) { IncrementReferenceCount( ); }
	inline void Release( ) { DecrementReferenceCount( ); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID( void ) const = 0;

	virtual void			GetLowResColorSample( float s, float t, float *color ) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots( ) = 0;

	// Are we translucent?
	virtual bool			IsTranslucent( ) = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested( ) = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit( ) = 0;

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat( ) const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy( void ) const = 0;

	virtual bool			UsesEnvCubemap( void ) = 0;

	virtual bool			NeedsTangentSpace( void ) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture( bool bCheckSpecificToThisFrame = true ) = 0;
	virtual bool			NeedsFullFrameBufferTexture( bool bCheckSpecificToThisFrame = true ) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning( void ) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate( float alpha ) = 0;
	virtual void			ColorModulate( float r, float g, float b ) = 0;

	// Material Var flags...
	virtual void			set_flag( MaterialVarFlags_t flag, bool on ) = 0;
	virtual bool			get_flag( MaterialVarFlags_t flag ) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity( vec3& reflect ) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag( MaterialPropertyTypes_t type ) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided( ) = 0;

	// Sets the shader associated with the material
	virtual void			SetShader( const char *pShaderName ) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses( void ) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes( void ) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh( ) = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha( void ) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting( void ) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount( ) const = 0;
	virtual IMaterialVar	**GetShaderParams( void ) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial( ) const = 0;

	virtual void			SetUseFixedFunctionBakedLighting( bool bEnable ) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation( ) = 0;
	virtual void			GetColorModulation( float *r, float *g, float *b ) = 0;

	// Gets the morph format
	virtual MorphFormat_t	GetMorphFormat( ) const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar*	FindVarFast( char const *pVarName, unsigned int *pToken ) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams( KeyValues *pKeyValues ) = 0;
	virtual const char *	GetShaderName( ) const = 0;

	virtual void			DeleteIfUnreferenced( ) = 0;

	virtual bool			IsSpriteCard( ) = 0;

	virtual void			CallBindProxy( void *proxyData ) = 0;

	virtual IMaterial		*CheckProxyReplacement( void *proxyData ) = 0;

	virtual void			RefreshPreservingMaterialVars( ) = 0;

	virtual bool			WasReloadedFromWhitelist( ) = 0;

	virtual bool			IsPrecached( ) const = 0;
};

class IMaterialSystem
{
public:
	inline IMaterial* find( const char* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = 0 )
	{
		typedef IMaterial*( __thiscall* OriginalFn )( void*, const char*, const char*, bool, const char* );
		return memory::get_vfunc<OriginalFn>( this, 84 )( this, pMaterialName, pTextureGroupName, complain, pComplainPrefix );
	}
	inline uint16_t first_material( )
	{
		typedef uint16_t( __thiscall* oFirstMaterial )( void* );
		return memory::get_vfunc<oFirstMaterial>( this, 86 )( this );
	}

	inline uint16_t next_material( uint16_t h )
	{
		typedef uint16_t( __thiscall* oNextMaterial )( void*, uint16_t );
		return memory::get_vfunc<oNextMaterial>( this, 87 )( this, h );
	}

	inline uint16_t invalid_material( )
	{
		typedef uint16_t( __thiscall* oInvalidMaterial )( void* );
		return memory::get_vfunc<oInvalidMaterial>( this, 88 )( this );
	}

	IMaterial* get( uint16_t h )
	{
		typedef IMaterial* ( __thiscall* oGetMaterial )( void*, uint16_t );
		return memory::get_vfunc<oGetMaterial>( this, 89 )( this, h );
	}
};

struct player_info_t
{
	unsigned long long      unknown;
	unsigned long long      xuid;
	char                    name[128];
	int                     userID;
	char                    guid[33];
	unsigned int            friendsID;
	char                    friendsName[128];
	bool                    fakeplayer;
	bool                    ishltv;
	unsigned long           customFiles[4];
	unsigned char           filesDownloaded;
};

class CHudChat
{
public:
	void ChatPrintf( int player_index, int filter, const char* fmt, ...)
	{
		using fn = void( __cdecl* )( void*, int, int, const char*, ... );
		return memory::get_vfunc< fn >( this, 26 )( this, player_index, filter, fmt );
	}
};

class CEngineClient
{
public:
	void GetScreenSize( int& width, int& height )
	{
		using fn = void( __thiscall* )( void*, int&, int& );
		return memory::get_vfunc< fn >( this, 5 )( this, width, height );
	}

	void ClientCmd( const char* szCommandString )
	{
		std::string command( szCommandString );
		std::replace( command.begin( ), command.end( ), ';', ':' );
		std::replace( command.begin( ), command.end( ), '"', '*' );
		using fn = void( __thiscall* )( void*, const char* );
		return memory::get_vfunc< fn >( this, 7 )( this, command.c_str( ) );
	}

	void ClientCmd_Unrestricted( const char* szCommandString )
	{
		std::string command( szCommandString );
		std::replace( command.begin( ), command.end( ), ';', ':' );
		std::replace( command.begin( ), command.end( ), '"', '*' );
		using fn = void( __thiscall* )( void*, const char*, const char* );
		return memory::get_vfunc< fn >( this, 114 )( this, command.c_str( ), nullptr );
	}

	bool GetPlayerInfo( int index, player_info_t* pInfo )
	{
		using fn = bool( __thiscall* )( void*, int, player_info_t* );
		return memory::get_vfunc< fn >( this, 8 )( this, index, pInfo );
	}

	int GetPlayerForUserID( int UserID )
	{
		using fn = int( __thiscall* )( void*, int );
		return memory::get_vfunc< fn >( this, 9 )( this, UserID );
	}

	int GetLocalPlayer( )
	{
		using fn = int( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 12 )( this );
	}

	float Time( )
	{
		using fn = float( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 14 )( this );
	}

	void GetViewAngles( vec3& ang )
	{
		using fn = void( __thiscall* )( void*, vec3& );
		return memory::get_vfunc< fn >( this, 18 )( this, ang );
	}

	vec3 GetViewAngles( )
	{
		vec3 ang;

		using fn = void( __thiscall* )( void*, vec3& );
		memory::get_vfunc< fn >( this, 18 )( this, ang );

		return ang;
	}

	void SetViewAngles( const vec3& ang )
	{
		using fn = void( __thiscall* )( void*, const vec3& );
		return memory::get_vfunc< fn >( this, 19 )( this, ang );
	}

	int GetMaxClients( )
	{
		using fn = int( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 20 )( this );
	}

	bool IsInGame( )
	{
		using fn = bool( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 26 )( this );
	}

	bool IsConnected( )
	{
		using fn = bool( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 27 )( this );
	}

	const VMatrix& WorldToScreenMatrix( )
	{
		using fn = const VMatrix& ( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 37 )( this );
	}

	const char* GetLevelName( )
	{
		using fn = const char* ( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 52 )( this );
	}

	INetChannelInfo* GetNetChannelInfo( )
	{
		using fn = INetChannelInfo * ( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 78 )( this );
	}
};
class clr_t
{
	uint8_t R, G, B, A;
public:
	clr_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

	clr_t( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) : R( r ), G( g ), B( b ), A( a ) { }

	clr_t( uint8_t r, uint8_t g, uint8_t b ) : R( r ), G( g ), B( b ), A( 255 ) { }

	uint8_t& r( ) { return R; }
	uint8_t& g( ) { return G; }
	uint8_t& b( ) { return B; }
	uint8_t& a( ) { return A; }

	clr_t& operator =( clr_t& c )
	{
		R = c.r( );
		G = c.g( );
		B = c.b( );
		A = c.a( );
		return *this;
	}

	clr_t operator+( const clr_t& v ) const
	{
		return clr_t( R + v.R, G + v.G, B + v.B, A + v.A );
	}

	clr_t operator*( float f )
	{
		return clr_t( uint8_t( R * f ), uint8_t( G * f ), uint8_t( B * f ), A );
	}

	explicit operator bool( ) const noexcept
	{
		return ( R > 0 || G > 0 || B > 0 || A > 0 );
	}

	float brightness( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.v;
	}

	float saturation( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.s;
	}

	static clr_t from_hsb( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - ( int ) h;
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if ( h < 1 )
		{
			return clr_t(
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( t * 255 ),
				( unsigned char ) ( p * 255 )
			);
		}
		else if ( h < 2 )
		{
			return clr_t(
				( unsigned char ) ( q * 255 ),
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( p * 255 )
			);
		}
		else if ( h < 3 )
		{
			return clr_t(
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( t * 255 )
			);
		}
		else if ( h < 4 )
		{
			return clr_t(
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( q * 255 ),
				( unsigned char ) ( brightness * 255 )
			);
		}
		else if ( h < 5 )
		{
			return clr_t(
				( unsigned char ) ( t * 255 ),
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( brightness * 255 )
			);
		}
		else
		{
			return clr_t(
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( q * 255 )
			);
		}
	}

	static clr_t blend( clr_t first, clr_t second, float t )
	{
		return clr_t(
			first.r( ) + static_cast< int >( t * ( second.r( ) - first.r( ) ) ),
			first.g( ) + static_cast< int >( t * ( second.g( ) - first.g( ) ) ),
			first.b( ) + static_cast< int >( t * ( second.b( ) - first.b( ) ) ),
			first.a( ) + static_cast< int >( t * ( second.a( ) - first.a( ) ) )
		);
	}

	float hue( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv         out;
		float       min, max, delta;

		min = static_cast< float >( R < G ? R : G );
		min = static_cast< float >( min < B ? min : B );

		max = static_cast< float >( R > G ? R : G );
		max = static_cast< float >( max > B ? max : B );

		out.v = max;
		delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = ( delta / max );
		}
		else
		{
			out.s = 0.0f;
			out.h = ( float ) NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast< float >( G - B ) / delta;
		else
			if ( G >= max )
				out.h = 2.0f + static_cast< float >( B - R ) / delta;
			else
				out.h = 4.0f + static_cast< float >( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.h;
	}

	fclr_t to_fclr( )
	{
		return fclr_t{ R / 255.f, G / 255.f, B / 255.f, A / 255.f };
	}

	operator fclr_t( )
	{
		return this->to_fclr( );
	}

	bool operator==( clr_t& c ) const
	{
		return ( R == c.r( ) && G == c.g( ) && B == c.b( ) );
	}
};
struct plane_t
{
	vec3	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[2];
};

class __declspec( align( 16 ) )VectorAligned : public vec3
{
public:
	VectorAligned( ) { }

	VectorAligned( const vec3 &vec )
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	float w;
};
enum SurfaceFlags_t
{
	DISPSURF_FLAG_SURFACE = 1 << 0,
	DISPSURF_FLAG_WALKABLE = 1 << 1,
	DISPSURF_FLAG_BUILDABLE = 1 << 2,
	DISPSURF_FLAG_SURFPROP1 = 1 << 3,
	DISPSURF_FLAG_SURFPROP2 = 1 << 4,
};
class c_base_trace
{
public:
	bool IsDispSurface( ) const { return ( m_displacement_flags & DISPSURF_FLAG_SURFACE ) != 0; }
	bool IsDispSurfaceWalkable( ) const { return ( m_displacement_flags & DISPSURF_FLAG_WALKABLE ) != 0; }
	bool IsDispSurfaceBuildable( ) const { return ( m_displacement_flags & DISPSURF_FLAG_BUILDABLE ) != 0; }
	bool IsDispSurfaceProp1( ) const { return ( m_displacement_flags & DISPSURF_FLAG_SURFPROP1 ) != 0; }
	bool IsDispSurfaceProp2( ) const { return ( m_displacement_flags & DISPSURF_FLAG_SURFPROP2 ) != 0; }

	vec3			m_src;
	vec3			m_dst;
	plane_t 		m_plane;
	float			m_fraction;
	int				m_contents;
	unsigned short	m_displacement_flags;
	bool			m_all_solid;
	bool			m_start_solid;

	c_base_trace( ) { };

private:
	c_base_trace( const c_base_trace& vOther );
};

class surface_t
{
public:
	const char*		m_name;
	short			m_srf_props;
	unsigned short	m_flags;
};

class c_base_entity;
class c_game_trace : public c_base_trace
{
public:

	bool hit_world( ) const;
	bool hit_non_world( ) const;
	int  ent_index( ) const;

	bool hit( ) const
	{
		return m_fraction < 1 || m_all_solid || m_start_solid;
	}

public:

	float					m_fraction_left_solid;
	surface_t               m_surface;
	int                     m_hitgroup;
	short					m_bone;
	unsigned short          m_world_surfce_index;
	c_base_entity*          m_ent;
	int                     m_hitbox;

	c_game_trace( ) { }
	c_game_trace( const c_game_trace& vOther );
};

class IClientEntity;
class IClientUnknown;
class IClientNetworkable;
class IClientRenderable;
class c_entity_list
{
public:
	virtual void* GetClientNetworkable( int iEnt ) = 0;
	virtual void* GetClientNetworkableFromHandle( ulong hEnt ) = 0;
	virtual void* GetClientUnknownFromHandle( ulong hEnt ) = 0;
	virtual c_base_entity* get_entity( int iEnt ) = 0;
	virtual c_base_entity* get_entity_from_base_handle( ulong hEnt ) = 0;
	virtual int NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
	virtual int highest_index( ) = 0;
	virtual void SetMaxEntities( int iMax ) = 0;
	virtual int GetMaxEntities( ) = 0;
};

using CVarDLLIdentifier_t = int;
class IConVar;
using FnChangeCallback_t = void( *)( IConVar *var, const char *pOldValue, float flOldValue );
class ConCommandBase;
class ConCommand;
class ICvarQuery;

class IConsoleDisplayFunc
{
public:
	virtual void ColorPrint( const uint8_t* clr, const char *pMessage ) = 0;
	virtual void Print( const char *pMessage ) = 0;
	virtual void DPrint( const char *pMessage ) = 0;
};

#define FCVAR_NONE				0 

// Command to ConVars and ConCommands
// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 


class cvar_t
{
public:
	void set_value( const char* value )
	{
		using fn = void( __thiscall* )( void*, const char* );
		return memory::get_vfunc< fn >( this, 14 )( this, value );
	}

	void set_value( float value )
	{
		using fn = void( __thiscall* )( void*, float );
		return memory::get_vfunc< fn >( this, 15 )( this, value );
	}

	void set_value( int value )
	{
		using fn = void( __thiscall* )( void*, int );
		return memory::get_vfunc< fn >( this, 16 )( this, value );
	}

	void set_value( clr_t value )
	{
		using fn = void( __thiscall* )( void*, clr_t );
		return memory::get_vfunc< fn >( this, 17 )( this, value );
	}

	char* get_name( )
	{
		using fn = char*( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 5 )( this );
	}

	char* get_default( )
	{
		return m_default;
	}

	const char* get_string( )
	{
		return m_string;
	}

	float get_float( )
	{
		using fn = float( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 12 )( this );
	}

	int get_int( )
	{
		using fn = int( __thiscall* )( void* );
		return memory::get_vfunc< fn >( this, 13 )( this );
	}

private:
	char pad_0x0000[0x4]; //0x0000
public:
	cvar_t * m_next; //0x0004 
	__int32 m_registered; //0x0008 
	char* m_name; //0x000C 
	char* m_help_str; //0x0010 
	__int32 m_flags; //0x0014 
private:
	char pad_0x0018[0x4]; //0x0018
public:
	cvar_t * m_parent; //0x001C 
	char* m_default; //0x0020 
	char* m_string; //0x0024 
	__int32 m_str_len; //0x0028 
	float m_flvalue; //0x002C 
	__int32 m_nvalue; //0x0030 
	__int32 m_has_min; //0x0034 
	float m_min; //0x0038 
	__int32 m_hax_max; //0x003C 
	float m_max; //0x0040 
	void* m_change_callback; //0x0044 
};//Size=0x0048

class AppSystemInfo_t;

enum InitReturnVal_t
{
	fuck
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};


class IAppSystem
{
public:
	virtual bool					Connect( void* factory ) = 0;
	virtual void					Disconnect( ) = 0;
	virtual void*					QueryInterface( const char* pInterfaceName ) = 0;
	virtual InitReturnVal_t			Init( ) = 0;
	virtual void					Shutdown( ) = 0;
	virtual const AppSystemInfo_t*	GetDependencies( ) = 0;
	virtual AppSystemTier_t			GetTier( ) = 0;
	virtual void					Reconnect( void* factory, const char* pInterfaceName ) = 0;
	virtual bool					IsSingleton( ) = 0;
};
class c_var : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier( ) = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void			UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase*	FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase*	FindCommandBase( const char *name ) const = 0;
	virtual cvar_t			*FindVar( const char *var_name ) = 0;
	virtual const cvar_t	*FindVar( const char *var_name ) const = 0;
	virtual ConCommand		*FindCommand( const char *name ) = 0;
	virtual const ConCommand *FindCommand( const char *name ) const = 0;



	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			CallGlobalChangeCallbacks( cvar_t *var, const char *pOldString, float flOldValue ) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			ConsoleColorPrintf( const clr_t& clr, const char *pFormat, ... ) const = 0;
	virtual void			ConsolePrintf( const char *pFormat, ... ) const = 0;
	virtual void			ConsoleDPrintf( const char *pFormat, ... ) const = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars( int nFlag ) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery( ICvarQuery *pQuery ) = 0;


	virtual void			SetMaxSplitScreenSlots( int nSlots ) = 0;
	virtual int				GetMaxSplitScreenSlots( ) const = 0;

	virtual void			AddSplitScreenConVars( ) = 0;
	virtual void			RemoveSplitScreenConVars( CVarDLLIdentifier_t id ) = 0;

	virtual int				GetConsoleDisplayFuncCount( ) const = 0;
	virtual void			GetConsoleText( int nDisplayFuncIndex, char *pchText, size_t bufSize ) const = 0;

	// Utilities for convars accessed by the material system thread
	virtual bool			IsMaterialThreadSetAllowed( ) const = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, const char *pValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, int nValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, float flValue ) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets( ) const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets( ) = 0;

	class ICVarIteratorInternal
	{
	public:
		virtual void		SetFirst( ) = 0;
		virtual void		Next( ) = 0;
		virtual	bool		IsAlive( ) = 0;
		virtual ConCommandBase *Get( ) = 0;
	};

	virtual ICVarIteratorInternal*	FactoryInternalIterator( ) = 0;
};

enum trace_type_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY	
};

struct virtualmeshlist_t;

struct ray_t
{
	VectorAligned m_Start;
	VectorAligned m_Delta;
	VectorAligned m_StartOffset;
	VectorAligned m_Extents;

	const matrix3x4_t* m_pWorldAxisTransform;

	bool m_IsRay;
	bool m_IsSwept;

	ray_t( )
		: m_pWorldAxisTransform( nullptr ), m_IsRay( false ), m_IsSwept( false ) { }

	ray_t( const vec3& start, const vec3& end )
	{
		Init( start, end );
	}

	void Init( const vec3& start, const vec3& end )
	{
		m_Delta = end - start;

		m_IsSwept = m_Delta.length_sqr( ) != 0;

		m_Extents = vec3( 0.f, 0.f, 0.f );
		m_pWorldAxisTransform = nullptr;
		m_IsRay = true;

		m_StartOffset = vec3( 0.f, 0.f, 0.f );
		m_Start = start;
	}

	void Init( const vec3& start, const vec3& end, const vec3& mins, const vec3& maxs )
	{
		m_Delta = end - start;

		m_pWorldAxisTransform = nullptr;
		m_IsSwept = m_Delta.length_sqr( ) != 0;

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = m_Extents.length_sqr( ) < 1e-6;

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

class ITraceListData
{
public:
	virtual ~ITraceListData( ) { }

	virtual void Reset( ) = 0;
	virtual bool IsEmpty( ) = 0;
	// CanTraceRay will return true if the current volume encloses the ray
	// NOTE: The leaflist trace will NOT check this.  Traces are intersected
	// against the culled volume exclusively.
	virtual bool CanTraceRay( const ray_t &ray ) = 0;
};

class IHandleEntity
{
public:
	virtual ~IHandleEntity( ) { }
	virtual void SetRefEHandle( ) = 0;
	virtual void GetRefEHandle( ) const = 0;
};

class IEntityEnumerator
{
public:
	// This gets called with each handle
	virtual bool EnumEntity( IHandleEntity *pHandleEntity ) = 0;
};


struct BrushSideInfo_t
{
	vec3 plane;			// The plane of the brush side
	float planec;
	unsigned short bevel;	// Bevel plane?
	unsigned short thin;	// Thin?
};

enum DebugTraceCounterBehavior_t
{
	kTRACE_COUNTER_SET = 0,
	kTRACE_COUNTER_INC,
};

class c_trace_filter
{
public:
	explicit c_trace_filter( c_base_entity* entity = nullptr, trace_type_t tracetype = TRACE_EVERYTHING )
	{
		m_filter  = entity;
		m_type    = tracetype;
	}

	virtual bool should_hit_entity( c_base_entity* entity, int mask )
	{
		return entity != m_filter;
	}

	virtual trace_type_t get_trace_type( )
	{
		return m_type;
	}

	c_base_entity*  m_filter;
	trace_type_t    m_type;
};

class CEngineTrace
{
public:
	// Returns the contents mask + entity at a particular world-space position
	virtual int		GetPointContents( const vec3 &vecAbsPosition, int contentsMask = MASK_ALL, IHandleEntity** ppEntity = NULL ) = 0;

	// Returns the contents mask of the world only @ the world-space position (static props are ignored)
	virtual int		GetPointContents_WorldOnly( const vec3 &vecAbsPosition, int contentsMask = MASK_ALL ) = 0;

	// Get the point contents, but only test the specific entity. This works
	// on static props and brush models.
	//
	// If the entity isn't a static prop or a brush model, it returns CONTENTS_EMPTY and sets
	// bFailed to true if bFailed is non-null.
	virtual int		GetPointContents_Collideable( ICollideable *pCollide, const vec3 &vecAbsPosition ) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToEntity( const ray_t &ray, unsigned int fMask, c_base_entity *pEnt, c_game_trace *pTrace ) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToCollideable( const ray_t &ray, unsigned int fMask, ICollideable *pCollide, c_game_trace *pTrace ) = 0;

	// A version that simply accepts a ray (can work as a traceline or tracehull)
	virtual void	TraceRay( const ray_t &ray, unsigned int fMask, c_trace_filter *pTraceFilter, c_game_trace *pTrace ) = 0;

	// A version that sets up the leaf and entity lists and allows you to pass those in for collision.
	virtual void	SetupLeafAndEntityListRay( const ray_t &ray, ITraceListData *pTraceData ) = 0;
	virtual void    SetupLeafAndEntityListBox( const vec3 &vecBoxMin, const vec3 &vecBoxMax, ITraceListData *pTraceData ) = 0;
	virtual void	TraceRayAgainstLeafAndEntityList( const ray_t &ray, ITraceListData *pTraceData, unsigned int fMask, c_trace_filter *pTraceFilter, c_game_trace *pTrace ) = 0;

	// A version that sweeps a collideable through the world
	// abs start + abs end represents the collision origins you want to sweep the collideable through
	// vecAngles represents the collision angles of the collideable during the sweep
	virtual void	SweepCollideable( ICollideable *pCollide, const vec3 &vecAbsStart, const vec3 &vecAbsEnd,
									  const vec3 &vecAngles, unsigned int fMask, c_trace_filter *pTraceFilter, c_game_trace *pTrace ) = 0;

	// Enumerates over all entities along a ray
	// If triggers == true, it enumerates all triggers along a ray
	virtual void	EnumerateEntities( const ray_t &ray, bool triggers, IEntityEnumerator *pEnumerator ) = 0;

	// Same thing, but enumerate entitys within a box
	virtual void	EnumerateEntities( const vec3 &vecAbsMins, const vec3 &vecAbsMaxs, IEntityEnumerator *pEnumerator ) = 0;

	// Convert a handle entity to a collideable.  Useful inside enumer
	virtual ICollideable *GetCollideable( IHandleEntity *pEntity ) = 0;

	// HACKHACK: Temp for performance measurments
	virtual int GetStatByIndex( int index, bool bClear ) = 0;

	//finds brushes in an AABB, prone to some false positives
	virtual void lolignorethisaswellrifk( ) = 0;

	//Creates a CPhysCollide out of all displacements wholly or partially contained in the specified AABB
	virtual void GetCollidableFromDisplacementsInAABB( ) = 0;

	// gets the number of displacements in the world
	virtual int GetNumDisplacements( ) = 0;

	// gets a specific diplacement mesh
	virtual void GetDisplacementMesh( int nIndex, virtualmeshlist_t *pMeshTriList ) = 0;

	//retrieve brush planes and contents, returns true if data is being returned in the output pointers, false if the brush doesn't exist
	virtual bool lolignorethis( ) = 0;

	virtual bool PointOutsideWorld( const vec3 &ptTest ) = 0; //Tests a point to see if it's outside any playable area

															  // Walks bsp to find the leaf containing the specified point
	virtual int GetLeafContainingPoint( const vec3 &ptTest ) = 0;

	virtual ITraceListData *AllocTraceListData( ) = 0;
	virtual void FreeTraceListData( ITraceListData * ) = 0;

	virtual int GetSetDebugTraceCounter( int value, DebugTraceCounterBehavior_t behavior ) = 0;
};
class c_client_mode
{

};

class c_direct_device_vtable 
{

};

class c_global_vars
{
public:
	float			realtime;
	int				framecount;
	float			absolute_frametime;
	float           absolute_framestart_times;
	float			curtime;
	float			frametime;
	int				max_clients;
	int				tickcount;
	float			interval_per_tick;
	float			interpolation_amount;
	int				simulation_ticks_this_frame;
	int				network_protocol;
	void *m_save_data;
	bool				client;
	bool				remove_client;
	int					networking_base;
	int					randomize_window;
public:
	__forceinline int TIME_TO_TICKS( float dt ) const { return static_cast<int>( 0.5f + dt / interval_per_tick ); }
};

class VPanel
{
public:
	const char* name( int panel )
	{
		return memory::get_vfunc< const char*( __thiscall* ) ( void*, int )>( this, 36 )( this, panel );
	}
};

class CEngineVGui
{
public:
	virtual					~CEngineVGui( void ) { }

	virtual VPanel	GetPanel( int type ) = 0;

	virtual bool			IsGameUIVisible( ) = 0;
};

#define FMTFUNCTION( a, b )

class OverlayText_t;

class CDebugOverlay
{
public:
	virtual void AddEntityTextOverlay( int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ... ) FMTFUNCTION( 9, 10 ) = 0;
	virtual void AddBoxOverlay( const vec3& origin, const vec3& mins, const vec3& max, vec3 const& orientation, int r, int g, int b, int a, float duration = 0 ) = 0;
	virtual void AddSphereOverlay( const vec3& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration = 0 ) = 0;
	virtual void AddTriangleOverlay( const vec3& p1, const vec3& p2, const vec3& p3, int r, int g, int b, int a, bool noDepthTest, float duration = 0 ) = 0;

	// New virtual added on the 26 MAY 15 update
	virtual void AddBoxOverlayNew( const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4, int r, int g, int b, int a ) = 0;

	virtual void AddLineOverlay( const vec3& origin, const vec3& dest, int r, int g, int b, bool noDepthTest, float duration = 0 ) = 0;
	virtual void AddTextOverlay( const vec3& origin, float duration, const char *format, ... ) FMTFUNCTION( 4, 5 ) = 0;
	virtual void AddTextOverlay( const vec3& origin, int line_offset, float duration, const char *format, ... ) FMTFUNCTION( 5, 6 ) = 0;
	virtual void AddScreenTextOverlay( float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text ) = 0;
	virtual void AddSweptBoxOverlay( const vec3& start, const vec3& end, const vec3& mins, const vec3& max, const vec3 & angles, int r, int g, int b, int a, float flDuration = 0 ) = 0;
	virtual void AddGridOverlay( const vec3& origin ) = 0;
	virtual void AddCoordFrameOverlay( const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL ) = 0;

	virtual int w2s( const vec3& point, vec3& screen ) = 0;
	virtual int w2s( float flXPos, float flYPos, vec3& screen ) = 0;

	virtual OverlayText_t *GetFirst( void ) = 0;
	virtual OverlayText_t *GetNext( OverlayText_t *current ) = 0;
	virtual void ClearDeadOverlays( void ) = 0;
	virtual void ClearAllOverlays( ) = 0;

	virtual void AddTextOverlayRGB( const vec3& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ... ) FMTFUNCTION( 9, 10 ) = 0;
	virtual void AddTextOverlayRGB( const vec3& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ... ) FMTFUNCTION( 9, 10 ) = 0;

	virtual void AddLineOverlayAlpha( const vec3& origin, const vec3& dest, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddBoxOverlay2( const vec3& origin, const vec3& mins, const vec3& max, vec3 const& orientation, const clr_t& faceColor, const clr_t& edgeColor, float duration ) = 0;

	virtual void PurgeTextOverlays( ) = 0;

	virtual void NewOverlay( const vec3& origin1, const vec3& origin2, int a3, int r, int g, int b, int a, int unusedParam ) = 0;

private:
	inline void AddTextOverlay( const vec3& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ... ) { } /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};

class c_user_cmd
{
public:
	virtual ~c_user_cmd( ) { };

	int       m_cmd_number;
	int       m_tickcount; 
	vec3      m_viewangles;
	vec3      m_aim_dir;
	vec3      m_move_dir;
	int       m_buttons;
	uint8_t   m_impulse;
	int       m_weapon_select;
	int       m_weapon_select_subtype;
	int       m_random_seed;
	short     m_mdx;
	short     m_mdy;
	bool      m_predicted;
};

class CMoveData
{
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	unsigned long m_nPlayerHandle;
	int m_nImpulseCommand;
	vec3 m_vecViewAngles;
	vec3 m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	vec3 m_vecVelocity;
	vec3 m_vecOldVelocity;
	float somefloat;
	vec3 m_vecAngles;
	vec3 m_vecOldAngles;

	float m_outStepHeight;
	vec3 m_outWishVel;
	vec3 m_outJumpVel;

	vec3 m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;

	void SetAbsOrigin( const vec3& vec );
	const vec3& GetAbsOrigin( ) const;

private:
	vec3 m_vecAbsOrigin; // edict::origin
	uint8_t pad_ending_001[0x160];
};


class c_input
{
public:
	void* pvftable; //0x00
	bool m_fTrackIRAvailable; //0x04
	bool m_fMouseInitialized; //0x05
	bool m_fMouseActive; //0x06
	bool m_fJoystickAdvancedInit; //0x07
	char pad_0x08[0x2C]; //0x08
	void* m_pKeys; //0x34
	char pad_0x38[0x64]; //0x38
	int pad_0x41;
	int pad_0x42;
	bool m_fCameraInterceptingMouse; //0x9C
	bool m_fCameraInThirdPerson; //0x9D
	bool m_fCameraMovingWithMouse; //0x9E
	vec3 m_vecCameraOffset; //0xA0
	bool m_fCameraDistanceMove; //0xAC
	int m_nCameraOldX; //0xB0
	int m_nCameraOldY; //0xB4
	int m_nCameraX; //0xB8
	int m_nCameraY; //0xBC
	bool m_CameraIsOrthographic; //0xC0
	vec3 m_angPreviousViewAngles; //0xC4
	vec3 m_angPreviousViewAnglesTilt; //0xD0
	float m_flLastForwardMove; //0xDC
	int m_nClearInputState; //0xE0
	char pad_0xE4[0x8]; //0xE4
	c_user_cmd* m_pCommands; //0xEC
	void* m_pVerifiedCommands; //0xF0

	c_user_cmd* GetUserCmd( int sequence_number )
	{
		return &m_pCommands[sequence_number % 150];
	}
};

class IGameMovement
{
public:
	void ProcessMovement( c_base_entity *pPlayer, CMoveData *pMove )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity*, CMoveData* ) >( this, 1 )( this, pPlayer, pMove );
	};

	void StartTrackPredictionErrors( c_base_entity *pPlayer )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity* ) >( this, 2 )( this, pPlayer );
	}

	void FinishTrackPredictionErrors( c_base_entity *pPlayer )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity* ) >( this, 3 )( this, pPlayer );
	}
};

typedef unsigned short ModelInstanceHandle_t;

enum
{
	MODEL_INSTANCE_INVALID = static_cast<ModelInstanceHandle_t>( ~0 )
};

struct ModelRenderInfo_t
{
	vec3 origin;
	vec3 angles;
	char pad[0x4]; // new
	void *pRenderable;
	const void *pModel;
	const matrix3x4_t *pModelToWorld;
	const matrix3x4_t *pLightingOffset;
	const vec3 *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	unsigned short instance;

	ModelRenderInfo_t( )
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};
class IVModelRender
{
public:
	virtual int DrawModel( int flags,
						   void* pRenderable,
						   ModelInstanceHandle_t instance,
						   int entity_index,
						   const model_t* model,
						   vec3 const& origin,
						   vec3 const& angles,
						   int skin,
						   int body,
						   int hitboxset,
						   const matrix3x4_t* modelToWorld = nullptr,
						   const matrix3x4_t* pLightingOffset = nullptr ) = 0;

	// This causes a material to be used when rendering the model instead 
	// of the materials the model was compiled with
	virtual void ForcedMaterialOverride( IMaterial* newMaterial, int a3 = 0, int a4 = 0 ) = 0;

	virtual void SetViewTarget( const vec3& target ) = 0;
	virtual void SetFlexWeights( int numweights, const float* weights ) = 0;
	virtual void SetFlexWeights( int numweights, const float* weights, const float* delayedweights ) = 0;

	virtual matrix3x4_t* pBoneToWorld( int i ) = 0;
	virtual matrix3x4_t* pBoneToWorldArray( ) = 0;

	// Creates, destroys instance data to be associated with the model
	virtual ModelInstanceHandle_t CreateInstance( void* pRenderable, void* pCache = NULL ) = 0;
	virtual void DestroyInstance( ModelInstanceHandle_t handle ) = 0;

	// Associates a particular lighting condition with a model instance handle.
	// FIXME: This feature currently only works for static props. To make it work for entities, etc.,
	// we must clean up the lightcache handles as the model instances are removed.
	// At the moment, since only the static prop manager uses this, it cleans up all LightCacheHandles 
	// at level shutdown.
	virtual void SetStaticLighting( ModelInstanceHandle_t handle, void* pHandle ) = 0;
	virtual void GetStaticLighting( ModelInstanceHandle_t handle ) = 0;

	// moves an existing InstanceHandle to a nex Renderable to keep decals etc. Models must be the same
	virtual bool ChangeInstance( ModelInstanceHandle_t handle, void* pRenderable ) = 0;

	// Creates a decal on a model instance by doing a planar projection
	// along the ray. The material is the decal material, the radius is the
	// radius of the decal to create.
	virtual void AddDecal( ModelInstanceHandle_t handle, ray_t const& ray,
						   vec3 const& decalUp, int decalIndex, int body, bool noPokeThru = false, int maxLODToDecal = 1 ) = 0;

	// Removes all the decals on a model instance
	virtual void RemoveAllDecals( ModelInstanceHandle_t handle ) = 0;

	// Remove all decals from all models
	virtual void RemoveAllDecalsFromAllModels( ) = 0;

	// Shadow rendering
	virtual void DrawModelShadow( void* pRenderable, int body ) = 0;

	// This gets called when overbright, etc gets changed to recompute static prop lighting.
	virtual bool RecomputeStaticLighting( ModelInstanceHandle_t handle ) = 0;

	virtual void ReleaseAllStaticPropColorData( void ) = 0;
	virtual void RestoreAllStaticPropColorData( void ) = 0;

	// Extended version of drawmodel
	virtual int DrawModelEx( ModelRenderInfo_t& pInfo ) = 0;

	// Shadow rendering extended version
	virtual void DrawModelShadowEx( void* pRenderable, int body, int skin ) = 0;

	virtual int DrawModelExStaticProp( ModelRenderInfo_t& pInfo ) = 0;
};

class IRefCounted
{
public:
	virtual int AddRef( ) = 0;
	virtual int Release( ) = 0;
};

class IWorldRenderList : public IRefCounted
{
};
struct VisibleFogVolumeInfo_t
{
	int		m_nVisibleFogVolume;
	int		m_nVisibleFogVolumeLeaf;
	bool	m_bEyeInFogVolume;
	float	m_flDistanceToWater;
	float	m_flWaterHeight;
	IMaterial *m_pFogVolumeMaterial;
};

typedef unsigned short LeafIndex_t;
enum
{
	INVALID_LEAF_INDEX = ( LeafIndex_t ) ~0
};

struct WorldListLeafData_t
{
	LeafIndex_t	leafIndex;	// 16 bits
	short waterData;
	uint16_t 	firstTranslucentSurface;	// engine-internal list index
	uint16_t	translucentSurfaceCount;	// count of translucent surfaces+disps
};

struct WorldListInfo_t
{
	int		m_ViewFogVolume;
	int		m_LeafCount;
	bool	m_bHasWater;
	WorldListLeafData_t	*m_pLeafDataList;
};

struct VisOverrideData_t
{
	vec3		m_vecVisOrigin;					// The point to to use as the viewpoint for area portal backface cull checks.
	float		m_fDistToAreaPortalTolerance;	// The distance from an area portal before using the full screen as the viewable portion.
};

struct BrushVertex_t
{
	vec3		m_Pos;
	vec3		m_Normal;
	vec3		m_TangentS;
	vec3		m_TangentT;
	vec2	m_TexCoord;
	vec2	m_LightmapCoord;

private:
	BrushVertex_t( const BrushVertex_t& src );
};

class IBrushSurface
{
public:
	// Computes texture coordinates + lightmap coordinates given a world position
	virtual void ComputeTextureCoordinate( vec3 const& worldPos, vec2& texCoord ) = 0;
	virtual void ComputeLightmapCoordinate( vec3 const& worldPos, vec2& lightmapCoord ) = 0;

	// Gets the vertex data for this surface
	virtual int  GetVertexCount( ) const = 0;
	virtual void GetVertexData( BrushVertex_t* pVerts ) = 0;

	// Gets at the material properties for this surface
	virtual IMaterial* GetMaterial( ) = 0;
};
class CViewSetup;

class IBrushRenderer
{
public:
	// Draws the surface; returns true if decals should be rendered on this surface
	virtual bool RenderBrushModelSurface( c_base_entity* pBaseEntity, IBrushSurface* pBrushSurface ) = 0;
};
class IVRenderView
{
public:

	// Draw normal brush model.
	// If pMaterialOverride is non-null, then all the faces of the bmodel will
	// set this material rather than their regular material.
	virtual void			DrawBrushModel(
		void *baseentity,
		model_t *model,
		const vec3& origin,
		const vec3& angles,
		bool bUnused ) = 0;

	// Draw brush model that has no origin/angles change ( uses identity transform )
	// FIXME, Material proxy IClientEntity *baseentity is unused right now, use DrawBrushModel for brushes with
	//  proxies for now.
	virtual void			DrawIdentityBrushModel( IWorldRenderList *pList, model_t *model ) = 0;

	// Mark this dynamic light as having changed this frame ( so light maps affected will be recomputed )
	virtual void			TouchLight( struct dlight_t *light ) = 0;
	// Draw 3D Overlays
	virtual void			Draw3DDebugOverlays( void ) = 0;
	// Sets global blending fraction
	virtual void			SetBlend( float blend ) = 0;
	virtual float			GetBlend( void ) = 0;

	// Sets global color modulation
	virtual void			SetColorModulation( float const* blend ) = 0;
	virtual void			GetColorModulation( float* blend ) = 0;

	// Wrap entire scene drawing
	virtual void			SceneBegin( void ) = 0;
	virtual void			SceneEnd( void ) = 0;

	// Gets the fog volume for a particular point
	virtual void			GetVisibleFogVolume( const vec3& eyePoint, VisibleFogVolumeInfo_t *pInfo ) = 0;

	// Wraps world drawing
	// If iForceViewLeaf is not -1, then it uses the specified leaf as your starting area for setting up area portal culling.
	// This is used by water since your reflected view origin is often in solid space, but we still want to treat it as though
	// the first portal we're looking out of is a water portal, so our view effectively originates under the water.
	virtual IWorldRenderList * CreateWorldList( ) = 0;

	virtual void			BuildWorldLists( IWorldRenderList *pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = NULL, bool bShadowDepth = false, float *pReflectionWaterHeight = NULL ) = 0;
	virtual void			DrawWorldLists( IWorldRenderList *pList, unsigned long flags, float waterZAdjust ) = 0;

	// Optimization for top view
	virtual void			DrawTopView( bool enable ) = 0;
	virtual void			TopViewBounds( vec2 const& mins, vec2 const& maxs ) = 0;

	// Draw lights
	virtual void			DrawLights( void ) = 0;
	// FIXME:  This function is a stub, doesn't do anything in the engine right now
	virtual void			DrawMaskEntities( void ) = 0;

	// Draw surfaces with alpha
	virtual void			DrawTranslucentSurfaces( IWorldRenderList *pList, int sortIndex, unsigned long flags, bool bShadowDepth ) = 0;

	// Draw Particles ( just draws the linefine for debugging map leaks )
	virtual void			DrawLineFile( void ) = 0;
	// Draw lightmaps
	virtual void			DrawLightmaps( IWorldRenderList *pList, int pageId ) = 0;
	// Wraps view render sequence, sets up a view
	virtual void			ViewSetupVis( bool novis, int numorigins, const vec3 origin[] ) = 0;

	// Return true if any of these leaves are visible in the current PVS.
	virtual bool			AreAnyLeavesVisible( int *leafList, int nLeaves ) = 0;

	virtual	void			VguiPaint( void ) = 0;
	// Sets up view fade parameters
	virtual void			ViewDrawFade( byte *color, IMaterial *pMaterial ) = 0;
	// Sets up the projection matrix for the specified field of view
	virtual void			OLD_SetProjectionMatrix( float fov, float zNear, float zFar ) = 0;
	// Determine lighting at specified position
	virtual vec3		GetLightAtPoint( vec3& pos ) = 0;
	// Whose eyes are we looking through?
	virtual int				GetViewEntity( void ) = 0;
	// Get engine field of view setting
	virtual float			GetFieldOfView( void ) = 0;
	// 1 == ducking, 0 == not
	virtual unsigned char	**GetAreaBits( void ) = 0;

	// Set up fog for a particular leaf
	virtual void			SetFogVolumeState( int nVisibleFogVolume, bool bUseHeightFog ) = 0;

	// Installs a brush surface draw override method, null means use normal renderer
	virtual void			InstallBrushSurfaceRenderer( IBrushRenderer* pBrushRenderer ) = 0;

	// Draw brush model shadow
	virtual void			DrawBrushModelShadow( void *pRenderable ) = 0;

	// Does the leaf contain translucent surfaces?
	virtual	bool			LeafContainsTranslucentSurfaces( IWorldRenderList *pList, int sortIndex, unsigned long flags ) = 0;

	virtual bool			DoesBoxIntersectWaterVolume( const vec3 &mins, const vec3 &maxs, int leafWaterDataID ) = 0;

	virtual void			SetAreaState(
		unsigned char chAreaBits[32],
		unsigned char chAreaPortalBits[24] ) = 0;

	// See i
	virtual void			VGui_Paint( int mode ) = 0;

	// Push, pop views (see PushViewFlags_t above for flags)
	virtual void			Push3DView( const CViewSetup &view, int nFlags, void* pRenderTarget, int frustumPlanes ) = 0;
	virtual void			Push2DView( const CViewSetup &view, int nFlags, void* pRenderTarget, int frustumPlanes ) = 0;
	virtual void			PopView( int frustumPlanes ) = 0;

	// Sets the main view
	virtual void			SetMainView( const vec3 &vecOrigin, const vec3 &angles ) = 0;

	enum
	{
		VIEW_SETUP_VIS_EX_RETURN_FLAGS_USES_RADIAL_VIS = 0x00000001
	};

	// Wraps view render sequence, sets up a view
	virtual void			ViewSetupVisEx( bool novis, int numorigins, const vec3 origin[], unsigned int &returnFlags ) = 0;

	//replaces the current view frustum with a rhyming replacement of your choice
	virtual void			OverrideViewFrustum( int custom ) = 0;

	virtual void			DrawBrushModelShadowDepth( void *baseentity, model_t *model, const vec3& origin, const vec3& angles, int DepthMode ) = 0;
	virtual void			UpdateBrushModelLightmap( model_t *model, void *pRenderable ) = 0;
	virtual void			BeginUpdateLightmaps( void ) = 0;
	virtual void			EndUpdateLightmaps( void ) = 0;
	virtual void			OLD_SetOffCenterProjectionMatrix( float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight ) = 0;
	virtual void			OLD_SetProjectionMatrixOrtho( float left, float top, float right, float bottom, float zNear, float zFar ) = 0;
	virtual void			Push3DView( const CViewSetup &view, int nFlags, void* pRenderTarget, int frustumPlanes, void* pDepthTexture ) = 0;
	virtual void			GetMatricesForView( const CViewSetup &view, void *pWorldToView, void *pViewToProjection, void *pWorldToProjection, void *pWorldToPixels ) = 0;
	virtual void			DrawBrushModelEx( void *baseentity, model_t *model, const vec3& origin, const vec3& angles, int mode ) = 0;
};

class c_move_helper
{
public:

	void set_host( c_base_entity* ent )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity* ) >( this, 1 )( this,ent );
	}

	void unk_func( )
	{
		return memory::get_vfunc< void( __thiscall* )( void* ) >( this, 4 )( this );
	}
};

class IPrediction
{
public:
	void setup_move( c_base_entity* ent, c_user_cmd* cmd, c_move_helper* move_helper, CMoveData* move_data )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity*, c_user_cmd*, c_move_helper*, CMoveData* ) >( this, 20 )( this, ent, cmd, move_helper, move_data );
	}

	void finish_move( c_base_entity* ent, c_user_cmd* cmd, CMoveData* move_data )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_base_entity*, c_user_cmd*, CMoveData* ) >( this, 21 )( this, ent, cmd, move_data );
	}

	uint8_t pad[8];
	bool m_in_prediction;
	bool m_first_time_predicted;
	bool m_engine_paused;
};

/* 10 */
enum CSGO_ACTS
{
	ACT_CSGO_DEFUSE = 958,
	ACT_CSGO_DEFUSE_WITH_KIT = 959,
	ACT_CSGO_FLASHBANG_REACTION = 960,
	ACT_CSGO_FIRE_PRIMARY = 961, // when shooting
	ACT_CSGO_FIRE_PRIMARY_OPT_1 = 962,
	ACT_CSGO_FIRE_PRIMARY_OPT_2 = 963,
	ACT_CSGO_FIRE_SECONDARY = 964, // shooting secondary
	ACT_CSGO_RELOAD = 967,
	ACT_CSGO_RELOAD_START = 968,
	ACT_CSGO_RELOAD_LOOP = 969,
	ACT_CSGO_RELOAD_END = 970,
	ACT_CSGO_PLANT_BOMB = 978,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST = 979,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING = 980,
};

class CAnimationLayer
{
public:
	char pad_0000[20];
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char pad_0038[4]; //0x0034
}; //Size: 0x0038

struct CCSGOPlayerAnimState
{
	uint8_t pad_0000[108]; //0x0000
	float m_update_time; //0x006C
	int32_t m_update_framecount; //0x0070
	float m_update_delta; //0x0074
	float m_yaw; //0x0078
	float m_pitch; //0x007C
	float m_goal_feet_yaw; //0x0080
	float m_cur_feet_yaw; //0x0084
	uint8_t pad_0088[28]; //0x0088
	float m_duck_amount; //0x00A4
	uint8_t pad_00A8[8]; //0x00A8
	vec3 m_origin; //0xB0
	vec3  m_last_origin; //0xBC
	vec3 m_velocity; //0xC8
	uint8_t pad_00D4[24]; //0x00D4
	float m_speed; //0xEC
	uint8_t pad_00F0[24]; //0x00F0
	bool m_onground; //0x108
	bool m_hitground_anim; //0x109
	uint8_t pad_010A[2]; //0x010A
	float m_next_lby_update; // not transmitted
	uint8_t pad_0110[576]; //0x0110
	int last_value; //0x0350
	/*
	char pad0[28];
	void*				    m_outer; //0x1C
	char pad1[64];
	void*				    m_player; //0x60
	void*			        m_active_weapon; //0x64
	void*			        m_last_active_weapon; //0x68
	float					m_update_time; //0x6C
	int						m_update_framecount; //0x70
	float					m_update_delta; //0x74
	float					m_yaw; //0x78
	float					m_pitch; //0x7C
	float					m_goal_feet_yaw; //0x80
	float					m_cur_feet_yaw; //0x84
	float                   m_cur_torso_yaw; //0x88
	char pad3[4];
	float                   m_lean; //0x90
	char pad4[4];
	float                   m_feet_cycle; //0x98
	float                   m_feet_yaw_rate; //0x9C
	char pad5[4];
	float                   m_duck_amount; //0xA4
	char pad6[8];
	vec3                    m_origin; //0xB0
	vec3                    m_last_origin; //0xBC
	vec3					m_velocity; //0xC8
	char pad7[24];
	float					m_speed; //0xEC
	char pad8[24];
	bool					m_onground; //0x108
	bool					m_hitground_anim; //0x109
	char pad9[2];
	float					m_next_lby_update; // not transmitted
	char pad10[468];
	float last_value; //0x02E4
	*/
};

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0, // must be first and is default group
		LOCALPLAYER, // bytes for local player entity update
		OTHERPLAYERS, // bytes for other players update
		ENTITIES, // all other entity bytes
		SOUNDS, // game sounds
		EVENTS, // event messages
		TEMPENTS, // temp entities
		USERMESSAGES, // user messages
		ENTMESSAGES, // entity messages
		VOICE, // voice data
		STRINGTABLE, // a stringtable update
		MOVE, // client move cmds
		STRINGCMD, // string command
		SIGNON, // various signondata
		TOTAL, // must be last and is not a real group
	};

	virtual const char* GetName( ) const = 0; // get channel name
	virtual const char* GetAddress( ) const = 0; // get channel IP address as string
	virtual float GetTime( ) const = 0; // current net time
	virtual float GetTimeConnected( ) const = 0; // get connection time in seconds
	virtual int GetBufferSize( ) const = 0; // netchannel packet history size
	virtual int GetDataRate( ) const = 0; // send data rate in byte/sec

	virtual bool IsLoopback( ) const = 0; // true if loopback channel
	virtual bool IsTimingOut( ) const = 0; // true if timing out
	virtual bool IsPlayback( ) const = 0; // true if demo playback

	virtual float GetLatency( int flow ) const = 0; // current latency (RTT), more accurate but jittering
	virtual float GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float GetAvgLoss( int flow ) const = 0; // avg packet loss[0..1]
	virtual float GetAvgChoke( int flow ) const = 0; // avg packet choke[0..1]
	virtual float GetAvgData( int flow ) const = 0; // data flow in bytes/sec
	virtual float GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int GetTotalData( int flow ) const = 0; // total flow in/out in bytes
	virtual int GetTotalPackets( int flow ) const = 0;
	virtual int GetSequenceNr( int flow ) const = 0; // last send seq number
	virtual bool IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool GetStreamProgress( int flow, int* received, int* total ) const = 0; // TCP progress if transmitting
	virtual float GetTimeSinceLastReceived( ) const = 0; // get time since last recieved packet in seconds
	virtual float GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void GetRemoteFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const = 0;

	virtual float GetTimeoutSeconds( ) const = 0;
};

struct mstudiohitboxset_t
{
	int sznameindex;

	inline char * const pszName( void ) const
	{
		return ( ( char* ) this ) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox( int i ) const
	{
		return ( mstudiobbox_t* ) ( ( ( byte* ) this ) + hitboxindex ) + i;
	}
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ( ( char * ) this ) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

											// default values
	vec3				pos;
	float			quat[4];
	vec3			rot;
	// compression scale
	vec3				posscale;
	vec3				rotscale;

	matrix3x4_t			poseToBone;
	float			alignment[4];
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void *pProcedure( ) const { if ( procindex == 0 ) return NULL; else return  ( void * ) ( ( ( byte * ) this ) + procindex ); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char * const pszSurfaceProp( void ) const { return ( ( char * ) this ) + surfacepropidx; }
	inline int			GetSurfaceProp( void ) const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate
};

struct mstudiobonecontroller_t
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};

struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct mstudiobodyparts_t;
struct mstudiotexture_t;

struct studiohdr_t
{
	unsigned char pad00[12];
	char name[64];
	unsigned char pad01[80];
	int numbones;
	int boneindex;
	unsigned char pad02[12];
	int hitboxsetindex;
	unsigned char pad03[8];
	int	numlocalseq;
	int	localseqindex;
	mutable int                     activitylistversion;
	mutable int                     eventsindexed;

	int                                     numtextures;
	int                                     textureindex;

	int                                     numcdtextures;
	int                                     cdtextureindex;
	unsigned char pad04[188];

	mstudiobone_t *GetBone( int i ) const
	{
		return reinterpret_cast<mstudiobone_t*>( uintptr_t( this ) + boneindex ) + i;
	};

	mstudiohitboxset_t* pHitboxSet( int i ) const
	{
		return reinterpret_cast<mstudiohitboxset_t*>( uintptr_t( this ) + hitboxsetindex ) + i;
	};

	mstudiobbox_t* pHitbox( int i, int set ) const
	{
		const mstudiohitboxset_t* s = pHitboxSet( set );
		if ( !s )
			return nullptr;

		return s->pHitbox( i );
	};

	int iHitboxCount( int set ) const
	{
		const mstudiohitboxset_t* s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};
};

class IVModelInfoClient
{
public:
	model_t* GetModel( int index )
	{
		using fn = model_t*( __thiscall* )( void*, int );
		return memory::get_vfunc< fn >( this, 1 )( this, index );
	}

	int GetModelIndex( const char* name )
	{
		using fn = int( __thiscall* )( void*, const char* );
		return memory::get_vfunc< fn >( this, 2 )( this, name );
	}

	const char* GetModelName( const model_t* model )
	{
		using fn = const char*( __thiscall* )( void*, const model_t* );
		return memory::get_vfunc< fn >( this, 3 )( this, model );
	}

	void GetModelMaterials( const model_t *model, int count, IMaterial** ppMaterial )
	{
		using fn = void( __thiscall* )( void*, const model_t*, int, IMaterial** );
		return memory::get_vfunc< fn >( this, 19 )( this, model, count, ppMaterial );
	}

	studiohdr_t* GetStudioModel( const model_t* model )
	{
		using fn = studiohdr_t*( __thiscall* )( void*, const model_t* );
		return memory::get_vfunc< fn >( this, 32 )( this, model );
	}
};
#define DEFCOLOR_SRC(name, r, g, b) constexpr static color_t name (uint8_t a = 255) { return color_t(r, g, b, a); }

class ColorF
{
	float R, G, B, A;
public:
	constexpr ColorF( ) : R( 0.f ), G( 0.f ), B( 0.f ), A( 0.f ) { }

	constexpr ColorF( float r, float g, float b, float a ) : R( r ), G( g ), B( b ), A( a ) { }

	constexpr ColorF( float r, float g, float b ) : R( r ), G( g ), B( b ), A( 1.f ) { }

	float r( ) const { return R; }
	float g( ) const { return G; }
	float b( ) const { return B; }
	float a( ) const { return A; }
	void r( float _r ) { R = _r; }
	void g( float _g ) { G = _g; }
	void b( float _b ) { B = _b; }
	void a( float _a ) { A = _a; }

	const float* raw( ) const { return &R; }
};

enum cs_team_t
{
	none,
	spectator,
	terrorist,
	counter_terrorist,
};

class color_t
{
	uint8_t R, G, B, A;
public:
	constexpr color_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

	constexpr color_t( int r, int g, int b, int a ) : R( r ), G( g ), B( b ), A( a ) { }

	constexpr color_t( int r, int g, int b ) : R( r ), G( g ), B( b ), A( 255 ) { }

	DEFCOLOR_SRC( black, 0, 0, 0 );
	DEFCOLOR_SRC( white, 255, 255, 255 );
	
	class boxes
	{
	public:
		static const color_t team_based( int team, bool visible = false )
		{
			if ( team == cs_team_t::terrorist )
				return t( visible );

			if ( team == cs_team_t::counter_terrorist )
				return ct( visible );

			return ent( visible );
		}

		static const color_t t( bool visible = false )
		{
			return visible ? color_t( 255, 200, 0 ) : color_t( 255, 50, 0 );
		}

		static const color_t ct( bool visible = false )
		{
			return visible ? color_t( 50, 255, 50 ) : color_t( 0, 100, 255 );
		}

		static const color_t ent( bool visible = false )
		{
			return visible ? color_t( 178, 178, 178 ) : color_t( 255, 255, 255 );
		}

		static const color_t character( bool visible = false )
		{
			return visible ? color_t( 255, 150, 50 ) : color_t( 255, 200, 0 );
		}

		static constexpr color_t box_text( bool warning = false )
		{
			return warning ? color_t( 230, 230, 100 ) : color_t( 230, 230, 230 );
		}
	};

	int r( ) const { return R; }
	int g( ) const { return G; }
	int b( ) const { return B; }
	int a( ) const { return A; }
	void r( int _r ) { R = _r; }
	void g( int _g ) { G = _g; }
	void b( int _b ) { B = _b; }
	void a( int _a ) { A = _a; }
	
	uint8_t* r_ptr( ) { return &R; }
	uint8_t* g_ptr( ) { return &G; }
	uint8_t* b_ptr( ) { return &B; }
	uint8_t* a_ptr( ) { return &A; }

	color_t& operator =( const color_t& c )
	{
		R = c.r( );
		G = c.g( );
		B = c.b( );
		A = c.a( );
		return *this;
	}

	color_t operator+( const color_t& v ) const
	{
		return color_t( R + v.R, G + v.G, B + v.B, A + v.A );
	}

	explicit operator bool( ) const noexcept
	{
		return R > 0 || G > 0 || B > 0 || A > 0;
	}

	bool operator==( const color_t& c ) const
	{
		return R == c.r( ) && G == c.g( ) && B == c.b( );
	}

	static color_t FromHSB( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - ( int ) h;
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - saturation * ( 1.0f - f ) );

		if ( h < 1 )
		{
			return color_t(
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( t * 255 ),
				( unsigned char ) ( p * 255 )
			);
		}
		if ( h < 2 )
		{
			return color_t(
				( unsigned char ) ( q * 255 ),
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( p * 255 )
			);
		}
		if ( h < 3 )
		{
			return color_t(
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( brightness * 255 ),
				( unsigned char ) ( t * 255 )
			);
		}
		if ( h < 4 )
		{
			return color_t(
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( q * 255 ),
				( unsigned char ) ( brightness * 255 )
			);
		}
		if ( h < 5 )
		{
			return color_t(
				( unsigned char ) ( t * 255 ),
				( unsigned char ) ( p * 255 ),
				( unsigned char ) ( brightness * 255 )
			);
		}
		return color_t(
			( unsigned char ) ( brightness * 255 ),
			( unsigned char ) ( p * 255 ),
			( unsigned char ) ( q * 255 )
		);
	}

	/*fColor ToFColor( ) const
	{
	return fColor( R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f );
	}*/

	ColorF base( )
	{
		//returned address of temp value before :>
		return ColorF( R / 255.0f, G / 255.0f, B / 255.0f );
	}

	color_t __base( )
	{
		return color_t( R / 255.0f, G / 255.0f, B / 255.0f );
	}

	static color_t blend( const color_t& first, const color_t& second, float t )
	{
		return color_t(
			first.r( ) + static_cast<int>( t * ( second.r( ) - first.r( ) ) ),
			first.g( ) + static_cast<int>( t * ( second.g( ) - first.g( ) ) ),
			first.b( ) + static_cast<int>( t * ( second.b( ) - first.b( ) ) ),
			first.a( ) + static_cast<int>( t * ( second.a( ) - first.a( ) ) )
		);
	}

	float Hue( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.h;
	}

	float brightness( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.v;
	}

	float saturation( )
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f )
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f )
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.s;
	}

	void ScaleColor( float scaleR, float scaleG, float scaleB )
	{
		R = static_cast<int>( scaleR * R );
		G = static_cast<int>( scaleG * G );
		B = static_cast<int>( scaleB * B );
	}

	void ScaleAlpha( float scaleA )
	{
		A = static_cast<int>( scaleA * A );
	}
};

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};
struct vertex
{
	vertex( ) { }

	vertex( const vec2& pos, const vec2& coord = vec2( 0, 0 ) )
	{
		m_position = pos;
		m_tex_coord = coord;
	}

	void init( float x, float y, const vec2& coord = vec2( 0, 0 ) )
	{
		m_position = vec2( x, y );
		m_tex_coord = coord;
	}

	vec2 m_position;
	vec2 m_tex_coord;
};

using HFont = unsigned long;
using VPANEL = unsigned long;
class IntRect;
class IHTMLEvents;
class IHTML;
using HCursor = unsigned long;
using HTexture = unsigned long;
class CharRenderInfo;
class IVguiMatInfo;
class IImage;
class DrawTexturedRectParms_t;
class IHTMLChromeController;

class ISurface : public IAppSystem
{
public:
	// frame
	virtual void RunFrame( ) = 0;

	// hierarchy root
	virtual VPANEL GetEmbeddedPanel( ) = 0;
	virtual void SetEmbeddedPanel( VPANEL pPanel ) = 0;

	// drawing context
	virtual void PushMakeCurrent( VPANEL panel, bool useInsets ) = 0;
	virtual void PopMakeCurrent( VPANEL panel ) = 0; //13

													 // rendering functions
	virtual void DrawSetColor( ) = 0; //15
	virtual void DrawSetColor( clr_t col ) = 0; //14

	virtual void DrawFilledRect( int x0, int y0, int x1, int y1 ) = 0; //16
	virtual void DrawFilledRectArray( IntRect *pRects, int numRects ) = 0;
	virtual void DrawOutlinedRect( int x0, int y0, int x1, int y1 ) = 0;

	virtual void DrawLine( int x0, int y0, int x1, int y1 ) = 0;
	virtual void DrawPolyLine( int *px, int *py, int numPoints ) = 0;

	virtual void DrawSetApparentDepth( float flDepth ) = 0;
	virtual void DrawClearApparentDepth( ) = 0;

	virtual void DrawSetTextFont( HFont font ) = 0;
	virtual void DrawSetTextColor( ) = 0; //25
	virtual void DrawSetTextColor( clr_t col ) = 0; //24
	virtual void DrawSetTextPos( int x, int y ) = 0; //26
	virtual void DrawGetTextPos( int& x, int& y ) = 0;
	virtual void DrawPrintText( const wchar_t *text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void DrawUnicodeChar( wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;

	virtual void DrawFlushText( ) = 0;		// flushes any buffered text (for rendering optimizations)
	virtual IHTML *CreateHTMLWindow( IHTMLEvents *events, VPANEL context ) = 0;
	virtual void PaintHTMLWindow( IHTML *htmlwin ) = 0;
	virtual void DeleteHTMLWindow( IHTML *htmlwin ) = 0;

	enum ETextureFormat
	{
		eTextureFormat_RGBA,
		eTextureFormat_BGRA,
		eTextureFormat_BGRA_Opaque, // bgra format but alpha is always 255, CEF does this, we can use this fact for better perf on win32 gdi
	};

	virtual int	 DrawGetTextureId( char const *filename ) = 0;
	virtual bool DrawGetTextureFile( int id, char *filename, int maxlen ) = 0;
	virtual void DrawSetTextureFile( int id, const char *filename, int hardwareFilter, bool forceReload ) = 0;
	virtual void DrawSetTextureRGBA( int id, const unsigned char *rgba, int wide, int tall ) = 0;
	virtual void DrawSetTexture( int id ) = 0; //38
	virtual void DrawGetTextureSize( int id, int &wide, int &tall ) = 0;
	virtual void DrawTexturedRect( int x0, int y0, int x1, int y1 ) = 0;
	virtual bool IsTextureIDValid( int id ) = 0;
	virtual bool DeleteTextureByID( int id ) = 0;

	virtual int CreateNewTextureID( bool procedural = false ) = 0; //43

	virtual void GetScreenSize( int &wide, int &tall ) = 0;
	virtual void SetAsTopMost( VPANEL panel, bool state ) = 0;
	virtual void BringToFront( VPANEL panel ) = 0;
	virtual void SetForegroundWindow( VPANEL panel ) = 0;
	virtual void SetPanelVisible( VPANEL panel, bool state ) = 0;
	virtual void SetMinimized( VPANEL panel, bool state ) = 0;
	virtual bool IsMinimized( VPANEL panel ) = 0;
	virtual void FlashWindow( VPANEL panel, bool state ) = 0;
	virtual void SetTitle( VPANEL panel, const wchar_t *title ) = 0;
	virtual void SetAsToolBar( VPANEL panel, bool state ) = 0;		// removes the window's task bar entry (for context menu's, etc.)

																	// windows stuff
	virtual void CreatePopup( VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true ) = 0;
	virtual void SwapBuffers( VPANEL panel ) = 0;
	virtual void Invalidate( VPANEL panel ) = 0;
	virtual void SetCursor( HCursor cursor ) = 0;
	virtual bool IsCursorVisible( ) = 0;
	virtual void ApplyChanges( ) = 0;
	virtual bool IsWithin( int x, int y ) = 0;
	virtual bool HasFocus( ) = 0;

	// returns true if the surface supports minimize & maximize capabilities
	enum SurfaceFeature_e
	{
		ANTIALIASED_FONTS = 1,
		DROPSHADOW_FONTS = 2,
		ESCAPE_KEY = 3,
		OPENING_NEW_HTML_WINDOWS = 4,
		FRAME_MINIMIZE_MAXIMIZE = 5,
		OUTLINE_FONTS = 6,
		DIRECT_HWND_RENDER = 7,
	};
	virtual bool SupportsFeature( SurfaceFeature_e feature ) = 0;

	// restricts what gets drawn to one panel and it's children
	// currently only works in the game
	virtual void RestrictPaintToSinglePanel( VPANEL panel ) = 0;

	// these two functions obselete, use IInput::SetAppModalSurface() instead
	virtual void SetModalPanel( VPANEL ) = 0;
	virtual VPANEL GetModalPanel( ) = 0;

	virtual void UnlockCursor( ) = 0;
	virtual void LockCursor( ) = 0;
	virtual void SetTranslateExtendedKeys( bool state ) = 0;
	virtual VPANEL GetTopmostPopup( ) = 0;

	// engine-only focus handling (replacing WM_FOCUS windows handling)
	virtual void SetTopLevelFocus( VPANEL panel ) = 0;

	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
	virtual HFont CreateFont( ) = 0; //71

									 // adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
	};

	virtual bool SetFontGlyphSet( HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0 ) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile( const char *fontName, const char *fontFileName ) = 0;

	// returns the details about the font
	virtual int GetFontTall( HFont font ) = 0;
	virtual int GetFontAscent( HFont font, wchar_t wch ) = 0;
	virtual bool IsFontAdditive( HFont font ) = 0;
	virtual void GetCharABCwide( HFont font, int ch, int &a, int &b, int &c ) = 0;
	virtual int GetCharacterWidth( HFont font, int ch ) = 0;
	virtual void GetTextSize( HFont font, const wchar_t *text, int &wide, int &tall ) = 0;

	// notify icons?!?
	virtual VPANEL GetNotifyPanel( ) = 0;
	virtual void SetNotifyIcon( VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text ) = 0;

	// plays a sound
	virtual void PlaySound( const char *fileName ) = 0; //82

														//!! these functions should not be accessed directly, but only through other vgui items
														//!! need to move these to seperate interface
	virtual int GetPopupCount( ) = 0;
	virtual VPANEL GetPopup( int index ) = 0;
	virtual bool ShouldPaintChildPanel( VPANEL childPanel ) = 0;
	virtual bool RecreateContext( VPANEL panel ) = 0;
	virtual void AddPanel( VPANEL panel ) = 0;
	virtual void ReleasePanel( VPANEL panel ) = 0;
	virtual void MovePopupToFront( VPANEL panel ) = 0;
	virtual void MovePopupToBack( VPANEL panel ) = 0;

	virtual void SolveTraverse( VPANEL panel, bool forceApplySchemeSettings = false ) = 0;
	virtual void PaintTraverse( VPANEL panel ) = 0;

	virtual void EnableMouseCapture( VPANEL panel, bool state ) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds( int &x, int &y, int &wide, int &tall ) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds( int &x, int &y, int &wide, int &tall ) = 0;

	// gets the base resolution used in proportional mode
	virtual void GetProportionalBase( int &width, int &height ) = 0;

	virtual void CalculateMouseVisible( ) = 0;
	virtual bool NeedKBInput( ) = 0;

	virtual bool HasCursorPosFunctions( ) = 0;
	virtual void SurfaceGetCursorPos( int &x, int &y ) = 0;
	virtual void SurfaceSetCursorPos( int x, int y ) = 0;

	// SRC only functions!!!
	virtual void DrawTexturedLine( const vertex &a, const vertex &b ) = 0;
	virtual void DrawOutlinedCircle( int x, int y, int radius, int segments ) = 0;
	virtual void DrawTexturedPolyLine( const vertex *p, int n ) = 0; // (Note: this connects the first and last points).
	virtual void DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 ) = 0;
	virtual void DrawTexturedPolygon( int n, vertex *pVertice, bool bClipVertices = true ) = 0; //106
	virtual const wchar_t* GetTitle( VPANEL panel ) = 0;
	virtual bool IsCursorLocked( ) const = 0;
	virtual void SetWorkspaceInsets( int left, int top, int right, int bottom ) = 0;

	// squarish comic book word bubble with pointer, rect params specify the space inside the bubble
	virtual void DrawWordBubble( int x0, int y0, int x1, int y1, int nBorderThickness, clr_t rgbaBackground, clr_t rgbaBorder,
								 bool bPointer = false, int nPointerX = 0, int nPointerY = 0, int nPointerBaseThickness = 16 ) = 0;


	// Lower level char drawing code, call DrawGet then pass in info to DrawRender
	virtual bool DrawGetUnicodeCharRenderInfo( wchar_t ch, CharRenderInfo& info ) = 0;
	virtual void DrawRenderCharFromInfo( const CharRenderInfo& info ) = 0;

	// global alpha setting functions
	// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
	virtual void DrawSetAlphaMultiplier( float alpha /* [0..1] */ ) = 0;
	virtual float DrawGetAlphaMultiplier( ) = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript( bool state ) = 0;

	// video mode changing
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;

	virtual HCursor CreateCursorFromFile( char const *curOrAniFile, char const *pPathID = nullptr ) = 0;

	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual IVguiMatInfo* DrawGetTextureMatInfoFactory( int id ) = 0;

	virtual void PaintTraverseEx( VPANEL panel, bool paintPopups = false ) = 0;

	virtual float GetZPos( ) const = 0;

	// From the Xbox
	virtual void SetPanelForInput( VPANEL vpanel ) = 0;
	virtual void DrawFilledRectFastFade( int x0, int y0, int x1, int y1, int fadeStartPt, int fadeEndPt, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;
	virtual void DrawFilledRectFade( int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0; //123

	virtual void DrawSetTextureRGBAEx( int id, const unsigned char *rgba, int wide, int tall, ImageFormat imageFormat ) = 0;
	virtual void DrawSetTextScale( float sx, float sy ) = 0;
	virtual bool SetBitmapFontGlyphSet( HFont font, const char *windowsFontName, float scalex, float scaley, int flags ) = 0;
	// adds a bitmap font file
	virtual bool AddBitmapFontFile( const char *fontFileName ) = 0;
	// sets a symbol for the bitmap font
	virtual void SetBitmapFontName( const char *pName, const char *pFontFilename ) = 0;
	// gets the bitmap font filename
	virtual const char* GetBitmapFontName( const char *pName ) = 0;
	virtual void ClearTemporaryFontCache( ) = 0;

	virtual IImage* GetIconImageForFullPath( char const *pFullPath ) = 0;
	virtual void DrawUnicodeString( const wchar_t *pwString, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void PrecacheFontCharacters( HFont font, const wchar_t *pCharacters ) = 0;
	// Console-only.  Get the string to use for the current video mode for layout files.
	//virtual const char *GetResolutionKey( void ) const = 0;

	virtual const char *GetFontName( HFont font ) = 0;
	//virtual const char *GetFontFamilyName( HFont font ) = 0;

	virtual bool ForceScreenSizeOverride( bool bState, int wide, int tall ) = 0;
	// LocalToScreen, ParentLocalToScreen fixups for explicit PaintTraverse calls on Panels not at 0, 0 position
	virtual bool ForceScreenPosOffset( bool bState, int x, int y ) = 0;
	virtual void OffsetAbsPos( int &x, int &y ) = 0;

	virtual void SetAbsPosForContext( int, int, int ) = 0;
	virtual void GetAbsPosForContext( int, int &, int & ) = 0;

	// Causes fonts to get reloaded, etc.
	virtual void ResetFontCaches( ) = 0;

	virtual bool IsScreenSizeOverrideActive( ) = 0;
	virtual bool IsScreenPosOverrideActive( ) = 0;

	virtual void DestroyTextureID( int id ) = 0;

	virtual int GetTextureNumFrames( int id ) = 0;
	virtual void DrawSetTextureFrame( int id, int nFrame, unsigned int *pFrameCache ) = 0;

	virtual void GetClipRect( int &x0, int &y0, int &x1, int &y1 ) = 0;
	virtual void SetClipRect( int x0, int y0, int x1, int y1 ) = 0;

	virtual void DrawTexturedRectEx( DrawTexturedRectParms_t *pDrawParms ) = 0;

	virtual void GetKernedCharWidth( HFont font, wchar_t ch, wchar_t chBefore, wchar_t chAfter, float &wide, float &abcA ) = 0;

	virtual void DrawUpdateRegionTextureRGBA( int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall, ImageFormat imageFormat ) = 0;
	virtual bool BHTMLWindowNeedsPaint( IHTML *htmlwin ) = 0;

	virtual void DrawSetTextureRGBALinear( int, unsigned char const*, int, int ) = 0;

	virtual const char *GetWebkitHTMLUserAgentString( ) = 0;

	virtual IHTMLChromeController *Deprecated_AccessChromeHTMLController( ) = 0;

	/*
	154	CMatSystemSurface::SetAppDrivesInput(bool)
	155	CMatSystemSurface::EnableWindowsMessages(bool)
	156	CMatSystemSurface::Begin3DPaint(int,int,int,int,bool)
	157	CMatSystemSurface::End3DPaint(bool)
	158	CMatSystemSurface::DisableClipping(bool)
	159	CMatSystemSurface::SetMouseCallbacks(void (*)(int &,int &),void (*)(int,int))
	160	CMatSystemSurface::InstallPlaySoundFunc(void (*)(char const*))
	161	CMatSystemSurface::DrawColoredCircle(int,int,float,int,int,int,int)
	162	CMatSystemSurface::DrawColoredText(unsigned long,int,int,int,int,int,int,char const*,...)
	163	CMatSystemSurface::DrawColoredTextRect(unsigned long,int,int,int,int,int,int,int,int,char const*,...)
	164	CMatSystemSurface::DrawTextHeight(unsigned long,int,int &,char *,...)
	165	CMatSystemSurface::DrawTextLen(unsigned long,char const*,...)
	166	CMatSystemSurface::DrawPanelIn3DSpace(unsigned long long,VMatrix const&,int,int,float,float)
	167	CMatSystemSurface::DrawSetTextureMaterial(int,IMaterial *)
	168	CMatSystemSurface::HandleInputEvent(InputEvent_t const&)
	169	CMatSystemSurface::Set3DPaintTempRenderTarget(char const*)
	170	CMatSystemSurface::Reset3DPaintTempRenderTarget(void)
	171	CMatSystemSurface::DrawGetTextureMaterial(int)
	172	CMatSystemSurface::SetInputContext(InputContextHandle_t__ *)
	173	CMatSystemSurface::DrawSetSubTextureRGBA(int,int,int,unsigned char const*,int,int)
	174	CMatSystemSurface::DrawTexturedSubRectGradient(int,int,int,int,float,float,float,float,Color,Color,bool)
	175	CMatSystemSurface::~CMatSystemSurface()
	176	CMatSystemSurface::~CMatSystemSurface()
	177	CMatSystemSurface::SupportsFontFeature(FontFeature_t)
	178	CMatSystemSurface::InEngine(void)
	179	CMatSystemSurface::IsInThink(unsigned long long)
	180	CMatSystemSurface::SetLanguage(char const*)
	181	CMatSystemSurface::GetLanguage(void)
	182	CMatSystemSurface::ComputeTextWidth(wchar_t const*)
	*/
};

enum observer_mode_t
{
	OBS_MODE_NONE = 0,	// not in spectator mode
	OBS_MODE_DEATHCAM,	// special mode for death cam animation
	OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
	OBS_MODE_FIXED,		// view from a fixed camera position
	OBS_MODE_IN_EYE,	// follow a player in first person view
	OBS_MODE_CHASE,		// follow a player in third person view
	OBS_MODE_POI,		// PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to tons of hard-coded "<ROAMING" enum compares
	OBS_MODE_ROAMING,	// free roaming

	NUM_OBSERVER_MODES,
};
enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};
enum MoveType_t
{
	MOVETYPE_NONE = 0, // never moves
	MOVETYPE_ISOMETRIC, // For players -- in TF2 commander view, etc.
	MOVETYPE_WALK, // Player only - moving on the ground
	MOVETYPE_STEP, // gravity, special edge handling -- monsters use this
	MOVETYPE_FLY, // No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY, // flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS, // uses VPHYSICS for simulation
	MOVETYPE_PUSH, // no clip to world, push and crush
	MOVETYPE_NOCLIP, // No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER, // Used by players only when going onto a ladder
	MOVETYPE_OBSERVER, // Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM, // Allows the entity to describe its own physics

					 // should always be defined as the last item in the list
					 MOVETYPE_LAST = MOVETYPE_CUSTOM,

					 MOVETYPE_MAX_BITS = 4
};
enum LifeStates_t
{
	LIFE_ALIVE = 0, // alive
	LIFE_DYING, // playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD, // dead. lying still.
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum ClearFlags_t
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

struct c_view_setup
{
	int			x, x_old;
	int			y, y_old;
	int			width, width_old;
	int			height, height_old;
	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix;
	matrix3x4_t	m_matCustomViewMatrix;
	char		pad_0x68[0x48];
	float		fov;
	float		fovViewmodel;
	vec3		origin;
	vec3		angles;
	float		zNear;
	float		zFar;
	float		zNearViewmodel;
	float		zFarViewmodel;
	float		m_flAspectRatio;
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	int			m_nMotionBlurMode;
	float		m_flShutterTime;
	vec3		m_vShutterOpenPosition;
	vec3		m_shutterOpenAngles;
	vec3		m_vShutterClosePosition;
	vec3		m_shutterCloseAngles;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
};

struct video_mode_t
{
	int m_Width;
	int m_Height;
	int m_Format;
	int m_RefreshRate;
};


class material_config_t
{
public:
	video_mode_t m_VideoMode;
	float m_fMonitorGamma;
	float m_fGammaTVRangeMin;
	float m_fGammaTVRangeMax;
	float m_fGammaTVExponent;
	bool m_bGammaTVEnabled;
	bool m_bTripleBuffered;
	int m_nAASamples;
	int m_nForceAnisotropicLevel;
	int m_nSkipMipLevels;
	int m_nDxSupportLevel;
	int m_nFlags;
	bool m_bEditMode;
	char m_nProxiesTestMode;
	bool m_bCompressedTextures;
	bool m_bFilterLightmaps;
	bool m_bFilterTextures;
	bool m_bReverseDepth;
	bool m_bBufferPrimitives;
	bool m_bDrawFlat;
	bool m_bMeasureFillRate;
	bool m_bVisualizeFillRate;
	bool m_bNoTransparency;
	bool m_bSoftwareLighting;
	bool m_bAllowCheats;
	char m_nShowMipLevels;
	bool m_bShowLowResImage;
	bool m_bShowNormalMap;
	bool m_bMipMapTextures;
	char m_nFullbright;
	bool m_bFastNoBump;
	bool m_bSuppressRendering;
	bool m_bDrawGray;
	bool m_bShowSpecular;
	bool m_bShowDiffuse;
	int m_nWindowedSizeLimitWidth;
	int m_nWindowedSizeLimitHeight;
	int m_nAAQuality;
	bool m_bShadowDepthTexture;
	bool m_bMotionBlur;
	bool m_bSupportFlashlight;
	bool m_bPaintEnabled;
	char pad[0xC];
};

enum TeamID
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_T,
	TEAM_CT,
	TEAM_MAX
};

enum ClassID
{
	CAI_BaseNPC,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity_,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBreakableProp,
	CBreakableSurface,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMaterialModifyControl,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPlantedC4,
	CPlasma,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWorld,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
};

struct surfacephysicsparams_t
{
	float	friction;
	float	elasticity;
	float	density;
	float	thickness;
	float	dampening;
};

struct surfaceaudioparams_t
{
	float	reflectivity;
	float	hardnessFactor;
	float	roughnessFactor;

	float	roughThreshold;
	float	hardThreshold;
	float	hardVelocityThreshold;
};

struct surfacesoundnames_t
{
	unsigned short	stepleft;
	unsigned short	stepright;

	unsigned short	impactSoft;
	unsigned short	impactHard;

	unsigned short	scrapeSmooth;
	unsigned short	scrapeRough;

	unsigned short	bulletImpact;
	unsigned short	rolling;

	unsigned short	breakSound;
	unsigned short	strainSound;
};

struct surfacesoundhandles_t
{
	short	stepleft;
	short	stepright;

	short	impactSoft;
	short	impactHard;

	short	scrapeSmooth;
	short	scrapeRough;

	short	bulletImpact;
	short	rolling;

	short	breakSound;
	short	strainSound;
};

struct surfacegameprops_t
{
	float maxSpeedFactor;
	float jumpFactor;
	float m_pen_mod;
	float m_dmg_mod;
	unsigned short m_material;
	byte climbable;
	char pad00[0x4];
};

struct surfacedata_t
{
	surfacephysicsparams_t		physics;
	surfaceaudioparams_t		audio;
	surfacesoundnames_t			sounds;
	surfacegameprops_t			game;

	surfacesoundhandles_t		soundhandles;
};

class c_physics_props
{
public:
	virtual ~c_physics_props( void ) { }

	// parses a text file containing surface prop keys
	virtual int		ParseSurfaceData( const char *pFilename, const char *pTextfile ) = 0;
	// current number of entries in the database
	virtual int		SurfacePropCount( void ) = 0;

	virtual int		GetSurfaceIndex( const char *pSurfacePropName ) = 0;
	virtual void	GetPhysicsProperties( int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity ) = 0;

	virtual surfacedata_t	*GetSurfaceData( int surfaceDataIndex ) = 0;
	virtual const char		*GetString( unsigned short stringTableIndex ) = 0;


	virtual const char		*GetPropName( int surfaceDataIndex ) = 0;

	// sets the global index table for world materials
	virtual void	SetWorldMaterialIndexTable( int *pMapArray, int mapSize ) = 0;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient
	virtual void	GetPhysicsParameters( int surfaceDataIndex, surfacephysicsparams_t *pParamsOut ) = 0;
};

// reversed by master reverser
struct weapon_info_t
{
	uint8_t pad_0000[4]; //0x0000
	char* weapon_name; //0x0004
	uint8_t pad_0008[12]; //0x0008
	int32_t max_clip_ammo; //0x0014
	uint8_t pad_0018[12]; //0x0018
	int32_t max_ammo; //0x0024
	uint8_t pad_0028[4]; //0x0028
	char* world_model; //0x002C
	char* view_model; //0x0030
	char* dropped_model; //0x0034
	uint8_t pad_0038[72]; //0x0038
	char* ammo_type; //0x0080
	uint8_t pad_0084[4]; //0x0084
	char* hud_name; //0x0088
	char* weapon_name2; //0x008C
	uint8_t pad_0090[56]; //0x0090
	int	weapon_type;
	int32_t price; //0x00CC
	int32_t kill_reward; //0x00D0
	uint8_t pad_00D4[20]; //0x00D4
	bool full_auto; //0x00E8
	uint8_t pad_00E9[3]; //0x00E9
	int32_t damage; //0x00EC
	float armor_ratio; //0x00F0
	uint8_t pad_00F4[4]; //0x00F4
	float penetration; //0x00F8
	uint8_t pad_00FC[8]; //0x00FC
	float range; //0x0104
	float range_modifier; //0x0108
	uint8_t pad_010C[16]; //0x010C
	bool has_silencer; //0x011C
	uint8_t pad_011D[15]; //0x011D
	float max_speed; //0x012C
	float max_speed_alt; //0x0130
	float move_speed_modifier; //0x0134
	float spread; //0x0138
	float spread_alt; //0x013C
	float inaccuracy_crouch; //0x0140
	float inaccuracy_crouch_alt; //0x0144
	float inaccuracy_stand; //0x0148
	float inaccuracy_stand_alt; //0x014C
	float inaccuracy_jump_start; //0x0150
	float inaccuracy_jump; //0x0154
	float inaccuracy_jump_alt; //0x0158
	float inaccuracy_land; //0x015C
	float inaccuracy_land_alt; //0x0160
	float inaccuracy_ladder; //0x0164
	float inaccuracy_ladder_alt; //0x0168
	float inaccuracy_fire; //0x016C
	float inaccuracy_fire_alt; //0x0170
	float inaccuracy_move; //0x0174
	float inaccuracy_move_alt; //0x0178
	uint8_t pad_017C[72]; //0x017C
	int32_t zoom_levels; //0x01C4
	int32_t zoom_fov1; //0x01C8
	int32_t zoom_fov2; //0x01CC
}; //Size: 0x1D0
class CGlowObjectManager
{
public:
	class GlowObjectDefinition_t
	{
	public:
		void set( float r, float g, float b, float a )
		{
			m_vGlowColor = vec3( r, g, b );
			m_flGlowAlpha = a;
			m_bRenderWhenOccluded = true;
			m_bRenderWhenUnoccluded = false;
			m_flBloomAmount = 1.0f;
		}

		void *entity( )
		{
			return m_hEntity;
		}

		bool unused( ) const
		{
			return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
		}

	public:
		void * m_hEntity;
		vec3            m_vGlowColor;
		float            m_flGlowAlpha;

		char            unknown[4]; //pad
		float            flUnk; //confirmed to be treated as a float while reversing glow functions
		float            m_flBloomAmount;
		float            localplayeriszeropoint3;


		bool            m_bRenderWhenOccluded;
		bool            m_bRenderWhenUnoccluded;
		bool            m_bFullBloomRender;
		char            unknown1[1]; //pad


		int                m_nFullBloomStencilTestValue; // 0x28
		int                iUnk; //appears like it needs to be zero  
		int                m_nSplitScreenSlot; //Should be -1

											   // Linked list of free slots
		int                m_nNextFreeSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	GlowObjectDefinition_t* m_GlowObjectDefinitions;
	int     max_size;
	int     pad;
	int     size;
	GlowObjectDefinition_t* m_GlowObjectDefinitions2;
	int     currentObjects; int m_nFirstFreeSlot;
};


class c_object_manager
{
public:
	class GlowObjectDefinition_t
	{
	public:
		void glow( color_t color )
		{
			m_color = vec3( color.base( ).r( ), color.base( ).g( ), color.base( ).b( ) );
			m_alpha = 1.0f;
			m_bloom_amt = color.a( ) / 255.0f;

			m_render_when_occluded = true;
			m_render_when_not_occluded = false;
		}

	public:
		c_base_entity*   m_ent;
		vec3             m_color;
		float            m_alpha;
		uint8_t			 pad[4];
		float            padfloat;
		float			 m_bloom_amt;
		uint8_t			 pad2[4];
		bool			 m_render_when_occluded;
		bool			 m_render_when_not_occluded;
		bool			 m_full_bloom_render;
		uint8_t 		 pad4;
		int				 m_full_bloom_stencil;
		char			 pad5[4];
		int				 m_split_screen_slot;
		int				 m_next_free_slot;

		static const int m_end_of_list = -1;
		static const int m_in_use = -2;
	};

	GlowObjectDefinition_t*   m_glow_object_definitions;
	int		                  m_max_size;
	uint8_t	                  pad6[4];
	int		                  m_size;
	uint8_t                   pad7[4];
	int		                  m_current_object;
	int                       m_first_free_slot;
};

class c_client_state
{
public:
	char pad0[0x9C];
	void* netchannel;
	uint32_t m_nChallengeNr;
	char pad_00A4[100];
	uint32_t m_nSignonState;
	char pad_010C[8];
	float m_flNextCmdTime;
	uint32_t m_nServerCount;
	uint32_t m_nCurrentSequence;
	char pad_0120[8];
	float m_ClockOffsets[17];
	uint32_t m_iCurClockOffset;
	uint32_t m_nServerTick;
	uint32_t m_nClientTick;
	uint32_t m_nDeltaTick;
	bool m_bPaused;
	char pad_017D[3];
	uint32_t m_nViewEntity;
	uint32_t m_nPlayerSlot;
	char m_szLevelName[260];
	char m_szLevelNameShort[40];
	char m_szGroupName[40];
	char pad_02DC[52];
	uint32_t m_nMaxClients;
	char pad_0314[18820];
	float m_flLastServerTickTime;
	bool insimulation;
	char pad_4C9D[3];
	uint32_t oldtickcount;
	float m_tickRemainder;
	float m_frameTime;
	uint32_t lastoutgoingcommand;
	uint32_t chokedcommands;
	uint32_t last_command_ack;
	uint32_t command_ack;
	uint32_t m_nSoundSequence;
	char pad_4CC0[80];
	vec3 viewangles;
};

class c_localize
{
public:
	wchar_t* localize( const char* unlocalized )
	{
		return memory::get_vfunc< wchar_t*( __thiscall* )( void*, const char* ) >( this, 11 )( this, unlocalized );
	}
};

class IGameEvent
{
public:
	const char* GetName( )
	{
		return memory::get_vfunc< const char*( __thiscall* )( void* ) >( this, 1 )( this );
	}

	int GetInt( const char* key_name, int default_ = 0 )
	{
		return memory::get_vfunc< int( __thiscall* )( void*, const char*, int ) >( this, 6 )( this, key_name, default_ );
	}

	float GetFloat( const char* key_name, float default_ = 0 )
	{
		return memory::get_vfunc< float( __thiscall* )( void*, const char*, float ) >( this, 8 )( this, key_name, default_ );
	}

	const char* GetString( const char* key_name )
	{
		return memory::get_vfunc< const char*( __thiscall* )( void*, const char*, int ) >( this, 9 )( this, key_name, 0 );
	}

	void SetString( const char* key_name, const char* value )
	{
		memory::get_vfunc< void( __thiscall* )( void*, const char*, const char* ) >( this, 16 )( this, key_name, value );
	}
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2( void ) { };

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent( IGameEvent* event ) = 0;

	virtual int GetEventDebugID( void ) = 0;
};

class IGameEventManager2
{
public:
	virtual ~IGameEventManager2( void ) { };

	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile( const char* filename ) = 0;

	// removes all and anything
	virtual void Reset( ) = 0;

	// adds a listener for a particular event
	virtual bool AddListener( IGameEventListener2* listener, const char* name, bool bServerSide ) = 0;

	// returns true if this listener is listens to given event
	virtual bool FindListener( IGameEventListener2* listener, const char* name ) = 0;

	// removes a listener 
	virtual void RemoveListener( IGameEventListener2* listener ) = 0;

	// create an event by name, but doesn't fire it. returns NULL is event is not
	// known or no listener is registered for it. bForce forces the creation even if no listener is active
	virtual IGameEvent* CreateEvent_( const char* name, bool bForce = false, int* pCookie = nullptr ) = 0;

	// fires a server event created earlier, if bDontBroadcast is set, event is not send to clients
	virtual bool FireEvent( IGameEvent* event, bool bDontBroadcast = false ) = 0;

	// fires an event for the local client only, should be used only by client code
	virtual bool FireEventClientSide( IGameEvent* event ) = 0;

	// create a new copy of this event, must be free later
	virtual IGameEvent* DuplicateEvent( IGameEvent* event ) = 0;

	// if an event was created but not fired for some reason, it has to bee freed, same UnserializeEvent
	virtual void FreeEvent( IGameEvent* event ) = 0;

	// write/read event to/from bitbuffer
	virtual bool SerializeEvent( IGameEvent* event, int* buf ) = 0;
	virtual IGameEvent* UnserializeEvent( void* buf ) = 0; // create new KeyValues, must be deleted
};

class c_stencil_state
{
public:
	bool m_enabled;
	int  m_fail_operation;
	int  m_z_fail_op;
	int  m_pass_operation;
	int  m_compare_function;
	int  m_reference_value;
	int  m_test_mask;
	int  m_write_mask;
};

enum StencilOperation_t
{
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
	STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};

enum StencilComparisonFunction_t
{
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};

struct c_model_render_info
{
	vec3               m_origin;
	vec3               m_angles;
	uint8_t            pad[4];
	void*              m_renderable;
	const model_t*     m_model;
	const matrix3x4_t* m_btw;
	const matrix3x4_t* m_light_offset;
	const vec3*        m_light_origin;
	int                m_flags;
	int                m_ent_index;
	uint8_t pad2[14];
};

class c_render_context
{
public:

	void set_stencil_state( c_stencil_state& state )
	{
		return memory::get_vfunc< void( __thiscall* )( void*, c_stencil_state& ) >( this, 127 )( this, state );
	}
};

enum
{

	MDLHANDLE_INVALID = ( uint16_t ) ~0

};


//-----------------------------------------------------------------------------

// Cache data types

//-----------------------------------------------------------------------------

enum MDLCacheDataType_t

{

	// Callbacks to get called when data is loaded or unloaded for these:

	MDLCACHE_STUDIOHDR = 0,

	MDLCACHE_STUDIOHWDATA,

	MDLCACHE_VCOLLIDE,



	// Callbacks NOT called when data is loaded or unloaded for these:

	MDLCACHE_ANIMBLOCK,

	MDLCACHE_VIRTUALMODEL,

	MDLCACHE_VERTEXES,

	MDLCACHE_DECODEDANIMBLOCK,

};



class IMDLCacheNotify
{

public:

	// Called right after the data is loaded

	virtual void OnDataLoaded( MDLCacheDataType_t type, uint16_t handle ) = 0;



	// Called right before the data is unloaded

	virtual void OnDataUnloaded( MDLCacheDataType_t type, uint16_t handle ) = 0;

};





//-----------------------------------------------------------------------------

// Flags for flushing 

//-----------------------------------------------------------------------------

enum MDLCacheFlush_t

{

	MDLCACHE_FLUSH_STUDIOHDR = 0x01,

	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,

	MDLCACHE_FLUSH_VCOLLIDE = 0x04,

	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,

	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,

	MDLCACHE_FLUSH_AUTOPLAY = 0x20,

	MDLCACHE_FLUSH_VERTEXES = 0x40,



	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,

	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF

};
class IMDLCache : public IAppSystem

{

public:

	// Used to install callbacks for when data is loaded + unloaded

	// Returns the prior notify

	virtual void SetCacheNotify( IMDLCacheNotify *pNotify ) = 0;



	// NOTE: This assumes the "GAME" path if you don't use

	// the UNC method of specifying files. This will also increment

	// the reference count of the MDL

	virtual uint16_t FindMDL( const char *pMDLRelativePath ) = 0;



	// Reference counting

	virtual int AddRef( uint16_t handle ) = 0;

	virtual int Release( uint16_t handle ) = 0;

	virtual int GetRef( uint16_t handle ) = 0;



	// Gets at the various data associated with a MDL

	virtual studiohdr_t *GetStudioHdr( uint16_t handle ) = 0;

	virtual void *GetHardwareData( uint16_t handle ) = 0;

	virtual void *GetVCollide( uint16_t handle ) = 0;

	virtual unsigned char *GetAnimBlock( uint16_t handle, int nBlock ) = 0;

	virtual void *GetVirtualModel( uint16_t handle ) = 0;

	virtual int GetAutoplayList( uint16_t handle, unsigned short **pOut ) = 0;

	virtual void *GetVertexData( uint16_t handle ) = 0;



	// Brings all data associated with an MDL into memory

	virtual void TouchAllData( uint16_t handle ) = 0;



	// Gets/sets user data associated with the MDL

	virtual void SetUserData( uint16_t handle, void* pData ) = 0;

	virtual void *GetUserData( uint16_t handle ) = 0;



	// Is this MDL using the error model?

	virtual bool IsErrorModel( uint16_t handle ) = 0;



	// Flushes the cache, force a full discard

	virtual void Flush( MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL ) = 0;



	// Flushes a particular model out of memory

	virtual void Flush( uint16_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL ) = 0;



	// Returns the name of the model (its relative path)

	virtual const char *GetModelName( uint16_t handle ) = 0;



	// faster access when you already have the studiohdr

	virtual void *GetVirtualModelFast( const studiohdr_t *pStudioHdr, uint16_t handle ) = 0;



	// all cache entries that subsequently allocated or successfully checked 

	// are considered "locked" and will not be freed when additional memory is needed

	virtual void BeginLock( ) = 0;



	// reset all protected blocks to normal

	virtual void EndLock( ) = 0;



	// returns a pointer to a counter that is incremented every time the cache has been out of the locked state (EVIL)

	virtual int *GetFrameUnlockCounterPtrOLD( ) = 0;



	// Finish all pending async operations

	virtual void FinishPendingLoads( ) = 0;



	virtual void *GetVCollideEx( uint16_t handle, bool synchronousLoad = true ) = 0;

	virtual bool GetVCollideSize( uint16_t handle, int *pVCollideSize ) = 0;



	virtual bool GetAsyncLoad( MDLCacheDataType_t type ) = 0;

	virtual bool SetAsyncLoad( MDLCacheDataType_t type, bool bAsync ) = 0;



	virtual void BeginMapLoad( ) = 0;

	virtual void EndMapLoad( ) = 0;

	virtual void MarkAsLoaded( uint16_t handle ) = 0;



	virtual void InitPreloadData( bool rebuild ) = 0;

	virtual void ShutdownPreloadData( ) = 0;



	virtual bool IsDataLoaded( uint16_t handle, MDLCacheDataType_t type ) = 0;



	virtual int *GetFrameUnlockCounterPtr( MDLCacheDataType_t type ) = 0;



	virtual studiohdr_t *LockStudioHdr( uint16_t handle ) = 0;

	virtual void UnlockStudioHdr( uint16_t handle ) = 0;



	virtual bool PreloadModel( uint16_t handle ) = 0;



	// Hammer uses this. If a model has an error loading in GetStudioHdr, then it is flagged

	// as an error model and any further attempts to load it will just get the error model.

	// That is, until you call this function. Then it will load the correct model.

	virtual void ResetErrorModelStatus( uint16_t handle ) = 0;



	virtual void MarkFrame( ) = 0;

};

template <typename T, typename I = int>
class CUtlMemory
{
public:
	T * GetBuffer( void )
	{
		return m_pMemory;
	}

	int GetAllocationCount( void )
	{
		return m_nAllocationCount;
	}

	int GetGrowSize( void )
	{
		return m_nGrowSize;
	}

	T* OffsetBufferByIndex( size_t index )
	{
		return m_pMemory + index;
	}

private:
	T * m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template <typename T, typename Allocator = CUtlMemory<T>>
class CUtlVector
{
public:
	Allocator GetMemory( void )
	{
		return m_Memory;
	}

	int GetSize( void )
	{
		return m_Size;
	}

	T* GetElements( void )
	{
		return m_pElements;
	}

private:
	Allocator m_Memory;
	int m_Size;
	T* m_pElements;
};

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void* data;
	void* watcher;
};

struct VarMapping_t
{
	CUtlVector<VarMapEntry_t> m_Entries;
	int m_nInterpolatedEntries; // +0x14
	float m_lastInterpolationTime; // +0x18
};

#pragma warning( pop )