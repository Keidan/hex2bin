#!/usr/bin/env python3
import argparse, sys, os, subprocess

testNum = 1
SAMPLE1 = "sample1.txt"
SAMPLE2 = "sample2.txt"
SAMPLE3 = "sample3.txt"
SAMPLE_TEMP = "sample.txt.temp"
RESULT1 = "00000000004030000000000000203000"
RESULT2 = "00000000005030000000000000603000"
RESULT3 = "00000000005030000000000000603000"

def testPass(label):
  global testNum
  print("Test \033[1m{0}\033[0m \033[38;5;208m{1}\033[0m \033[32mOK\033[0m".format(testNum, label))
  testNum = testNum + 1
    
def testFail(label):
  global testNum
  print("Test \033[1m{0}\033[0m \033[38;5;208m{1}\033[0m \033[31mFAILED\033[0m".format(testNum, label))
  sys.exit(1)
    
def exec(args):
  process = subprocess.Popen(args)
  stdout, stderr = process.communicate()
  exitCode = process.wait()
  return exitCode

def readFile(file):
  with open(file) as f:
    content = f.read()
  return content

def getSample(file):
    p = os.path.join(os.getcwd(), "samples")
    return os.path.join(p, file)

def main(argv):
  parser = argparse.ArgumentParser()
  parser.add_argument("-f", "--file", help="Binary file.")
  args = parser.parse_args()
  if args.file == None:
    print("Unspecified file name")
    sys.exit(1)
  if not os.path.exists(args.file):
    print("File {0} not found".format(args.file))
    sys.exit(1)
    
  curDir = os.getcwd()
  sampleDir = getSample(SAMPLE_TEMP)
  retCode = exec([args.file, "-i", getSample(SAMPLE1), "-o", sampleDir, "-s", "6", "-l", "47"])
  if retCode != 0:
    os.remove(sampleDir)
    testFail("sample1 exec")

  testPass("sample1 exec   ")
  result = readFile(sampleDir)
  if result != RESULT1:
    os.remove(sampleDir)
    testFail("sample1 compare")

  testPass("sample1 compare")
  retCode = exec([args.file, "-i", getSample(SAMPLE2), "-o", sampleDir, "-l", "47"])
  if retCode != 0:
    os.remove(sampleDir)
    testFail("sample2 exec")

  testPass("sample2 exec   ")
  result = readFile(sampleDir)
  if result != RESULT2:
    os.remove(sampleDir)
    testFail("sample2 compare")

  testPass("sample2 compare")
  retCode = exec([args.file, "-i", getSample(SAMPLE3), "-o", sampleDir, "-s", "1"])
  if retCode != 0:
    os.remove(sampleDir)
    testFail("sample3 exec")

  testPass("sample3 exec   ")
  if result != RESULT3:
    os.remove(sampleDir)
    testFail("sample3 compare")
    
  testPass("sample3 compare")
  print("TEST \033[32mPASSED\033[0m")
  os.remove(sampleDir)
  
if __name__ == '__main__':
  main(sys.argv[1:])