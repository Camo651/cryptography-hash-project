import argparse
import os
import subprocess
import time

#Time for whole document
"""def get_function_metrics(hash_function, input_file):
  #Hash the contents of the input file and store the metrics
  hash_start_time = time.time()
  result = subprocess.run([f"./{hash_function}", input_file], stdout=subprocess.PIPE, text=True)
  hash_end_time = time.time()

  #Print and save the results
  print(f"Output: \n{result.stdout}")
  print(f"Execution Time: {hash_end_time - hash_start_time:.3f}s\n")
  with open(f"{input_file[:input_file.find('.')]}_hashed_with_{hash_function[:hash_function.find('.')]}.txt", "w") as output:
    output.write(result.stdout)
    """

#Time for line by line passwords
def get_function_metrics(hash_function, input_file):
    #Prepare output file
    output_file = f"{input_file[:input_file.find('.')]}_hashed_with_{hash_function[:hash_function.find('.')]}".replace(".", "_") + ".txt"

    total_start_time = time.time()
    total_hashes = 0

    with open(input_file, "r") as infile, open(output_file, "w") as outfile:
        for line in infile:
            password = line.strip()
            if not password:
                continue  #skip blank lines

            #Time each individual hash
            hash_start_time = time.time()
            result = subprocess.run(
                [f"./{hash_function}"],
                input=password,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            hash_end_time = time.time()

            elapsed = hash_end_time - hash_start_time
            total_hashes += 1

            #Save result
            outfile.write(f"{password} -> {result.stdout.strip()} time: {elapsed:.6f}s\n")

            #Print to console too
            print(f"{password} -> {result.stdout.strip()} time: {elapsed:.6f}s")

        total_end_time = time.time()
        total_elapsed = total_end_time - total_start_time

        #Save conclusion
        outfile.write(f"\nHashed {total_hashes} passwords with {hash_function}\n")
        outfile.write(f"Total Run Time: {total_elapsed:.6f}s\n")

    #Print conclusion
    print(f"\nHashed {total_hashes} passwords with {hash_function}")
    print(f"Total Run Time: {total_elapsed:.6f}s")

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