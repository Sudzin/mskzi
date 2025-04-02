import csv
import tkinter as tk
from tkinter import Canvas

def read_autocorrelation(filename):
    data = []
    
    with open(filename, newline='', encoding='utf-8') as file:
        reader = csv.reader(file)
        next(reader)  # Пропуск заголовка

        for row in reader:
            print("Считанная строка из CSV (АКФ):", row)
            shift = int(row[0])
            value = float(row[1])
            data.append((shift, value))
    
    return data

def draw_autocorrelation(data):
    window = tk.Tk()
    window.title("Автокорреляционная функция (АКФ)")

    width, height = 800, 600
    canvas = Canvas(window, width=width, height=height, bg="white")
    canvas.pack()

    x_offset = 100
    y_offset = 500
    max_bar_height = 300
    point_size = 5

    # Получаем максимальный сдвиг для нормализации оси X
    max_shift = max(shift for shift, _ in data)
    min_value = -1
    max_value = 1

    # Рисуем оси
    canvas.create_line(50, 50, 50, y_offset, width=2)
    canvas.create_line(50, y_offset, width - 50, y_offset, width=2)

    # Линии для значений -1, 0, 1 на оси Y
    for i in range(-1, 2):
        y = y_offset - ((i + 1) * max_bar_height / 2)
        canvas.create_line(50, y, width - 50, y, fill="gray", dash=(2, 2))
        canvas.create_text(30, y, text=f"{i:.1f}", font=("Arial", 10))

    # Рисуем точки для каждого сдвига и значения автокорреляции
    for shift, value in data:
        # Нормализуем по оси X
        x_pos = 50 + shift * (width - 100) / max_shift
        
        # Нормализуем по оси Y (значение автокорреляции)
        y_pos = y_offset - ((value - min_value) / (max_value - min_value) * max_bar_height)

        # Рисуем точку на графике
        canvas.create_oval(x_pos - point_size, y_pos - point_size, 
                           x_pos + point_size, y_pos + point_size, 
                           fill="black")

        # Добавляем подпись с сдвигом под точкой
        canvas.create_text(x_pos, y_offset + 15, text=str(shift), font=("Arial", 8))

    window.mainloop()

def main():
    filename = "autocorrelation.csv"  # Путь к CSV файлу с результатами автокорреляции
    data = read_autocorrelation(filename)
    draw_autocorrelation(data)

if __name__ == "__main__":
    main()
