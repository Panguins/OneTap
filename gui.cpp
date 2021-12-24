#include "renderer.h"
#include "context.h"

#include "gui.h"

#include <stdio.h>
#include <algorithm>

#include "menu.h"

// symbols often used that are hard to get
// °

c_gui gui;
UI::c_window menu;
UI::c_cursor cursor;

void c_gui::tab_legit( )
{
	static UI::c_groupbox aimbot_;
	static UI::c_label    pad;
	static UI::c_groupbox misc;

	static UI::c_checkbox enable;
	static UI::c_checkbox aimbot;
	static UI::c_key_bind aim_key;
	static UI::c_slider   fov;
	static UI::c_slider   threshold;
	static UI::c_slider   retarget;
	static UI::c_slider   speed;
	static UI::c_multi_dropdown aimbot_hitbox;
	static UI::c_checkbox friendlies;
	static UI::c_checkbox auto_fire;

	static UI::c_checkbox rcs;
	static UI::c_checkbox rcs_standalone;
	static UI::c_slider   amount_x;
	static UI::c_slider   amount_y;
	static UI::c_checkbox override;

	static UI::c_groupbox lagcomp;
	static UI::c_checkbox lagcomp_enable;
	static UI::c_checkbox_smoll lagcomp_visualize;
	static UI::c_color_picker lagcomp_visualize_color;
	static UI::c_slider   lagcomp_range;


	static UI::c_checkbox trigger;
	static UI::c_key_bind trigger_key;
	static UI::c_checkbox trigger_friendlies;
	static UI::c_checkbox trigger_scope_check;
	static UI::c_checkbox trigger_smoke_check;
	static UI::c_slider   trigger_delay;

	menu.column_left( );
	{
		enable.handle( menu.data( ), xors( "enable" ), &ctx.m_settings.legit_enabled );
		
		aimbot_.start( menu.data( ), xors( "aimbot" ) );
		{
			// hack_restrictions setting should handle this instead
			aimbot.handle( menu.data( ), xors( "aimbot" ), &ctx.m_settings.legit_aimbot );

			if ( ctx.m_settings.legit_aimbot )
			{
				aim_key.handle( menu.data( ), xors( "key" ), &ctx.m_settings.legit_aim_key );
				fov.handle( menu.data( ), xors( "fov" ), &ctx.m_settings.legit_aim_fov, 1, 50, xors( L"°" ) );
				threshold.handle( menu.data( ), xors( "threshold" ), &ctx.m_settings.legit_aim_threshold, 0, 80,  xors( L"°" ) );
				retarget.handle( menu.data( ), xors( "retarget time" ), &ctx.m_settings.legit_retarget_time, 0, 1500,  xors( L"ms" ) );
				speed.handle( menu.data( ), xors( "speed" ), &ctx.m_settings.legit_smooth, 1, 100,  xors( L"\045" ) );
				aimbot_hitbox.handle( menu.data( ), xors( "hitbox" ), { { &ctx.m_settings.legit_hitbox_head, xors( "head" ) }, { &ctx.m_settings.legit_hitbox_chest, xors( "chest" ) }, { &ctx.m_settings.legit_hitbox_pelvis, xors( "pelvis" ) },{ &ctx.m_settings.legit_hitbox_legs , xors( "legs" ) },{ &ctx.m_settings.legit_hitbox_arms, xors( "arms" ) }, } );
				friendlies.handle( menu.data( ), xors( "target friendlies" ), &ctx.m_settings.legit_aim_friendlies );
				auto_fire.handle( menu.data( ), xors( "auto fire" ), &ctx.m_settings.legit_auto_shoot );
			}

			rcs.handle( menu.data( ), xors( "rcs" ), &ctx.m_settings.legit_rcs );

			if ( ctx.m_settings.legit_rcs )
			{
				rcs_standalone.handle( menu.data( ), xors( "standalone" ), &ctx.m_settings.legit_rcs_standalone );
				amount_x.handle( menu.data( ), xors( "pitch amount" ), &ctx.m_settings.legit_rcs_x, 0, 100,  xors( L"\045" ) );
				amount_y.handle( menu.data( ), xors( "yaw amount" ), &ctx.m_settings.legit_rcs_y, 0, 100,  xors( L"\045" ) );
			}



			aimbot_.end( menu.data( ) );

			aimbot_hitbox.dropdown( menu.data( ) );
		}

		lagcomp.start( menu.data( ), xors( "backtracking" ) );
		{
			lagcomp_enable.handle( menu.data( ), xors( "enable" ), &ctx.m_settings.legit_backtrack );

			if ( ctx.m_settings.legit_backtrack )
			{
				lagcomp_visualize.handle( menu.data( ),&ctx.m_settings.legit_backtrack_visualize );
				lagcomp_visualize_color.handle( menu.data( ), xors( "visualize" ), ctx.m_settings.legit_backtrack_visualize_color( ).r_ptr( ), ctx.m_settings.legit_backtrack_visualize_color( ).g_ptr( ), ctx.m_settings.legit_backtrack_visualize_color( ).b_ptr( ) );
				lagcomp_range.handle( menu.data( ), xors( "range" ), &ctx.m_settings.legit_backtrack_range, 1, 200,  xors( L"ms" ) );
			}
		}

		lagcomp.end( menu.data( ) );		
		lagcomp_visualize_color.picker( menu.data( ) );
	}

	menu.column_right( );
	{
		pad.handle( menu.data( ), nullptr );

		misc.start( menu.data( ), xors( "misc" ) );
		{
			trigger.handle( menu.data( ), xors( "triggerbot" ), &ctx.m_settings.legit_trigger );

			if ( ctx.m_settings.legit_trigger )
			{
				trigger_key.handle( menu.data( ), xors( "key" ), &ctx.m_settings.legit_trigger_key );
				trigger_friendlies.handle( menu.data( ), xors( "target friendlies" ), &ctx.m_settings.legit_trigger_friendlies );
				trigger_scope_check.handle( menu.data( ), xors( "scope check" ), &ctx.m_settings.legit_trigger_scope );
				trigger_smoke_check.handle( menu.data( ), xors( "smoke check" ), &ctx.m_settings.legit_trigger_smoke );
				trigger_delay.handle( menu.data( ), xors( "delay" ), &ctx.m_settings.legit_trigger_delay, 0, 2500,  xors( L"ms" ) );
			}

			misc.end( menu.data( ) );
		}
	}
}

