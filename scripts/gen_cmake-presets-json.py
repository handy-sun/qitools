#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import json
import platform
import subprocess

def get_compiler_paths():
    gcc_path = subprocess.getoutput("which gcc").strip()
    gpp_path = subprocess.getoutput("which g++").strip()

    clang_path = subprocess.getoutput("which clang").strip()
    clangpp_path = subprocess.getoutput("which clang++").strip()
    
    return {
        "gcc": gcc_path,
        "gpp": gpp_path,
        "clang": clang_path,
        "clangpp": clangpp_path
    }


def gen_linux_cmake_presets():
    system = platform.system()
    if not system == "Linux":
        return {}

    uname_tup = platform.uname()
    architecture = uname_tup[-2]
    compiler_paths = get_compiler_paths()

    generator = "Ninja" if subprocess.getoutput("which ninja").strip() else "Unix Makefiles"

    presets = {
        "version": 3,
        "cmakeMinimumRequired": {
            "major": 3,
            "minor": 19,
            "patch": 0
        },
        "configurePresets": [],
        "buildPresets": []
    }

    # descr = "Set generator, compilers, build and install directory, release build type"
    descr = "Set generator, compilers, build directory, release build type"
    if compiler_paths["gcc"] and compiler_paths["gpp"]:
        triad = f"{system.lower()}-{architecture}-gnu"

        presets["configurePresets"].append({
            "name": "gnu-rel",
            "displayName": f"Configure {system} compiler: {compiler_paths["gpp"]}",
            "description": descr,
            "generator": generator,
            "binaryDir": f"${{sourceDir}}/build-{triad}-release",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_SYSTEM_NAME": system,
                "CMAKE_INSTALL_PREFIX": f"${{sourceDir}}/install-{triad}",
                "CMAKE_C_COMPILER": compiler_paths["gcc"],
                "CMAKE_CXX_COMPILER": compiler_paths["gpp"],
                "CMAKE_AUTOMOC": "ON",
                "CMAKE_AUTORCC": "ON",
                "CMAKE_AUTOUIC": "ON"
            }
        })
        presets["buildPresets"].append({
            "name": "build gnu-rel",
            "configurePreset": "gnu-rel",
            "displayName": f"Compiling (Release) use {compiler_paths["gpp"]}",
            "configuration": "Release"
        })

    if compiler_paths["clang"] and compiler_paths["clangpp"]:
        triad = f"{system.lower()}-{architecture}-clang"

        presets["configurePresets"].append({
            "name": "clang-rel",
            "displayName": f"Configure {system} compiler: {compiler_paths["clangpp"]}",
            "description": descr,
            "generator": generator,
            "binaryDir": f"${{sourceDir}}/build-{triad}-release",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_SYSTEM_NAME": system,
                "CMAKE_INSTALL_PREFIX": f"${{sourceDir}}/install-{triad}",
                "CMAKE_C_COMPILER": compiler_paths["clang"],
                "CMAKE_CXX_COMPILER": compiler_paths["clangpp"],
                "CMAKE_AUTOMOC": "ON",
                "CMAKE_AUTORCC": "ON",
                "CMAKE_AUTOUIC": "ON"
            }
        })
        presets["buildPresets"].append({
            "name": "build clang-rel",
            "configurePreset": "clang-rel",
            "displayName": f"Compiling (Release) use {compiler_paths["clangpp"]}",
            "configuration": "Release"
        })

    return presets


if __name__ == "__main__":
    presets_fmt = gen_linux_cmake_presets()
    os.path.pardir
    script_dir = os.path.abspath(os.path.dirname(__file__))
    # this script parent's dir(abs)
    parent_dir = os.path.dirname(script_dir)
    filename = parent_dir + '/CMakePresets.json'
    with open(filename, 'w') as f:
        json.dump(presets_fmt, f, indent=2)

    print("\'{}\' has been generated.".format(filename))
