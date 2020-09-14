"""
IAudioEndpointVolumeCallback.OnNotify() example.
The OnNotify() callback method gets called on volume change.
"""
from __future__ import print_function

from ctypes import POINTER, cast
from threading import Thread
from comtypes import CLSCTX_ALL, COMObject
import volume.audio_controller as audio_controller

from pycaw.pycaw import (AudioUtilities, IAudioEndpointVolume,
                         IAudioEndpointVolumeCallback)


class AudioEndpointVolumeCallback(COMObject):
    _com_interfaces_ = [IAudioEndpointVolumeCallback]

    def OnNotify(self, pNotify):
        print('OnNotify callback') #Message to be read on volume change


callback = AudioEndpointVolumeCallback()

def start():
    audio_controller.volume.RegisterControlChangeNotify(callback)

def end():
    audio_controller.volume.UnregisterControlChangeNotify(callback)
