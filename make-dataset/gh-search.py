import os
import time

from dotenv import load_dotenv
from github import Auth, Github


def get_results(keyword, filepath):
    with open(filepath, "w") as f:
        for i in range(0, 100, 2):
            lower = i * 1000
            upper = lower + 1999
            print(f"{lower}..{upper}")
            likely = g.search_code(keyword, size=f"{lower}..{upper}")
            print(likely.totalCount)
            for j, result in enumerate(likely):
                print(result.download_url)
                f.write(result.download_url + "\n")
                if j % 10 == 0:
                    time.sleep(0.1)


load_dotenv()
auth = Auth.Token(os.environ.get("GH_TOKEN", None))
with Github(auth=auth) as g:
    get_results("[[likely]]", "likely.txt")
    # get_results("[[unlikely]]", "unlikely.txt")
    print("Done!")