void c_gui::tab_rage( )
{
	static  const  std::vector< std::string > fake_yaws = { xors( "off" ), xors( "backwards" ), xors( "spin" ), xors( "sideways" ), xors( "jitter" ), xors( "switch" ), };
	static  const  std::vector< std::string > real_yaws = { xors( "off" ), xors( "backwards" ), xors( "spin" ), xors( "sideways" ), xors( "jitter" ), xors( "switch" ), xors( "freestanding" ), };
	const   bool   draw_specials = ctx.m_settings.rage_yaw != 6;

	static UI::c_groupbox aim;
	static UI::c_groupbox accuracy;
	static UI::c_label    pad;
	static UI::c_groupbox anti_hit;

	static UI::c_checkbox enable;
	static UI::c_key_bind key;
	static UI::c_checkbox silent;
	static UI::c_checkbox auto_shoot;
	static UI::c_dropdown auto_shoot_mode;
	static UI::c_dropdown aim_mode;
	static UI::c_dropdown target_selection;
	static UI::c_checkbox hitscan;
	static UI::c_dropdown hitbox; // multi
	static UI::c_slider   minimum_damage;
	static UI::c_slider   max_misses;
	static UI::c_checkbox autowall;
	static UI::c_checkbox baim_fakewalk;
	static UI::c_checkbox save_fps;

	static UI::c_checkbox no_recoil;
	static UI::c_dropdown no_spread;
	static UI::c_dropdown resolver;
	static UI::c_checkbox hitchance;
	static UI::c_slider   hitchance_amt;

	static UI::c_checkbox fake_lag;
	static UI::c_slider   fake_lag_rand_min;
	static UI::c_slider   fake_lag_rand_max;
	static UI::c_dropdown fake_lag_type;
	static UI::c_checkbox fake_land;
	static UI::c_checkbox_smoll visualize_lag;
	static UI::c_color_picker visualize_lag_color;
	static UI::c_checkbox aa_enable;
	static UI::c_checkbox edge;
	static UI::c_dropdown edge_type;
	static UI::c_dropdown pitch;
	static UI::c_dropdown yaw;
	static UI::c_dropdown fake_yaw;
	static UI::c_checkbox break_lby;
	static UI::c_slider	  lby_delta;
	static UI::c_checkbox at_targets;
	static UI::c_slider   switch_offset;
	static UI::c_slider   switch_range;
	static UI::c_slider   switch_speed;
	static UI::c_slider   spin_speed;
	static UI::c_key_bind fake_walk;
	static int filler[1];


	menu.column_left( );
	{
		enable.handle( menu.data( ), xors( "enable" ), &ctx.m_settings.rage_aimbot_enabled );

		aim.start( menu.data( ), xors( "aimbot" ) );
		{
			key.handle( menu.data( ), xors( "key" ), &ctx.m_settings.rage_key );
			silent.handle( menu.data( ), xors( "silent" ), &ctx.m_settings.rage_silent );
			auto_shoot.handle( menu.data( ), xors( "auto fire" ), &ctx.m_settings.rage_auto_shoot );

			if ( ctx.m_settings.rage_auto_shoot )
				auto_shoot_mode.handle( menu.data( ), "", { xors( "normal" ), xors( "force send on shot" ), xors( "force choke on shot" ), xors( "avoid animation update" ) }, &ctx.m_settings.rage_auto_shoot_mode );

			aim_mode.handle( menu.data( ), xors( "mode" ), { xors( "normal" ), xors( "priority baim" ), xors( "agressive baim" ) }, &ctx.m_settings.rage_aim_mode );
			target_selection.handle( menu.data( ), xors( "target selection" ), { xors( "fov" ), xors( "distance" ), xors( "lag" ), xors( "health" ), xors( "cycle" ) }, &ctx.m_settings.rage_selection );
			minimum_damage.handle( menu.data( ), xors( "minimum damage" ), &ctx.m_settings.rage_min_dmg, 1, 100,  xors( L"hp" ) );
			autowall.handle( menu.data( ), xors( "autowall" ), &ctx.m_settings.rage_autowall );
			save_fps.handle( menu.data( ), xors( "save fps" ), &ctx.m_settings.rage_save_fps );
		}

		aim.end( menu.data( ) );

		accuracy.start( menu.data( ), xors( "accuracy" ) );
		{
			no_recoil.handle( menu.data( ), xors( "no recoil" ), &ctx.m_settings.rage_anti_recoil );

			if ( !ctx.m_settings.hack_restrictions )
				no_spread.handle( menu.data( ), xors( "no spread" ), { xors( "off" ), xors( "pitch/ yaw" ), xors( "roll" ), }, &ctx.m_settings.rage_anti_spread );
			else
				ctx.m_settings.rage_anti_spread( ) = 0;

			resolver.handle( menu.data( ), xors( "resolver" ), { xors( "off" ), xors( "spread" ), xors( "no spread" ), xors( "anti boss nigga fix" ), }, &ctx.m_settings.rage_resolver );
			max_misses.handle( menu.data( ), xors( "max misses" ), &ctx.m_settings.rage_max_misses, 0, 4 );
			baim_fakewalk.handle( menu.data( ), xors( "baim fakewalk" ), &ctx.m_settings.rage_baim_fakewalk );
			hitchance.handle( menu.data( ), xors( "hitchance" ), &ctx.m_settings.rage_hitchance );

			if ( ctx.m_settings.rage_hitchance )
				hitchance_amt.handle( menu.data( ), xors( "" ) /* lol */, &ctx.m_settings.rage_hitchance_amt, 0, 100,  xors( L"\045\045" ) );

		}
		accuracy.end( menu.data( ) ); 

		resolver.dropdown( menu.data( ) );
		no_spread.dropdown( menu.data( ) );
		hitbox.dropdown( menu.data( ) );
		aim_mode.dropdown( menu.data( ) );
		target_selection.dropdown( menu.data( ) );
		auto_shoot_mode.dropdown( menu.data( ) );
	}

	menu.column_right( );
	{
		pad.handle( menu.data( ), nullptr );
		anti_hit.start( menu.data( ), xors( "anti hit" ) );
		{
			fake_lag.handle( menu.data( ), xors( "fake lag" ), &ctx.m_settings.misc_fakelag );

			if ( ctx.m_settings.misc_fakelag )
			{
				// passive = off unless landing
				fake_lag_type.handle( menu.data( ), xors( "" ), { xors( "adaptive" ), xors( "passive" ), xors( "random" ), }, &ctx.m_settings.misc_fakelag_type );

				if ( ctx.m_settings.misc_fakelag_type == 2 )
				{
					fake_lag_rand_min.handle( menu.data( ), xors( "minimum" ), &ctx.m_settings.misc_fakelag_rand_min, 1, 14,  xors( L" ticks" ) );
					fake_lag_rand_max.handle( menu.data( ), xors( "maximum" ), &ctx.m_settings.misc_fakelag_rand_max, ctx.m_settings.misc_fakelag_rand_min, 15,  xors( L" ticks" ) );
				}

				fake_land.handle( menu.data( ), xors( "fake land" ), &ctx.m_settings.misc_fakelag_land );
				visualize_lag.handle( menu.data( ), &ctx.m_settings.misc_fakelag_visualize );
				visualize_lag_color.handle( menu.data( ), xors( "visualize lag" ), ctx.m_settings.misc_fakelag_visualize_color( ).r_ptr( ), ctx.m_settings.misc_fakelag_visualize_color( ).g_ptr( ), ctx.m_settings.misc_fakelag_visualize_color( ).b_ptr( ) );
			}

			aa_enable.handle( menu.data( ), xors( "anti aim" ), &ctx.m_settings.rage_anti_aim );

			if ( ctx.m_settings.rage_anti_aim )
			{
				if ( draw_specials )
				{
					edge.handle( menu.data( ), xors( "edge" ), &ctx.m_settings.rage_wall_dtc );

					if ( ctx.m_settings.rage_wall_dtc )
						edge_type.handle( menu.data( ), xors( "" ), { xors( "hide" ), xors( "peek" ), xors( "fake" ), }, &filler[0] );

					at_targets.handle( menu.data( ), xors( "at targets" ), &ctx.m_settings.rage_at_targets );
				}
				
				if ( !ctx.m_settings.hack_restrictions )
					pitch.handle( menu.data( ), xors( "pitch" ), { xors( "off" ), xors( "emotion" ), xors( "down" ), xors( "fake down" ), xors( "fake up" ), xors( "zero" ), xors( "fake zero" ), }, &ctx.m_settings.rage_pitch );
				else
					pitch.handle( menu.data( ), xors( "pitch" ), { xors( "off" ), xors( "emotion" ), }, &ctx.m_settings.rage_pitch );

				yaw.handle( menu.data( ), xors( "yaw" ), real_yaws, &ctx.m_settings.rage_yaw );
				fake_yaw.handle( menu.data( ), xors( "fake yaw" ), fake_yaws, &ctx.m_settings.rage_fake_yaw );

				if ( ctx.m_settings.rage_yaw == 5 || ctx.m_settings.rage_fake_yaw == 5 )
				{
					switch_offset.handle( menu.data( ), xors( "switch offset" ), &ctx.m_settings.rage_yaw_switch_offset, 0, 360,  xors( L"°" ) );
					switch_range.handle( menu.data( ), xors( "switch range" ), &ctx.m_settings.rage_yaw_switch_range, 0, 180,  xors( L"°" ) );
					switch_speed.handle( menu.data( ), xors( "switch speed" ), &ctx.m_settings.rage_yaw_switch_speed, 1, 2500 );
				}

				if ( ctx.m_settings.rage_yaw == 2 || ctx.m_settings.rage_fake_yaw == 2 )
				{
					spin_speed.handle( menu.data( ), xors( "spin speed" ), &ctx.m_settings.rage_yaw_spin_speed, 1, 2500 );
				}

				if ( ctx.m_settings.rage_yaw > 0 && ctx.m_settings.rage_fake_yaw > 0 )
				{
					break_lby.handle( menu.data( ), xors( "break lby" ), &ctx.m_settings.rage_break_lby );
					lby_delta.handle( menu.data( ), xors( "delta" ), &ctx.m_settings.rage_lby_delta, 0, 180,  xors( L"°" ) );
				}

				fake_walk.handle( menu.data( ), xors( "fake walk" ), &filler[1] );
			}

			anti_hit.end( menu.data( ) );
		}

		fake_yaw.dropdown( menu.data( ) );
		yaw.dropdown( menu.data( ) );
		pitch.dropdown( menu.data( ) );
		edge_type.dropdown( menu.data( ) );
		visualize_lag_color.picker( menu.data( ) );
		fake_lag_type.dropdown( menu.data( ) );
	}
}

