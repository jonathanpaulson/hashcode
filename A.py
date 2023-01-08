import os
import subprocess
subprocess.check_call(f'g++ -std=c++17 -O3 -Wall -Werror -Wextra -Wshadow -Wno-sign-compare -fsanitize=undefined -fsanitize=address A.cc -o A', shell=True)
total_score = 0
for input_file in os.listdir('data'):
    stdout = subprocess.check_output(f'./A < data/{input_file}', shell=True)
    score = int(stdout.strip())
    total_score += score
    print(input_file, score)
print(total_score)
