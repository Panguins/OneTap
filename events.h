#pragma once

class c_event_listener : public IGameEventListener2
{
public:
	c_event_listener( ) 
	{

	}

	~c_event_listener( )
	{

	}

	void FireGameEvent( IGameEvent* event ) override
	{
		m_function( event );
	}

	int GetEventDebugID( ) override
	{
		return 0x2A;
	}

	__forceinline void listen( IGameEventManager2* event_manager, const char* event_name, void( *function )( IGameEvent* ) )
	{
		m_function = function;
		event_manager->AddListener( this, event_name, false );
	}

	__forceinline void stop( IGameEventManager2* event_manager )
	{
		event_manager->RemoveListener( this );
	}
private:
	void( *m_function )( IGameEvent* ) { };
};

class c_event_manager
{
	c_event_listener player_hurt_listener{ };
	c_event_listener player_death_listener{ };
	c_event_listener player_disconnect_listener{ };
	c_event_listener player_footstep_listener{ };
	c_event_listener item_purchase_listener{ };
	c_event_listener bullet_impact_listener{ };
	c_event_listener weapon_fire_listener{ };
	c_event_listener round_prestart_listener{ };
	c_event_listener round_start_listener{ };
	c_event_listener round_freeze_end_listener{ };

	static void player_hurt( IGameEvent* _event );
	static void player_death( IGameEvent* _event );
	static void player_disconnect( IGameEvent* _event );
	static void player_footstep( IGameEvent* _event );
	static void item_purchase( IGameEvent* _event );
	static void bullet_impact( IGameEvent* _event );
	static void weapon_fire( IGameEvent* _event );
	static void round_prestart( IGameEvent* _event );
	static void round_start( IGameEvent* _event );
	static void round_freeze_end( IGameEvent* _event );
public:
	void start( IGameEventManager2* event_manager )
	{
		player_hurt_listener.listen( event_manager,       xors("player_hurt"),       &player_hurt );
		player_death_listener.listen( event_manager,      xors("player_death"),      &player_death );
		player_disconnect_listener.listen( event_manager, xors("player_disconnect"), &player_disconnect );
		player_footstep_listener.listen( event_manager,   xors("player_footstep"),   &player_footstep );
		item_purchase_listener.listen( event_manager,     xors("item_purchase"),     &item_purchase );
		bullet_impact_listener.listen( event_manager,     xors("bullet_impact"),     &bullet_impact );
		weapon_fire_listener.listen( event_manager,       xors("weapon_fire"),       &weapon_fire );
		round_prestart_listener.listen( event_manager,    xors("round_prestart"),    &round_prestart );
		round_start_listener.listen( event_manager,       xors("round_start"),       &round_start );
		round_freeze_end_listener.listen( event_manager,  xors("round_freeze_end"),  &round_freeze_end );
	}

	void stop ( IGameEventManager2* event_manager )
	{
		player_hurt_listener.stop( event_manager );
		player_death_listener.stop( event_manager );
		player_disconnect_listener.stop( event_manager );
		player_footstep_listener.stop( event_manager );
		item_purchase_listener.stop( event_manager );
		bullet_impact_listener.stop( event_manager );
		weapon_fire_listener.stop( event_manager );
		round_prestart_listener.stop( event_manager );
		round_start_listener.stop( event_manager );
		round_freeze_end_listener.stop( event_manager );
	}
}; extern c_event_manager event_manager;