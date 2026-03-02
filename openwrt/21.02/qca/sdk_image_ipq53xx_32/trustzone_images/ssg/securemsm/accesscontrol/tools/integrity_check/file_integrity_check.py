import hashlib
import re

md5_info_re = re.compile("(/\*MD5:)([0-9a-fA-F]+)(\*/)")
md5_info_len = len("\n/*MD5:*/\n") + len(hashlib.md5().hexdigest())


def exists(env):
    return True


def generate(env):
    env.AddMethod(check_integrity_scons, "CheckFileIntegrity")


def check_integrity_scons(env, sources):
    for path in sources:
        if not check_integrity(path):
            print("Error: File {} is modified manually.".format(path))
            print("       Please modify the source data file and run the generation script.")
            exit(2)


def write_with_md5sum(file_path, content):
    hash_md5 = hashlib.md5()
    hash_md5.update(content)
    md5_info = "\n/*MD5:" + hash_md5.hexdigest() + "*/\n"
    with open(file_path, "w") as fd:
        fd.write(content)
        fd.write(md5_info)


def read_file_content(file_path):
    with open(file_path, "r") as fd:
        lines = fd.readlines()
        content = ""
        for line in lines:
            content += line.rstrip() + "\n"

        text = content[:-md5_info_len]
        md5_info = content[-md5_info_len:].strip()
        return (content, text, md5_info)


def rewrite_with_md5_sum(file_path):
    (content, text, md5_info) = read_file_content(file_path)
    m = md5_info_re.match(md5_info)
    if m is None:
        text = content
    write_with_md5sum(file_path, text)


def check_integrity(file_path):
    hash_md5 = hashlib.md5()
    (content, text, md5_info) = read_file_content(file_path)
    m = md5_info_re.match(md5_info)
    if m is None:
        print("Error: MD5 info not found in " + file_path)
        return False

    md5_hex = m.group(2)
    hash_md5.update(text)
    if md5_hex != hash_md5.hexdigest():
        print("Error: MD5 mismatch")
        print("       Found : " + md5_hex)
        print("       Actual: " + hash_md5.hexdigest())
        return False

    return True


def test():
    file_path = "../../cfg/855/hyp/ACAssignRulesInfo.c"
    check_integrity(file_path)
    rewrite_with_md5_sum(file_path)


#if __name__ == "__main__":
#    test()
