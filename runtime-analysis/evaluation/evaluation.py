import os
import time
import sys

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
    images = ["ocean", "forest", "mountain"]
    # Run run times
    for image in images:
        for run in range(runs):
            os.system("make clean")
            os.system("make exec")
            curr_time = time.time()
            os.system(
                f"./bin/exec images/{image}.ppm -v 120 -h 120 -o {image}_carved.ppm"
            )
            end_time = time.time()

            if f"{project_name}_{image}" in running_times:
                running_times[f"{project_name}_{image}"][type == "groundtruths"].append(
                    (end_time - curr_time)
                )
            else:
                running_times[f"{project_name}_{image}"] = [[], 0]
                running_times[f"{project_name}_{image}"][type == "groundtruths"].append(
                    end_time - curr_time
                )

        # Compute Average
        # running_times[f"{project_name}_{image}"][type == "groundtruths"] /= runs

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
        print(
            f"""{project}: 
              [ unlabeled: {unlabeled_times[project][0]}
                generated: {running_times[project][0]} ]"""
        )

        with open(f"{project}.csv", "a") as f:
            f.write("unlabeled, generated")
            for v1, v2 in zip(unlabeled_times[project][0], running_times[project][0]):
                f.write(f"{v1}, {v2}")
