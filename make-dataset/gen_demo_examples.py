import os
import sys
import random


def find_start_of_function(lines: list, i: int) -> int:
    '''
    Starting at an "if"' statement, finds the start of the outermost function it is located in (no indent).
    @param lines: list of lines in the c file
    @param i: ndex of the '{' after the "if" statement
    '''

    # 1) iterate over upward lines, detect line with no indent
    # (optional, may cause issues) 2) check "assert(lines[i].lstrip().startswith("{"))"
    # 3) iterate upwards until find empty line (should be 1-4 lines, maybe set hard stop)
    while len(lines[i]) > len(lines[i].lstrip()):
        i -= 1
    # no indent (start of function, ideally) found
    # assert(lines[i].startswith("{"))

    # limit to 5 lines
    count = 0
    while bool(lines[i].strip()) and count <= 5: # while string not empty and less than 5 iterations
        i -= 1
        count += 1 
    return i

def end_of_if(indent: int, lines: list, i: int):
    while i < len(lines):
        code = lines[i].lstrip()
        curr_indent = len(lines[i]) - len(code)
        if curr_indent == indent and code.startswith("}"):
            return i
        i += 1
    return i

def gather_mp_examples(file: str) -> list[str]:
    new_file =  file[:-3]+"_clanged.cc" # assuming file extension is .cc
    os.system(f"clang-format {file} > {new_file}")
    data = []
    with open(new_file, "r") as f:
        lines = list(f)
        for i, line in enumerate(lines):
            code = line.lstrip()
            indent = len(line) - len(code) 
            if code.startswith("if"):
                sentinel_line = line.rstrip() + " // SENTINEL" + "\n"
                func_start_idx = find_start_of_function(lines, i)
                if_end_idx = end_of_if(indent, lines, i)
                prefix_str = "".join(lines[func_start_idx:i])
                suffix_str = "".join(lines[i+1:if_end_idx+1])
                data.append(prefix_str + sentinel_line + suffix_str)
    return data

def label_example(example: str, class_int: int) -> list[str]:
    class_str = "[[likely]]" if class_int == 1 else "[[unlikely]]"
    return example.replace("// SENTINEL", class_str)
    
if __name__ == "__main__":
    assert(len(sys.argv) == 2)
    data = gather_mp_examples(sys.argv[1])
    for datum in data:
        print("-------------------------")
        print(datum)
    random_class_ints = [random.randint(0,1) for i in range(len(data))]
    new_data = []
    for i in range(len(data)):
        new_datum = label_example(data[i], random_class_ints[i])
        # print('-------------------') 
        # print(new_datum)
        new_data.append(new_datum)
    
