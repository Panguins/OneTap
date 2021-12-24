#pragma once
#include "netvars.h"
#include "sdk.h"
#include <codecvt>

enum hitboxes_t
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX,
	HITBOX_CLOSEST = 30,
};

enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

enum SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST = 0x0001,	// Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST = 0x0002,	// Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID = 0x0004,	// Are we currently not solid?
	FSOLID_TRIGGER = 0x0008,	// This is something may be collideable but fires touch functions
								// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE = 0x0010,	// You can't stand on this
	FSOLID_VOLUME_CONTENTS = 0x0020,	// Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED = 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS = 0x0080,	// Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED = 0x0100,	// Collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200,	// This trigger will touch debris objects

	FSOLID_MAX_BITS = 10
};

class c_base_weapon;
class c_base_entity
{
	template < typename t >
	__forceinline t& get( std::ptrdiff_t offset )
	{
		return *reinterpret_cast< t* >( uintptr_t( this ) + offset );
	}

public:

	NETVAR_OFFSET( coordinate_frame, matrix3x4_t, "DT_BaseEntity", "m_CollisionGroup", -0x30 )
	NETVAR_OFFSET( v_angle, vec3,                 "DT_BasePlayer", "deadflag", 4 );
	NETVAR_OFFSET( writable_bones, int,           "DT_BaseAnimating", "m_nForceBone", 0x20 );
	NETVAR_OFFSET( flash_time, float,             "DT_CSPlayer", "m_flFlashMaxAlpha", -0x10 );

	NETVAR( hitbox_set, int,          "DT_BaseAnimating", "m_nHitboxSet" );
	NETVAR( force_bone, int,          "DT_BaseAnimating", "m_nForceBone" );
	NETVAR( force, vec3,              "DT_BaseAnimating", "m_vecForce" );
	NETVAR( clientside_anim, bool,    "DT_BaseAnimating", "m_bClientSideAnimation" );

	NETVAR( ragdoll_velocity, vec3,   "DT_CSRagdoll", "m_vecRagdollVelocity" );

	NETVAR( origin, vec3,             "DT_BaseEntity", "m_vecOrigin" );
	NETVAR( simulation_time, float,   "DT_BaseEntity", "m_flSimulationTime" );
	NETVAR( model_index, int,         "DT_BaseEntity", "m_nModelIndex" );
	NETVAR( mins, vec3,               "DT_BaseEntity", "m_vecMins" );
	NETVAR( maxs, vec3,               "DT_BaseEntity", "m_vecMaxs" );
	NETVAR( animation_time, float,    "DT_BaseEntity", "m_flAnimTime" );
	NETVAR( owner, ulong,             "DT_BaseEntity", "m_hOwnerEntity" );
	NETVAR( team, int,                "DT_BaseEntity", "m_iTeamNum" );
	PNETVAR( collideable, ICollideable,"DT_BaseEntity", "m_Collision" );
	NETVAR( collision_group, int,	  "DT_BaseEntity", "m_CollisionGroup" );
	NETVAR( solid_flags, int,		  "DT_BaseEntity", "m_usSolidFlags" );
	NETVAR( solid, int,				  "DT_BaseEntity", "m_nSolidType" );
	NETVAR( spotted, bool,            "DT_BaseEntity", "m_bSpotted" );

	NETVAR( health, int,              "DT_BasePlayer", "m_iHealth" );
	NETVAR( flags, int,               "DT_BasePlayer", "m_fFlags" );
	NETVAR( lifestate, uint8_t,       "DT_BasePlayer", "m_lifeState" );
	NETVAR( spec_mode, int,           "DT_BasePlayer", "m_iObserverMode" );
	NETVAR( spec_target, uint32_t,    "DT_BasePlayer", "m_hObserverTarget" );
	NETVAR( punch, vec3,              "DT_BasePlayer", "m_aimPunchAngle" );
	NETVAR( punch_velocity, vec3,     "DT_BasePlayer", "m_aimPunchAngleVel" );
	NETVAR( view_punch, vec3,         "DT_BasePlayer", "m_viewPunchAngle" );
	NETVAR( fall_velocity, float,     "DT_BasePlayer", "m_flFallVelocity" );
	NETVAR( tickbase, int,            "DT_BasePlayer", "m_nTickBase" );
	NETVAR( view_offset, vec3,        "DT_BasePlayer", "m_vecViewOffset[0]" );
	NETVAR( duck_amount, float,       "DT_BasePlayer", "m_flDuckAmount" );

