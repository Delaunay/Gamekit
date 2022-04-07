from dis import code_info
import os
import json

from appdirs import user_config_dir
from argparse import ArgumentParser


NAME = 'gkcli'
AUTHOR = 'gamekit'
CONFIG = user_config_dir(NAME, AUTHOR)
CONFIGNAME = 'loc.json'
LATEST_CONF = None

def load_conf():
    global LATEST_CONF
    config = os.path.join(CONFIG, CONFIGNAME)
    os.makedirs(CONFIG, exist_ok=True)

    with open(config, 'r') as conffile:
        conf = json.load(conffile)

    LATEST_CONF = conf
    return conf


def save_conf(conf):
    config = os.path.join(CONFIG, CONFIGNAME)
    os.makedirs(CONFIG, exist_ok=True)

    with open(config, 'w') as conffile:
        json.dump(conf, conffile)


def command_init(subparsers):
    init = subparsers.add_parser('init', help='Initialize engine location')

def init(args):
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


def command_install(subparsers):
    install = subparsers.add_parser('install', help='Install Gamekit in a unreal project')
    install.add_argument("name", type=str, help='Plugin name')


def install(args):
    pass


def parse_args():
    parser = ArgumentParser()

    subparsers = parser.add_subparsers(dest='command')

    command_init(subparsers)
    command_install(subparsers)

    return parser.parse_args()

def main():
    commands = dict(
        init=init,
        install=install,
    )
    args = parse_args()

    command = args.command

    fun = commands.get(command)

    if fun is None:
        print(f'Action `{command}` not implemented')
        return

    fun(args)


if __name__ == '__main__':
    main()
