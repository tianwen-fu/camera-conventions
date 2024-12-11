from pathlib import Path
import json

import numpy as np
from camera_conventions.camera import Camera, parse_alicevision_camera
from camera_conventions.parser import parse_ini_convention

def main():
    conventions_path = Path(__file__).parent.parent / 'conventions'
    aliceCamera = parse_ini_convention(conventions_path / 'alicevision.ini')
    nersemble = parse_ini_convention(conventions_path / 'nersemble.ini')
    camera_param_path = Path('/nas/RobFacialPerformace/00/cameras')
    
    downsample_factor = 4
    intrinsics = None
    extrinsics = {}
    for camera_param_file in camera_param_path.glob('*.txt'):
        raw_cam_data = {}
        camera = parse_alicevision_camera(camera_param_file, aliceCamera, raw_cam_data)
        camera_name = camera_param_file.stem[len('camera'):]
        camera_name = camera_name.zfill(3)
        print(camera_name)
        f = raw_cam_data['focal_length'] / downsample_factor
        w = raw_cam_data['resolution'][0] / downsample_factor
        h = raw_cam_data['resolution'][1] / downsample_factor
        intrinsics = np.array([[f, 0., w/2], [0, f, h/2], [0, 0, 1]])
        extrinsics[camera_name] = camera.convert(nersemble).T.tolist()

    json.dump({'intrinsics': intrinsics.tolist(), 'world_2_cam': extrinsics}, open('/nas/data/rob_npn_ner/camera_params/00/camera_params.json', 'wt'))
    
if __name__ == '__main__':
    main()
    
    