from vapoursynth import core, RGB24

video = core.ffms2.Source(source='test.mp4')
#video = video.resize.Bicubic(format=RGB24)
video.set_output(0)

video2 = core.ffms2.Source(source='test2.mp4')
#video2 = video2.resize.Bicubic(format=RGB24)
video2.set_output(1)
