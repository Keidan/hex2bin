#!/usr/bin/env python3
import argparse, sys, os, re

VERSION="version.txt"
SONAR_PROJECT="sonar-project.properties"

def read_file(filename) -> str:
  lines = ""
  with open(filename) as f:
    lines += f.read()
  return lines

def main() -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument("-r", "--root", help="Root folder.")
  args = parser.parse_args()
  version = ""
  sonar_project = ""
  if len(args.root) != 0:
    version = os.path.join(args.root, VERSION)
    sonar_project = os.path.join(args.root, SONAR_PROJECT)
  else:
    version = VERSION
    sonar_project = SONAR_PROJECT

  if not os.path.exists(version):
    print("File {0} not found".format(version))
    return 1
  if not os.path.exists(sonar_project):
    print("File {0} not found".format(sonar_project))
    return 1
    
  major = ""
  minor = ""
  release = ""
  result = re.search(r"VERSION_MAJOR ([\d]*)\nVERSION_MINOR ([\d]*)\nVERSION_RELEASE ([\d]*)", read_file(version))
  major = result.group(1)
  minor = result.group(2)
  release = result.group(3)
  if len(major) == 0:
    print("Major version number not found")
    return 1
  if len(minor) == 0:
    print("Minor version number not found")
    return 1
  if len(release) == 0:
    print("Release version number not found")
    return 1
  vers = major + "." + minor + "." + release
  content = read_file(sonar_project)
  result = re.search(r"([\s\S]+sonar\.projectVersion=)\d+(?:\.\d+){0,2}([\s\S]+)", content)
  new_content = result.group(1) + vers + result.group(2)

  if new_content != content:
    print("Update sonar-project version")
    with open(sonar_project, "w") as f:
      f.write(new_content)
  return 0

if __name__ == '__main__':
  sys.exit(main())
