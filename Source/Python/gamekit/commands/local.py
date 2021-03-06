import os

import subprocess
from gamekit.conf import load_conf, save_conf, CONFIG, CONFIGNAME, Command


class Local(Command):
    """Generate localization files"""

    name: str = "local"

    @staticmethod
    def arguments(subparsers):
        init = subparsers.add_parser(Local.name, help="Initialize engine location")
        init.add_argument(
            "project", default=None, type=str, help="name of your project"
        )
        init.add_argument(
            "action", default=None, type=str, help="Gather, Compile, import, export"
        )

    @staticmethod
    def editor():
        engine = load_conf().get("engine_path")
        # TODO: check for linux
        return os.path.join(
            engine,
            "Binaries",
            "Win64",
            # UE4
            # 'UE4Editor.exe'
            "UnrealEditor.exe",
            # 'UnrealEditor-Cmd.exe',
        )

    @staticmethod
    def execute(args):
        name = args.project

        projects_folder = load_conf().get("project_path")
        project_folder = os.path.join(projects_folder, name)
        uproject = os.path.join(project_folder, f"{name}.uproject")

        args = [
            Local.uat(),
            uproject,
            "-run=GatherText",
            "-config=Config/Localization/Game_Gather.ini",
            "-EnableSCC",
            "-DisableSCCSubmit",
        ]

        p = subprocess.run(
            args, stdin=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, check=True
        )

        print(f"Subprocess terminated with (rc: {p.returncode})")
        return


COMMAND = Local