void c_gui::tab_players( )
{
	static UI::c_groupbox esp;
	static UI::c_groupbox other;

	static UI::c_dropdown players_esp;
	static UI::c_checkbox_smoll enemy_box;
	static UI::c_color_picker enemy_box_color;
	static UI::c_checkbox_smoll enemy_glow;
	static UI::c_color_picker enemy_glow_color;
	static UI::c_checkbox_smoll friendly_box;
	static UI::c_color_picker friendly_box_color;
	static UI::c_checkbox_smoll friendly_glow;
	static UI::c_color_picker friendly_glow_color;

	static UI::c_checkbox name;
	static UI::c_checkbox health;
	static UI::c_checkbox weapon;
	static UI::c_checkbox_smoll ammo;
	static UI::c_color_picker ammo_color;
	static UI::c_checkbox_smoll skeleton;
	static UI::c_color_picker skeleton_color;
	static UI::c_checkbox dormant;

	static UI::c_multi_dropdown flags;

	static UI::c_checkbox       radar;
	static UI::c_checkbox_smoll offscreen_arrows_friendly;
	static UI::c_color_picker offscreen_arrows_friendly_color;
	static UI::c_checkbox_smoll offscreen_arrows_enemy;
	static UI::c_color_picker offscreen_arrows_enemy_color;

	static UI::c_checkbox_smoll chams_enemy;
	static UI::c_color_picker chams_enemy_color;
	static UI::c_checkbox xqz_enemy;

	static UI::c_checkbox_smoll chams_friendly;
	static UI::c_color_picker chams_friendly_color;
	static UI::c_checkbox xqz_friendly;

	static UI::c_slider chams_reflectivity;
	static UI::c_slider chams_translucency;
	static UI::c_slider chams_shine; // ?

	menu.column_left( );
	{
		esp.start( menu.data( ), xors( "esp" ) );
		{
			players_esp.handle( menu.data( ), xors( "esp mode" ), { xors( "off" ), xors( "enemy only" ), xors( "all" ) }, &ctx.m_settings.players_esp );

			enemy_box.handle( menu.data( ), &ctx.m_settings.players_box_enemy );
			enemy_box_color.handle( menu.data( ), xors( "enemy box" ), ctx.m_settings.players_box_enemy_color( ).r_ptr( ), ctx.m_settings.players_box_enemy_color( ).g_ptr( ), ctx.m_settings.players_box_enemy_color( ).b_ptr( ) );

			friendly_box.handle( menu.data( ), &ctx.m_settings.players_box_friendly );
			friendly_box_color.handle( menu.data( ), xors( "friendly box" ), ctx.m_settings.players_box_friendly_color( ).r_ptr( ), ctx.m_settings.players_box_friendly_color( ).g_ptr( ), ctx.m_settings.players_box_friendly_color( ).b_ptr( ) );

			name.handle( menu.data( ), xors( "name" ), &ctx.m_settings.players_name );
			health.handle( menu.data( ), xors( "health" ), &ctx.m_settings.players_health );
			weapon.handle( menu.data( ), xors( "weapon" ), &ctx.m_settings.players_weapon );
			ammo.handle( menu.data( ), &ctx.m_settings.players_ammo );
			ammo_color.handle( menu.data( ), xors( "ammo" ), ctx.m_settings.players_ammo_color( ).r_ptr( ), ctx.m_settings.players_ammo_color( ).g_ptr( ), ctx.m_settings.players_ammo_color( ).b_ptr( ) );
			skeleton.handle( menu.data( ), &ctx.m_settings.players_skeleton );
			skeleton_color.handle( menu.data( ), xors( "skeleton" ), ctx.m_settings.players_skeleton_color( ).r_ptr( ), ctx.m_settings.players_skeleton_color( ).g_ptr( ), ctx.m_settings.players_skeleton_color( ).b_ptr( ) );
			dormant.handle( menu.data( ), xors( "dormant" ), &ctx.m_settings.players_dormant );

			flags.handle( menu.data( ), xors( "flags" ), { { &ctx.m_settings.players_money, xors( "money" ) }, { &ctx.m_settings.players_armor, xors( "armor" ) }, { &ctx.m_settings.players_flashed, xors( "flashed" ) }, { &ctx.m_settings.players_reload, xors( "reload" ) }, { &ctx.m_settings.players_zoom, xors( "zoom" ) } } );
		}
		esp.end( menu.data( ) );
	}

	menu.column_right( );
	{
		other.start( menu.data( ), xors( "other" ) );
		{
			radar.handle( menu.data( ), xors( "reveal on radar" ), &ctx.m_settings.players_radar );
			chams_enemy.handle( menu.data( ), &ctx.m_settings.players_chams_enemy );
			chams_enemy_color.handle( menu.data( ), xors( "enemy chams" ), ctx.m_settings.players_chams_enemy_color( ).r_ptr( ), ctx.m_settings.players_chams_enemy_color( ).g_ptr( ), ctx.m_settings.players_chams_enemy_color( ).b_ptr( ) );
			xqz_enemy.handle( menu.data( ), xors( "enemy xqz" ), &ctx.m_settings.players_xqz_enemy );

			enemy_glow.handle( menu.data( ), &ctx.m_settings.players_glow_enemy );
			enemy_glow_color.handle( menu.data( ), xors( "enemy glow" ), ctx.m_settings.players_glow_enemy_color( ).r_ptr( ), ctx.m_settings.players_glow_enemy_color( ).g_ptr( ), ctx.m_settings.players_glow_enemy_color( ).b_ptr( ), ctx.m_settings.players_glow_enemy_color( ).a_ptr( ) );

			//offscreen_arrows_enemy.handle( menu.data( ), &ctx.m_settings.players_offscreen_enemy );
			//offscreen_arrows_enemy_color.handle( menu.data( ), "enemy offscreen indicator", ctx.m_settings.players_offscreen_enemy_color( ).r_ptr( ), ctx.m_settings.players_offscreen_enemy_color( ).g_ptr( ), ctx.m_settings.players_offscreen_enemy_color( ).b_ptr( ) );

			chams_friendly.handle( menu.data( ), &ctx.m_settings.players_chams_friendly );
			chams_friendly_color.handle( menu.data( ), xors( "friendly chams" ), ctx.m_settings.players_chams_friendly_color( ).r_ptr( ), ctx.m_settings.players_chams_friendly_color( ).g_ptr( ), ctx.m_settings.players_chams_friendly_color( ).b_ptr( ) );
			xqz_friendly.handle( menu.data( ), xors( "friendly xqz" ), &ctx.m_settings.players_xqz_friendly );

			friendly_glow.handle( menu.data( ), &ctx.m_settings.players_glow_friendly );
			friendly_glow_color.handle( menu.data( ), xors( "friendly glow" ), ctx.m_settings.players_glow_friendly_color( ).r_ptr( ), ctx.m_settings.players_glow_friendly_color( ).g_ptr( ), ctx.m_settings.players_glow_friendly_color( ).b_ptr( ), ctx.m_settings.players_glow_friendly_color( ).a_ptr( ) );

			//offscreen_arrows_friendly.handle( menu.data( ), &ctx.m_settings.players_offscreen_friendly );
			//offscreen_arrows_friendly_color.handle( menu.data( ), "friendly offscreen indicator", ctx.m_settings.players_offscreen_friendly_color( ).r_ptr( ), ctx.m_settings.players_offscreen_friendly_color( ).g_ptr( ), ctx.m_settings.players_offscreen_friendly_color( ).b_ptr( ) );

			chams_reflectivity.handle( menu.data( ), xors( "reflectivity" ), &ctx.m_settings.player_chams_reflectivity, 0, 100 );
			chams_translucency.handle( menu.data( ), xors( "translucency" ), &ctx.m_settings.player_chams_translucency, 0, 100 );
			chams_shine.handle( menu.data( ), xors( "shine" ), &ctx.m_settings.player_chams_shine, 0, 100 );
		}

		other.end( menu.data( ) );
	}

	flags.dropdown( menu.data( ) );
	skeleton_color.picker( menu.data( ) );
	ammo_color.picker( menu.data( ) );
	friendly_glow_color.picker( menu.data( ) );
	offscreen_arrows_friendly_color.picker( menu.data( ) );
	friendly_box_color.picker( menu.data( ) );
	enemy_glow_color.picker( menu.data( ) );
	offscreen_arrows_enemy_color.picker( menu.data( ) );
	enemy_box_color.picker( menu.data( ) );
	chams_enemy_color.picker( menu.data( ) );
	chams_friendly_color.picker( menu.data( ) );

	players_esp.dropdown( menu.data( ) );

	/*
	static bool nigga[3];

	static UI::c_checkbox enemies_only;
	static UI::c_dropdown box;
	static UI::c_checkbox dormant;
	static UI::c_checkbox name;
	static UI::c_checkbox health;
	static UI::c_checkbox weapon;
	static UI::c_checkbox ammo;

	static UI::c_checkbox flags;
	static UI::c_checkbox skeleton;
	static UI::c_checkbox chams;
	static UI::c_checkbox xqz;
	static UI::c_checkbox glow;
	static UI::c_slider   glow_alpha;
	static UI::c_checkbox skeet_arrows;

	menu.column_left( );
	{
		enemies_only.handle( menu.data( ), "only enemies", &ctx.m_settings.visuals_filter_enemies_only );
		box.handle( menu.data( ), "box", { "off", "bounding", "3d", "dynamic", }, &ctx.m_settings.visuals_box );
		dormant.handle( menu.data( ), "dormant", &nigga[0] );
		name.handle( menu.data( ), "name", &ctx.m_settings.visuals_name );
		health.handle( menu.data( ), "health", &ctx.m_settings.visuals_health );
		weapon.handle( menu.data( ), "weapon", &ctx.m_settings.visuals_weapon );
		ammo.handle( menu.data( ), "ammo", &nigga[1] );

		box.dropdown( menu.data( ) );
	}

	menu.column_right( );
	{
		flags.handle( menu.data( ), "info", &ctx.m_settings.visuals_items );
		skeleton.handle( menu.data( ), "skeleton", &ctx.m_settings.visuals_skeleton );
		chams.handle( menu.data( ), "chams", &ctx.m_settings.visuals_chams );
		xqz.handle( menu.data( ), "xqz", &ctx.m_settings.visuals_xqz );
		glow.handle( menu.data( ), "glow", &ctx.m_settings.visuals_glow );

		if ( ctx.m_settings.visuals_glow )
			glow_alpha.handle( menu.data( ), "alpha", &ctx.m_settings.visuals_glow_alpha, 0, 100 );

		skeet_arrows.handle( menu.data( ), "off screen indicator", &nigga[3] );
	}
	*/
}

