Import("env")
import os, zipfile, shutil
from pathlib import Path
import json

# Get the version number from the build environment.
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

def copy_fw_files(source, target, env):
    fw_file_name=str(target[0])
    
    if fw_file_name[-3:] == "bin":
        fw_file_name=fw_file_name[0:-3] + "bin"

    shutil.copy(fw_file_name, "./bin" + "/firmware.bin")
    createCommunityZipFile(source, target, env)

def createCommunityZipFile(source, target, env):
    original_folder_path = "./bin/"
    zip_file_path = './dist/' + "fw_" + firmware_version + '.zip'
    createZIP(original_folder_path, zip_file_path)

def createZIP(original_folder_path, zip_file_path):
    if os.path.exists("./dist") == False:
        os.mkdir("./dist")
    with zipfile.ZipFile(zip_file_path, 'w') as zipf:
        for root, dirs, files in os.walk(original_folder_path):
            for file in files:
                # Create a new path in the ZIP file
                new_path = os.path.join("", os.path.relpath(os.path.join(root, file), original_folder_path))
                # Add the file to the ZIP file
                zipf.write(os.path.join(root, file), new_path)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", copy_fw_files)
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_fw_files)