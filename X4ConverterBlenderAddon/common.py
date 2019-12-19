import subprocess
import os
import logging

log = logging.getLogger(__name__)


# TODO make me an operator
def get_converter_path():
    script_path = os.path.realpath(__file__)
    dir_path = os.path.dirname(script_path)
    linux_path = dir_path + "/X4ConverterApp"
    windows_path = dir_path + "/X4ConverterApp.exe"
    if (os.path.exists(linux_path)):
        return linux_path
    elif (os.path.exists(windows_path)):
        return windows_path
    else:
        error = "Error, could not find converter - are you sure it's in the same directory as the script? It was not found in " + dir_path
        log.error(error)
        raise Exception(error)

def get_data_dir(target):
    # Keeps iterating up until it runs out of places to look or finds jobeditor.html
    # A bit hacky but it should work
    possible_dir = target
    while ((not os.path.ismount(possible_dir)) and (not os.path.exists(possible_dir+"/jobeditor.html"))):
        possible_dir = os.path.dirname(possible_dir)
    if (not os.path.ismount(possible_dir)):
        return possible_dir
    else:
        error = "Could not find unpacked root"
        log.error(error)
        raise Exception(error)
def call_converter(action, target):
    # As per the converter's command line args:
    # executable <action> <datdir> <target>
    converter_path = get_converter_path()
    dat_dir = get_data_dir(target)
    result = subprocess.check_output([converter_path, action, dat_dir, target])
    log.warning(result.decode('utf-8'))
    log.warning("Converter appears to have been successful")


def parse_anim_states(obj):
    data=obj["X4_anim_states"]


def find_anim_metadata_source(obj):
    tgt = obj
    while ((tgt is not None) and (not tgt.X4_anim_data)):
        log.warning(tgt.name)
        tgt = tgt.parent
    return tgt


