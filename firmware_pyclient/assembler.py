import os

def assemble(src):
    out_file = "a.out"
    ret = os.system(f"./bin/vasm6502_oldstyle -dotdir -Fbin -o {out_file} {src}")
    if ret != 0:
        return (False, None)

    with open(out_file, mode='rb') as file:
        assembled = file.read()

    os.remove(out_file)
    return (True, assembled)