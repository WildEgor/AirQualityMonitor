Import("env")
import os
import json

firmware_version = os.environ.get('VERSION', "")

if firmware_version == "":
    try:
        with open('project.json', 'r', encoding='utf-8') as f:
            project_data = json.load(f)
            firmware_version = project_data.get('version', "0.0.1")
    except (FileNotFoundError, json.JSONDecodeError, KeyError):
        firmware_version = "0.0.1"

firmware_version = firmware_version.lstrip("v")
firmware_version = firmware_version.strip(".")

print(f'Using version {firmware_version} for the build')

env.Append(CPPDEFINES=[
  f'BUILD_VERSION={firmware_version}'
])

env.Replace(PROGNAME=f'{env["PIOENV"]}_{firmware_version.replace(".", "_")}')