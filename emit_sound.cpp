#include "context.h"

decltype( hooked::original::o_emit_sound ) hooked::original::o_emit_sound;

void __fastcall hooked::emit_sound(
	void* ecx, 
	void* edx,
	void* filter,
	int index,
	int sprite_channel,
	const char* sound_entry,
	uint32_t sound_entry_hash,
	const char* name, 
	float volume, 
	float attenuation, 
	int seed,
	int flags, 
	int pitch_scale,
	const vec3* origin,
	const vec3* dir, 
	int idk, 
	bool update_positions,
	float sound_play_time, 
	int emitting_ent
)
{
	if ( ctx.m_settings.misc_footstep_volume < 100 )
	{
		const float volume_scale = static_cast< float >( ctx.m_settings.misc_footstep_volume ) / 100;

		if ( strstr( name, xors( "step" ) ) )
		{
			// allow for better echolocation
			volume = std::clamp( volume * volume_scale, 0.0f, 1.0f );
		}
	}

	if ( ctx.m_settings.misc_weapon_volume < 100 )
	{
		const float volume_scale = static_cast< float >( ctx.m_settings.misc_weapon_volume ) / 100;

		if ( strstr( name, xors( "weapon" ) ) )
		{
			// we don't need that racket.
			volume = std::clamp( volume * volume_scale, 0.0f, 1.0f );
		}
	}

	if ( ctx.m_settings.misc_other_volume < 100 )
	{
		const float volume_scale = static_cast< float >( ctx.m_settings.misc_other_volume ) / 100;

		if ( !strstr( name, xors( "weapon" ) ) && !strstr( name, xors( "step" ) ) )
		{
			// ...
			volume = std::clamp( volume * volume_scale, 0.0f, 1.0f );
		}
	}

	hooked::original::o_emit_sound( ecx, edx, filter, index, sprite_channel, sound_entry, sound_entry_hash, name, volume, attenuation, seed, flags, pitch_scale, origin, dir, idk, update_positions, sound_play_time, emitting_ent );
}