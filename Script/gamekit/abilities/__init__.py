import unreal

default_datatable = "/Gamekit/Data/AbilityData"
default_destination = '/Gamekit/Abilities/Generated'


def generate_ability(root, name, table, parent):
    """Create a new ability from a datatable"""
    bpfactory = unreal.BlueprintFactory()
    bpfactory.set_editor_property('parent_class', parent)
    bpfactory.set_editor_property('edit_after_new', False)
    bpfactory.set_editor_property('supported_class', unreal.Blueprint)

    asset_name = f'GA_{name}'

    # Create the basic blueprint
    ability = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name=asset_name,
        package_path=root,
        asset_class=None,
        factory=bpfactory
    )

    # Save the blueprint (to compile it)
    unreal.EditorAssetLibrary.save_loaded_asset(ability)

    # Get the Generated class
    bp_gc = unreal.load_object(None, root + '/' + f'{asset_name}.{asset_name}_C')

    # Get the CDO of the class
    bp_cdo = unreal.get_default_object(bp_gc)

    # Set the properties
    bp_cdo.set_editor_property("AbilityDatatable", table)
    bp_cdo.set_editor_property("AbilityRowName", name)


def get_default_parent():
    parent_obj = unreal.load_object(None, '/Gamekit/Abilities/GAC_AbilityBase_Prototype.GAC_AbilityBase_Prototype_C')
    return unreal.get_default_object(parent_obj).get_class()


def does_end_with_number(name):
    try:
        int(name.split('_')[-1])
        return True
    except:
        return False


def generate_ability_internal(datatable, table, package, fname):
    parent_cls = get_default_parent()
    name = str(fname)
    asset_name = f'GA_{name}'

    asset_path = package + '/' + asset_name

    row, valid = unreal.GKAbilityBlueprintLibrary.get_ability_data(table, fname)

    if not valid:
        print(f"Skipping {name} because its ability data row was not found")
        return

    if not row.auto_generate:
        print(f"Skipping {name} because it disabled auto generate")
        return

    if does_end_with_number(name):
        print(f"Skipping {name} because name ends with number")
        return

    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        print(f'Skipping {asset_path} because it already exist')
        return

    generate_ability(package, name, table, parent_cls)

    # the ability was generated, lets ignore it for future updates
    unreal.GKAbilityBlueprintLibrary.DisableAutoGeneration(table, fname)

    unreal.EditorAssetLibrary.save_asset(datatable, only_if_is_dirty=False)

    # unreal.EditorLoadingAndSavingUtils.save_packages(datatable.get_package())


def generate_ability(datatable, fname, package):
    table = unreal.EditorAssetLibrary.load_asset(datatable)
    generate_ability_internal(datatable, table, package, fname)


def generate_abilities_from_table(datatable, package):
    """Generate a new Gameplay ability for every row of a given datatable"""

    table = unreal.EditorAssetLibrary.load_asset(datatable)

    for fname in unreal.DataTableFunctionLibrary.get_data_table_row_names(table):
        generate_ability_internal(datatable, table, package, fname)


def generate_gamekit_abilities():
    #  import Abilities.generate_abilities as gkgen
    #  reload(gkgen); gkgen.generate_gamekit_abilities()
    generate_abilities_from_table(default_datatable, default_destination)


def generate_test():
    #  from importlib import reload
    #  import Abilities.generate_abilities as gkgen
    #  reload(gkgen); gkgen.generate_test()

    package = '/Gamekit/Abilities/Generated'
    generate_ability(package, "Test", None)