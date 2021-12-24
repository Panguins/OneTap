#pragma once

#include <memory>
#include <vector>

extern void simple_save( const char* path, const char* name, const void* src, size_t size, const char* filename );
extern void simple_load( const char* path, const char* name, void* dest, size_t size, const char* filename );

class node_t
{
public:
	virtual ~node_t( ) = default;

	virtual void add( node_t* node_ptr ) = 0;

	virtual void load( const char* path, const char* file ) = 0;
	virtual void save( const char* path, const char* file ) const = 0;
};

class holder_t : public node_t
{
public:
	__forceinline holder_t( const char* name ) : m_name( name ) { }
	__forceinline holder_t( holder_t* holder_ptr, const char* name ) : m_name( name ) { holder_ptr->add( this ); }

	void add( node_t* node_ptr ) override
	{
		m_nodes.push_back( node_ptr );
	}

	void load( const char* path, const char* file ) override
	{
		char full_path[64];
		strcpy_s( full_path, path );
		strcat_s( full_path, "_" );
		strcat_s( full_path, m_name );
		for ( auto x : m_nodes )
			x->load( full_path, file );
	}

	void save( const char* path, const char* file ) const override
	{
		char full_path[64];
		strcpy_s( full_path, path );
		strcat_s( full_path, "_" );
		strcat_s( full_path, m_name );
		for ( auto x : m_nodes )
			x->save( full_path, file );
	}

	const char* m_name;
	std::vector<node_t*> m_nodes;
};

class ISetting : public node_t
{
	void add( node_t* ) override { }
};

template <typename T>
class setting_t : ISetting
{
public:
	__forceinline setting_t( holder_t* holder_ptr, const char* name ) : m_name( name ) { holder_ptr->add( this ); }
	__forceinline setting_t( holder_t* holder_ptr, const char* name, const T& rhs ) : m_value( rhs ), m_name( name ) { holder_ptr->add( this ); }

	void load( const char* path, const char* file ) override { simple_load( path, m_name, &m_value, sizeof( m_value ), file ); }
	void save( const char* path, const char* file ) const override { simple_save( path, m_name, &m_value, sizeof( m_value ), file ); }

	__forceinline operator T&( ) { return m_value; }
	__forceinline operator const T&( ) const { return m_value; }
	__forceinline T* operator &( ) { return &m_value; }
	__forceinline const T* operator &( ) const { return &m_value; }

	__forceinline T& operator ()( ) { return m_value; }
	__forceinline const T& operator ()( ) const { return m_value; }
private:
	T m_value;
	const char* m_name;
};

class c_settings
{
public:

	holder_t m_holder{ "" };

	setting_t<bool> legit_enabled{ &m_holder, xors( "legit_active" ), false };
	setting_t<bool> legit_aimbot{ &m_holder, xors( "legit_aimbot" ), 0 };
	setting_t<int> legit_aim_key{ &m_holder, xors( "legit_aim_key" ), 0 };
	setting_t<int> legit_aim_fov{ &m_holder, xors( "legit_aim_fov" ), 0 };
	setting_t<int> legit_aim_threshold{ &m_holder, xors( "legit_aim_threshold" ), 0 };
	setting_t<int> legit_retarget_time{ &m_holder, xors( "legit_retarget_time" ), 0 };
	//setting_t<vec2> legit_smoothing{ &m_holder, xors( "legit_smoothing" ), vec2{} };
	setting_t<int> legit_smooth{ &m_holder, xors( "legit_smooth" ), 0 };
	setting_t<bool> legit_hitbox_head { &m_holder, xors( "legit_hitbox_head" ), true };
	setting_t<bool> legit_hitbox_chest{ &m_holder, xors( "legit_hitbox_chest" ), false };
	setting_t<bool> legit_hitbox_pelvis{ &m_holder, xors( "legit_hitbox_pelvis" ), false };
	setting_t<bool> legit_hitbox_legs{ &m_holder, xors( "legit_hitbox_legs" ), false };
	setting_t<bool> legit_hitbox_arms{ &m_holder, xors( "legit_hitbox_arms" ), false };
	setting_t<bool> legit_aim_friendlies{ &m_holder, xors( "legit_aim_friendlies" ), false };
	setting_t<bool> legit_auto_shoot{ &m_holder, xors( "legit_auto_shoot" ), false };
	setting_t<int> legit_rcs_y{ &m_holder, xors( "legit_rcs_y" ), false };
	setting_t<int> legit_rcs_x{ &m_holder, xors( "legit_rcs_x" ), false };
	setting_t<bool> legit_rcs_standalone{ &m_holder, xors( "legit_rcs_standalone" ), false };
	setting_t<bool> legit_rcs{ &m_holder, xors( "legit_rcs" ), false };

