#include "Types/InventoryNativeTags.h"

namespace MGITags
{
	// Fragment Category Root
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment, "Item.Fragment", "Root tag for all item fragments");

	// Fragments
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_UI, "Item.Fragment.UI", "UI display metadata fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Stats, "Item.Fragment.Stats", "Item stats and weight fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Stacking, "Item.Fragment.Stacking", "Item stacking rules fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Montage, "Item.Fragment.Montage", "Item animation montages fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Audio, "Item.Fragment.Audio", "Item audio cues fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_VFX, "Item.Fragment.VFX", "Item visual effects fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Mesh, "Item.Fragment.Mesh", "Item static and skeletal mesh visual fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Material, "Item.Fragment.Material", "Item material overrides fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Texture, "Item.Fragment.Texture", "Item texture assets fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Attachment, "Item.Fragment.Attachment", "Item visual socket attachment fragment tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Fragment_Container, "Item.Fragment.Container", "Item nested container inventory fragment tag");

	// Animation Montages
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Equip, "Animation.Montage.Equip", "Equip animation montage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Unequip, "Animation.Montage.Unequip", "Unequip animation montage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Attack, "Animation.Montage.Attack", "Primary attack animation montage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Reload, "Animation.Montage.Reload", "Reload animation montage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Inspect, "Animation.Montage.Inspect", "Inspect animation montage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Animation_Montage_Use, "Animation.Montage.Use", "Use or consume animation montage");

	// Stats
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_Durability, "Item.Stat.Durability", "Current durability of the item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_MaxDurability, "Item.Stat.MaxDurability", "Maximum durability of the item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_Ammo, "Item.Stat.Ammo", "Current ammo count in magazine");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_Ammo_Reserve, "Item.Stat.Ammo.Reserve", "Reserve ammo count");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_MaxAmmo, "Item.Stat.MaxAmmo", "Maximum ammo capacity of magazine");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Stat_Weight, "Item.Stat.Weight", "Weight of the item");

	// Dynamic Parameters & Mods
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Param_IsSilenced, "Item.Param.IsSilenced", "Whether item has suppressor/silencer equipped");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Param_Crafter, "Item.Param.Crafter", "Name of player who crafted this item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Param_Attachments, "Item.Param.Attachments", "Installed attachment mod tags");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Mod_Scope_ACOG, "Item.Mod.Scope.ACOG", "ACOG Optical Scope");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Mod_Barrel_Suppressor, "Item.Mod.Barrel.Suppressor", "Tactical Barrel Suppressor");

	// Audio & VFX Roles
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Audio_Item_Fire, "Audio.Item.Fire", "Primary weapon fire sound");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(VFX_Item_MuzzleFlash, "VFX.Item.MuzzleFlash", "Muzzle flash particle effect");

	// States & Modifiers
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_State_Jammed, "Item.State.Jammed", "Weapon is jammed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_State_Broken, "Item.State.Broken", "Item is completely broken");

	// Item Types
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Weapon, "Item.Type.Weapon", "Root weapon item category");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Weapon_Sword, "Item.Type.Weapon.Sword", "One-handed sword weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Consumable_Potion, "Item.Type.Consumable.Potion", "Consumable potion");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Herb, "Item.Type.Herb", "Herb or alchemical ingredient");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Resource, "Item.Type.Resource", "Crafting resource or material");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Currency_GoldCoin, "Item.Type.Currency.GoldCoin", "Gold coins currency");

	// Inventory Container Types
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Type_Main, "Inventory.Type.Main", "Main player backpack container");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Type_Equipment, "Inventory.Type.Equipment", "Equipped items container");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Type_Bank, "Inventory.Type.Bank", "Bank storage container");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Type_Chest, "Inventory.Type.Chest", "World chest container");

	// Inventory Capacity Limits
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Capacity_Slots, "Inventory.Capacity.Slots", "Maximum distinct slots in inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Capacity_Weight, "Inventory.Capacity.Weight", "Maximum carry weight of inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Capacity_Volume, "Inventory.Capacity.Volume", "Maximum volume capacity of inventory");

	// Attachment Sockets
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attachment_Socket_Back, "Attachment.Socket.Back", "Back spine socket");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attachment_Socket_Hand_Right, "Attachment.Socket.Hand.Right", "Right hand weapon socket");
}
