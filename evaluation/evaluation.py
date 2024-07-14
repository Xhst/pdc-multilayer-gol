import subprocess
import time
import os

ROOT_DIR = os.path.dirname(os.path.abspath(__file__)) + "/.."

def run_executable(executable_path: str, params: list[str] = [], environment: dict[str, str] = {}, print_output: bool = True, cwd: str = None):
    
    extra = []
    if os.name == "nt":
        # Prepare the command to run on WSL
        executable_path = executable_path.replace("c:", "/mnt/c").replace("\\", "/")
        extra = ["wsl", "bash", "-c"]

    cmd = extra + [executable_path] + params
    
    print(f"Running command: {' '.join(cmd)}")

    # Measure execution time
    start_time = time.time()
    result = subprocess.run(cmd, env=environment, cwd=cwd, shell=True, capture_output=True, text=True)
    end_time = time.time()

    execution_time = end_time - start_time

    if result.returncode != 0 or 'DONE' not in result.stdout:
        raise RuntimeError(f"Command failed or did not complete: {result.stderr}")

    if print_output:
        print(f"Execution time: {execution_time:.2f} seconds")

    return execution_time


def mean_execution_time(executable_path, params: list[str] = [], environment: dict[str, str] = {}, repetitions: int = 10, print_output: bool = True, cwd: str = None):
    execution_times = [run_executable(executable_path, params, environment, print_output=False, cwd=cwd) for _ in range(repetitions)]
    mean_exec = sum(execution_times) / repetitions

    if print_output:
        print(f"Mean execution time: {mean_exec:.2f} seconds")

    return mean_exec