	NETVAR( detonation_time, float,   "DT_PlantedC4", "m_flC4Blow" );
	NETVAR( timer_length, float,      "DT_PlantedC4", "m_flTimerLength" );

	NETVAR( armor, int,               "DT_CSPlayer", "m_ArmorValue" );
	NETVAR( has_defuser, bool,        "DT_CSPlayer", "m_bHasDefuser" );
	NETVAR( defusing, bool,           "DT_CSPlayer", "m_bIsDefusing" );
	NETVAR( has_helmet, bool,         "DT_CSPlayer", "m_bHasHelmet" );
	NETVAR( immune, bool,             "DT_CSPlayer", "m_bGunGameImmunity" );
	NETVAR( eye_angles, vec3,         "DT_CSPlayer", "m_angEyeAngles[0]" );
	PNETVAR( pose_parameters, float,  "DT_CSPlayer", "m_flPoseParameter" );
	NETVAR( velocity, vec3,           "DT_CSPlayer", "m_vecVelocity[0]" );
	NETVAR( base_velocity, vec3,      "DT_CSPlayer", "m_vecBaseVelocity" );
	NETVAR( lowerbodyyaw, float,      "DT_CSPlayer", "m_flLowerBodyYawTarget" );
	NETVAR( in_scope, bool,           "DT_CSPlayer", "m_bIsScoped" );
	NETVAR( money, int,               "DT_CSPlayer", "m_iAccount" );
	NETVAR( flash_max_alpha, float,   "DT_CSPlayer", "m_flFlashMaxAlpha" );
	NETVAR( flash_duration, float,    "DT_CSPlayer", "m_flFlashDuration" );
	NETVAR( shots_fired, int,         "DT_CSPlayer", "m_iShotsFired" );

	NETVAR( weapons, uint32_t*,       "DT_BaseCombatCharacter",  "m_hMyWeapons" );
	NETVAR( active_weapon, uint32_t,  "DT_BaseCombatCharacter",  "m_hActiveWeapon" );
	NETVAR( next_attack, float,       "DT_BaseCombatCharacter", "m_flNextAttack" )

	NETVAR( throw_time, float,        "DT_BaseCSGrenade", "m_fThrowTime" );

	NETVAR_OFFSET( move_type, uint8_t, "DT_BaseEntity", "m_nRenderMode", 1 );

	NETVAR_OFFSET( rgfl_coordinate_frame, matrix3x4_t, "DT_BaseEntity", "m_CollisionGroup", -48 );

	inline void* renderable( )
	{
		return reinterpret_cast<void*>( this + 0x4 );
	}

	inline void* networkable( )
	{
		return reinterpret_cast<void*>( this + 0x8 );
	}

	inline int index( )
	{
		return *reinterpret_cast< int* >( this + 0x64 );
	}

	inline bool is_weapon( )
	{
		// hbp a lil under the material checks

		const bool IsBaseCombatWeapon = memory::get_vfunc< bool( __thiscall* )( void* ) >( this, 160 )( this );
		return IsBaseCombatWeapon || client( )->m_class_id == CC4; /*|| client( )->m_class_id == CEconEntity;*/ // defuse kit, but idk how to get FileWeaponInfo for it
	}

	inline bool is_player( )
	{
		// lazy
		//return client( )->m_class_id == CCSPlayer;
		return memory::get_vfunc< bool( __thiscall* )( void* ) >( this, 152 )( this );
	}

	inline c_client* client( )
	{
		return memory::get_vfunc< c_client*( __thiscall* )( void* ) >( this->networkable( ), 2 )( this->networkable( ) );
	}

	const char* place_name( )
	{
		static const size_t offset = g_netvars.get_netvar( fnv( "DT_BasePlayer" ), fnv( "m_szLastPlaceName" ) );
		return reinterpret_cast< char* >( this + offset );
	}

	inline vec3& abs_origin( )
	{
		return memory::get_vfunc< vec3&( __thiscall* )( void* ) >( this, 10 )( this );
	}

