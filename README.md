# russ

3D Software renderer written from scratch (except for windowing with SDL)

## Build Instructions

```
  $ make teapot # Downloads teapot obj
  $ mkdir -p bin
  $ make
  $ ./bin/main ./obj/teapot.obj
```

Press 'Space' to print current frame time and FPS information.
Use arrow buttons to move the model around.
Hold left click and move mouse to rotate the model.
Use mouse wheel to change model scale.

## Acknowledgments
 - Teapot model: https://github.com/alecjacobson/common-3d-test-models/tree/master
