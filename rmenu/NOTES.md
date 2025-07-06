# Notes


`egui` window initialisation is too long.

We need sub 200ms to first draw, the basic egui frame app takes over a second to render.

We need this tool to compile for Linux and Mac, and potentially windows. 

Here are some alternative options:

* [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) (C)
* [Gio](https://gioui.org/) (golang)
* [imGui](https://github.com/ocornut/imgui) (C++)
* [FLTK](https://www.fltk.org/) (C++)
* [Fyne](https://github.com/fyne-io/fyne) (Go)
* [Slint](https://slint.dev/) (Rust)
* [Iced](https://iced.rs/) (Rust)
* [Azul](https://azul.rs/) (Rust)
