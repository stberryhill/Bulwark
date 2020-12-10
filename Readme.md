```
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

Bulwark is a terminal manipulation library; It's meant to be a simple ncurses alternative that works exactly how you'd expect. By simplifying and giving functions self-explanatory names, Bulwark aims to be easily discoverable in any mondern code editor.

Some of the goals of Bulwark include:
* Target only POSIX environments.
* Support 16, 256, and full RGB colors.
* Support extended character set.
* Support Async keyboard input through an event queue.
* Throw away legacy features that no one uses.
* Be self-documenting in a modern code editor.
* Be easy to port to other languages.
* (Eventually) Support mouse events.

## Core Ideas
* Bulwark does away with row/column coordinates in favor of width/height and x/y.
* You don't have to define foreground/background pairs, like in ncurses. Can set them individually or together via single function calls.
* The 16 builtin colors are mapped to the values 0-15 (diagram coming soon).

## The Plan
Bulwark is in early stages of development so most of these goals are incomplete. The plan is to develop a nice/fun app with it during development to showcase it's abilites. I just finished migrating to Autotools for the build system making it much easier to install, and adding 256 color support. Still deciding on a good planning tool to lay down a roadmap.