	inline vec3 old_origin( )
	{
		// FF 90 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 5E 44 24
		// if it ever changes, C_BasePlayer::PostDataUpdate, has snd_soundmixer and a million other strings
		/*
			float flTimeDelta = m_flSimulationTime - m_flOldSimulationTime;
			if ( flTimeDelta > 0  &&  !( IsEffectActive(EF_NOINTERP) || bForceEFNoInterp ) )
			{
					Vector newVelo = (GetNetworkOrigin() - GetOldOrigin()  ) / flTimeDelta;
					SetAbsVelocity( newVelo);
			}
		*/

		return *reinterpret_cast< vec3* >( this + 936 );
	}

	vec3 eye_origin( )
	{
		return origin() + view_offset( );
	}

	vec3& abs_angles()
	{
		return memory::get_vfunc<vec3&(__thiscall*)(void*)>(this, 11)(this);
	}

	//nline bool compute_hitbox_surrounding_box( vec3& mins, vec3& maxs )
	//	
	//	// Note that this currently should not be called during Relink because of IK.
	//	// The code below recomputes bones so as to get at the hitboxes,
	//	// which causes IK to trigger, which causes raycasts against the other entities to occur,
	//	// which is illegal to do while in the Relink phase.
	//

	inline vec3 world_space_center( )
	{
		return vec3( ( this->mins( ) + this->maxs( ) ) * 0.5f );
	}

	// fyi, server lagcomp uses viewoffset, and modifyeyeposition isnt called
	inline vec3 firebullets_position( )
	{
		vec3 out;

		memory::get_vfunc< void( __thiscall* )( void*, vec3& ) >( this, 277 )( this, out );

		return out;
	}

	inline vec3 head_position( bool interp = false )
	{
		if ( !interp )
			return ( this->view_offset( ) + this->origin( ) );
		else
			return ( this->view_offset( ) + this->abs_origin( ) );
	}

	c_base_weapon* weapon( );

	inline bool alive()
	{
		return this->lifestate( ) == LIFE_ALIVE && health( ) > 0;
	}

	inline bool& dormant( )
	{
		return *reinterpret_cast< bool* >( reinterpret_cast<uintptr_t>(this) + 0xe9 );
	}

	std::wstring name( )
	{
		player_info_t player_info;

		if ( !info( &player_info ) )
			return L"";

		//clean it up
		char buffer[32];
		for ( int i = 0; i < 32; i++ )
		{
			if ( player_info.name[i] < 0 )
			{
				buffer[i] = ' ';
				continue;
			}

			switch ( player_info.name[i] )
			{
			case '"':
			case '\\':
			case ';':
			case '\n':
				buffer[i] = ' ';
				break;
			default:
				buffer[i] = player_info.name[i];
				break;
			}
		}
		buffer[31] = '\0';

		std::wstring_convert< std::codecvt_utf8< wchar_t >, wchar_t > convert;
		return convert.from_bytes( std::string( buffer ) );
	}

	vec3& render_angles()
	{
		return *reinterpret_cast<vec3*>(reinterpret_cast<uintptr_t>(this) + 0x128);
	}

	CAnimationLayer* anim_overlay()
	{
		return *reinterpret_cast<CAnimationLayer**>(reinterpret_cast<uintptr_t>(this) + 0x2970);
	}

	void set_anim_overlay(int i, const CAnimationLayer& layer)
	{
		(*reinterpret_cast<CAnimationLayer**>(reinterpret_cast<uintptr_t>(this) + 0x2970))[i] = layer;
	}

	CCSGOPlayerAnimState* anim_state()
	{
		return *reinterpret_cast<CCSGOPlayerAnimState**>(reinterpret_cast<uintptr_t>(this) + 0x3874);
	}

	void invalidate_bone_cache( );

	/*
	///C_BaseEntity::SetupBones
	m_CachedBoneData.Count = *(_DWORD *)(renderable_player + 0x2908);
	if ( nMaxBones >= m_CachedBoneData.Count )
	{
	sub_107F9140(LODWORD(a4), *(_DWORD *)(renderable_player + 0x28FC), 48 * m_CachedBoneData.Count);
	goto LABEL_149;
	}
	Warning("SetupBones: invalid bone array size (%d - needs %d)\n", nMaxBones, *(_DWORD *)(renderable_player + 0x2908));
	v146 = renderable_player + 0x26A0;
	goto LABEL_153;
	*/

	matrix3x4_t* bone_cache()
	{
		return *reinterpret_cast<matrix3x4_t**>(reinterpret_cast<uintptr_t>(this) + 0x28FC); // m_CachedBoneData.Base()
	}

