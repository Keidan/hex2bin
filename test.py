#!/usr/bin/env python3
import argparse, sys, os, subprocess

SAMPLE1 = "sample1.txt"
SAMPLE2 = "sample2.txt"
SAMPLE3 = "sample3.txt"
SAMPLE_TEMP = "sample.txt.temp"
RESULT1 = "00000000004030000000000000203000"
RESULT2 = "00000000005030000000000000603000"
RESULT3 = "00000000005030000000000000603000"

class Test:
  def __init__(self):
    self.testNum = 1
  def test_pass(self, label):
    print("Test \033[1m{0}\033[0m \033[38;5;208m{1}\033[0m \033[32mOK\033[0m".format(self.testNum, label))
    self.testNum = self.testNum + 1
        
  def test_fail(self, label):
    print("Test \033[1m{0}\033[0m \033[38;5;208m{1}\033[0m \033[31mFAILED\033[0m".format(self.testNum, label))
    sys.exit(1)
    
def exec_process(args):
  process = subprocess.Popen(args)
  stdout, stderr = process.communicate()
  exit_code = process.wait()
  return exit_code

def read_file(file):
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
    
  t = Test()
  sample_dir = getSample(SAMPLE_TEMP)
  ret_code = exec_process([args.file, "-i", getSample(SAMPLE1), "-o", sample_dir, "-s", "6", "-l", "47"])
  if ret_code != 0:
    os.remove(sample_dir)
    t.test_fail("sample1 exec")

  t.test_pass("sample1 exec   ")
  result = read_file(sample_dir)
  if result != RESULT1:
    os.remove(sample_dir)
    t.test_fail("sample1 compare")

  t.test_pass("sample1 compare")
  ret_code = exec_process([args.file, "-i", getSample(SAMPLE2), "-o", sample_dir, "-l", "47"])
  if ret_code != 0:
    os.remove(sample_dir)
    t.test_fail("sample2 exec")

  t.test_pass("sample2 exec   ")
  result = read_file(sample_dir)
  if result != RESULT2:
    os.remove(sample_dir)
    t.test_fail("sample2 compare")

  t.test_pass("sample2 compare")
  ret_code = exec_process([args.file, "-i", getSample(SAMPLE3), "-o", sample_dir, "-s", "1"])
  if ret_code != 0:
    os.remove(sample_dir)
    t.test_fail("sample3 exec")

  t.test_pass("sample3 exec   ")
  if result != RESULT3:
    os.remove(sample_dir)
    t.test_fail("sample3 compare")
    
  t.test_pass("sample3 compare")
  ret_code = exec_process([args.file, "-i", getSample(SAMPLE3), "-o", sample_dir, "-p"])
  if ret_code != 0:
    os.remove(sample_dir)
    t.test_fail("sample4 exec")

  t.test_pass("sample4 exec   ")
  ret_code = exec_process([args.file, "-i", getSample(SAMPLE3), "-o", sample_dir, "-e"])
  if ret_code != 0:
    os.remove(sample_dir)
    t.test_fail("sample5 exec")

  t.test_pass("sample5 exec   ")
  print("TEST \033[32mPASSED\033[0m")
  os.remove(sample_dir)
  
if __name__ == '__main__':
  main(sys.argv[1:])
