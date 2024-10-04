# system-monitor

C++

## Desktop System Monitor
- app to monitor computer system recources and perfomance, such as CPU,RAM, SWAP,Fan, Network and more. 
Use [Dear ImGui](https://github.com/ocornut/imgui/wiki#about-the-imgui-paradigm), a library for the GUI.

## Dear ImGui
 ImGui is an immediate mode graphic user interface, as the name implies it: "ImGui". 
 + Immediate mode: the application state is separated from the graphics library. It is the application responsibility for drawing commands when necessary. In other words, in immediate mode the client calls directly results in the rendering of graphics objects to the display.

The files are added to the existing project. To integrate Dear ImGui you must use a backend, this backend passes mouse/keyboard/gamepad inputs and a variety of settings. It is in charge of rendering the resulting vertices. 

```
apt install libsdl2-dev
```

