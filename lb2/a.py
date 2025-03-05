import csv
import tkinter as tk
from tkinter import Canvas

def read_autocorrelation(filename):
    data = []
    
    with open(filename, newline='', encoding='utf-8') as file:
        reader = csv.reader(file, delimiter=',')
        next(reader)

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

    max_tau = max(shift for shift, _ in data)
    min_value = -1
    max_value = 1

    canvas.create_line(50, 50, 50, y_offset, width=2)
    canvas.create_line(50, y_offset, width - 50, y_offset, width=2)
    for i in range(-1, 2):
        y = y_offset - ((i + 1) * max_bar_height / 2)
        canvas.create_line(50, y, width - 50, y, fill="gray", dash=(2, 2))
        canvas.create_text(30, y, text=f"{i:.1f}", font=("Arial", 10))

    for shift, value in data:
        x_pos = 50 + shift * (width - 100) / max_tau
        y_pos = y_offset - ((value - min_value) / (max_value - min_value) * max_bar_height)

        canvas.create_oval(x_pos - point_size, y_pos - point_size, 
                           x_pos + point_size, y_pos + point_size, 
                           fill="black")

        canvas.create_text(x_pos, y_offset + 15, text=str(shift), font=("Arial", 8))

    window.mainloop()

def main():
    filename = "autocorrelation.csv"
    data = read_autocorrelation(filename)
    draw_autocorrelation(data)

if __name__ == "__main__":
    main()
