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
> A much-needed ncurses replacement, written in C89 for maximum portability.

Bulwark is a terminal manipulation library meant to be a more modern, slimmed-down replacement for ncurses. The ncurses library is old and unintuitive, having lots of functionality that isn't even useful - and it simply wasn't designed to be used how people use it today. By simplifying and giving functions self-explanatory names, Bulwark aims to be easily discoverable in any mondern code editor.

Some of the goals of Bulwark include:
* Target only POSIX environments.
* Provide an asynchronous input event system for real-time terminal apps (games, for example).
* Support 16, 256, and full RGB colors.
* Easy use of extended characters for more interesting-looking apps.
* Throw away all the legacy stuff that nobody uses.
* Give functions long, self-explanatory names so the library can be discovered during programming, without lots of googling.

## The Plan
Bulwark is in early stages of development so most of these goals are largely incomplete. The evil plan is to develop a nice/fun app with it during development to showcase it's capabilites. I'm currently working on a simple website to host in parallel and migrating the build system from hand-written, brittle makefiles to the Autotools (a.k.a. *autoconfiscating* it). Also looking for a good planning tool, might use Trello.
