import os

from gamekit.conf import load_conf, save_conf, CONFIG, CONFIGNAME


class Init:
    name: str = "init"

    @staticmethod
    def arguments(subparsers):
        init = subparsers.add_parser(Init.name, help='Initialize engine location')

    @staticmethod
    def execute(args):
        config = os.path.join(CONFIG, CONFIGNAME)
        conf = dict()

        default_engine = '/UnrealEngine/Engine'
        default_project = os.path.abspath(os.path.join('..', default_engine))

        if os.path.exists(config):
            conf = load_conf()
            default_engine = conf.get('engine_path', default_engine)
            default_project = conf.get('project_path', default_project)

        engine_path = input(f'Engine Folder [{default_engine}]: ')
        project_folders = input(f'Project Folder [{default_project}]: ')

        engine_path = engine_path or default_engine
        project_folders = project_folders or default_project

        conf['engine_path'] = engine_path
        conf['project_path'] = project_folders

        save_conf(conf)
        print(f'Updated Engine paths inside `{config}`')

COMMAND = Init