void c_gui::tab_visuals( )
{
	static UI::c_groupbox esp;
	static UI::c_groupbox effects;
	static UI::c_groupbox other;

	// this should be a listbox thing, containing glow, box, chams
	static UI::c_checkbox enable;

	// multi dropdown with glow/chams/boxes?
	static UI::c_checkbox_smoll dropped_weapons;
	static UI::c_color_picker dropped_weapons_color;
	static UI::c_checkbox_smoll projectiles;
	static UI::c_color_picker projectiles_color;
	static UI::c_checkbox_smoll c4;
	static UI::c_color_picker c4_color;
	static UI::c_checkbox_smoll other_ents;
	static UI::c_color_picker other_ents_color;

	static UI::c_checkbox no_vis_recoil;
	static UI::c_checkbox no_smoke;
	static UI::c_checkbox no_flash;
	static UI::c_checkbox no_fog;
	static UI::c_checkbox no_scope;
	static UI::c_checkbox no_post_processing;
	static UI::c_dropdown render_mode;
	static UI::c_slider   asus_walls;
	static UI::c_checkbox world_color;
	static UI::c_color_picker world_color_val;
	static UI::c_slider   fov;
	static UI::c_checkbox fov_in_scope;
	static UI::c_checkbox spec_list;
	static UI::c_checkbox player_list;
	static UI::c_checkbox nade_prediction;
	static UI::c_checkbox recoil_crosshair;
	static UI::c_checkbox shot_tracers;
	static UI::c_multi_dropdown flags;
	static UI::c_key_bind thirdperson;

	static bool nigga[5];

	menu.column_left( );
	{
		esp.start( menu.data( ), xors( "esp" ) );
		{
			enable.handle( menu.data( ), xors( "enable" ), &ctx.m_settings.visuals_active );

			dropped_weapons.handle( menu.data( ), &ctx.m_settings.visuals_dropped_weapons );
			dropped_weapons_color.handle( menu.data( ), xors( "dropped weapons" ), ctx.m_settings.visuals_dropped_weapons_color( ).r_ptr( ), ctx.m_settings.visuals_dropped_weapons_color( ).g_ptr( ), ctx.m_settings.visuals_dropped_weapons_color( ).b_ptr( ) );

			projectiles.handle( menu.data( ), &ctx.m_settings.visuals_projectiles );
			projectiles_color.handle( menu.data( ), xors( "projectiles" ), ctx.m_settings.visuals_projectiles_color( ).r_ptr( ), ctx.m_settings.visuals_projectiles_color( ).g_ptr( ), ctx.m_settings.visuals_projectiles_color( ).b_ptr( ) );

			c4.handle( menu.data( ), &ctx.m_settings.visuals_bomb );
			c4_color.handle( menu.data( ), xors( "bomb" ), ctx.m_settings.visuals_bomb_color( ).r_ptr( ), ctx.m_settings.visuals_bomb_color( ).g_ptr( ), ctx.m_settings.visuals_bomb_color( ).b_ptr( ) );

			other_ents.handle( menu.data( ), &ctx.m_settings.visuals_other_ents );
			other_ents_color.handle( menu.data( ), xors( "other entities" ), ctx.m_settings.visuals_other_ents_color( ).r_ptr( ), ctx.m_settings.visuals_other_ents_color( ).g_ptr( ), ctx.m_settings.visuals_other_ents_color( ).b_ptr( ) );
		}

		esp.end( menu.data( ) );
	}

	menu.column_right( );
	{
		effects.start( menu.data( ), xors( "effects" ) );
		{
			no_vis_recoil.handle( menu.data( ), xors( "no visual recoil" ), &ctx.m_settings.visuals_no_recoil );
			no_smoke.handle( menu.data( ), xors( "no smoke" ), &ctx.m_settings.visuals_no_smoke );
			no_flash.handle( menu.data( ), xors( "no flash" ), &ctx.m_settings.visuals_no_flash );
			no_fog.handle( menu.data( ), xors( "no fog" ), &ctx.m_settings.visuals_no_fog );
			no_scope.handle( menu.data( ), xors( "no scope" ), &ctx.m_settings.visuals_no_scope );
			no_post_processing.handle( menu.data( ), xors( "no post processing" ), &ctx.m_settings.visuals_no_post_processing );
			render_mode.handle( menu.data( ), xors( "render mode" ), { xors( "off" ), xors( "full bright" ), xors( "low quality" ), xors( "night mode" ), }, &ctx.m_settings.visuals_render_mode );
			asus_walls.handle( menu.data( ), xors( "wall alpha" ), &ctx.m_settings.visuals_asus_walls, 0, 100 );
			world_color.handle( menu.data( ), xors( "custom world color" ), &ctx.m_settings.visuals_world_color );

			if ( ctx.m_settings.visuals_world_color )
				world_color_val.handle( menu.data( ), xors( "color" ), ctx.m_settings.visuals_world_color_val( ).r_ptr( ), ctx.m_settings.visuals_world_color_val( ).g_ptr( ), ctx.m_settings.visuals_world_color_val( ).b_ptr( ) );

			// nice vs settings whoever 
			fov.handle( menu.data( ), xors( "fov" ), &ctx.m_settings.visuals_view_fov, 0, 89,  xors( L"°" ) );

			if ( ctx.m_settings.visuals_view_fov > 0 )
				fov_in_scope.handle( menu.data( ), xors( "while scoped" ), &ctx.m_settings.visuals_view_fov_scope );
		}
		effects.end( menu.data( ) );

		other.start( menu.data( ), xors( "other" ) );
		{
			spec_list.handle( menu.data( ), xors( "spectator list" ), &ctx.m_settings.visuals_spec_list );
			nade_prediction.handle( menu.data( ), xors( "grenade prediction" ), &ctx.m_settings.visuals_nade_prediction );
			recoil_crosshair.handle( menu.data( ), xors( "recoil crosshair" ), &ctx.m_settings.visuals_recoil_crosshair );
			shot_tracers.handle( menu.data( ), xors( "shot tracers" ), &nigga[5] );
			flags.handle( menu.data( ), xors( "indicate flags" ), { { &ctx.m_settings.visuals_indicate_lby, xors( "lower body yaw" ) },{ &ctx.m_settings.visuals_indicate_lc, xors( "lag compensation" ) },{ &ctx.m_settings.visuals_indicate_ns, xors( "nospread" ) } } );
			thirdperson.handle( menu.data( ), xors( "third person" ), &ctx.m_settings.visuals_third_person );

			other.end( menu.data( ) );

			render_mode.dropdown( menu.data( ) );
			world_color_val.picker( menu.data( ) );
			flags.dropdown( menu.data( ) );
		}
	}
}