	size_t bone_cache_count()
	{
		return *reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(this) + 0x2908); // m_CachedBoneData.Count()
	}

	inline bool good_boy( )
	{
		if ( !this )
			return false;

		if ( this->dormant( ) )
			return false;

		if ( !this->alive( ) )
			return false;

		if ( this->immune( ) )
			return false;

		return true;
	}

	bool is_standable( )
	{
		if ( solid_flags( ) & FSOLID_NOT_STANDABLE )
			return false;

		if ( solid( ) == SOLID_BSP || solid( ) == SOLID_VPHYSICS || solid( ) == SOLID_BBOX )
			return true;

		return false;
	}

	inline model_t* model( )
	{
		return memory::get_vfunc< model_t*( __thiscall* )( void* ) >( this->renderable( ), 8 )( this->renderable( ) );
	}

	inline int draw_model( int flags, uint8_t alpha = 255 )
	{
		return memory::get_vfunc< int( __thiscall* )( void*, int, uint8_t ) >( this->renderable( ), 9 )( this->renderable( ), flags, alpha );
	}

	inline bool setup_bones( matrix3x4_t* bone_matrix, int max_bones, int bone_mask, float curtime )
	{
		return memory::get_vfunc< bool( __thiscall* )( void*, matrix3x4_t*, int, int, float ) >( this->renderable( ), 13 )( this->renderable( ), bone_matrix, max_bones, bone_mask, curtime );
	}

	inline bool has_c4( )
	{
		static auto func = memory::pattern::first_code_match< bool( __thiscall* )( void* ) >( GetModuleHandle( xors( "client.dll" ) ), "0F 44 C8 33 C0", -0x65 );

		return func( this );
	}

	void update_clientside_anim()
	{
		memory::get_vfunc< void( __thiscall* )( void* ) >( this, 218 )( this );
	}

	inline bool physics_run_think( )
	{
		static auto physics_run = memory::pattern::first_code_match< bool( __thiscall* )( void*, int ) >( GetModuleHandle( xors( "client.dll" ) ), xors( "B0 01 83 FE 01" ), -0xc1 );
		return physics_run( this, 0 );
	}

	void run_pre_think( )
	{
		if ( this->physics_run_think( ) )
			return memory::get_vfunc< void( __thiscall* )( void* ) >( this, 307 )( this );
	}

	void think( )
	{
		return memory::get_vfunc< void( __thiscall* )( void* ) >( this, 137 )( this );
	}

	void run_think( );

	void run_post_think( )
	{
		return memory::get_vfunc< void( __thiscall* )( void* ) >( this, 308 )( this );
	}

	vec3 bone_position( int bone, matrix3x4_t* matrix )
	{
		return vec3( matrix[bone][0][3], matrix[bone][1][3], matrix[bone][2][3] );
	}

	mstudiobbox_t* get_hitbox(int hitbox);
	bool get_hitbox_position( int hitbox, vec3& out );

	bool info( player_info_t* );
	void select_item( const char* name, int subtype, c_base_weapon* weapon );
	bool visible( c_base_entity* attacker, const vec3& src, const vec3& dst );

	inline void set_abs_origin( vec3 origin )
	{
		static auto SetAbsOrigin = memory::pattern::first_code_match< void( __thiscall* )( void*, vec3& ) >( GetModuleHandle( xors( "client.dll" ) ), xors( "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8" ) );
		SetAbsOrigin( this, origin );
	}

	inline void set_abs_angles( vec3 angles )
	{
		static auto SetAbsAngles = memory::pattern::first_code_match< void( __thiscall* )( void*, vec3& ) >( GetModuleHandle( xors( "client.dll" ) ), xors( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );
		SetAbsAngles( this, angles );
	}

	__forceinline void fuck_ik()
	{
		auto& m_EntClientFlags = *reinterpret_cast< uint16_t* >( uintptr_t( this ) + 0x68 );

		//#define ENTCLIENTFLAG_DONTUSEIK 0x2
		m_EntClientFlags |= 2;
	}

	__forceinline float get_reload_progress( )
	{
		auto gun_layer = anim_overlay( )[1];
		float playback_rate = gun_layer.m_flPlaybackRate; // this layer both has shots and reloads, so check playback rate, this seems to be the lowest for awp, 0.6, while reloads never go over 0.5
		
		if ( playback_rate < 0.55f )
			return gun_layer.m_flCycle;

		return 1.f;
	}

	__forceinline bool is_reloading( )
	{
		return get_reload_progress( ) < 0.99f;
	}

	bool is_flashed( );

	void set_interpolation( bool interp )
	{
		VarMapping_t* varmapping = reinterpret_cast<VarMapping_t*>( uintptr_t( this ) + 0x24 );

		for ( int j = 0; j < varmapping->m_nInterpolatedEntries; j++ )
		{
			if ( !varmapping->m_Entries.GetElements( ) )
				continue;

			auto e = &varmapping->m_Entries.GetElements( )[j];

			if ( e )
				e->m_bNeedsToInterpolate = interp;
		}

		// da real interp killa
		varmapping->m_nInterpolatedEntries = interp ? 6 : 0;
	}
};

