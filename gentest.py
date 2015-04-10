import os
import re
import subprocess
import sys
import threading

# Note that PS2HOSTNAME is expected to be set in env.

PS2CLIENT = "ps2client"
MAKE = "make"
TEST_ROOT = "tests/"
TIMEOUT = 10
RECONNECT_TIMEOUT = 10

tests_to_generate = [
    "cpu/ee/alu",
    "cpu/ee/branch",
    "cpu/ee/branchdelay",
]

class Command(object):
  def __init__(self, cmd):
    self.cmd = cmd
    self.process = None
    self.output = None
    self.timeout = False
    self.thread = None

  def start(self, capture=True):
    def target():
      self.process = subprocess.Popen(self.cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
      if capture:
        self.process.stdin.close()
        self.output = ""
        while True:
          line = self.process.stdout.readline()
          self.output += str(line)
          if line == "" or line == "-- TEST END" or line == "-- TEST END\r\n" or line == "-- TEST END\n":
            break
      self.finish()

    self.thread = threading.Thread(target=target)
    self.thread.start()

  def stop(self):
    if self.thread.is_alive():
      self.timeout = True
      self.finish()

      self.thread.join()

  def finish(self):
    try:
      self.process.terminate()
    except WindowsError:
      pass

  def run(self, timeout):
    self.start()
    self.thread.join(timeout)

def prepare_test(test, args):
  if not ("-k" in args or "--keep" in args):
    olddir = os.getcwd()
    os.chdir(TEST_ROOT + os.path.dirname(test))

    make_target = "all"
    if "-r" in args or "--rebuild" in args:
      make_target = "rebuild"

    make_result = os.system("%s MAKE=\"%s\" %s" % (MAKE, MAKE, make_target))
    os.chdir(olddir)

    # Don't run the test if make failed, let them fix it.
    if make_result > 0:
      sys.exit(make_result)

def gen_test(test, args):
  elf_path = TEST_ROOT + test + ".elf"
  elf_exists = os.path.exists(elf_path);
  irx_path = TEST_ROOT + test + ".irx"
  irx_exists = os.path.exists(irx_path)

  if not elf_exists and not irx_exists:
    print("You must compile the test into a ELF first (" + elf_path + ")")
    return False

  # Seems like the PS2 can hang if it's not reset, let's just always reset for now.
  c = Command([PS2CLIENT, "reset"])
  c.run(RECONNECT_TIMEOUT)

  # Okay, time to run the command.
  if elf_exists:
    c = Command([PS2CLIENT, "-t", str(TIMEOUT), "execee", "host:" + elf_path] + args)
  else:
    # For some reason, it says "invalid IOP module" with less than one extra arg.
    c = Command([PS2CLIENT, "-t", str(TIMEOUT), "execiop", "host:" + irx_path, "host:" + irx_path] + args)
  c.run(TIMEOUT)
  output = c.output

  if not re.search(r"^-- TEST END\s*$", output, re.MULTILINE):
    print(output)
  else:
    # Strip out debug output from ps2link, etc.
    output = re.sub(r"\A[^\Z]+?-- TEST BEGIN", "-- TEST BEGIN", output, re.MULTILINE)
    output = re.sub(r"\n-- TEST END\s*\n[^\Z]+\Z", "\n-- TEST END\n", output, re.MULTILINE)
    output = re.sub(r"\r\n", "\n", output)
    # IOP seems to give an extra pair of \r\ns on Windows.
    output = re.sub(r"\r\n", "\n", output)
    return output

  return False

def gen_test_expected(test, args):
  print("Running test " + test + " on the PS2...")
  prepare_test(test, args)
  result = gen_test(test, args)

  expected_path = TEST_ROOT + test + ".expected"
  if result != False:
    # Normalize line endings on windows to avoid spurious git warnings.
    open(expected_path, "wt").write(result)
    print("Expected file written: " + expected_path)
    return True

  return False

def main():
  tests = []
  args = []
  for arg in sys.argv[1:]:
    if arg[0] == "-":
      args.append(arg)
    else:
      tests.append(arg.replace("\\", "/"))

  if not tests:
    tests = tests_to_generate

  if "-h" in args or "--help" in args:
    print("Usage: %s [options] cpu/ee/alu cpu/ee/branch...\n" % (os.path.basename(sys.argv[0])))
    print("Tests should be found under %s and omit the .elf extension." % (TEST_ROOT))
    print("Automatically runs make in the test by default.\n")
    print("Options:")
    print("  -r, --rebuild         run make rebuild for each test")
    print("  -k, --keep            do not run make before tests")
    return

  for test in tests:
    gen_test_expected(test, args)

main()