/*
void c_gui::tab_skins( )
{
	static UI::c_multi_dropdown multi_test;
	static UI::c_checkbox_smoll checkbox_smoll;
	static UI::c_color_picker colorpicker;

	static bool boolin_nigga[10];

	menu.column_left( );
	{
		multi_test.handle( menu.data( ), "TEST", 
		{ 
			{ &boolin_nigga[0], "abc" }, 
			{ &boolin_nigga[1], "def" }, 
			{ &boolin_nigga[2], "ghi" },
			{ &boolin_nigga[3], "jkl" },
			{ &boolin_nigga[4], "mno" },
			{ &boolin_nigga[5], "pqr" },
			{ &boolin_nigga[6], "stu" },
			{ &boolin_nigga[7], "vwx" },
			{ &boolin_nigga[8], "yza" },
			{ &boolin_nigga[9], "bcd" },
		} 
		);

		static bool test_bool{ };
		static uint8_t r = 255;
		static uint8_t g = 0;
		static uint8_t b = 0;
		static uint8_t a = 150;

		checkbox_smoll.handle( menu.data( ), &test_bool );
		colorpicker.handle( menu.data( ), "color test", &r, &g, &b, &a );

		colorpicker.picker( menu.data( ) );
		multi_test.dropdown( menu.data( ) );
	}
}
*/

