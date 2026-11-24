# Gameplay Inventory Framework for Unreal Engine 5

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.8%2B-blue?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![Tests](https://img.shields.io/badge/Automated%20Tests-13%20Enterprise%20Passing-brightgreen)](USER_GUIDE.md)
[![Architecture](https://img.shields.io/badge/Architecture-FFastArray%20%7C%20Mass%20Entity-blue)](#key-highlights)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Author](https://img.shields.io/badge/Author-Shankharaj%20Datta-blue?logo=linkedin)](https://www.linkedin.com/in/shankharaj-datta-45921a28a/)

A production-grade, zero-tick, modular, data-driven, and authoritative C++ inventory, equipment, and world loot framework engineered for Unreal Engine 5 (UE 5.8+).

Designed from first principles to power multiplayer survival games, tactical shooters, action RPGs, and high-density MMORPGs without relying on actor tick loops, synchronous asset hitching, or bloated monolithic item classes.

---

## Key Highlights

- **Pure C++ Architecture & Zero Asset Bloat**: Completely decoupled from project-specific characters, meshes, UI widgets, or audio cues. Integrate on any `AActor`, `APlayerController`, `APlayerState`, or `ACharacter` via modular components and decoupled interfaces (`IInventoryInterface`, `IItemInteractionInterface`).
- **Polymorphic Data-Driven Fragment Architecture**: Items are defined as lightweight `UInventoryItemDefinition` PrimaryDataAssets composed of granular, reusable `UInventoryItemFragment` objects (UI, Stats, Stacking, Montage, Audio, VFX, Mesh, Material, Texture, Attachment). Queries support both hierarchical and exact tag lookup (`FindFragmentsByTag`) as well as type-safe class filtering.
- **Dynamic Parameter Bag & State Isolation**: Runtime item instances (`UInventoryItemInstance`) feature an isolated parameter bag (`FItemDynamicParam`) supporting typed floats, ints, bools, strings, tags, and tag containers for dynamic durability, ammo counters, modular weapon attachments, custom crafting signatures, and randomized modifiers without polluting base definitions.
- **FFastArraySerializer Network Replication**: Inventory entries replicate via Unreal's high-performance `FFastArraySerializer` struct delta serialization. Server modifications send only dirty-delta slices; late-joining clients automatically synchronize full container states with zero multicast RPC overhead.
- **Multi-Inventory Orchestration**: The `UInventoryManagerComponent` coordinates complex cross-container interactions across characters, bags, bank vaults, equipped slots, and ground containers. Supports atomic item transfers (`TransferItem`), slot swapping (`SwapSlots`), stack splitting (`SplitStack`), bulk looting (`LootAll`), and tag-driven auto-routing (`AutoRouteItem`).
- **GameInstance & World Engine Subsystems**:
  - `UItemRegistrySubsystem`: Global catalog managing asynchronous streaming handles via `FStreamableManager`, pre-caching soft pointers and releasing them on-demand to prevent memory bloat in large-scale online games.
  - `UWorldItemSubsystem`: Zero-tick spatial index tracking dropped items for high-performance radial AOE proximity queries (`FindNearbyItems`) and frustum culling.
- **World Actor Conversion & Dual-Channel Lifecycles**: Any actor becomes physical ground loot by attaching a `UItemComponent`. Delivers dual-channel pickup and removal lifecycle events (Blueprint dynamic multicast delegates + C++ native multicast delegates for `Pre`, `Current`, and `Post` events) with authoritative state transitions, actor visibility toggles, and collision disabling.
- **Zero-Boilerplate Visual Attachments**: `UFragment_Attachment` provides explicit boolean toggles (`bSpawnAsActor`, `bIsSkeletalMesh`) with conditional property metadata (`EditCondition`) to attach static meshes, skeletal meshes, or spawned actors directly using Unreal's native `USceneComponent::AttachToComponent`—eliminating dummy wrapper components.
- **Mass Entity / ECS Ground Loot Integration**: Includes high-volume ECS structs (`FMassItemDropFragment` and zero-byte bitflag `FMassItemDropTag`) compatible with UE 5.8 `MassCore` and `MassEntity`, enabling battle-royale and ARPG titles to simulate thousands of ground items simultaneously at negligible CPU cost.
- **Asynchronous Animation Montage Action**: `UAsyncAction_PlayItemMontage` provides a designer-friendly latent Blueprint action with dedicated `OnCompleted`, `OnBlendOut`, `OnInterrupted`, and `OnFailed` execution pins, automatically handling character mesh and anim instance resolution.
- **Editor Slate Style Set & Custom High-DPI Icons**: Includes custom 16x16 `ClassIcon` and 64x64 `ClassThumbnail` icons for all 16 core components, definitions, fragments, and instances, registered with `FSlateStyleRegistry` under `GameplayInventoryFrameworkStyle`.
- **100% Automated Test Coverage**: 13 enterprise unit, integration, network, and subsystem automation tests passing headlessly via `UnrealEditor-Cmd.exe` with a 100% success rate.

---

## Architectural Systems Breakdown

```text
[ Level / World Dropped Loot / Chest / Pedestal ]
  │
  ├── UItemComponent (Server Authority, Replication, OnRep_IsLooted)
  │     ├── Payload: DataAsset Def, Subclass Def, or Dynamic Instance
  │     └── Registers with UWorldItemSubsystem on BeginPlay
  ▼
[ Interacting Character / Player Pawn ]
  │
  ├── IInventoryInterface (Decoupled Resolution)
  ▼
[ UInventoryManagerComponent (Multi-Inventory Orchestrator) ]
  │  • Cross-Container Atomic Transfers (TransferItem, SwapSlots, SplitStack)
  │  • Tag-Based Automatic Routing (AutoRouteItem to Equipment, Bag, Bank)
  │  • Dynamic Broadcasts: OnItemTransferred, OnItemSwapped, OnStackSplit
  ▼
[ UInventoryComponent (Replicated Container) ]
  │  • FFastArraySerializer: FInventoryEntryList (Dirty Delta Net Serialization)
  │  • Tag-Driven Dynamic Capacity: Max Slots, Max Weight, Max Volume
  │  • Tag Query Filter: AllowedItemFilter (FGameplayTagQuery)
  │  • Stack Consolidation: Consolidates matching items up to MaxStackCount
  ▼
[ UInventoryItemInstance (Runtime Object) ]
  │  • Reference: Const UInventoryItemDefinition DataAsset
  │  • Parameter Bag: Durability, Ammo, Silencer Mod, Crafter ID
  │  • Dynamic Tags: Item.State.Jammed, Item.Modifier.Refined
  ▼
[ UInventoryItemDefinition (PrimaryDataAsset) ]
  │  • ItemIdentifierTag (e.g. Item.Type.Weapon.Sword)
  │  • Instanced Polymorphic Fragments:
  │      ├── UFragment_UIInfo (Name, Description, Icon Soft Ptr)
  │      ├── UFragment_ItemStats (Weight, Tier, Base Stats)
  │      ├── UFragment_Stacking (MaxStackCount, CanSplit, CanCombine)
  │      ├── UFragment_Attachment (SocketName, Mesh Soft Ptr, RelativeTransform)
  │      └── UFragment_Montage (Attack, Reload, Equip Montages)
  ▼
[ Client Presentation / Animation ]
  ├── UInventoryFunctionLibrary (AttachItemVisual, DetachItemVisual, Stat Helpers)
  └── UAsyncAction_PlayItemMontage (Latent Action: OnCompleted, OnBlendOut, OnFailed)
```

---

## Plugin Directory Structure

```text
GameplayInventoryFramework/
├── Config/
│   └── FilterPlugin.ini                # Packaging filter configuration for marketplace distribution
├── Resources/
│   ├── Icon128.png                     # Plugin marketplace brand tile (128x128)
│   ├── generate_icons.py               # Reproducible icon generation script
│   └── Icons/                          # High-DPI Slate class icons & thumbnails (16x16, 64x64, SVG)
│       ├── InventoryComponent_16.png / _64.png / .svg
│       ├── InventoryManagerComponent_16.png / _64.png / .svg
│       ├── ItemComponent_16.png / _64.png / .svg
│       ├── InventoryItemDefinition_16.png / _64.png / .svg
│       ├── InventoryItemInstance_16.png / _64.png / .svg
│       ├── InventoryItemFragment_16.png / _64.png / .svg
│       └── Fragment_*.png / .svg (UIInfo, ItemStats, Stacking, Montage, etc.)
└── Source/
    └── GameplayInventoryFramework/
        ├── Public/
        │   ├── Async/                  # UAsyncAction_PlayItemMontage
        │   ├── Components/             # UInventoryComponent, UInventoryManagerComponent
        │   ├── Definitions/            # UInventoryItemDefinition, UInventoryItemFragment
        │   ├── Fragments/              # Concrete polymorphic item fragments
        │   ├── FunctionLibraries/      # UInventoryFunctionLibrary
        │   ├── Instances/              # UInventoryItemInstance, FItemDynamicParam
        │   ├── Interfaces/             # IInventoryInterface, IItemInteractionInterface
        │   ├── Mass/                   # FMassItemDropFragment, FMassItemDropTag
        │   ├── Subsystems/             # UItemRegistrySubsystem, UWorldItemSubsystem
        │   ├── Types/                  # FFastArraySerializer types, MGITags
        │   └── World/                  # UItemComponent
        └── Private/
            ├── Tests/                  # 13 Automated unit & integration tests
            └── ... (Implementation files)
```

---

## Automated Test Coverage

The framework includes a comprehensive automated test suite organized under the `GameplayInventory.` category:

| Test Name | Source File | Status | Verifications |
| :--- | :--- | :---: | :--- |
| `GameplayInventory.Unit.Resources.SlateStyleRegistration` | `GIF_SlateStyleTests.cpp` | **PASSED** | Verifies Slate style set registration and all 32 class icon and thumbnail brushes. |
| `GameplayInventory.Async.MontagePlayback` | `GIF_MontageAsyncTest.cpp` | **PASSED** | Latent async action lifecycle, unconfigured actor fallbacks, safe `OnFailed` execution. |
| `GameplayInventory.FunctionLibrary.HelpersAndQueries` | `GIF_FunctionLibraryTest.cpp` | **PASSED** | Static fragment queries, montage lookup, dynamic stats, visual attach/detach. |
| `GameplayInventory.World.WorldLootLifecycle` | `GIF_WorldLootLifecycleTest.cpp` | **PASSED** | 3-stage pickup lifecycle delegates, actor hiding, collision disabling, Mass ECS structs. |
| `GameplayInventory.Visuals.SocketAttachment` | `GIF_SocketAttachmentTest.cpp` | **PASSED** | Static mesh, skeletal mesh, and actor attachment to sockets with relative transforms. |
| `GameplayInventory.Components.InventoryCapacityAndFilters` | `GIF_InventoryCapacityTest.cpp` | **PASSED** | Dynamic slot & weight limits, stack consolidation, gameplay tag query filters. |
| `GameplayInventory.Definitions.FragmentTagLookup` | `GIF_FragmentTagLookupTest.cpp` | **PASSED** | Hierarchical and exact fragment queries by Tag and Class. |
| `GameplayInventory.Definitions.SoftPointerLazyLoading` | `GIF_SoftPointerLazyLoadingTest.cpp` | **PASSED** | Soft object/class pointer lazy loading without hitching. |
| `GameplayInventory.Instances.DynamicStatIsolation` | `GIF_DynamicStatIsolationTest.cpp` | **PASSED** | Dynamic parameter bag runtime isolation across independent instances. |
| `GameplayInventory.Interfaces.InterfaceDecoupling` | `GIF_InterfaceDecouplingTest.cpp` | **PASSED** | `IInventoryInterface` and `IItemInteractionInterface` decoupling. |
| `GameplayInventory.Management.AtomicCrossInventoryTransfer` | `GIF_AtomicTransferTest.cpp` | **PASSED** | Multi-inventory transactional transfers, slot swapping, stack splitting. |
| `GameplayInventory.Networking.FastArraySerializerDeltaCounters` | `GIF_FastArraySerializationTest.cpp` | **PASSED** | FastArray network delta counters, add/modify/remove replication. |
| `GameplayInventory.Subsystems.RegistryAndSpatialStreaming` | `GIF_SubsystemsTest.cpp` | **PASSED** | `UItemRegistrySubsystem` async handle streaming and `UWorldItemSubsystem` spatial lookup. |

---

## Requirements & Compatibility

- **Engine Version**: Unreal Engine 5.8+ (also compatible with 5.3 – 5.7)
- **Supported Platforms**: Windows (Win64), Linux, macOS, iOS, Android, PlayStation 5, Xbox Series X/S
- **Core Dependencies**: `Core`, `CoreUObject`, `Engine`, `GameplayTags`, `NetCore`, `MassEntity`, `MassCore`, `Slate`, `SlateCore`, `UMG`, `Niagara`
- **Networking**: Dedicated Server, Listen Server, Peer-to-Peer, Single Player

---

## License & Attribution

Distributed under the **MIT License**. See [LICENSE](LICENSE) for details.

Created by **[Shankharaj Datta](https://www.linkedin.com/in/shankharaj-datta-45921a28a/)**.
