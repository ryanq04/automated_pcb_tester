import numpy as np

# Example data points
y = np.array([0.8, 1.55, 2.2, 2.7])
x = np.array([60, 100, 150, 180])

# Compute best-fit slope and intercept
m, b = np.polyfit(x, y, 1)  # Degree 1 polynomial fit (linear)

print(f"Consistent slope: {m}")