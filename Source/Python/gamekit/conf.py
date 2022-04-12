import os
import json

from appdirs import user_config_dir


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


class Command:
    @staticmethod
    def arguments(subparsers):
        raise NotImplementedError()

    @staticmethod
    def execute(args):
        raise NotImplementedError()
