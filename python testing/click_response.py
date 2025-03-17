import tkinter as tk

class ClickableGrid(tk.Tk):
    def __init__(self, rows=10, cols=10, cell_size=50):
        super().__init__()
        self.rows = rows
        self.cols = cols
        self.cell_size = cell_size
        self.title("Clickable Grid")
        
        self.canvas = tk.Canvas(self, width=cols * cell_size, height=rows * cell_size)
        self.canvas.pack()
        
        self.draw_grid()
        self.canvas.bind("<Button-1>", self.on_click)

    def draw_grid(self):
        """Draws the grid lines on the canvas."""
        for i in range(self.cols + 1):
            x = i * self.cell_size
            self.canvas.create_line(x, 0, x, self.rows * self.cell_size, fill="black")
        
        for i in range(self.rows + 1):
            y = i * self.cell_size
            self.canvas.create_line(0, y, self.cols * self.cell_size, y, fill="black")
    
    def on_click(self, event):
        """Handles the click event and calculates the grid coordinates."""
        col = event.x // self.cell_size
        row = event.y // self.cell_size
        print(f"Clicked at: ({col}, {row})")
        self.highlight_cell(row, col)
    
    def highlight_cell(self, row, col):
        """Highlights the clicked cell by filling it with a color."""
        x1, y1 = col * self.cell_size, row * self.cell_size
        x2, y2 = x1 + self.cell_size, y1 + self.cell_size
        self.canvas.create_rectangle(x1, y1, x2, y2, fill="lightblue", outline="black")

if __name__ == "__main__":
    app = ClickableGrid()
    app.mainloop()
