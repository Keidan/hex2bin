#!/usr/bin/env python3
import argparse, sys, os, subprocess, hashlib

SAMPLE1 = 'sample1.txt'
SAMPLE2 = 'sample2.txt'
SAMPLE3 = 'sample3.txt'
SAMPLE4 = 'sample4.txt'
SAMPLE1_HEX = 'sample1.hex'
SAMPLE1_BIN = 'sample1.bin'
SAMPLE2_HEX = 'sample2.hex'
SAMPLE2_BIN = 'sample2.bin'
SAMPLE_HEX_TEMP = 'sample.hex.temp'
SAMPLE_BIN_TEMP1 = 'sample1.bin.temp'
SAMPLE_BIN_TEMP2 = 'sample2.bin.temp'
SAMPLE_TEMP = 'sample.txt.temp'
RESULT1 = '00000000004030000000000000203000'
RESULT2 = '00000000005030000000000000603000'
RESULT3 = '00000000005030000000000000603000'
MAX_CHAR_RESULT = 25

class Result:
  def __init__(self):
    self.num = 0
    self.passed = 0
    self.failed = 0
  
  def print(self, error, label):
    if error:
      self.error(label)
    else:
      self.success(label)

  def success(self, label):
    self.num = self.num + 1
    print(f'Test \033[1m{self.num:02d}\033[0m \033[38;5;208m{label}\033[0m \033[32mOK\033[0m')
    self.passed = self.passed + 1
        
  def error(self, label):
    self.num = self.num + 1
    print(f'Test \033[1m{self.num:02d}\033[0m \033[38;5;208m{label}\033[0m \033[31mFAILED\033[0m')
    self.failed = self.failed + 1

  def get_count(self) -> int:
    return self.num
  
  def get_passed(self) -> int:
    return self.passed
  
  def get_failed(self) -> int:
    return self.failed

