#!/usr/bin/env python3
import argparse, sys, os, subprocess

SAMPLE1 = 'sample1.txt'
SAMPLE2 = 'sample2.txt'
SAMPLE3 = 'sample3.txt'
SAMPLE4 = 'sample4.txt'
SAMPLE_TEMP = 'sample.txt.temp'
RESULT1 = '00000000004030000000000000203000'
RESULT2 = '00000000005030000000000000603000'
RESULT3 = '00000000005030000000000000603000'
MAX_CHAR_RESULT = 20

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
    self.sample_dir = self.get_sample(SAMPLE_TEMP)

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
    with open(self.sample_dir, 'rb') as f:
      content = f.read()
    return content
    
  def generic(self):
    label = self.format('opt error')
    ret_code = self.exec_process([self.file, '-0'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('help')
    ret_code = self.exec_process([self.file, '-h'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('version')
    ret_code = self.exec_process([self.file, '-v'], False)
    self.res.print(ret_code != 0, label)
    
  def samples(self):
    label = self.format('sample1 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1), '-o', self.sample_dir, '-s', '6', '-l', '47'])
    self.res.print(ret_code != 0, label)

    label = self.format('sample1 compare')
    result = self.read_temp_file()
    self.res.print(result.decode('utf-8') != RESULT1, label)
    os.remove(self.sample_dir)

    label = self.format('sample2 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE2), '-o', self.sample_dir, '-l', '47'])
    self.res.print(ret_code != 0, label)

    label = self.format('sample2 compare')
    result = self.read_temp_file()
    self.res.print(result.decode('utf-8') != RESULT2, label)
    os.remove(self.sample_dir)

    label = self.format('sample3 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-s', '1'])
    self.res.print(ret_code != 0, label)

    label = self.format('sample3 compare')
    result = self.read_temp_file()
    self.res.print(result.hex() != RESULT3, label)
    os.remove(self.sample_dir)

  def extra_limits(self):
    label = self.format('limit1 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-l', 'az'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('limit2 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-l', '4294967295'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('limit3 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-l', '-1'], False)
    self.res.print(ret_code == 0, label)

  def extra_starts(self):
    label = self.format('start1 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-s', 'az'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('start2 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.sample_dir, '-s', '6'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('start3 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.sample_dir, '-s', '4294967295'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('start4 error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.sample_dir, '-s', '-1'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('start warning')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE1), '-o', self.sample_dir, '-s', '1', '-p'], False)
    self.res.print(ret_code != 0, label)

  def extra(self):
    label = self.format('sample4 exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE4), '-o', self.sample_dir, '-s', '1'], False)
    self.res.print(ret_code == 0, label)
    os.remove(self.sample_dir)

    label = self.format('print exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-p'], False)
    self.res.print(ret_code != 0, label)
    os.remove(self.sample_dir)

    label = self.format('extract exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir, '-e'], False)
    self.res.print(ret_code != 0, label)
    os.remove(self.sample_dir)

    label = self.format('no in error')
    ret_code = self.exec_process([self.file, '-o', 'file_not_found'], False)
    os.remove('file_not_found')
    self.res.print(ret_code == 0, label)

    label = self.format('no out error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-s', '1'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('nofile error')
    ret_code = self.exec_process([self.file], False)
    self.res.print(ret_code == 0, label)

    label = self.format('file in error')
    ret_code = self.exec_process([self.file, '-i', 'file_not_found'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('file out error')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', '/file_not_found'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('even exec')
    ret_code = self.exec_process([self.file, '-i', self.get_sample(SAMPLE3), '-o', self.sample_dir], False)
    self.res.print(ret_code == 0, label)
    os.remove(self.sample_dir)
    
    self.extra_limits()
    self.extra_starts()
  
  def print_end(self):
    print(f'Total of tests \033[1m{self.res.get_count():02d}\033[0m')
    print(f'- \033[32mPASSED\033[0m \033[1m{self.res.get_passed():2d}\033[0m')
    print(f'- \033[31mFAILED\033[0m \033[1m{self.res.get_failed():2d}\033[0m')

def main() -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument('-f', '--file', help='Binary file.')
  parser.add_argument('-e', '--extra', action='store_true', help='Extra tests.')
  args = parser.parse_args()
  if args.file == None:
    print('Unspecified file name')
    return 1
  if not os.path.exists(args.file):
    print('File {0} not found'.format(args.file))
    return 1
    
  tu = TU(args.file)
  tu.generic()
  tu.samples()
  if args.extra:
    tu.extra()
  tu.print_end()
  sample = tu.get_sample(SAMPLE_TEMP)
  if os.path.exists(sample):
    os.remove(sample)
  return 0
  
if __name__ == '__main__':
  sys.exit(main())
