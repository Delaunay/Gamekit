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


def generate_abilities_from_table(datatable, package):
    """Generate a new Gameplay ability for every row of a given datatable"""
    #  import Abilities.generate_abilities as gkgen
    #  reload(gkgen); gkgen.generate_abilities_from_table()

    parent_obj = unreal.load_object(None, '/Gamekit/Abilities/GAC_AbilityBase_Prototype.GAC_AbilityBase_Prototype_C')
    parent_cls = unreal.get_default_object(parent_obj).get_class()

    table = unreal.EditorAssetLibrary.load_asset(datatable)

    for fname in unreal.DataTableFunctionLibrary.get_data_table_row_names(table):
        name = str(fname)
        asset_path = package + '/' + name

        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            generate_ability(package, name, table, parent_cls)
        else:
            print(f'Skipping {asset_path}')


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