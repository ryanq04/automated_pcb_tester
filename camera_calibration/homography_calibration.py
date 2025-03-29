import cv2
import numpy as np

# Image points (2D) and their real-world counterparts (3D but assumed to be on a plane)
image_pts = np.array([[449, 24], [458, 197], [286, 29], [139, 399], [292, 206]], dtype=np.float32)
world_pts = np.array([[0, 0, 0], [3, 0, 0], [0, 3, 0], [6, 6, 0], [3, 3, 0]], dtype=np.float32)

# Compute homography
H, _ = cv2.findHomography(image_pts, world_pts[:, :2])

# Convert new 2D points to 3D assuming Z=0 (lying on a plane)
def project_2D_to_3D(pt_2D, H):
    pt_3D = np.dot(H, np.array([pt_2D[0], pt_2D[1], 1]))
    pt_3D_normalized = pt_3D / pt_3D[2]
    print("after normalzie: ", pt_3D)
    
    pt_3D_rounded = np.round(pt_3D_normalized, 2)
    
    return pt_3D_rounded

new_3D_coord = project_2D_to_3D([131, 220], H)
print("Approximate 3D Coordinates:", new_3D_coord)