enum weapon_t
{
	rifle,
	pistol,
	sniper,
	smg,
	heavy,
	knife,
	grenade,
};

enum ItemDefinitionIndex
{
	WEAPON_NONE = 0,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

class c_base_weapon : public c_base_entity
{
	template < typename t >
	__forceinline t& get( std::ptrdiff_t offset )
	{
		return *reinterpret_cast< t* >( uintptr_t( this ) + offset );
	}
public:
	NETVAR( item_index, int,            "DT_BaseAttributableItem", "m_iItemDefinitionIndex" );

	NETVAR( clip1, int,                 "DT_BaseCombatWeapon",     "m_iClip1" );
	NETVAR( clip2, int,                 "DT_BaseCombatWeapon",     "m_iPrimaryReserveAmmoCount" );
	NETVAR( next_primary, float,        "DT_BaseCombatWeapon",     "m_flNextPrimaryAttack" );

	NETVAR( recoil_index, float,        "DT_WeaponCSBase",         "m_flRecoilIndex" );
	NETVAR( shot_time, float,           "DT_WeaponCSBase",         "m_fLastShotTime" );

	// thx senator
	inline weapon_info_t* wpn_data( )
	{
		return memory::get_vfunc< weapon_info_t*( __thiscall* )( void* ) >( this, 445 )( this );
	}

	// k i don't fucking get this
	// GetPrintName is supposed to be
	// 1 after GetName, however, it's not ):
	inline const char* get_name( )
	{
		return memory::get_vfunc< const char*( __thiscall* )( void* ) >( this, 376 )( this );
	}

	std::string print_name( );

	/*inline const char* print_name( )
	{
		auto weapon_data = wpn_data( );

		if ( !weapon_data )
			return " ";

		//return weapon_data->hud_name
		return weapon_data->weapon_name2 + 7;
	}*/

	inline int max_clip1( )
	{
		auto weapon_info = wpn_data( );

		if ( !weapon_info )
			return 30;

		return weapon_info->max_clip_ammo;
	}
	
	inline float spread( )
	{
		return memory::get_vfunc< float( __thiscall* )( void* ) >( this, 437 )( this );
	}

	inline void update_accuracy_penalty( )
	{
		return memory::get_vfunc< void( __thiscall* )( void* ) >( this, 469 )( this );
	}

	inline float inaccuracy( )
	{
		return memory::get_vfunc< float( __thiscall* )( void* ) >( this, 468 )( this );
	}

	bool can_fire( );

	//. did i write thsi..?
	// weapon info pls
	__forceinline weapon_t type( )
	{
		switch ( this->item_index( ) )
		{
		case WEAPON_DEAGLE:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_HKP2000:
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_REVOLVER:
		case WEAPON_GLOCK:
		case WEAPON_P250:
		return pistol;
		break;
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_GALILAR:
		case WEAPON_AUG:
		case WEAPON_FAMAS:
		case WEAPON_SG556:
		return rifle;
		break;
		case WEAPON_P90:
		case WEAPON_BIZON:
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_MAC10:
		case WEAPON_UMP45:
		return smg;
		break;
		case WEAPON_AWP:
		case WEAPON_SSG08:
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		return sniper;
		break;
		case WEAPON_MAG7:
		case WEAPON_SAWEDOFF:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_NEGEV:
		case WEAPON_M249:
		return heavy;
		break;
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_BAYONET:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_PUSH:
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
		case WEAPON_KNIFE_T:
		return knife;
		break;
		case WEAPON_HEGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_MOLOTOV:
		case WEAPON_INCGRENADE:
		case WEAPON_DECOY:
		return grenade;
		break;
		default:
			return knife;
		}
	}
};
