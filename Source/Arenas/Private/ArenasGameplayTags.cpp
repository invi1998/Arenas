// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasGameplayTags.h"

namespace ArenasGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_None, "InputTag.None", "无输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_BasicAttack, "InputTag.BasicAttack", "基础攻击输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability1, "InputTag.Ability1", "技能1输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability2, "InputTag.Ability2", "技能2输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability3, "InputTag.Ability3", "技能3输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability4, "InputTag.Ability4", "技能4输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability5, "InputTag.Ability5", "技能5输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability6, "InputTag.Ability6", "技能6输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Confirm, "InputTag.Confirm", "确认输入" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Cancel, "InputTag.Cancel", "取消输入" );
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack, "Ability.BasicAttack", "基础攻击技能" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Pressed, "Ability.BasicAttack.Pressed", "基础攻击技能-按下" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Uppercut, "Ability.Uppercut", "上勾拳技能" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Passive_Launch_Activated, "Ability.Passive.Launch.Activated", "被动技能-发射器-激活" );

	/***** Event Tags *****/
	// Event-Combo
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Change_1, "Event.Ability.Combo.Change.1", "连招技能切换到连招1" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Change_2, "Event.Ability.Combo.Change.2", "连招技能切换到连招2" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Change_3, "Event.Ability.Combo.Change.3", "连招技能切换到连招3" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Change_4, "Event.Ability.Combo.Change.4", "连招技能切换到连招4" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Change_End, "Event.Ability.Combo.Change.End", "连招技能切换到连招结束" );

	// Event-Uppercut
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Uppercut_Launch, "Event.Ability.Uppercut.Launch", "上勾拳技能起跳" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Uppercut_Land, "Event.Ability.Uppercut.Land", "上勾拳技能落地" );
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Ability_Combo_Damage, "Event.Ability.Combo.Damage", "连招技能伤害" );
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_BaseDamage, "SetByCaller.BaseDamage", "基础伤害" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_ComboIndex, "SetByCaller.ComboIndex", "连招索引" );

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Dead, "Status.Dead", "死亡状态" );
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Stun, "Status.Stun", "眩晕状态" );

	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Ability_Uppercut, "Cooldown.Ability.Uppercut", "上勾拳技能冷却" );
	
}