class TU:
  def __init__(self, file):
    self.file = file
    self.res = Result()
    self.temp_sample = self.get_sample(SAMPLE_TEMP)
    self.temp_hex = self.get_sample(SAMPLE_HEX_TEMP)
    self.temp_bin1 = self.get_sample(SAMPLE_BIN_TEMP1)
    self.temp_bin2 = self.get_sample(SAMPLE_BIN_TEMP2)

  def md5(self, fname):
    hash_md5 = hashlib.md5()
    with open(fname, "rb") as f:
      for chunk in iter(lambda: f.read(4096), b""):
        hash_md5.update(chunk)
    return hash_md5.hexdigest()
  
  def get_sample(self, file) -> str:
      p = os.path.join(os.getcwd(), 'samples')
      return os.path.join(p, file)
  
  def format(self, label) -> str:
    return f'{label:{MAX_CHAR_RESULT}s}'
    
  def exec_process(self, args, use_std = True) -> int:
    if use_std:
      process = subprocess.Popen(args)
      _, _ = process.communicate()
    else:
      process = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
      process.communicate()
    exit_code = process.wait()
    return exit_code

  def read_temp_file(self) -> bytes:
    with open(self.temp_sample, 'rb') as f:
      content = f.read()
    return content
    
  def generic(self):
    label = self.format('gen: opt error')
    ret_code = self.exec_process([self.file, '-0'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('gen: help')
    ret_code = self.exec_process([self.file, '-h'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('gen: version')
    ret_code = self.exec_process([self.file, '-v'], False)
    self.res.print(ret_code != 0, label)

  
  
  def files(self):
    label = self.format('files: no in error')
    ret_code = self.exec_process([self.file, '-o', 'file_not_found'], False)
    os.remove('file_not_found')
    self.res.print(ret_code == 0, label)

    label = self.format('files: no out error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-s', '1'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('files: nofile error')
    ret_code = self.exec_process([self.file], False)
    self.res.print(ret_code == 0, label)

    label = self.format('files: file in error')
    ret_code = self.exec_process([self.file, '-i', 'file_not_found'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('files: file out error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', '/file_not_found'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('files: even exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample], False)
    self.res.print(ret_code == 0, label)
    os.remove(self.temp_sample)

  def h2b_samples(self):
    label = self.format('h2b: limit1 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-l', 'az'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: limit2 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-l', '4294967295'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: limit3 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-l', '-1'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: start1 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-s', 'az'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: start2 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.temp_sample, '-s', '6'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: start3 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.temp_sample, '-s', '4294967295'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: start4 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.temp_sample, '-s', '-1'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('h2b: start warning')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1), '-o', self.temp_sample, '-s', '1', '-p'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('h2b: sample1 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1), '-o', self.temp_sample, '-s', '6', '-l', '47'])
    self.res.print(ret_code != 0, label)

    label = self.format('h2b: sample1 compare')
    result = self.read_temp_file()
    self.res.print(result.decode('utf-8') != RESULT1, label)
    os.remove(self.temp_sample)

    label = self.format('h2b: sample2 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE2), '-o', self.temp_sample, '-l', '47'])
    self.res.print(ret_code != 0, label)

    label = self.format('h2b: sample2 compare')
    result = self.read_temp_file()
    self.res.print(result.decode('utf-8') != RESULT2, label)
    os.remove(self.temp_sample)

    label = self.format('h2b: sample3 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-s', '1'])
    self.res.print(ret_code != 0, label)

    label = self.format('h2b: sample3 compare')
    result = self.read_temp_file()
    self.res.print(result.hex() != RESULT3, label)
    os.remove(self.temp_sample)

    label = self.format('h2b: sample4 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.temp_sample, '-s', '1'], False)
    self.res.print(ret_code == 0, label)
    os.remove(self.temp_sample)

    label = self.format('h2b: print exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-p'], False)
    self.res.print(ret_code != 0, label)
    os.remove(self.temp_sample)

    label = self.format('h2b: extract exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.temp_sample, '-e'], False)
    self.res.print(ret_code != 0, label)
    os.remove(self.temp_sample)

  def intelhex(self):
    label = self.format('ihex: no dir')
    ret_code = self.exec_process([self.file, '--ihex'], False)
    self.res.print(ret_code == 0, label)
    
    label = self.format('ihex: invalid dir')
    ret_code = self.exec_process([self.file, '--ihex', 'dir'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('ihex: offset1 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1_HEX), '-o', self.temp_bin1, '--offset', 'az'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('ihex: offset2 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1_HEX), '-o', self.temp_bin1, '--offset', '4294967295'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('ihex: h2b ori hex1')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1_HEX), '-o', self.temp_bin1, '--ihex', 'h2b'], False)
    self.res.print(ret_code != 0, label)
    label = self.format('ihex: h2b ori hex1 md5')
    self.res.print(self.md5(self.get_sample(SAMPLE1_BIN)) != self.md5(self.temp_bin1), label)

    label = self.format('ihex: b2h ori bin1')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1_BIN), '-o', self.temp_hex, '--ihex', 'b2h', '--offset', '0x08000000', '--linear', '0x080002c5'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('ihex: h2b gen hex1')
    ret_code = self.exec_process([self.file, '-i', self.temp_hex, '-o', self.temp_bin2, '--ihex', 'h2b'], False)
    self.res.print(ret_code != 0, label)
    label = self.format('ihex: h2b gen bin1 md5')
    self.res.print(self.md5(self.temp_bin1) != self.md5(self.temp_bin2), label)

    label = self.format('ihex: h2b ori hex2')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE2_HEX), '-o', self.temp_bin1, '--ihex', 'h2b'], False)
    self.res.print(ret_code != 0, label)
    label = self.format('ihex: h2b ori hex2 md5')
    self.res.print(self.md5(self.get_sample(SAMPLE2_BIN)) != self.md5(self.temp_bin1), label)

    label = self.format('ihex: b2h ori bin2')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE2_BIN), '-o', self.temp_hex, '--ihex', 'b2h', '--offset', '0x08000000', '--linear', '0x080002c5', '--padding_width', "32"], False)
    self.res.print(ret_code != 0, label)

    label = self.format('ihex: h2b gen hex2')
    ret_code = self.exec_process([self.file, '-i', self.temp_hex, '-o', self.temp_bin2, '--ihex', 'h2b'], False)
    self.res.print(ret_code != 0, label)
    label = self.format('ihex: h2b gen bin2 md5')
    self.res.print(self.md5(self.temp_bin1) != self.md5(self.temp_bin2), label)

    if os.path.exists(self.temp_hex):
      os.remove(self.temp_hex)
    if os.path.exists(self.temp_bin1):
      os.remove(self.temp_bin1)
    if os.path.exists(self.temp_bin2):
      os.remove(self.temp_bin2)


  def print_end(self):
    print(f'Total of tests \033[1m{self.res.get_count():02d}\033[0m')
    print(f'- \033[32mPASSED\033[0m \033[1m{self.res.get_passed():2d}\033[0m')
    print(f'- \033[31mFAILED\033[0m \033[1m{self.res.get_failed():2d}\033[0m')

def main() -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument('-f', '--file', help='Binary file.')
  args = parser.parse_args()
  if args.file == None:
    print('Unspecified file name')
    return 1
  if not os.path.exists(args.file):
    print('File {0} not found'.format(args.file))
    return 1
    
  tu = TU(args.file)
  tu.generic()
  tu.files()
  tu.h2b_samples()
  tu.intelhex()
  tu.print_end()
  sample = tu.get_sample(SAMPLE_TEMP)
  if os.path.exists(sample):
    os.remove(sample)
  return 0
  
if __name__ == '__main__':
  sys.exit(main())
