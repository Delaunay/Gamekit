import os
import pkg_resources

from gamekit.conf import load_conf, save_conf, CONFIG, CONFIGNAME, Command


class Docs(Command):
    """Add a docs folder to your project"""

    # TODO: create a cookiecutter for Unreal projects documentation

    name: str = "docs"

    @staticmethod
    def arguments(subparsers):
        init = subparsers.add_parser(Docs.name, help='Add documentation to your project')
        init.add_argument('project', default=None, type=str, help='name of your project')

    @staticmethod
    def execute(args):
        pass


COMMAND = Docs