void c_gui::tab_misc( )
{
	static UI::c_groupbox movement;
	static UI::c_groupbox matchmaking;
	static UI::c_groupbox misc;

	static UI::c_checkbox auto_bhop;
	static UI::c_checkbox auto_strafe;
	static UI::c_checkbox duck_in_air;
	static UI::c_key_bind circle_strafe;
	static UI::c_key_bind snake_strafe;

	static UI::c_checkbox auto_accept;
	static UI::c_checkbox rank_reveal;
	static UI::c_checkbox unlock_inventory;
	static UI::c_checkbox hitmarker;
	static UI::c_checkbox defuse;
	static UI::c_key_bind defuse_key;
	static UI::c_checkbox defuse_last_tick;
	static UI::c_checkbox ragdoll_force;
	static UI::c_checkbox ragdoll_gravity;
	static UI::c_slider   preserve_killfeed;
	static UI::c_slider   weapon_volume;
	static UI::c_slider   footstep_volume;
	static UI::c_slider   other_volume;

	static int inigga;
	
	menu.column_left( );
	{
		movement.start( menu.data( ), xors( "movement" ) );
		{
			auto_bhop.handle( menu.data( ), xors( "auto bhop" ), &ctx.m_settings.misc_bhop );
			auto_strafe.handle( menu.data( ), xors( "auto strafe" ), &ctx.m_settings.misc_auto_strafe );

			if ( ctx.m_settings.misc_auto_strafe )
			{
				circle_strafe.handle( menu.data( ), xors( "circle strafe" ), &ctx.m_settings.misc_autostrafe_circle );
				snake_strafe.handle( menu.data( ), xors( "snake strafe" ), &inigga );
			}

			movement.end( menu.data( ) );
		}
	}

	menu.column_right( );
	{
		matchmaking.start( menu.data( ), xors( "matchmaking" ) );
		{
			auto_accept.handle( menu.data( ), xors( "auto accept competitive matches" ), &ctx.m_settings.misc_auto_accept );
			rank_reveal.handle( menu.data( ), xors( "reveal competitive ranks" ), &ctx.m_settings.misc_rank_reveal );
			unlock_inventory.handle( menu.data( ), xors( "unlock inventory access" ), &ctx.m_settings.misc_unlock_inventory );
			matchmaking.end( menu.data( ) );
		}

		misc.start( menu.data( ), xors( "misc" ) );
		{
			hitmarker.handle( menu.data( ), xors( "hitmarker" ), &ctx.m_settings.misc_hitmarker );
			defuse.handle( menu.data( ), xors( "auto defuse" ), &ctx.m_settings.misc_auto_defuse );

			if ( ctx.m_settings.misc_auto_defuse )
			{
				defuse_key.handle( menu.data( ), "", &ctx.m_settings.misc_auto_defuse_key );
				defuse_last_tick.handle( menu.data( ), xors( "defuse on last tick" ), &ctx.m_settings.misc_auto_defuse_last_tick );
			}

			ragdoll_force.handle( menu.data( ), xors( "ragdoll force" ), &ctx.m_settings.misc_ragdoll_force );
			ragdoll_gravity.handle( menu.data( ), xors( "ragdoll gravity" ), &ctx.m_settings.misc_ragdoll_gravity );
			preserve_killfeed.handle( menu.data( ), xors( "preserve killfeed" ), &inigga, 0, 300,  xors( L"s" ) );
			weapon_volume.handle( menu.data( ), xors( "weapon volume" ), &ctx.m_settings.misc_weapon_volume, 0, 100,  xors( L"%%" ) );
			footstep_volume.handle( menu.data( ), xors( "footstep volume" ), &ctx.m_settings.misc_footstep_volume, 0, 100,  xors( L"%%" ) );
			other_volume.handle( menu.data( ), xors( "other volume" ), &ctx.m_settings.misc_other_volume, 0, 100,  xors( L"%%" ) );
			misc.end( menu.data( ) );
		}
	}
}

