import json
import os

def load():
    full_path = os.path.realpath(__file__)
    with open(os.path.dirname(full_path) + '/secret.pass') as data_file:
        secrets = json.load(data_file)

    return secrets
