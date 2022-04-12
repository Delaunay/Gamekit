import os
import subprocess

from gamekit.conf import Command, load_conf


class RunTests(Command):
    """Execute the tests for gamekit"""

    name: str = "tests"

    @staticmethod
    def arguments(subparsers):
        uat = subparsers.add_parser(RunTests.name, help='Run Unreal Automation Test (UAT)')
        uat.add_argument("name", type=str, help='Project name')
        uat.add_argument("map", type=str, help='map name')

    @staticmethod
    def editor():
        engine = load_conf().get('engine_path')
        # TODO: check for linux
        return os.path.join(
            engine,
            'Binaries',
            'Win64',
            # UE4
            # 'UE4Editor.exe'
            'UnrealEditor.exe',
            # 'UnrealEditor-Cmd.exe',
        )

    @staticmethod
    def execute(args):
        name = args.name

        projects_folder = load_conf().get('project_path')
        project_folder = os.path.join(projects_folder, name)
        uproject = os.path.join(project_folder, f'{name}.uproject')

        args = [
            RunTests.editor(),
            uproject,
            args.map,
            '-log',
            '-Unattended',
            '-NullRHI',
            '-NoSplash',
            '-NoSound',
            '-NoPause',
            f'-ReportOutputPath="{project_folder}/Saved/Automation/Report"',
            f'ABSLOG="{project_folder}/Saved/Automation/Log.txt"',
            '-TestExit=Automation Test Queue Empty',
            '-ExecCmds=Automation RunTests Gamekit',
        ]

        p = subprocess.run(args,
            stdin=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True,
            check=True
        )

        print(f"Subprocess terminated with (rc: {p.returncode})")
        return


COMMAND = RunTests
