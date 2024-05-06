import os
import subprocess
import sys
import time

import numpy as np
from scipy.stats import ttest_ind

PATH = ""
dirs = os.path.realpath(__file__).split("/")
for dir in dirs[:-2]:
    PATH += dir + "/"

# GT_PATH = PATH + "groundtruths/"
GEN_PATH = PATH + "generated/"
UNLABELED_PATH = PATH + "unlabeled/"
# print(GT_PATH, GEN_PATH)


def get_dir_paths_from_dir(project_path):
    return [
        project_path + "/" + name
        for name in os.listdir(project_path)
        if os.path.isdir(os.path.join(project_path, name))
    ]


def run_project(path, running_times, runs=1):
    # Change Dir
    os.chdir(path)
    # Get Project Parameters
    path_elems = path.split("/")
    project_name = path_elems[-1]
    type = path_elems[-2]
    # images = ["ocean", "mountain"]
    images = ["ocean", "forest", "mountain"]
    # Run run times

    os.system("make clean")
    os.system("make exec")

    for image in images:
        for run in range(runs):

            command = f"./bin/exec images/{image}.ppm -v 4 -h 4 -o {image}_carved.ppm"
            env = os.environ.copy()
            env["TIMEFORMAT"] = "%3U"
            result = subprocess.run(["/bin/bash", "-c", f"time {command}"], env=env, stderr=subprocess.PIPE).stderr

            if not f"{project_name}_{image}" in running_times:
                running_times[f"{project_name}_{image}"] = []

            utime = float(result.decode("utf-8").split()[0])
            running_times[f"{project_name}_{image}"].append(utime)

    return running_times


if __name__ == "__main__":
    if len(sys.argv) == 2:
        runs = int(sys.argv[1])
    else:
        runs = 1

    gen_code_paths = get_dir_paths_from_dir(GEN_PATH)
    # gt_code_paths = get_dir_paths_from_dir(GT_PATH)
    unlabeled_paths = get_dir_paths_from_dir(UNLABELED_PATH)

    running_times = {}
    for path in gen_code_paths:
        running_times = run_project(path, running_times, runs)

    # for path in gt_code_paths:
    #     running_times = run_project(path, running_times, runs)

    unlabeled_times = {}
    for path in unlabeled_paths:
        unlabeled_times = run_project(path, unlabeled_times, runs)

    for project in running_times.keys():
        unlabeled = unlabeled_times[project]
        generated = running_times[project]

        t, p = ttest_ind(unlabeled, generated)
        print(
            f"""{project}: 
              [ unlabeled: (mean={np.mean(unlabeled)}, std={np.std(unlabeled)}) {unlabeled}
                generated: (mean={np.mean(generated)}, std={np.std(generated)}) {generated}
                p-value: {p} ]"""
        )

        with open(f"{project}.csv", "a") as f:
            f.write("unlabeled, generated")
            for v1, v2 in zip(unlabeled_times[project], running_times[project]):
                f.write(f"{v1}, {v2}")
