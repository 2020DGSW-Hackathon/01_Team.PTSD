

import atexit as _atexit
import os as _os
import platform as _platform
import sys as _sys
from ctypes.util import find_library as _find_library
from _sounddevice import ffi as _ffi

#! built path to my dlls (in my `bin` dir)
#! use _platform.architecture to infer whether to use 32 or 64-bit dll
try:
    _libname = 'libportaudio' + _platform.architecture()[0] + '.dll'
    _libname = _os.path.join('bin', _libname)
    _lib = _ffi.dlopen(_libname)
except OSError:
    if _platform.system() == 'Windows':  #! use Windows, not default 'Darwin'
        _libname = 'libportaudio' + _platform.architecture()[0] + '.dll'
    else:
        #! custom error
        raise OSError('PortAudio library not found! Make sure the system is Windows 64 or 32 bit and you have the '
                      'correct libportaudio dll saved in  site-packages')

from volume.audio_controller import AudioController
#import time as t
import sounddevice as sd
import numpy as np
#import ctypes

class Sound:
    def __init__(self):
        self.cv = 0

        #self.down = 0xAE
        #self.up = 0xAF
        #self.user = ctypes.windll.user32

        self.ac = AudioController()
        self.initial_volume = self.ac.volume
        self.previous_volume = self.initial_volume
        self.current_volume = self.initial_volume

        self.volume_speed = 0.1

        self.sensibility = 1.0

        self.up_range = 30
        self.down_range = 5  # 0 ~ +10

    def set_volume(self, volume):

        self.initial_volume = volume
        self.previous_volume = self.initial_volume
        self.current_volume = self.initial_volume

    def change(self):

        def print_sound(in_data, out_data, frames, time, status):
            volume_norm = np.linalg.norm(in_data) * 10
            present_wave.insert(0, volume_norm)

            if len(present_wave) > 50:

                sound_mean = self.sensibility * (np.mean(present_wave) - 10)
                sound_mean = max(-10, min(sound_mean, 10)) / 10
                # 특정 소리보다 클 시

                # -10 = 원래소리 - range
                # 0 = 원래소리
                # 10 = 원래소리 + range


                if sound_mean > 0.0:
                    sound_value = self.initial_volume + sound_mean * self.up_range
                else:
                    sound_value = self.initial_volume + sound_mean * self.down_range

                #diff_volume = sound_value - self.previous_volume

                #volume = self.previous_volume + diff_volume * self.volume_speed
                #self.previous_volume = volume

                self.ac.set_volume(max(0, min(100, int(sound_value))))
                print(sound_mean)

                if len(present_wave) > 100:
                    present_wave.pop()


        with sd.Stream(callback=print_sound):
            sd.sleep(30000)

if __name__ == "__main__":
    sound = Sound()
    bv = input('기본 음량을 입력해주세요\n')
    sound.initial_volume = int(bv)
    while True:
        present_wave = []
        #compare_wave = 3

        sound.change()