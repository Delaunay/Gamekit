import os
import subprocess

from gamekit.conf import Command, load_conf


class CookGame(Command):
    """Execute the tests for gamekit"""

    name: str = "cook"

    @staticmethod
    def arguments(subparsers):
        uat = subparsers.add_parser(CookGame.name, help='Run Unreal Automation Test (UAT)')
        uat.add_argument("name", type=str, help='Project name')
        uat.add_argument("map", type=str, help='map name')

    @staticmethod
    def uat():
        engine = load_conf().get('engine_path')
        # TODO: check for linux
        return os.path.join(
            engine,
            'Build',
            'BatchFiles',
            'RunUAT.bat'
        )

    @staticmethod
    def execute(args):
        name = args.name

        projects_folder = load_conf().get('project_path')
        project_folder = os.path.join(projects_folder, name)
        uproject = os.path.join(project_folder, f'{name}.uproject')

        args = [
            CookGame.uat(),
            # Base
            # ====
            # "-Help",
            # "-Verbose",
            # "-VeryVerbose",
            # "-List",
            # "-Compile",
            # "-NoCompile",

            # Commands
            # ========
            "BuildCookRun",
            # "BuildPlugin",
            # "BuildServer",
            # "BuildGame",
            # "Localise",
            # "RunUnreal",
            # "RunLowLevelTests",
            # "TestGauntlet",
            # "RunEditorTests",

            f"-project={uproject}",
            f"-archivedirectory={projects_folder}/Cooked",

            # Options
            # =======
            "-unattended",
            "-utf8output",
            "-platform=Linux",
            "-clientconfig=Shipping",
            "-serverconfig=Shipping",
            "-noP4",
            "-nodebuginfo",
            "-allmaps",
            "-cook",
            "-build",
            "-stage",
            "-prereqs",
            "-pak",
            "-archive",
        ]

        p = subprocess.run(args,
            stdin=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True,
            check=True
        )

        print(f"Subprocess terminated with (rc: {p.returncode})")
        print(p.stdout, p.stderr)
        return

COMMAND = CookGame
