```sh
                               |>
                               /\
                 ,            /  \       ,
                 |>   / \ ,  /____\     / \   |>
                 ,   /   /_\ |    |    /___\  ,
                /_\  |   | : :    :    :   : / \
                | | _;--;__;--;__;--;__;--;_ | |
                | |        |  _--_  |        | |
                | |        ||"____"||        | |
                | | ______ || ____ || ______ | |

 _______  __   __  ___      _     _  _______  ______    ___   _ 
|  _    ||  | |  ||   |    | | _ | ||   _   ||    _ |  |   | | |
| |_|   ||  | |  ||   |    | || || ||  |_|  ||   | ||  |   |_| |
|       ||  |_|  ||   |    |       ||       ||   |_||_ |      _|
|  _   | |       ||   |___ |       ||       ||    __  ||     |_ 
| |_|   ||       ||       ||   _   ||   _   ||   |  | ||    _  |
|_______||_______||_______||__| |__||__| |__||___|  |_||___| |_|

                      Keep (n)curses away!
```

# Bulwark

> A much-needed ncurses alternative, written in C for maximum portability.

Bulwark is a small but powerful library for making terminal apps. It's meant to be a simple ncurses alternative that works exactly how you'd expect. By simplifying and giving functions self-explanatory names, Bulwark aims to be easily discoverable in any mondern code editor.

The current goals of Bulwark are:

* Target only POSIX environments.
* Support 16, 256, and full RGB colors.
* Support mouse events.
* Support ascii and unicode characters
* Have a decent rendering algorithm efficient enough for games.
* Throw away legacy features no one uses.
* Be easy to port.

## Core Ideas

* Bulwark does away with row/column coordinates in favor of width/height and x/y.
* You don't have to define foreground/background pairs, like in ncurses. Can set them individually or together via single function calls.
* The 16 builtin colors are mapped to the values 0-15 (diagram coming soon).

## Feature Status

Please note the library is still in early stages and there are some major bugs I'm still working through (and general cleanup/refactoring that's on my to-do list). API is not currently in a stable state like you would want for writing real apps but things will stabilize over time and as the rest of the features get hammered out.

* [x] Base 16 colors
* [x] 256 colors
* [x] Non-blocking input via event queue
* [x] Diff rendering
* [ ] Mouse events
* [ ] Full RGB colors
* [ ] Unicode characters

## How To Use Bulwark

### Installation

Bulwark can be installed just like any other cmake project (will add more specific instructions once the project is further along).

### Core Application Loop

```c
#include <Bulwark.h>
#include <stdbool.h>

int main() {
  bool running = true;
  Bulwark_Initialize();

  while (running) {
    // Handle input from Bulwark, decide if the app should keep running.

    Bulwark_ClearScreen();

    // Make draw calls

    Bulwark_UpdateScreen();
  }

  // Clean up

  Bulwark_Quit();

  return 0;
}
```