	setting_t<bool> legit_backtrack{ &m_holder, xors( "legit_backtrack" ), false };
	setting_t<bool> legit_backtrack_visualize{ &m_holder, xors( "legit_backtrack_visualize" ), false };
	setting_t<color_t> legit_backtrack_visualize_color{ &m_holder, xors( "legit_backtrack_visualize_color" ), color_t( 255, 0, 0 ) };
	setting_t<int> legit_backtrack_range{ &m_holder, xors( "legit_backtrack_range" ), 200 };


	setting_t<bool> legit_trigger{ &m_holder, xors( "legit_trigger" ), false };
	setting_t<int> legit_trigger_key{ &m_holder, xors( "legit_trigger_key" ), false };
	setting_t<bool> legit_trigger_friendlies{ &m_holder, xors( "legit_trigger_friendlies" ), false };
	setting_t<bool> legit_trigger_scope{ &m_holder, xors( "legit_trigger_scope" ), false };
	setting_t<bool> legit_trigger_smoke{ &m_holder, xors( "legit_trigger_smoke" ), false };
	setting_t<int> legit_trigger_delay{ &m_holder, xors( "legit_trigger_delay" ), false };

	setting_t<bool> rage_aimbot_enabled{ &m_holder, xors( "rage_aimbot_enabled" ), false };
	setting_t<bool> rage_friendlies{ &m_holder, xors( "rage_friendlies" ), false };
	setting_t<int> rage_min_dmg{ &m_holder, xors( "rage_min_dmg" ), 0 };
	setting_t<int> rage_max_misses{ &m_holder, xors( "rage_max_misses" ), 2 };
	setting_t<int> rage_aim_mode{ &m_holder, xors( "rage_aim_mode "), 0 };
	setting_t<bool> rage_hitchance{ &m_holder, xors( "rage_hitchance" ), false };
	setting_t<int> rage_hitchance_amt{ &m_holder, xors( "rage_hitchance_amt" ), 0 };
	setting_t<bool> rage_silent{ &m_holder, xors( "rage_silent" ), false };
	setting_t<int> rage_key{ &m_holder, xors( "rage_key" ), 0 };
	setting_t<bool> rage_auto_shoot{ &m_holder, xors( "rage_auto_shoot" ), false };
	setting_t<int> rage_auto_shoot_mode{ &m_holder, xors( "rage_auto_shoot_mode" ), false };
	setting_t<bool> rage_wait_unlag{ &m_holder, xors( "rage_wait_unlag" ), false };
	setting_t<int> rage_anti_spread{ &m_holder, xors( "rage_anti_spread" ), false };
	setting_t<int> rage_selection{ &m_holder, xors( "rage_selection" ), 0 };
	setting_t<bool> rage_anti_recoil{ &m_holder, xors( "rage_anti_recoil" ), false };
	setting_t<int> rage_resolver{ &m_holder, xors( "rage_resolver" ), false };
	setting_t<int> rage_position_adjustment{ &m_holder, xors( "rage_position_adjustment" ), false };
	setting_t<bool> rage_autowall{ &m_holder, xors( "rage_autowall" ), false };
	setting_t<int> rage_autowall_damage{ &m_holder, xors( "rage_autowall_damage" ), 0 };
	setting_t<bool> rage_baim_fakewalk{ &m_holder, xors( "rage_baim_fakewalk" ), false };
	setting_t<bool> rage_save_fps{ &m_holder, xors( "rage_save_fps" ), false };
	setting_t<bool> rage_anti_aim{ &m_holder, xors( "rage_anti_aim" ), false };
	setting_t<int> rage_pitch{ &m_holder, xors( "rage_pitch" ), 0 };
	setting_t<int> rage_yaw{ &m_holder, xors( "rage_yaw" ), 0 };
	setting_t<int> rage_fake_yaw{ &m_holder, xors( "rage_fake_yaw" ), 0 };
	setting_t<bool> rage_break_lby{ &m_holder, xors( "rage_break_lby" ), 0 };
	setting_t<int> rage_lby_delta{ &m_holder, xors( "rage_lby_delta" ), 0 };
	setting_t<int> rage_yaw_switch_offset{ &m_holder, xors( "rage_yaw_switch_offset" ), 180 };
	setting_t<int> rage_yaw_switch_range{ &m_holder, xors( "rage_yaw_switch_range" ), 35 };
	setting_t<int> rage_yaw_switch_speed{ &m_holder, xors( "rage_yaw_switch_speed" ), 200 };
	setting_t<int> rage_yaw_spin_speed{ &m_holder, xors( "rage_yaw_spin_speed" ), 979 };

