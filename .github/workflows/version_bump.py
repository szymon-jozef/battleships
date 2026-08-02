import json
import sys

METADATA_FILE_PATH = "../../metadata.json"


def load_version() -> str:
    try:
        with open(METADATA_FILE_PATH, "r") as f:
            return str(json.load(f)["version"])
    except FileNotFoundError as e:
        print(f"Error: {e}")
        return ""


def save_version(version: str):
    data = None
    with open(METADATA_FILE_PATH, "r") as f:
        data = json.load(f)

    data["version"] = version

    with open(METADATA_FILE_PATH, "w") as f:
        f.write(json.dumps(data, indent=4))


def main() -> None:
    release_type = sys.argv[1]
    version: str = load_version()
    if version == "":
        return

    version_nums: list[str] = version.split(".")
    try:
        major_num: int = int(version_nums[0])
        minor_num: int = int(version_nums[1])
        patch_num: int = int(version_nums[2])
    except ValueError as e:
        print(f"Error: {e}")
        return

    match release_type:
        case "patch":
            patch_num += 1
        case "minor":
            minor_num += 1
            patch_num = 0
        case "major":
            major_num += 1
            minor_num = 0
            patch_num = 0
        case _:
            print("Wrong release type")
            return

    new_version: str = f"{major_num}.{minor_num}.{patch_num}"
    save_version(new_version)


if __name__ == "__main__":
    main()
