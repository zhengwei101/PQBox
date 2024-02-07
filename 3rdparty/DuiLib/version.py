#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
version.py -- version string substitution utility.
"""

import argparse
import re
import sys
import os
import configparser
import subprocess


class ConfigParserIgnoreCase(configparser.ConfigParser):
    def __init__(self, defaults=None):
        configparser.ConfigParser.__init__(self, defaults=None)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('header_path', help='version file path')
    args = parser.parse_args()
    f = open(args.header_path)
    try:
        original = f.read()
        ver_major = int(
            re.search(r'^#define\s+VERSION_MAJOR\s+(\d+)$', original, re.M).group(1))
        ver_minor = int(
            re.search(r'^#define\s+VERSION_MINOR\s+(\d+)$', original, re.M).group(1))
        ver_patch = int(
            re.search(r'^#define\s+VERSION_PATCH\s+(\d+)$', original, re.M).group(1))
        ver_build = int(
            re.search(r'^#define\s+VERSION_BUILD\s+(\d+)$', original, re.M).group(1))
        ver_source = re.search(
            r'^#define\s+VERSION_SOURCE\s+(\S+)$', original, re.M).group(1)
        ver_prerelease = re.search(
            r'^#define\s+VERSION_PRERELEASE\s+(\S+)$', original, re.M).group(1)
    except re.error:
        print('parse version file failed ...')
        sys.exit(0)
    finally:
        f.close()

    old_ver_build = '#define VERSION_BUILD         ' + str(ver_build)
    new_ver_build = '#define VERSION_BUILD         ' + str(ver_build+1)
    old_ver_source = str(ver_source)
    ver_source = subprocess.check_output(
        ["git", "rev-parse", "--short", "HEAD"]).strip()
    new_ver_source = str(ver_source,encoding="utf-8")
    
    print('old version:%d.%d.%d.%d.%s %s' % (ver_major, ver_minor,
          ver_patch, ver_build, old_ver_source, ver_prerelease))
          
    ver_build += 1
    
    print('new version:%d.%d.%d.%d.%s %s' % (ver_major, ver_minor,
          ver_patch, ver_build, new_ver_source, ver_prerelease))

    f = open(args.header_path)
    old_content = f.read()
    f.close()

    new_content = re.sub(old_ver_build,  new_ver_build, old_content)
    new_content = re.sub(old_ver_source, new_ver_source, new_content)
    sb = bytes(new_content, encoding="utf-8")
    f = open(args.header_path, 'wb')
    f.write(sb)
    f.close()
