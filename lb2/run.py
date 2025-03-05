import os
import time
import subprocess

def compile_cpp_program():
    print("Компиляция C++ программы...")
    result = subprocess.run(["g++", "main.cpp", "-o", "up"])
    if result.returncode != 0:
        print("Ошибка при компиляции C++ программы")
        exit(1)
    else:
        print("C++ программа успешно скомпилирована")

def run_cpp_program():
    print("Запуск C++ программы...")
    input_file = input("Введите путь к входному файлу: ")
    sequence_length = input("Введите длину маскируемой последовательности (0 - весь файл): ")
    combination_length = input("Введите длину комбинации (1-4): ")
    result = subprocess.run(["./up"], input=f"{input_file}\n{sequence_length}\n{combination_length}\n", text=True)    
    if result.returncode != 0:
        print("Ошибка при запуске C++ программы")
    else:
        print("C++ программа завершена успешно")

def run_python_scripts():
    print("Запуск Python скриптов для обработки...")
    subprocess.run(["python3", "a.py"])
    subprocess.run(["python3", "p.py"])

def main():
    print("Текущая директория:", os.getcwd())
    compile_cpp_program()
    run_cpp_program()
    time.sleep(1)
    if os.path.exists("autocorrelation.csv") and os.path.exists("probabilities.csv"):
        run_python_scripts()
    else:
        print("Ошибка: Не удалось создать необходимые файлы для обработки.")

if __name__ == "__main__":
    main()
