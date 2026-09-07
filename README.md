# russ

3D Software renderer written from scratch (except for windowing with SDL)

## Build Instructions

```
  $ make teapot # Downloads teapot obj
  $ mkdir -p bin
  $ make
  $ ./bin/main ./obj/teapot.obj
```

Press 'Escape' to quit.
Press 'Space' to print current frame time and FPS information.
Move mouse to rotate the camera around.
Use WASD to move.
Hold left click and move mouse to rotate the model.
Use mouse wheel to change model scale.

## Acknowledgments
 - Teapot model: https://github.com/alecjacobson/common-3d-test-models/tree/master
