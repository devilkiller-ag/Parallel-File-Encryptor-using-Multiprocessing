import os
import string
import random

def make_test_files(path, num_files=10):
    os.makedirs(path, exist_ok=True)
    os.chdir(path)
    for i in range(num_files):
        filename = f"test_file_{i+1}.txt"
        with open(filename, "w") as file:
            random_string = "".join(random.choices(string.ascii_uppercase + string.digits, k=1000))
            file.write(random_string)

if __name__ == "__main__":
    make_test_files("test")