void c_gui::tab_config( )
{
	static UI::c_groupbox menu_setts;
	static UI::c_groupbox hack;

	static UI::c_color_picker menu_color;
	static UI::c_slider       menu_fade_speed;
	static UI::c_checkbox     rainbow_mode;

	static UI::c_dropdown restriction;
	// static UI::c_listbox cur_cfg;
	// static UI::c_textbox cfg_name;
	static UI::c_button   save;
	static UI::c_button   load;
	static UI::c_button   uninject;

	menu.column_left( );
	{
		menu_setts.start( menu.data( ), xors( "menu" ) );
		{
			menu_color.handle( menu.data( ), xors( "accent" ), ctx.m_settings.menu_color( ).r_ptr( ), ctx.m_settings.menu_color( ).g_ptr( ), ctx.m_settings.menu_color( ).b_ptr( ) );
			menu_fade_speed.handle( menu.data( ), xors( "fade speed" ), &ctx.m_settings.menu_fade_speed, 0, 10 );
			rainbow_mode.handle( menu.data( ), xors( "rainbow mode" ), &ctx.m_settings.menu_rainbow );

			menu_setts.end( menu.data( ) );
		}
	}

	menu.column_right( );
	{
		hack.start( menu.data( ), xors( "hack" ) );
		{
			restriction.handle( menu.data( ), xors( "restrictions" ), { xors( "none" ), xors( "anti untrusted" ), xors( "anti smac" ), }, &ctx.m_settings.hack_restrictions );
			save.handle( menu.data( ), xors( "save" ), [] { ctx.m_settings.save( "./onetap.cfg" ); } );
			load.handle( menu.data( ), xors( "load" ), [] { ctx.m_settings.load( "./onetap.cfg" ); } );
			uninject.handle( menu.data( ), xors( "unload" ), [] { ctx.m_panic = true; } );

			restriction.dropdown( menu.data( ) );

			hack.end( menu.data( ) );
		}
	}

	menu_color.picker( menu.data( ) );
}

void c_gui::draw( )
{
	ctx.m_menu_open = menu.is_open( );

	if ( !menu.handle( m_tabs, UI::UIcolor_t( ctx.m_settings.menu_color( ).r( ), ctx.m_settings.menu_color( ).g( ), ctx.m_settings.menu_color( ).b( ) ) ) )
		return;

	switch ( menu.m_current_tab )
	{
		case TABS_LEGIT:
			tab_legit( );
			break;
		case TABS_RAGE:
			// need to remove the tab entirely
			// have a vector of tabs, int, string template, easy
			if ( ctx.m_settings.hack_restrictions != 2 )
				tab_rage( );
			break;
		case TABS_PLAYERS:
			tab_players( );
			break;
		case TABS_VISUALS:
			tab_visuals( );
			break;
		case TABS_MISC:
			tab_misc( );
			break;		
		case TABS_CONFIG:
			tab_config( );
			break;
	}

	cursor.handle( );
}