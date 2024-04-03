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
                    example.append((indent * " ") + "{}\n")
                    cond_expression = False
                    add_example(dataset, example)
                    example = []
                else:
                    example.append(line)
            elif len(example) == 0:
                if (code.startswith("if ") or code == "if"):
                    do_control = random.randrange(10) == 0
                    if "[[likely]]" in code or "[[unlikely]]" in code or do_control:
                        example.extend(lines[i-50:i+1])
                        curr_indent = indent
                        cond_expression = True
                        last_control = do_control
                        if "[[likely]]" in code or "[[unlikely]]" in code:
                            tag_count += 1
                        else:
                            control_count += 1
    return (tag_count, control_count)


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

    df = pandas.DataFrame(data, columns=["code"])
    print(df)
    df.to_parquet("dataset.parquet")
