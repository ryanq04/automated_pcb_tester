import numpy as np
import cv2

def generate_dummy_image():
    return np.random.randint(0, 255, (144, 174), dtype=np.uint8)

def decode_ycbcr422(raw_data, rows=144, cols=174):
    img_ycbcr422 = np.frombuffer(raw_data, dtype=np.uint8).reshape((rows, cols // 2, 4))
    Y0, Cb, Y1, Cr = [img_ycbcr422[:, :, i].astype(np.float32) for i in range(4)]

    Y = np.zeros((rows, cols), dtype=np.float32)
    Y[:, 0::2] = Y0
    Y[:, 1::2] = Y1
    Cb = np.repeat(Cb, 2, axis=1)
    Cr = np.repeat(Cr, 2, axis=1)

    R = np.clip((Y + 1.402 * (Cr - 128)), 0, 255).astype(np.uint8)
    G = np.clip((Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128)), 0, 255).astype(np.uint8)
    B = np.clip((Y + 1.772 * (Cb - 128)), 0, 255).astype(np.uint8)

    return np.rot90(np.stack((G, R, B), axis=-1), 1)

def project_2D_to_3D(pt_2D):
    image_pts = np.array([
        [138.1, 144.2], [120.4, 97.4], [100.4, 78.8], [80.85, 59.8],
        [61.3, 42.1], [40.88, 25.37], [20.9, 10.87],
        [24.08, 115.55], [133.1, 9.51], [82.67, 114.64],
        [137.2, 59.8], [21.35, 62.5], [77.68, 9.51]
    ], dtype=np.float32)

    world_pts = np.array([
        [0, 0, 0], [1, 1, 0], [2, 2, 0], [3, 3, 0],
        [4, 4, 0], [5, 5, 0], [6, 6, 0],
        [6, 0, 0], [0, 6, 0], [3, 0, 0],
        [0, 3, 0], [6, 3, 0], [3, 6, 0]
    ], dtype=np.float32)

    H, _ = cv2.findHomography(image_pts, world_pts[:, :2])
    pt_3D = np.dot(H, np.array([pt_2D[0], pt_2D[1], 1]))
    pt_3D_normalized = pt_3D / pt_3D[2]
    return [round(pt_3D_normalized[0], 2), round(pt_3D_normalized[1], 2)]
