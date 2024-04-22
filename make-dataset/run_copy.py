import os
import random
from time import sleep
from urllib.request import urlretrieve

import pandas
from tqdm.auto import tqdm


VALID_FILE_EXTS = (
    ".c",
    ".cc",
    ".cpp",
    ".cxx",
    ".c++",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
    ".h++",
)
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



def add_example(dataset, example):
    code = "".join(example)
    dataset.append(code)

def add_examples(dataset, path):
    example = []
    cond_expression = False
    control_count = 0
    tag_count = 0
    with open(path, "r") as f:
        lines = list(f)
        for i, line in enumerate(lines):
            code = line.lstrip()
            indent = len(line) - len(code)
            if cond_expression:
                if code.startswith("{"):
                    if_end_idx = end_of_if(indent, lines, i)
                    example.extend(lines[i:if_end_idx+1]) # should contain if body
                    cond_expression = False
                    add_example(dataset, example)
                    example = []
                else:
                    example.append(line) # case without curly braces for if (I think), but Pranav is sus of this case's purpose
            elif len(example) == 0:
                if (code.startswith("if ") or code == "if"):
                    do_control = random.randrange(10) == 0
                    if "[[likely]]" in code or "[[unlikely]]" in code or do_control:
                        func_start_idx = find_start_of_function(lines, i)
                        # example.extend(lines[i-50:i])
                        example.extend(lines[func_start_idx:i])
                        if (lines[i][-1] == "\n"): # appending sentinel before newline
                            example.append(lines[i][:-1] + " // SENTINEL"+ "\n")
                        else: 
                            example.append(lines[i] + "// SENTINEL" + "\n") 
                        curr_indent = indent
                        cond_expression = True
                        last_control = do_control
                        if "[[likely]]" in code or "[[unlikely]]" in code:
                            tag_count += 1
                        else:
                            control_count += 1
    return (tag_count, control_count)


if __name__ == "__main__":
    random.seed(10)
    with open("urls.txt", "r") as f:
        data = []
        tag_total = 0
        control_total = 0

        for i, line in tqdm(enumerate(list(f))):
            url = line[:-1]
            if url.endswith(VALID_FILE_EXTS):
                try:
                    urlretrieve(url, "file.cc")
                    os.system("clang-format file.cc > file2.cc")
                except Exception as e:
                    pass
                else:
                    tag_count, control_count = add_examples(data, "file2.cc")
                    tag_total += tag_count
                    control_total += control_count
                    if i % 100 == 0:
                        print(f"{tag_total}, {control_total}")
                        print("****")

                    os.remove("file.cc")
                    os.remove("file2.cc")


        split_idx = int(0.9*len(data))

        data_train = data[:split_idx]
        data_test = data[split_idx:]

        df_train = pandas.DataFrame(data_train, columns=["code"]) 
        df_test = pandas.DataFrame(data_test, columns=["code"]) 

        df_train.to_parquet("dataset_train.parquet")
        df_test.to_parquet("dataset_test.parquet")