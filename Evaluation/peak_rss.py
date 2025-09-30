import argparse, os, subprocess, resource, sys

def peak_rss_child_kb():
    return resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss

def run_once(binary, input_path):
    # read all inputs and feed one process
    with open(input_path, "rb") as f:
        before_kb = peak_rss_child_kb()
        p = subprocess.Popen([f"./{binary}"], stdin=subprocess.PIPE,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        # stream stdin to child
        for line in f:
            p.stdin.write(line)
        p.stdin.close()
        p.wait()
        after_kb = peak_rss_child_kb()
    peak_kb = max(0, after_kb - before_kb)  # child’s own peak
    print(f"{binary}: peak RSS ≈ {peak_kb*1024} bytes")  # bytes on Linux
    return peak_kb

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("binary")
    ap.add_argument("input_file")
    args = ap.parse_args()
    run_once(args.binary, args.input_file)
