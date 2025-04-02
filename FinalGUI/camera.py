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


    image_pts = np.array([[130.37, 131.4], [111.75, 114.64], [92.21, 95.61], [74.04, 76.13], 
                          [54.51, 57.55], [34.52, 39.42], [14.54, 24.47], 
                          [15.44, 130.05], [128.10, 24.92], [74.04, 130.95], [137.2, 59.8], [14.54, 75.67], [72.23, 23.56]], dtype=np.float32)
    world_pts = np.array([[0, 0, 0], [1, 1, 0], [2, 2, 0], [3, 3, 0], 
                          [4, 4, 0], [5, 5, 0], [6, 6, 0], 
                          [6, 0, 0], [0, 6, 0], [3, 0, 0], [0, 3, 0], [6, 3, 0], [3, 6, 0]], dtype=np.float32)


    #image_pts = np.array([[124.9, 125.06], [69.5, 68.4], [14.1, 14.95], [9.99, 120.1], 
    #                      [127.2, 19.0]], dtype=np.float32)
    #worldpts = np.array([[0, 0, 0], [3, 3, 0], [6, 6, 0], [6, 0, 0], 
    #                      [0, 6, 0]], dtype=np.float32)

    # Compute homography
    H, _ = cv2.findHomography(image_pts, world_pts[:, :2])

    pt_3D = np.dot(H, np.array([pt_2D[0], pt_2D[1], 1]))
    pt_3D_normalized = pt_3D / pt_3D[2]
    pt_3D_rounded = np.round(pt_3D_normalized, 2)

    return ([pt_3D_rounded[0], pt_3D_rounded[1]])