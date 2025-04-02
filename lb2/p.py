import csv
import tkinter as tk
from tkinter import Canvas

def read_csv(filename):
    data = {}
    with open(filename, newline='', encoding='utf-8') as file:
        reader = csv.reader(file, delimiter=',')
        current_length = None
        
        for row in reader:
            if len(row) == 1 and "Длина последовательности:" in row[0]:
                current_length = int(row[0].split()[2].split('-')[0])
                data[current_length] = {}
            elif len(row) == 4 and current_length is not None:
                combination = row[0].strip()
                try:
                    prob_10 = float(row[1])
                    prob_1000 = float(row[2])
                    prob_1000000 = float(row[3])
                except ValueError:
                    continue
                data[current_length][combination] = [prob_10, prob_1000, prob_1000000]
    return data

def draw_histogram(data, length):
    window = tk.Tk()
    window.title(f"Гистограмма вероятностей для последовательности длины {length}")

    width, height = 1200, 600
    canvas = Canvas(window, width=width, height=height, bg="white")
    canvas.pack()

    x_offset = 100  
    y_offset = 500
    bar_width = 20
    space_between_bars = 10  
    group_spacing = 40  
    max_bar_height = 300  

    colors = ['#A9BCE0', '#E0B0A8', '#A8E0A0']
    
    max_prob = max(max(prob_list) for prob_list in data[length].values())

    canvas.create_line(50, 50, 50, y_offset, width=2)  
    canvas.create_line(50, y_offset, width - 50, y_offset, width=2)  

    for i in range(5):
        y = y_offset - (i * max_bar_height / 4)
        canvas.create_line(50, y, width - 50, y, fill="gray", dash=(2, 2))  
        canvas.create_text(30, y, text=f"{(i * max_prob / 4):.2f}", font=("Arial", 10))

    canvas.create_text(width // 2, 30, text=f"Последовательности длины {length}", font=("Arial", 14, 'bold'))

    for i, (combination, probs) in enumerate(data[length].items()):
        for j, prob in enumerate(probs):
            x_pos = x_offset + (i * (bar_width + space_between_bars) * 3) + (j * (bar_width + space_between_bars))
            canvas.create_rectangle(
                x_pos, y_offset - prob * max_bar_height / max_prob,
                x_pos + bar_width, y_offset,
                fill=colors[j], outline="black", width=2
            )
            canvas.create_text(x_pos + bar_width / 2, y_offset + 15, text=combination, font=("Arial", 10))
        
    window.mainloop()

def main():
    filename = "probabilities.csv"
    data = read_csv(filename)
    
    for length in sorted(data.keys()):
        draw_histogram(data, length)

if __name__ == "__main__":
    main()
