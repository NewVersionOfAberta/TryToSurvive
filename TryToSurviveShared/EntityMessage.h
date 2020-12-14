#pragma once
enum class EntityMessage {
	Move, Is_Moving, Frame_Change, State_Changed,
	Direction_Changed, Switch_State, Attack, Bullet_add,
	Being_Attacked, Hurt, Die, Respawn, Removed_Entity
};