	setting_t<bool> rage_at_targets{ &m_holder, xors( "rage_at_targets" ), false };
	setting_t<bool> rage_wall_dtc{ &m_holder, xors( "rage_wall_dtc" ), false };

	setting_t<bool> visuals_active{ &m_holder, xors( "visuals_active" ), false };

	setting_t<bool> visuals_dropped_weapons{ &m_holder, xors( "visuals_dropped_weapons" ), false };
	setting_t<color_t> visuals_dropped_weapons_color{ &m_holder, xors( "visuals_dropped_weapons_color" ), { 255, 255, 255 } };

	setting_t<bool> visuals_projectiles{ &m_holder, xors( "visuals_projectiles" ), false };
	setting_t<color_t> visuals_projectiles_color{ &m_holder, xors( "visuals_projectiles_color" ),{ 255, 100, 0 } };

	setting_t<bool> visuals_bomb{ &m_holder, xors( "visuals_bomb" ), false };
	setting_t<color_t> visuals_bomb_color{ &m_holder, xors( "visuals_bomb_color" ), { 255, 100, 100 } };

	setting_t<bool> visuals_other_ents{ &m_holder, xors( "visuals_other_ents" ), false };
	setting_t<color_t> visuals_other_ents_color{ &m_holder, xors( "visuals_other_ents_color" ),{ 255, 100, 0 } };

	setting_t<bool> visuals_no_recoil{ &m_holder, xors( "visuals_no_recoil" ), false };
	setting_t<bool> visuals_nosky{ &m_holder, xors( "visuals_nosky" ), false };
	setting_t<int> visuals_asus_walls{ &m_holder, xors( "visuals_asus_walls" ), 100 };
	setting_t<bool> visuals_world_color{ &m_holder, xors( "visuals_world_color" ), false };
	setting_t< color_t> visuals_world_color_val{ &m_holder, xors( "visuals_world_color_val" ),{ 50, 50, 50, 255 } };
	setting_t<bool> visuals_no_flash{ &m_holder, xors( "visuals_no_flash" ), false };
	setting_t<bool> visuals_no_smoke{ &m_holder, xors( "visuals_no_smoke" ), false };
	setting_t<bool> visuals_no_scope{ &m_holder, xors( "visuals_no_scope" ), false };
	setting_t<bool> visuals_no_fog{ &m_holder, xors( "visuals_no_fog" ), false };
	setting_t<bool> visuals_no_post_processing{ &m_holder, xors( "visuals_no_post_processing" ), false };
	setting_t<bool> visuals_spec_list{ &m_holder, xors( "visuals_spec_list" ), false };
	setting_t<int> visuals_view_fov{ &m_holder, xors( "visuals_view_fov" ), 0 };
	setting_t<bool> visuals_view_fov_scope{ &m_holder, xors( "visuals_view_fov_scope" ), 0 };
	setting_t<int> visuals_render_mode{ &m_holder, xors( "visuals_render_mode" ), 0 };
	setting_t<bool> visuals_indicate_lby{ &m_holder, xors( "visuals_indicate_lby" ), 0 };
	setting_t<bool> visuals_indicate_ns{ &m_holder, xors( "visuals_indicate_ns" ), 0 };
	setting_t<bool> visuals_indicate_lc{ &m_holder, xors( "visuals_indicate_lc" ), 0 };
	setting_t<int> visuals_third_person{ &m_holder, xors( "visuals_third_person" ), 0 };
	setting_t<bool> visuals_nade_prediction{ &m_holder, xors( "visuals_nade_prediction" ), 0 };
	setting_t<bool> visuals_recoil_crosshair{ &m_holder, xors( "visuals_recoil_crosshair" ), 0 };

