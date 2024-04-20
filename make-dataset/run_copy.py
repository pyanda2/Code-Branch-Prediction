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
def find_end_of_if(lines: list, i: int) -> int:
    '''
    Starting at an "if" statement, finds its ending bracket.
    @param lines: list of lines in the file
    @param i: index of '{' after the "if" statement

    NOTE: Function assumes every '{' has a matching '}'. WILL FAIL ON EDGE CASE WHEN SINGLE, UNMATCHED BRACKET EXISTS
    '''
    assert(lines[i].lstrip().startswith("{"))

    net_l_brack_count = 1 
    while net_l_brack_count > 0:
        if i == len(lines) - 1:
            raise Exception("Unmatched '{' bracket(s), incorrect C++ code or smth went wrong")
        i += 1 
        l_brack_count = lines[i].count("{")
        r_brack_count = lines[i].count("}")

        net_l_brack_count += l_brack_count - r_brack_count

    # if net_l_brack_count < 0:
    #     raise Exception("Unmatched '}' bracket(s), incorrect C++ code or smth went wrong")
    return i # net_l_brack_count == 0

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
                    # example.append((indent * " ") + "{}\n")
                    # example.append((indent * " ") + "{}\n")
                    # example.append(code + "")
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
                        example.extend(lines[i-50:i])
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
    with open("ORIG/urls_original.txt", "r") as f:
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

        df = pandas.DataFrame(data, columns=["code"])
        print(df)

        df.to_parquet("dataset.parquet")
