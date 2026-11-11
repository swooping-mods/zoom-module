# Zoom Module: behavior and implementation

Describes the exported implementation **0.2.0**. The linked source and metadata define this version; gameplay qualification is a separate requirement.

## Behavior

Buy the $125 Zoom Module through Modded Shop, assign it in Equipment inventory and select its quickslot. Holding Z smoothly enters three-times optical zoom; release restores the captured field of view. The normal bottom item legend provides controls and native raycast range feedback. No permanent purchase/status overlay is added.

## State and lifecycle

Purchased ownership uses the shared provider sidecar and native quickslot IDs. Captured camera/FOV state is temporary and restored on release, unequip, input loss or world replacement; rooted restoration targets remain valid until cleanup. Provider purchases enforce native debit, persisted ownership and checked refund-on-save-failure handling.

## Integration and configuration

Runtime dependencies: [shop-framework](https://github.com/swooping-mods/shop-framework), [equipment-framework](https://github.com/swooping-mods/equipment-framework), [hints-framework](https://github.com/swooping-mods/hints-framework).

The [module metadata](../source/mods/zoom-module/mod.json) owns the version and declared integration contract. Native modules start at `MoonLab_ModStart`. Game/Unity integrations target the pinned Steam Windows x64 IL2CPP build.

| Service | ABI | Table size |
|---|---:|---|
| Requires `shop-framework.shop` | 1 | at least 56 bytes |
| Requires `equipment-framework.equipment` | 1 | at least 32 bytes |
| Requires `hints-framework.hints` | 1 | at least 24 bytes |

## Implementation sources

- [mod.c](../source/mods/zoom-module/mod.c)
- [camera_restore.h](../source/mods/zoom-module/camera_restore.h)
- [tool_runtime.h](../source/mods/zoom-module/tool_runtime.h)

## Verification boundary

No weapon magnification, saved camera override or extra camera. Actual FOV changes, restoration during camera conflicts, rendered hints and co-op require scoped checks; ownership/equip alone does not qualify them.

`python3 build.py --check` verifies the exported source hashes. It does not execute the mod or qualify the behaviors above. Installation and independent build prerequisites are in the [README](../README.md).
