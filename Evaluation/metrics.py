import argparse
import os
import psutil
import subprocess
import time

#Time for line by line passwords
def get_function_metrics(hash_function, input_file):
    #Prepare output file
    hash_file_name = hash_function[max(0, hash_function.rfind(os.path.sep)+1):hash_function.rfind('.')]
    input_file_name = input_file[max(0, input_file.rfind(os.path.sep)+1):input_file.rfind('.')]
    output_file = f"{input_file_name}_hashed_with_{hash_file_name}.txt"
    print(f"Saving the results to: {output_file}")

    total_start_time = time.time()
    total_hashes = 0
    total_elapsed_time = 0;
    total_memory_usage = 0;
    total_memory_usage_count = 0;
    
    with open(input_file, "r") as infile, open(output_file, "w") as outfile:
        for line in infile:
            password = line.strip()
            if not password:
                continue  #skip blank lines
            max_memory_usage = -1;

            #Start the process
            hash_process = subprocess.Popen(
                [f"./{hash_function}"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            ps_mem = psutil.Process(hash_process.pid)

            hash_start_time = time.time()
            #Give the password to hash
            hash_process.stdin.write(password)
            hash_process.stdin.close()

            #Collect memory usage
            try:
               while hash_process.poll() is None:
                  memory_usage = ps_mem.memory_info().rss
                  max_memory_usage = max(max_memory_usage, memory_usage)
            except ps_mem.NoSuchProcess:
               print("Error: Issue occured while hashing: {password}")
            hash_end_time = time.time()

            #store the summary data
            elapsed = hash_end_time - hash_start_time
            total_elapsed_time += elapsed
            total_hashes += 1
            total_memory_usage += memory_usage
            if memory_usage > 0:
              total_memory_usage_count += 1
            
            stdout = hash_process.stdout.read()
            #Save result
            outfile.write(f"{password} -> {stdout.strip()} time: {elapsed:.6f}s mem usage: {memory_usage} bytes\n")

            #Print to console too
            #Commenting out for now, printing slows down execution time a lot
            #print(f"{password} -> {stdout.strip()} time: {elapsed:.6f}s mem usage: {memory_usage} bytes")

        total_end_time = time.time()
        total_elapsed = total_end_time - total_start_time

        #Save conclusion
        outfile.write(f"\nHashed {total_hashes} passwords with {hash_function}\n")
        outfile.write(f"Total Run Time: {total_elapsed:.6f}s\n")

    #Print conclusion
    print(f"Hashed {total_hashes} passwords with {hash_function}")
    print(f"Total Run Time: {total_elapsed:.6f}s")
    print(f"Total Time Hashing: {total_elapsed_time:.6f}s")
    print(f"Average Hash Time: {(total_elapsed_time/total_hashes):.6f}s")
    print(f"Average Memory Usage: {(total_memory_usage/total_memory_usage_count):.2f} bytes\n")

def valid_args(hfs, htf):
  for hf in hfs:
    if not os.path.isfile(hf) or not os.access(hf, os.X_OK):
      print(f"Error: \"{hf}\" is not a valid file")
      return False
  if not os.path.isfile(htf) or not os.access(htf, os.R_OK):
    print(f"Error: \"{htf}\" is not a valid file")
    return False
  return True

def main():
  #Parse the command line arguments
  parser = argparse.ArgumentParser()
  parser.add_argument('files', nargs="+", help='Hash function executables followed by input file')
  args = parser.parse_args()
  if len(args.files) < 2:
    print("Error: Not enough args\n\tUsage: <hash.exe> <hash_input.txt>")
  *hash_functions, hash_test_file = args.files
  if not valid_args(hash_functions, hash_test_file):
    print("Error: Invalid command line arguments")
    return
  
  #Show the metrics for each hash function passed in
  for hash_function in hash_functions:
    print(f"Metrics for {hash_function}:\n{'-'*30}")
    get_function_metrics(hash_function, hash_test_file)  
  
if __name__ == "__main__":
  main()