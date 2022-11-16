#!/usr/bin/python3

# Testing framework for IFJ projects

"""
USER TESTING SCRIPT - RUN BUT DO NOT EDIT
"""

# Imports
import argparse
import os
import subprocess
import shutil
import re
import traceback

"""
CODE DEPENDENT ON THE PROJECT SPECIFICATION - changed every year
"""

# Common constants changed between years
PROJECT_YEAR = '22'
PROJECT_FILE_EXTENSION = 'php'
DEFAULT_NATIVE_INTERPRET = 'php8.1'
LINE_COMMENT_PREFIX = '//'

# List of extensions
EXTENSIONS = ['STRNUM', 'BOOLTHEN', 'CYCLES' 'FUNEXP', 'GLOBAL']

# Checks for native interpret - chenged every year
def CheckNativeInterpret (interpret, quiet):
    OutputPrint('checking native interpret', quiet)
    command = [interpret, '--version']
    EXPECTED_VERSION = 'PHP 8.1.9 (cli)'
    output = subprocess.check_output(command).decode('utf-8')
    version = re.match('PHP [0-9]+\.[0-9]+\.[0-9]+ \(cli\)', output)
    if version is None:
        raise Exception('Native interpreter is not valid. Output of command: \'' + ' '.join(command) + '\' is not a valid PHP version string')
    OutputPrint(DEFAULT_NATIVE_INTERPRET + ' interpret found in version \'' + version.group(0) + '\'', quiet)
    if version.group(0) != EXPECTED_VERSION:
        OutputPrint('Warning: Version of the native interpret \'' + version.group(0) + '\' is not the same as expected version on Merlin server \'' + EXPECTED_VERSION + '\'', quiet)

# preparation of native interpret activation - changed every year
def PrepareNativeCmd (test_code, interpret, tmp_dir):
    tmp_file = os.path.join(tmp_dir, TMP_NATIVE_FILE_NAME)
    template_file = os.path.join(tmp_dir, TMP_TEMPLATE_FILE_NAME)
    with open(tmp_file, 'w') as f:
        f.write(test_code)
    return [interpret, template_file, tmp_file]

"""
CODE INDEPENDENT ON THE PROJECT SPECIFICATION
"""

# Default argument values
DEFAULT_COMPILER_PATH = './ifj' + PROJECT_YEAR
DEFAULT_LOG_FILE = './log'
DEFAULT_TIMEOUT_NATIVE = 5
DEFAULT_TIMEOUT_IFJCODE = 5
DEFAULT_TIMEOUT_COMPILER = 5
DEFAULT_OUTPUT_FOLDER = './outputs'
DEFAULT_TEST_DIR = './tests'
DEFAULT_IFJCODE_INTERPRET = './ic' + PROJECT_YEAR + 'int'
DEFAULT_NATIVE_INCLUDE = './ifj' + PROJECT_YEAR + '.' + PROJECT_FILE_EXTENSION
DEFAULT_TMP_DIR = './tmp'

# Tmp file names
TMP_TEMPLATE_FILE_NAME = 'ifj' + PROJECT_YEAR + '.' + PROJECT_FILE_EXTENSION
TMP_NATIVE_FILE_NAME = 'in.' + PROJECT_FILE_EXTENSION
TMP_IFJCODE_FILE_NAME = 'out.ifjcode'

# Global variables
test_index = 0
test_id = ''
log = None
logEnable = True
process = None
last_phase = ''

def OutputPrint(message, quiet):
    if not quiet:
        print(message)

def Log (message):
    if logEnable:
        if log is None:
            print(message)
        else:
            log.write(message + '\n')

