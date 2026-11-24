# Gameplay Inventory Framework — Complete User Guide

Welcome to the **Gameplay Inventory Framework** user guide. This document provides step-by-step instructions for game designers and C++/Blueprint programmers to configure inventory systems, author data-driven items, manage equipment, handle multiplayer replication, spawn world ground loot, attach visuals to character sockets, and optimize performance in Unreal Engine 5.

---

## Table of Contents

1. [Quick Start: Setting Up Player Inventories](#1-quick-start-setting-up-player-inventories)
2. [Authoring Items (Primary Data Assets & Fragments)](#2-authoring-items-primary-data-assets--fragments)
3. [Configuring Modular Item Fragments](#3-configuring-modular-item-fragments)
   - [UI Info Fragment](#ui-info-fragment)
   - [Item Stats Fragment](#item-stats-fragment)
   - [Stacking Fragment](#stacking-fragment)
   - [Attachment Fragment](#attachment-fragment)
   - [Montage Fragment](#montage-fragment)
   - [Audio, VFX, Mesh, Material & Texture Fragments](#audio-vfx-mesh-material--texture-fragments)
4. [Runtime Instances & Dynamic Parameter Bag](#4-runtime-instances--dynamic-parameter-bag)
5. [Multi-Inventory Orchestration (Manager Component)](#5-multi-inventory-orchestration-manager-component)
6. [World Ground Loot & Physical Item Drops](#6-world-ground-loot--physical-item-drops)
7. [Visual Socket Attachment & Weapon Snapping](#7-visual-socket-attachment--weapon-snapping)
8. [Playing Item Animations (Async Montage Action)](#8-playing-item-animations-async-montage-action)
9. [Mass Entity / ECS High-Volume Ground Loot](#9-mass-entity--ecs-high-volume-ground-loot)
10. [Subsystems & Memory Optimization](#10-subsystems--memory-optimization)
11. [Blueprint API Quick Reference](#11-blueprint-api-quick-reference)
12. [Troubleshooting & FAQ](#12-troubleshooting--faq)

---

## 1. Quick Start: Setting Up Player Inventories

To give an actor (e.g. Character, PlayerController, Chest, or Vehicle) an inventory container:

1. Open your Actor Blueprint (e.g., `BP_HeroCharacter` or `BP_TreasureChest`).
2. In the **Components** panel, click **Add** and select **Inventory Component** (`UInventoryComponent`).
3. In the Details panel, configure container parameters:
   - **Capacity Limits**: Set limits for slots (`Inventory.Capacity.Slots`), weight (`Inventory.Capacity.Weight`), or custom limit tags.
   - **Allowed Item Filter**: (Optional) Assign an `FGameplayTagQuery` to restrict what this container accepts (e.g. only allowing `Item.Type.Equipment` in an armor bag).
   - **Replicate To Owner Only**: Keep enabled (`true`) for player character bags to conserve bandwidth; disable (`false`) for public loot chests.
4. (Recommended) Add the **Inventory Interface** (`IInventoryInterface`) in **Class Settings** and implement `GetInventoryComponent` returning your component. This allows other systems to interact with the actor without hard casts.

```
[ Character / Chest Actor Blueprint ]
  ├── Add Component: UInventoryComponent
  └── Implement Interface: IInventoryInterface -> returns InventoryComponent
```

---

## 2. Authoring Items (Primary Data Assets & Fragments)

Items are authored as static, lightweight **Primary Data Assets**:

1. In the Content Browser, right-click and choose **Miscellaneous -> Data Asset**.
2. Select **InventoryItemDefinition** (or your derived C++/Blueprint subclass) and name it (e.g., `DA_Item_Broadsword`).
3. Configure the base properties:
   - **Item Identifier Tag**: Unique hierarchical tag (e.g., `Item.Type.Weapon.Sword`).
   - **Max Stack Count**: Maximum items per stack (defaults to 1 for weapons/armor, higher for consumables/resources).
   - **Fragments**: Click **+** to add polymorphic property modules.

---

## 3. Configuring Modular Item Fragments

### UI Info Fragment (`UFragment_UIInfo`)
Defines how the item appears in inventory screens, tooltips, and action bars:
- **Display Name**: Localized text name.
- **Description**: Detailed description text.
- **Icon**: Soft object pointer (`TSoftObjectPtr<UTexture2D>`) loaded asynchronously to prevent UI hitches.
- **Rarity Tag**: Gameplay tag indicating rarity tier (`Item.Rarity.Legendary`).

### Item Stats Fragment (`UFragment_ItemStats`)
Defines static baseline attributes and gear stats:
- **Weight**: Item unit weight for encumbrance calculations.
- **Item Tier**: Integer level/tier.
- **Base Stats Array**: Pairs of `FGameplayTag` and `float` values (e.g., `Item.Stat.Durability: 100`, `Item.Stat.Damage: 45`).

### Stacking Fragment (`UFragment_Stacking`)
Defines stack consolidation behavior:
- **Max Stack Count**: Maximum capacity per slot (overrides definition default).
- **bCanSplit**: Whether the player can split this stack into smaller stacks.
- **bCanCombine**: Whether stacks of this item can be merged together.

### Attachment Fragment (`UFragment_Attachment`)
Defines physical character weapon rail or socket attachment:
- **Socket Name**: Target bone/socket on skeleton (e.g., `WeaponSocket_R`, `BackpackSocket`).
- **bSpawnAsActor**: When `true`, spawns a dedicated `AActor` class. When `false`, attaches a direct component.
- **bIsSkeletalMesh**: (When `bSpawnAsActor` is false) Toggles between `USkeletalMeshComponent` and `UStaticMeshComponent`.
- **SkeletalMesh / StaticMesh / SpawnedActorClass**: Soft references loaded on-demand.
- **Relative Transform**: Position, rotation, and scale offsets relative to the target socket.

### Montage Fragment (`UFragment_Montage`)
Defines item-driven character animations:
- **Montages Array**: List of `FItemMontageEntry` mappings:
  - **Montage Tag**: Action tag (e.g., `Animation.Montage.Attack`, `Animation.Montage.Reload`).
  - **Anim Montage**: Soft pointer to `UAnimMontage`.
  - **Play Rate**: Speed multiplier.
  - **Starting Section**: Optional montage section name.

### Audio, VFX, Mesh, Material & Texture Fragments
- **`UFragment_Audio`**: Sound cues and metasounds mapped by role tags (`Audio.Item.Fire`, `Audio.Item.Reload`).
- **`UFragment_VFX`**: Niagara systems and cascade effects mapped by role tags (`VFX.Item.MuzzleFlash`).
- **`UFragment_Mesh`**: Static or skeletal mesh representations for 3D inspection.
- **`UFragment_Material`**: Dynamic material overrides and material instances.
- **`UFragment_Texture`**: Extra texture maps (normal, roughness, detail overlays).

---

## 4. Runtime Instances & Dynamic Parameter Bag

When an item enters a container or is modified in the world, it is encapsulated in a `UInventoryItemInstance`:

- **Static Definitions Remain Untouched**: Hundreds of swords share a single `DA_Item_Broadsword` DataAsset.
- **Isolated Parameter Bag**: Modify per-instance state without data contamination:
  ```blueprint
  // Reading dynamic ammo in Blueprint:
  float CurrentAmmo = InventoryFunctionLibrary::GetItemDynamicFloat(ItemInstance, "Item.Stat.Ammo", 0.0);

  // Authoritative server deduction:
  InventoryFunctionLibrary::SetItemDynamicFloat(ItemInstance, "Item.Stat.Ammo", CurrentAmmo - 1.0);
  ```
- **Dynamic Gameplay Tags**: Add temporary statuses (e.g., `Item.State.Jammed`, `Item.Modifier.PoisonCoated`) via `AddDynamicTag` and `RemoveDynamicTag`.

---

## 5. Multi-Inventory Orchestration (Manager Component)

For players with multiple containers (Equipped Gear, Backpack, Hotbar, Bank Vault):

1. Add **Inventory Manager Component** (`UInventoryManagerComponent`) to the player character or controller.
2. In `BeginPlay`, register containers with distinct tags:
   ```cpp
   ManagerComp->RegisterInventory(BackpackComp, MGITags::Inventory_Type_Main);
   ManagerComp->RegisterInventory(EquipmentComp, MGITags::Inventory_Type_Equipment);
   ManagerComp->RegisterInventory(BankVaultComp, MGITags::Inventory_Type_Bank);
   ```
3. Execute transactional operations:
   - **`TransferItem`**: Moves items between two inventories with automatic capacity checks.
   - **`SwapSlots`**: Exchanges slots between containers or within the same container.
   - **`SplitStack`**: Splits a quantity from one slot into another.
   - **`AutoRouteItem`**: Automatically routes an item into the first container that accepts it.
   - **`LootAll`**: Bulk-transfers all items from a chest or corpse into the player's bags.

---

## 6. World Ground Loot & Physical Item Drops

To place loot in a level or drop items from an inventory:

1. Create a loot drop actor Blueprint (or use a bare `AActor`).
2. Add a `USceneComponent` as the root component (with an optional mesh).
3. Add a **Item Component** (`UItemComponent`).
4. Initialize the payload:
   - In editor: Set `Payload.ItemDef` (or `ItemDefClass`) and `StackCount`.
   - At runtime: Call `InitFromInstance(DroppedInstance, Count)` or `InitFromDefinition(ItemDef, Count)`.
5. Interaction & Pickup:
   ```blueprint
   // When player presses 'E' or overlaps trigger:
   ItemComponent->ExecutePickup(PlayerInventoryComponent);
   ```
   - Enforces server authority.
   - Automatically checks target capacity and tag filters.
   - Fires 3-stage lifecycle callbacks (`OnPreItemAdded`, `OnItemAdded`, `OnPostItemAdded`).
   - Automatically hides the actor and disables collision upon success.
   - Prevents duplicate looting idempotently.

---

## 7. Visual Socket Attachment & Weapon Snapping

Attach equipped weapons, armor, or tools to character meshes with zero boilerplate:

```blueprint
// Attach weapon visual using item fragment rules:
USceneComponent* AttachedVisual = InventoryFunctionLibrary::AttachItemVisual(CharacterMesh, WeaponItemInstance);

// Detach weapon when unequipped:
InventoryFunctionLibrary::DetachItemVisual(CharacterMesh, "RightHandSocket");
```

- Honors `bSpawnAsActor`: Spawns and attaches an actor, or attaches a mesh component directly.
- Automatically applies relative transform offsets configured by designers in the attachment fragment.

---

## 8. Playing Item Animations (Async Montage Action)

Play item-specific attack, reload, or equip montages with latent execution pins:

1. In your Character Blueprint, add the **Play Item Montage Async** node.
2. Connect `TargetCharacter`, `ItemInstance`, and the desired `MontageTag` (e.g. `Animation.Montage.Attack`).
3. Connect logic to the output execution pins:
   - **On Completed**: Normal animation finish.
   - **On Blend Out**: Montage starts blending out (ideal for chaining combos).
   - **On Interrupted**: Player dodged or was hit.
   - **On Failed**: Missing montage asset or invalid animation instance.

---

## 9. Mass Entity / ECS High-Volume Ground Loot

For games with thousands of ground loot items (Battle Royale supply drops, ARPG dungeon loot explosions):

- Use `FMassItemDropFragment` and `FMassItemDropTag` in `Public/Mass/MassItemFragments.h`.
- `FMassItemDropTag` is a zero-byte archetype marker tag for instant Mass query filtering without memory overhead.
- Mass processors can update physics, culling, and proximity queries for tens of thousands of loot items at 60+ FPS.

---

## 10. Subsystems & Memory Optimization

- **`UItemRegistrySubsystem` (GameInstance)**:
  - Call `RequestAsyncLoadItemAssets(ItemDef, Callback)` to stream icons, meshes, and montages before opening menus.
  - Call `ReleaseItemAssets(ItemDef)` when leaving zones or closing UI to free memory.
- **`UWorldItemSubsystem` (World)**:
  - Tracks all registered `UItemComponent` actors in the world.
  - Query nearby items in a radius via `FindNearbyItems(Center, Radius, FilterQuery)` with zero tick overhead.

---

## 11. Blueprint API Quick Reference

| Function / Node | Target | Purpose |
| :--- | :--- | :--- |
| `TryAddItem` | `UInventoryComponent` | Adds items by DataAsset definition with automatic stack consolidation. |
| `TryAddItemInstance` | `UInventoryComponent` | Adds a specific dynamic item instance. |
| `RemoveItem` | `UInventoryComponent` | Removes item by definition. |
| `RemoveItemInstance` | `UInventoryComponent` | Removes a specific instance. |
| `ExecutePickup` | `UItemComponent` | Authoritatively loots a world item into target inventory. |
| `ExtractItem` | `UItemComponent` | Extracts instance and count from a world item. |
| `TransferItem` | `UInventoryManagerComponent` | Atomically transfers an item between two inventories. |
| `SwapSlots` | `UInventoryManagerComponent` | Swaps items between two slot indices. |
| `SplitStack` | `UInventoryManagerComponent` | Splits a stack across slots. |
| `AttachItemVisual` | `UInventoryFunctionLibrary` | Attaches visual mesh or actor to a scene component socket. |
| `DetachItemVisual` | `UInventoryFunctionLibrary` | Detaches all attached visuals from a socket. |
| `PlayItemMontageAsync` | Async Node | Plays item montage with `OnCompleted`, `OnBlendOut`, `OnFailed` pins. |

---

## 12. Troubleshooting & FAQ

**Q: Items are not stacking when added.**  
*A: Verify that the item definition has `MaxStackCount > 1` (or includes a `UFragment_Stacking`). Also ensure items share the same definition asset and matching dynamic tags.*

**Q: World item is looted but still visible.**  
*A: `ExecutePickup` automatically calls `SetActorHiddenInGame(true)` and `SetActorEnableCollision(false)`. If you custom-spawn sub-components, ensure they are attached to the root component or override `OnPostItemAdded`.*

**Q: Weapon attaches to the character's feet instead of the hand.**  
*A: Verify that `SocketName` in `UFragment_Attachment` matches the exact socket name on your character's skeletal mesh, and check `RelativeTransform` for unexpected translation offsets.*

**Q: Multiplayer clients don't see items in inventory.**  
*A: Ensure `bReplicateToOwnerOnly` is set appropriately. If you want other players to inspect another player's inventory, set `bReplicateToOwnerOnly = false` on that component.*
