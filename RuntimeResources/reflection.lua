testString = "lua string"

result = 0

P = {Clan=0}

G = {Story_State=0, Read_Email=0, Werewolf_Quest=0, Tommy_Disgusted=0,Tommy_Review=0,Tommy_Hostile=0,Prince_Mercurio=0,Mercurio_Dead=0,
     Mitnick_Quest=0,Shubs_Act=0,Therese_Dead=0,Jeanette_Seduce=0,Jeanette_Boink=0,Gary_Voerman=0,Gary_Damsel=0,Velvet_Poster=0,
     Velvet_Like=0,Velvet_Pissed=0,Velvet_Quest=0,Romero_Boink=0,Mercurio_times_talked=0,Mercurio_Know=0,Mercurio_Combat=0,Mercurio_Asked=0,Mercurio_Get=0,
	 Mercurio_Aid=0}
     
sm_hideout_items = {anaconda=665,lockpicks=1}
sm_hideout_doors = {door507=1}
sm_hideout_loot = {drawer_lootbox={id="ammo",amount=333}}

inventory = {anaconda=0}

function Eval(expr)
    f = load("result = " .. expr)
    f()
end

function Do(expr)
    f = load(expr)
    f()
end

function GetQuest(quest_id)
    return GetQuestImpl(quest_id)
end

function SetQuest(quest_id, stage_id)
    SetQuestImpl(quest_id, stage_id)
    return 1
end

function GetQuestValue(quest_id, var_name)
    return GetQuestValueImpl(quest_id, var_name)
end

function SetQuestValue(quest_id, var_name, value)
    SetQuestValueImpl(quest_id, var_name, value)
    return 1
end

function GetGlobalValue(var_name)
    return GetGlobalValueImpl(var_name)
end

function SetGlobalValue(var_name, value)
    SetGlobalValueImpl(var_name, value)
    return 1
end

function HasItem(item_id)
    return HasItemImpl(item_id)
end

function GiveExp(amount)
    return GiveExpImpl(amount)
end

function GiveMoney(amount)
    return GiveMoneyImpl(amount)
end

function bool_to_number(value)
  return value and 1 or 0
end