import numpy as np
import cv2

def undistort_pixel_coordinates(x_distorted, y_distorted, K, dist_coeffs):
    """
    Undistort the distorted pixel coordinates.
    
    Args:
        x_distorted, y_distorted: Distorted pixel coordinates.
        K: Camera intrinsic matrix (3x3).
        dist_coeffs: Distortion coefficients (k1, k2, p1, p2, k3).
    
    Returns:
        Undistorted pixel coordinates (x_undistorted, y_undistorted).
    """
    # Convert to a 2D array (x, y)
    pixel_coords = np.array([[x_distorted, y_distorted]], dtype='float32')
    
    # Undistort the points using OpenCV's undistortPoints
    undistorted_points = cv2.undistortPoints(pixel_coords, K, dist_coeffs)
    
    # Return the undistorted 2D coordinates
    return undistorted_points[0][0]

def pixel_to_world(x_pixel, y_pixel, K, dist_coeffs, depth, R, T):
    """
    Convert 2D pixel coordinates to 3D world coordinates using intrinsic matrix and depth.
    
    Args:
        x_pixel, y_pixel: The 2D pixel coordinates from the image.
        K: Camera intrinsic matrix (3x3).
        dist_coeffs: Camera distortion coefficients (k1, k2, p1, p2, k3).
        depth: The depth (z-coordinate) for the pixel (in world units, such as meters).
    
    Returns:
        3D world coordinates (X_world, Y_world, Z_world).
    """
    # Step 1: Undistort the pixel coordinates
    x_undistorted, y_undistorted = undistort_pixel_coordinates(x_pixel, y_pixel, K, dist_coeffs)
    
    # Step 2: Convert the 2D undistorted pixel coordinates to normalized camera coordinates
    # Normalize using the intrinsic matrix K (invert the intrinsic matrix)
    inv_K = np.linalg.inv(K)
    normalized_coords = np.dot(inv_K, np.array([x_undistorted, y_undistorted, 1.0]))

    
    # Step 3: Multiply by depth to obtain the 3D coordinates in camera coordinates
    X_camera = normalized_coords[0] * depth
    Y_camera = normalized_coords[1] * depth
    Z_camera = depth
    
    camera_coords = np.array([X_camera, Y_camera, Z_camera])
    world_coords = np.dot(R, camera_coords) + T
    
    # Return the 3D world coordinates
    return world_coords[0], world_coords[1], world_coords[2]

def main():
    # Example usage
    K = np.array([
        [834.42, 0, 132.95],
        [0, 916.67, 185.22],
        [0, 0, 1]
    ])

    dist_coeffs = np.array([-2.216549133059442234e-01, 1.464474971085246047e+00, 
                            -7.200328998363824534e-03, -1.149629291304830566e-04, 
                            -5.665112447586057698e+00])  # Radial and tangential distortion coefficients
    
    R = np.array([
        [1, 0, 0],
        [0, -1, 0],
        [0, 0, -1]
    ])
    T = np.array([2.5/100, 1.3/100, 22.5 / 100])  # Camera at the origin (no translation)

    # Input pixel coordinates (distorted)
    #x_pixel = 0
    #y_pixel = 0

    # Known depth of the point in the camera's view (in meters)
    #depth = 5.0  # example depth in meters
    
    while(1):
    
    # Get input from the user
        x_pixel = float(input("Enter the x pixel coordinate (distorted): "))
        y_pixel = float(input("Enter the y pixel coordinate (distorted): "))
        # Known depth of the point in the camera's view (in meters)
        depth = 22.5 / 100

        # Get 3D world coordinates
        X_world, Y_world, Z_world = pixel_to_world(x_pixel, y_pixel, K, dist_coeffs, depth, R, T)
        print(f"3D World Coordinates: X={X_world*100}, Y={Y_world*100}, Z={Z_world*100}")

if __name__ == "__main__":
    main()