	setting_t<int> players_esp{ &m_holder, xors( "players_esp" ), 0 };
	setting_t<bool> players_box_friendly{ &m_holder, xors( "players_box_friendly" ), 0 };
	setting_t<bool> players_box_enemy{ &m_holder, xors( "players_box_enemy" ), 0 };
	setting_t<color_t> players_box_friendly_color{ &m_holder, xors( "players_box_friendly_color" ),{ 250, 200, 0, 255 } };
	setting_t<color_t> players_box_enemy_color{ &m_holder, xors( "players_box_enemy_color" ),{ 150, 200, 50, 255 } };
	setting_t<bool> players_glow_friendly{ &m_holder, xors( "players_glow_friendly" ), 0 };
	setting_t<bool> players_glow_enemy{ &m_holder, xors( "players_glow_enemy" ), 0 };
	setting_t<color_t> players_glow_friendly_color{ &m_holder, xors( "players_glow_friendly_color" ),{ 250, 200, 0, 255 } };
	setting_t<color_t> players_glow_enemy_color{ &m_holder, xors( "players_glow_enemy_color" ),{ 150, 200, 50, 255 } };

	setting_t<bool> players_radar{ &m_holder, xors( "players_radar" ), 0 };
	setting_t<bool> players_offscreen_friendly{ &m_holder, xors( "players_offscreen_friendly" ), 0 };
	setting_t<bool> players_offscreen_enemy{ &m_holder, xors( "players_offscreen_enemy" ), 0 };
	setting_t<color_t> players_offscreen_friendly_color{ &m_holder, xors( "players_offscreen_friendly_color" ),{ 250, 200, 0, 255 } };
	setting_t<color_t> players_offscreen_enemy_color{ &m_holder, xors( "players_offscreen_enemy_color" ),{ 250, 200, 0, 255 } };

	setting_t<color_t> players_ammo_color{ &m_holder, xors( "players_ammo_color" ),{ 181, 0, 41, 255 } };
	setting_t<color_t> players_skeleton_color{ &m_holder, xors( "players_glow_enemy_color" ),{ 255, 255, 255, 255 } };

	setting_t<bool> players_name{ &m_holder, xors( "players_name" ), 0 };
	setting_t<bool> players_health{ &m_holder, xors( "players_health" ), 0 };
	setting_t<bool> players_weapon{ &m_holder, xors( "players_weapon" ), 0 };
	setting_t<bool> players_ammo{ &m_holder, xors( "players_ammo" ), 0 };
	setting_t<bool> players_skeleton{ &m_holder, xors( "players_skeleton" ), 0 };
	setting_t<bool> players_dormant{ &m_holder, xors( "players_dormant" ), 0 };

	setting_t<bool> players_money{ &m_holder, xors( "players_money" ), 0 };
	setting_t<bool> players_armor{ &m_holder, xors( "players_armor" ), 0 };
	setting_t<bool> players_flashed{ &m_holder, xors( "players_flashed" ), 0 };
	setting_t<bool> players_reload{ &m_holder, xors( "players_reload" ), 0 };
	setting_t<bool> players_zoom{ &m_holder, xors( "players_zoom" ), 0 };