# Function to parse command line arguments
def ParseArgs ():
    # Define parser
    parser = argparse.ArgumentParser(description="run tests for IFJ project")

    # Define arguments for mode selection
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--mode-compile-only', '-mc', action='store_true', help='compile only mode runs only compiler for each test (no interpretation and no output checking)')
    group.add_argument('--mode-interpret-only', '-mi', action='store_true', help='interpret only mode runs only compiler and interpret for each test (no output checking)')
    group.add_argument('--mode-all', '-ma', action='store_true', help='all mode runs compiler and interpret for each test and then checks outputs with native language interpret. This option is default')

    # Define arguments for test selection
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--select', '-s', default=DEFAULT_TEST_DIR, help='single test file or test directory that should be run (relative to main tests folder). example: "lex/symbols". default: All tests in main tests directory are run')
    group.add_argument('--select-file', '-sf', help="path to a file that specifies multiple test files or tests directories to be run (each on separate line).")

    # Define commonly used arguments
    parser.add_argument('--quiet', '-q', action='store_true', help='disable prints to standart output except final results and logs if \'--log-output\' is used')
    parser.add_argument('--compiler', '-c', default=DEFAULT_COMPILER_PATH, help='path to the IFJ' + PROJECT_YEAR + ' language compiler (the IFJ project executable). default: ' + DEFAULT_COMPILER_PATH)
    parser.add_argument('--extensions', '-e', default='', help='list of implemented extensions. example: "' + EXTENSIONS[0] + ',' + EXTENSIONS[1] + '". default: No extension implemented. options: ' + ', '.join(EXTENSIONS))
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--log-file', '-lf', default=DEFAULT_LOG_FILE, help='path to the log file created by the testsuite (if file already exists, it will be deleted). default: ' + DEFAULT_LOG_FILE)
    group.add_argument('--log-output', '-lo', action='store_true', help='print logs to the standard output instead of a file')
    group.add_argument('--log-none', '-ln', action='store_true', help='do not print logs to output or a file')
    parser.add_argument('--log-success-output', '-ls', action='store_true', help='log interpret output from successfull tests')
    parser.add_argument('--save-ifjcode-all', '-a', action='store_true', help='stores compiler results (ifjcode files) for all tests (not only failed ones)')
    parser.add_argument('--timeout-native-interpret', '-tn', default=DEFAULT_TIMEOUT_NATIVE, type=int, help='specify maximum timeout for each run of native interpret in seconds (required to detect infinite run errors). defult = ' + str(DEFAULT_TIMEOUT_NATIVE))
    parser.add_argument('--timeout-ifjcode-interpret', '-ti', default=DEFAULT_TIMEOUT_IFJCODE, type=int, help='specify maximum timeout for each run of ifjcode interpret in seconds (required to detect infinite run errors). defult = ' + str(DEFAULT_TIMEOUT_IFJCODE))
    parser.add_argument('--timeout-compiler', '-tc', default=DEFAULT_TIMEOUT_COMPILER, type=int, help='specify maximum timeout for each run of ifj' + PROJECT_YEAR + ' compiler in seconds (required to detect infinite run errors). defult = ' + str(DEFAULT_TIMEOUT_COMPILER))
    parser.add_argument('--output-folder', '-o', default=DEFAULT_OUTPUT_FOLDER, help='path to the folder where compiler output (IFJ' + PROJECT_YEAR + 'code language programs) is stored for every test that fails on interpretation or checking (if folder already exists, it will be deleted). default: ' + DEFAULT_OUTPUT_FOLDER)

    # Define other arguments
    parser.add_argument('--native-interpret', default=DEFAULT_NATIVE_INTERPRET, help='command to execute native interpret for output checking. default: ' + DEFAULT_NATIVE_INTERPRET)
    parser.add_argument('--ifjcode-interpret', default=DEFAULT_IFJCODE_INTERPRET, help='command to execute IFJ' + PROJECT_YEAR + 'code interpret for compiler output interpretation. default: ' + DEFAULT_IFJCODE_INTERPRET)
    parser.add_argument('--native-include-file', default=DEFAULT_NATIVE_INCLUDE, help='path to the file that is required to be included in native language programs to execute ifj language. default: ' + DEFAULT_NATIVE_INCLUDE)
    parser.add_argument('--tmp-dir', default=DEFAULT_TMP_DIR, help='path to a temp directory that will be created to store temp files for tests. default: ' + DEFAULT_TMP_DIR)

    # Parse arguments from command line
    args = parser.parse_args()

    # Argument postprrocessing
    if not args.mode_compile_only and not args.mode_interpret_only and not args.mode_all:
        OutputPrint('setting default mode --mode-all', args.quiet)
        args.mode_all = True

    if args.select_file is not None:
        if not os.path.isfile(args.select_file):
            raise Exception('Test selection file\'' + args.select_file + '\' does not exists')
        OutputPrint('parsing test selection file', args.quiet)
        with open(args.select_file) as f:
            while True:
                line = f.readline()
                if line == '':
                    break
                line = line.strip()
                if not os.path.isfile(line) and not os.path.isdir(line):
                    raise Exception('\'' + line + '\' line in test selection file is not a valid test file or test directory')
                args.select.append(line)
    else:
        if not os.path.isfile(args.select) and not os.path.isdir(args.select):
            raise Exception('Test selection \'' + args.select + '\'  is not a valid test file or test directory')
        args.select = [args.select]

    if not os.path.isfile(args.compiler):
        raise Exception('The path \'' + args.compiler + '\' is not a valid compiler executable')

    if args.extensions != '':
        OutputPrint('parsing extensions', args.quiet)
        args.extensions = args.extensions.split(',')
        for ext in args.extensions:
            if ext not in EXTENSIONS:
                raise Exception('Invalid extension \'' + ext + '\'')
    else:
        args.extensions = []

    if args.log_output or args.log_none:
        pass
    elif os.path.isfile(args.log_file):
        OutputPrint('removing old log file \'' + args.log_file + '\'', args.quiet)
        os.remove(args.log_file)
    if os.path.isdir(args.log_file):
        raise Exception('There is a directory with the same name as specified log file \'' + args.log_file + '\'')

    if args.timeout_native_interpret <= 0:
        raise Exception('Value of native interpret timeout must be greater then zero, but is \'' + str(args.timeout_native_interpret) + '\'')
    if args.timeout_ifjcode_interpret <= 0:
        raise Exception('Value of ifjcode interpret timeout must be greater then zero, but is \'' + str(args.timeout_ifjcode_interpret) + '\'')
    if args.timeout_compiler <= 0:
        raise Exception('Value of compiler timeout must be greater then zero, but is \'' + str(args.timeout_compiler) + '\'')

    if os.path.isfile(args.output_folder):
        raise Exception('There is a file with the same name as specified output folder \'' + args.output_folder + '\'')
    if os.path.isdir(args.output_folder):
        OutputPrint('removing old output folder \'' + args.output_folder + '\'', args.quiet)
        shutil.rmtree(args.output_folder)
    OutputPrint('creating output folder \'' + args.output_folder + '\'', args.quiet)
    os.mkdir(args.output_folder)

    if args.mode_all:
        CheckNativeInterpret(args.native_interpret, args.quiet)

    if args.mode_all or args.mode_interpret_only:
        OutputPrint('checking ifjcode interpret', args.quiet)
        if not os.path.isfile(args.ifjcode_interpret):
            raise Exception('The path \'' + args.ifjcode_interpret + '\' is not a valid ifjcode interpret executable')
        output = subprocess.check_output([args.ifjcode_interpret, '--help']).decode('utf-8')
        if output[:7] != 'BUILD: ':
            raise Exception('Ifjcode interpret is not valid. Command: \'' + args.ifjcode_interpret + ' --help\' didn\'t produce correct output')
        OutputPrint('ifjcode interpret found', args.quiet)

    if args.mode_all and not os.path.isfile(args.native_include_file):
        raise Exception('The path \'' + args.native_include_file + '\' is not a valid native include file')

    if os.path.isfile(args.tmp_dir):
        raise Exception('There is a file with the same name as specified tmp directory \'' + args.tmp_dir + '\'')
    if os.path.isdir(args.tmp_dir):
        OutputPrint('using exesting tmp directory  \'' + args.tmp_dir + '\'', args.quiet)
    else:
        OutputPrint('creating tmp directory  \'' + args.tmp_dir + '\'', args.quiet)
        os.mkdir(args.tmp_dir)
    if args.mode_all:
        shutil.copyfile(args.native_include_file, os.path.join(args.tmp_dir, TMP_TEMPLATE_FILE_NAME))

    return args

