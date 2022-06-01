import unreal

default_datatable = "/Gamekit/Data/AbilityData"
default_destination = '/Gamekit/Abilities/Generated'

#  from importlib import reload
#  import Abilities.generate_abilities as gkgen
#  reload(gkgen)
#  gkgen.generate_gamekit_abilities()

def generate_abilities_from_table(datatable, package):
    """Generate a new Gameplay ability for every row of a given datatable"""

    table = unreal.EditorAssetLibrary.load_asset(datatable)

    for fname in unreal.DataTableFunctionLibrary.get_data_table_row_names(table):
        name = str(fname)
        asset_path = package + '/' + name

        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            print(name)

            ability = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
                asset_name=f'GA_{name}',
                package_path=package,
                asset_class=unreal.GKGameplayAbility,
                factory=unreal.GKGameplayAbilityFactory()
            )

            # Set their config
            ability.ability_data_table = table
            ability.ability_row_name = fname


def generate_gamekit_abilities():
    generate_abilities_from_table(default_datatable, default_destination)


# ga = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
#     asset_name=f'GA_Test',
#     package_path=package,
#     asset_class=unreal.GKGameplayAbility,
#     factory=unreal.GKGameplayAbilityFactory()
# )
