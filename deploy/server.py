import subprocess
import socket

def run_exe(exe_path):
    try:
        process = subprocess.Popen(
            exe_path,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True,
            text=True
        )

        stdout, stderr = process.communicate()

        result = ""
        if stdout:
            result += f"Output:\n{stdout}\n"
        if stderr:
            result += f"Errors:\n{stderr}\n"

        return result if result else "No output or errors."

    except FileNotFoundError:
        return f"Executable not found: {exe_path}"
    except Exception as e:
        return f"An error occurred: {e}"

def start_server(host='0.0.0.0', port=44444, exe_path='/Users/burdi/vorking/ctf/tasks_cutf/crypto_snow_3G/cpp_tasks/snow_1.0'):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((host, port))
        server_socket.listen(5)
        print(f"Server listening on {host}:{port}")

        while True:
            client_socket, client_address = server_socket.accept()
            with client_socket:
                print(f"Connection from {client_address}")
                output = run_exe(exe_path)
                client_socket.sendall(output.encode('utf-8'))

if __name__ == "__main__":
    start_server()
