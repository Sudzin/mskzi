import csv
import tkinter as tk
from tkinter import Canvas

def read_csv(filename):
    data = {}
    with open(filename, newline='', encoding='utf-8') as file:
        reader = csv.reader(file, delimiter=',')
        next(reader)

        for row in reader:
            print("Считанная строка из CSV:", row)
            if len(row) != 4:
                print("Пропущена строка (некорректный формат):", row)
                continue
            try:
                length = int(row[0])
            except ValueError:
                print(f"Ошибка при преобразовании длины: {row[0]}")
                continue
            combination = row[1].strip()
            try:
                before = float(row[2])
                after = float(row[3])
            except ValueError:
                print(f"Ошибка при преобразовании вероятностей: {row[2]}, {row[3]}")
                continue

            if length not in data:
                data[length] = []

            data[length].append((combination, before, after))
            
    for length in data:
        data[length].sort(key=lambda x: x[0])

    print("Прочитанные данные (отсортированные):", data)
    return data

def draw_histogram(data):
    window = tk.Tk()
    window.title("Гистограмма вероятностей")

    width, height = 1200, 600
    canvas = Canvas(window, width=width, height=height, bg="white")
    canvas.pack()

    x_offset = 100  
    y_offset = 500
    bar_width = 30
    space_between_bars = 15  
    group_spacing = 50  
    max_bar_height = 300  

    before_color = '#A9BCE0'
    after_color = '#E0B0A8'

    max_prob = max(max(before, after) for values in data.values() for _, before, after in values)

    canvas.create_line(50, 50, 50, y_offset, width=2)  
    canvas.create_line(50, y_offset, width - 50, y_offset, width=2)  

    for i in range(5):
        y = y_offset - (i * max_bar_height / 4)
        canvas.create_line(50, y, width - 50, y, fill="gray", dash=(2, 2))  
        canvas.create_text(30, y, text=f"{(i * max_prob / 4):.2f}", font=("Arial", 10))

    for length, values in data.items():
        canvas.create_text(x_offset - 50, 30, text=f"Длина {length}", font=("Arial", 12, 'bold'))

        for i, (combination, before, after) in enumerate(values):
            x_pos = x_offset + i * (bar_width * 2 + space_between_bars + group_spacing)

            canvas.create_rectangle(
                x_pos, y_offset - before * max_bar_height / max_prob,
                x_pos + bar_width, y_offset,
                fill=before_color, outline="black", width=2
            )

            canvas.create_rectangle(
                x_pos + bar_width + space_between_bars, y_offset - after * max_bar_height / max_prob,
                x_pos + 2 * bar_width + space_between_bars, y_offset,
                fill=after_color, outline="black", width=2
            )

            canvas.create_text(x_pos + bar_width / 2, y_offset + 15, text=combination, font=("Arial", 10))

        x_offset += len(values) * (bar_width * 2 + space_between_bars + group_spacing)

    window.mainloop()

def main():
    filename = "probabilities.csv"
    data = read_csv(filename)
    draw_histogram(data)

if __name__ == "__main__":
    main()