	setting_t<bool> players_chams_friendly{ &m_holder, xors( "players_chams_friendly" ), 0 };
	setting_t<bool> players_chams_enemy{ &m_holder, xors( "players_chams_enemy" ), 0 };
	setting_t<color_t> players_chams_friendly_color{ &m_holder, xors( "players_chams_friendly_color" ), { 250, 200, 0, 255 } };
	setting_t<color_t> players_chams_enemy_color{ &m_holder, xors( "players_chams_enemy_color" ), { 50, 200, 50, 255 } };
	setting_t<bool> players_xqz_enemy{ &m_holder, xors( "players_xqz_enemy" ), 0 };
	setting_t<bool> players_xqz_friendly{ &m_holder, xors( "players_xqz_friendly" ), 0 };
	setting_t<int> player_chams_reflectivity{ &m_holder, xors( "player_chams_reflectivity" ), 0 };
	setting_t<int> player_chams_translucency{ &m_holder, xors( "player_chams_translucency" ), 0 };
	setting_t<int> player_chams_shine{ &m_holder, xors( "player_chams_shine" ), 0 };

	setting_t<bool> misc_bhop{ &m_holder, xors( "misc_bhop" ), false };
	setting_t<int> misc_airstuck{ &m_holder, xors( "misc_airstuck" ), 0 };
	setting_t<bool> misc_auto_strafe{ &m_holder, xors( "misc_auto_strafe" ), false };
	setting_t<int> misc_autostrafe_circle{ &m_holder, xors( "misc_autostrafe_circle" ), 0 };
	setting_t<bool> misc_fakelag{ &m_holder, xors( "misc_fakelag" ), false };
	setting_t<int> misc_fakelag_type{ &m_holder, xors( "misc_fakelag_type" ), false };
	setting_t<int> misc_fakelag_rand_max{ &m_holder, xors( "misc_fakelag_rand_max" ), false };
	setting_t<int> misc_fakelag_rand_min{ &m_holder, xors( "misc_fakelag_rand_min" ), false };
	setting_t<bool> misc_fakelag_land{ &m_holder, xors( "misc_fakelag_land" ), false };
	setting_t<bool> misc_fakelag_visualize{ &m_holder, xors( "misc_fakelag_visualize" ), false };
	setting_t<color_t> misc_fakelag_visualize_color{ &m_holder, xors( "misc_fakelag_visualize_color" ), color_t( 100, 100, 250 ) };

	setting_t<bool> misc_auto_accept{ &m_holder, xors( "misc_auto_accept" ), false };
	setting_t<bool> misc_rank_reveal{ &m_holder, xors( "misc_rank_reveal" ), false };
	setting_t<bool> misc_unlock_inventory{ &m_holder, xors( "misc_unlock_inventory" ), false };
	setting_t<bool> misc_hitmarker{ &m_holder, xors( "misc_hitmarker" ), false };
	setting_t<bool> misc_auto_defuse{ &m_holder, xors( "misc_auto_defuse" ), false };
	setting_t<int> misc_auto_defuse_key{ &m_holder, xors( "misc_auto_defuse_key" ), false };
	setting_t<bool> misc_auto_defuse_last_tick{ &m_holder, xors( "misc_auto_defuse_last_tick" ), false };
	setting_t<bool> misc_ragdoll_force{ &m_holder, xors( "misc_ragdoll_force" ), false };
	setting_t<bool> misc_ragdoll_gravity{ &m_holder, xors( "misc_ragdoll_gravity" ), false };
	setting_t<bool> misc_preserve_killfeed{ &m_holder, xors( "misc_preserve_killfeed" ), false };
	setting_t<int> misc_weapon_volume{ &m_holder, xors( "misc_weapon_volume" ), 100 };
	setting_t<int> misc_footstep_volume{ &m_holder, xors( "misc_footstep_volume" ), 100 };
	setting_t<int> misc_other_volume{ &m_holder, xors( "misc_other_volume" ), 100 };

	setting_t< int > hack_restrictions{ &m_holder, xors( "hack_restrictions" ), 1 };

	setting_t< color_t> menu_color{ &m_holder, xors( "menu_color" ), { 181, 0, 41, 255 } };
	setting_t< bool > menu_rainbow{ &m_holder, xors( "menu_rainbow" ), 0 };
	setting_t< int > menu_fade_speed{ &m_holder, xors( "menu_fade_speed" ), 5 };

	void save( const char* name ) { m_holder.save( "", name ); }
	void load( const char* name ) { m_holder.load( "", name ); }
};