def ProcessTests(args):
    def ProcessTestFile(path):
        result = {}
        result['name'] = path
        result['code'] = None
        result['compile_only'] = False
        result['interpret_only'] = False
        result['compiler'] = []
        result['interpret'] = []
        result['extensions+'] = []
        result['extensions-'] = []
        result['input'] = None
        result['output'] = None
        result['native'] = False
        with open(path, 'r') as f:
            while True:
                line = f.readline().strip()
                if line.startswith(LINE_COMMENT_PREFIX + 'compiler '):
                    if result['compiler'] != []:
                        raise Exception('compiler pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['compiler'].append(int(item))
                elif line.startswith(LINE_COMMENT_PREFIX + 'interpret '):
                    if result['interpret'] != []:
                        raise Exception('interpret pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['interpret'].append(int(item))
                elif line.startswith(LINE_COMMENT_PREFIX + 'extensions+ '):
                    if result['extensions+'] != []:
                        raise Exception('extensions+ pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['extensions+'].append(int(item))
                elif line.startswith(LINE_COMMENT_PREFIX + 'extensions- '):
                    if result['extensions-'] != []:
                        raise Exception('extensions- pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['extensions-'].append(int(item))
                elif line.startswith(LINE_COMMENT_PREFIX + 'native'):
                    if result['native'] == True:
                        raise Exception('native pragma present multiple times in test header of test file \'' + path + '\'')
                    result['native'] = True
                elif line.startswith(LINE_COMMENT_PREFIX + 'input '):
                    if result['input'] is not None:
                        raise Exception('input pragma present multiple times in test header of test file \'' + path + '\'')
                    scenario = line.split()
                    if (len(scenario) != 2):
                        raise Exception('input pragma does not contain a single file')
                    inputFile = os.path.join(os.path.dirname(path), scenario[1])
                    if not os.path.isfile(inputFile):
                        raise Exception('input file \'' + inputFile + '\' for test \'' + path + '\' is not valid file')
                    with open(inputFile, 'r') as inputF:
                        content = inputF.read()
                    result['input'] = content
                elif line.startswith(LINE_COMMENT_PREFIX + 'output '):
                    if result['output'] is not None:
                        raise Exception('output pragma present multiple times in test header of test file \'' + path + '\'')
                    scenario = line.split()
                    if (len(scenario) != 2):
                        raise Exception('output pragma does not contain a single file')
                    outputFile = os.path.join(os.path.dirname(path), scenario[1])
                    if not os.path.isfile(outputFile):
                        raise Exception('output file \'' + outputFile + '\' for test \'' + path + '\' is not valid file')
                    with open(outputFile, 'r') as outputF:
                        content = outputF.read()
                    result['output'] = content
                elif line == LINE_COMMENT_PREFIX:
                    content = ''
                    while True:
                        line = f.readline()
                        if line == '':
                            break
                        content += line
                    result['code'] = content
                    break
                elif line.startswith(LINE_COMMENT_PREFIX):
                    continue
                else:
                    raise Exception('invalid test header in test file \'' + path + '\'')

        for ext in result['extensions+']:
            if ext not in EXTENSIONS:
                raise Exception('Unrecognized extension \'' + ext + '\' in test header of test file \'' + path + '\'')
        for ext in result['extensions-']:
            if ext not in EXTENSIONS:
                raise Exception('Unrecognized extension \'' + ext + '\' in test header of test file \'' + path + '\'')
        if (result['compiler'] == [0] and result['interpret'] == [] and result['native'] == False):
            result['compile_only'] = True
        if (result['interpret'] == [0] and result['native'] == False):
            result['interpret_only'] = True
        if result['compiler'] == []:
            result['compiler'].append(0)
        if result['interpret'] == []:
            result['interpret'].append(0)
        if result['code'] is None:
            raise Exception('No test code loaded from test file \'' + path + '\'')
        if result['input'] is None:
            result['input'] = ''

        return result

    result = []
    for item in args.select:
        if os.path.isfile(item) and item.endswith('.' + PROJECT_FILE_EXTENSION):
            result.append(ProcessTestFile(item))
        elif os.path.isdir(item):
            for directory, _, files in os.walk(item):
                for f in files:
                    if f.endswith('.' + PROJECT_FILE_EXTENSION):
                        result.append(ProcessTestFile(os.path.join(directory, f)))
        else:
            raise Exception('\'' + item + '\' is not a test file or a test directory')

    for i in range(len(result)):
        for j in range(i+1, len(result)):
            split_a = result[i]['name'].split(':')
            split_b = result[j]['name'].split(':')
            if os.path.basename(split_a[0]) == os.path.basename(split_b[0]):
                if len(split_a) > 1 and len(split_b) > 1 and split_a[1] == split_b[1]:
                    raise Exception('Test files \'' + result[i]['name'] + '\' and \'' + result[j]['name'] + '\' have the same name')


    OutputPrint('Total tests selected: \'' + str(len(result)) + '\'', args.quiet)
    return result

# Run test on native interpret
def RunNative(test_code, program_input, interpret, tmp_dir, timeout):
    global process
    global last_phase
    last_phase = 'compiler'
    # Execute test on native interpret
    cmd = PrepareNativeCmd(test_code, interpret, tmp_dir)
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=program_input.encode(), timeout=timeout)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Run test on ifj language compiler
def RunIfjcomp(test_code, compiler, timeout):
    global process
    global last_phase
    last_phase = 'ifjcomp interpret'
    # Execute test on ifj language compiler
    process = subprocess.Popen(compiler, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=test_code.encode(), timeout=timeout)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Run interpret with intermediate code
def RunIclint(input_data, program_input, tmp_dir, interpret, timeout):
    global process
    global last_phase
    last_phase = 'ifj interpret'
    # Save intermediate code to file
    tmp_file = os.path.join(tmp_dir, TMP_IFJCODE_FILE_NAME)
    with open(tmp_file, 'w') as f:
        f.write(input_data.decode('utf-8'))
    # Execute code on interpret
    cmd = [interpret, tmp_file]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=program_input.encode(), timeout=timeout)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Check error code from ifj language compiler
def CheckCompilerError(process_info, error_code):
    # Check for correct error code
    if process_info['exit_code'] not in error_code:
	# Log error
        Log('Compiler error output:\n' + (process_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        error = 'Unexpected exit code of IFJ compiler. Actual: ' + str(process_info['exit_code']) + ' Expected: ' + str(error_code) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

# Check error code from ifj language interpret
def CheckInterpretError(process_info, error_code):
    if process_info['exit_code'] not in error_code:
	# Log error
        Log('Interpret error output:\n' + (process_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        error = 'Unexpected exit code of IFJ interpret. Actual: ' + str(process_info['exit_code']) + ' Expected: ' + str(error_code) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

# Check output from native interpret and ifj language interpret
def CheckSameOutputNative(ifj_language_info, native_info, log_success):
    # Check if native and ifj language interprets have the same exit code
    if ifj_language_info['exit_code'] != native_info['exit_code']:
	# Log error
        Log('Native interpretr error output:\n' + (native_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('IFJ language interpret error output:\n' + (ifj_language_info["stderr"].decode('utf-8') or '<empty>'))
        Log('----')
        error = 'Native and IFJ language interprets have different exit codes. Native: ' + str(native_info['exit_code']) + ' IFJ language: ' + str(ifj_language_info['exit_code']) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

    # Check standart output of native and ifj language interpret
    if (ifj_language_info['stdout'] != native_info['stdout']) and (ifj_language_info['stdout'] != native_info['stdout'].decode('utf-8')):
	# Log error
        Log('Native interpret error output:\n' + (native_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('IFJ language interpret error output:\n' + (ifj_language_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('Native interpret output:\n' + (native_info['stdout'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('IFJ language interpret output:\n' + (ifj_language_info['stdout'].decode('utf-8') or '<empty>'))
        Log('----')
        error = 'Native and IFJ language interprets have different outputs.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

    # Log successfull test output
    if log_success:
        Log('IFJ language interpret output:\n' + (ifj_language_info['stdout'].decode('utf-8') or '<empty>'))
        Log('----')

# Check output of the ifj language interpret against predefined output from the test
def CheckSameOutputConstant(ifj_language_info, output_text, log_success):
    # Check standart output of native and ifj language interpret
    if (ifj_language_info['stdout'] != output_text):
	# Log error
        Log('IFJ language interpret error output:\n' + (ifj_language_info['stderr'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('IFJ language interpret output:\n' + (ifj_language_info['stdout'].decode('utf-8') or '<empty>'))
        Log('----')
        Log('Expected output:\n' + (output_text or '<empty>'))
        Log('----')
        error = 'IFJ language interpret has different output than defined in the test.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

    # Log successfull test output
    if log_success:
        Log('IFJ language interpret output:\n' + (ifj_language_info['stdout'].decode('utf-8') or '<empty>'))
        Log('----')

def CheckExtensions(required, forbiden, extensions):
    for item in required:
        if item not in extensions:
            return False
    for item in forbiden:
        if item in extensions:
            return False
    return True

# Save intermetiate code
def SaveIfjcode(name, directory, data):
        parts = name.split(':')
        fileName = os.path.basename(parts[0])
        if len(parts) > 1:
            fileName += ':' + os.path.basename(parts[1])
        outputFileName = fileName + '.ifjcode'
        f = open(os.path.join(directory, outputFileName), 'w')
        f.write(data)
        f.close()

def RunTest(test, args):
    # Global variables must be accessed here
    global test_index
    global test_id
    test_index = test_index + 1
    test_id = test['name']
    # Log current test
    Log('\n********************\nTEST ' + str(test_index) + ': ' + test_id + '\n********************\n')
    if test['input'] is not None:
        Log('PROGRAM INPUT:')
        Log(test['input'])
        Log('----')
    # Check extensions
    if not CheckExtensions(test['extensions+'], test['extensions-'], args.extensions):
        Log('test skiped due to extensions missmatch')
        Log('Required extensions: ' ' '.join(test['extensions+']))
        Log('Forbiden extensions: ' ' '.join(test['extensions-']))
        Log('Implemented extensions: ' ' '.join(args.extensions))
        Log('----')
        Log('SKIPED')
        return False
    # Run ifj language compiler
    compiler_info = RunIfjcomp(test['code'], args.compiler, args.timeout_compiler)
    # Check compiler for error
    CheckCompilerError(compiler_info, test['compiler'])
    # End execution if tests are compile only
    if args.mode_compile_only or compiler_info['exit_code'] != 0:
        if args.mode_compile_only and compiler_info['exit_code'] == 0 and not test['compile_only']:
	    # Log warning about incomplete testing
            Log('WARNING: This test was not entirely completed, because of the MODE_COMPILE testing mode.')
            Log('         Interpretation and output checks were not run.')
            Log('----')
        Log('SUCCESS')
        return True

    # If this part fails, the intermedate code must be saved for further analysis
    try:
	# Run ifj language interpret
        ifj_language_info = RunIclint(compiler_info['stdout'], test['input'], args.tmp_dir, args.ifjcode_interpret, args.timeout_ifjcode_interpret)
	# Check interpret for error
        CheckInterpretError(ifj_language_info, test['interpret'])
        if test['output'] is not None:
            CheckSameOutputConstant(ifj_language_info, test['output'], args.log_success_output)
	# End execution if tests are compile and interpret only
        if args.mode_interpret_only or ifj_language_info['exit_code'] != 0:
            if args.mode_interpret_only and ifj_language_info['exit_code'] == 0 and test['native']:
		# Log warning about incomplete testing
                Log('WARNING: This test was not entirely completed, because of the MODE_INTERPRET testing mode.')
                Log('         Output checks were not run.')
                Log('----')
            Log('SUCCESS')
            return True

	# Run test on native interpret and compare results with ifj language interpret
        native_info = RunNative(test['code'], test['input'], args.native_interpret, args.tmp_dir, args.timeout_native_interpret)
        CheckSameOutputNative(ifj_language_info, native_info, args.log_success_output and test['output'] is None)
    except:
        SaveIfjcode(test['name'], args.output_folder, compiler_info['stdout'].decode('utf-8'))
        raise
    else:
        if args.save_ifjcode_all:
            SaveIfjcode(test['name'], args.output_folder, compiler_info['stdout'].decode('utf-8'))

    # Test successfull
    Log('SUCCESS')
    return True



# Main program

args = ParseArgs()
tests = ProcessTests(args)
OutputPrint('\n-------- RESULTS --------\n', args.quiet)
if args.log_none:
    logEnable = False
elif not args.log_output:
    log = open(args.log_file, 'w')
passed = 0
failed = 0
skiped = 0
failed_names = []
for test in tests:
    was_error = False
    try:
        result = RunTest(test, args)
    except subprocess.TimeoutExpired:
        Log('ERROR: test timeout (' + last_phase + ')')
        was_error = True
    except RuntimeError:
        was_error = True
    except Exception as error:
        OutputPrint(traceback.format_exc(), args.quiet)
        Log('ERROR: exception: ' + str(error))
        was_error = True

    if was_error:
        was_error = False
        if process is not None:
            process.terminate()
        failed = failed + 1
        failed_names.append(test['name'])
        OutputPrint(test['name'] + ': FAILED', args.quiet)
    elif result:
        passed = passed + 1
        OutputPrint(test['name'] + ': PASSED', args.quiet)
    else:
        skiped = skiped + 1
        OutputPrint(test['name'] + ': SKIPED', args.quiet)

if log is not None:
    log.close()
if os.path.isdir(args.tmp_dir):
    shutil.rmtree(args.tmp_dir)

if failed_names != []:
    OutputPrint('\n-------- FILED TESTS --------\n', args.quiet)
    for name in failed_names:
        OutputPrint(name, args.quiet)

print('\n-------- SUMMARY --------\n')
print('PASSED: ' + str(passed))
print('FAILED: ' + str(failed))
print('SKIPED: ' + str(skiped))
