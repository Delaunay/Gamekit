import unreal

import automation_test.unittest_utilities as util

# unreal.new_object(type, outer=Transient, name=Default, base_type=Object)
#       .find_asset
#       .get_engine_subsystem
#       .get_editor_subsystem
#       .get_type_from_class
#       .load_asset
#       .load_class
#       .load_module
#       .load_object
#       .register_python_shutdown_callback


# EditorLevelLibrary.editor_end_play
#                   .load_level
#                   .new_level
#                   .new_level_from_template
#                   .pilot_level_actor
#                   .spawn_actor_from_class

# EditorLevelUtils.add_level_to_world
#                   .move_actors_to_level

# EditorActorSubsystem.spawn_actor_from_class


# EditorLoadingAndSavingUtils.load_map
#                            .new_blank_map
#                            .new_map_from_template
#
def load_map(filename=None, template=None):
    if filename is None and template is None:
        return unreal.EditorLoadingAndSavingUtils.new_blank_map(False)

    if template is not None:
        return unreal.EditorLoadingAndSavingUtils.new_map_from_template(template, False)

    return unreal.EditorLoadingAndSavingUtils.load_map(filename)


# AutomationLibrary.take_automation_screenshot
#                  .take_automation_screenshot_at_camera
#                  .take_automation_screenshot_of_ui

# AutomationLibrary
def wait_for_loading(
    world_ctx,
    latent_info=unreal.LatentActionInfo(),
    options=unreal.AutomationWaitForLoadingOptions(),
):
    unreal.AutomationLibrary.automation_wait_for_loading(
        world_ctx, latent_info, options
    )


def take_gameplay_screenshot(
    screenshot_name,
    max_global_error=0.020000,
    max_local_error=0.120000,
    map_name_override="",
):
    """Shortcut"""
    return unreal.AutomationUtilsBlueprintLibrary.take_gameplay_automation_screenshot(
        screenshot_name, max_global_error, max_local_error, map_name_override
    )


run = util.TestRunner()


@run.add_test
def test_something():
    unreal.log("logging")
    print("printing")
    util.expect_false(lambda: False)


def trash():
    # "Class'/Script/Gamekit.GKCharacterBase'"
    # Gamekit/Content/Modes/HacknSlash/HNSPlayerCharacter.uasset

    # /Gamekit/Modes/HacknSlash/HNSPlayerCharacter.HNSPlayerCharacter
    # C:/Users/Archimedes/work/gamekit/Plugins/Gamekit/Content/Modes/HacknSlash/HNSPlayerCharacter.uasset
    # Blueprint'/Gamekit/Modes/HacknSlash/HNSPlayerCharacter.HNSPlayerCharacter'
    pkg_kit = unreal.load_package("/Gamekit/Modes/HacknSlash/HNSPlayerCharacter")
    blueprint = unreal.load_object(pkg_kit, "HNSPlayerCharacter")

    # EditorActorSubsystem
    # unreal.EditorLevelLibrary.spawn_actor_from_class(
    #    blueprint,
    #    unreal.Vector(0, 0, 0),
    #    transient=True
    # )

    # print(unreal.BlueprintEditorLibrary.generated_class(ActorClass))
    #
    # object = unreal.find_object("")
    # GamekitModule = unreal.load_module("Gamekit")
    #
    #  AttributeError: module 'unreal' has no attribute 'EditorAssetLibrary'
    # print(unreal.EditorAssetLibrary.list_assets("/Game/", recursive=True, include_folder=False))

    #
    #    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostProcessMaterial(
    #            TEXT("Material'/Gamekit/FogOfWar/FoWPostProcess.FoWPostProcess'"));


@run.add_test
def test_scratch_test():
    # None of this work :(
    unreal.log("Starting tests")

    # load our testing template
    world = load_map(template="/GamekitTests/FogOfWarTests/FogOfWar_Test_Level")

    wait_for_loading(world)

    # pawn = unreal.AIHelperLibrary.spawn_ai_from_class(
    #    world,
    #    unreal.GKCharacterBase,
    #    None,
    #    unreal.Vector(0, 0, 0),
    # )

    unreal.AutomationLibrary.take_automation_screenshot(
        world,
        unreal.LatentActionInfo(),
        "ScreenShotTest_1",
        "Whatber",
        unreal.AutomationLibrary.get_default_screenshot_options_for_gameplay(),
    )


@run.add_test
@util.test_on_linux
def test_something_linux():
    pass


run.run_all()
