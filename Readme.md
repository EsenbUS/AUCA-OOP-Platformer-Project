<h1 align=center> COM-119 2025S Project 1: platformer-project </h1>
<h5 align=center>This is the starting point for Project 1 of the Object-Oriented Programming course at AUCA, which students are required to translate into OOP </h5>

![Preview](https://i.imgur.com/2AscgVH.png)

The primary goal of this project is to refactor and enhance a generic [Platformer](https://en.wikipedia.org/wiki/Platformer) game, originally written in a procedural programming style in C++, into an object-oriented design utilizing C++. This transformation should focus on encapsulating game functionalities into classes and objects, specifically targeting game levels, the player character, and the enemy characters code. Additionally, the project involves altering the level loading mechanism to dynamically parse level designs from `.rll` files, which utilize a specific run-length encoding format for level representation. A detailed description of the `.rll` file format can be found [below](#run-length-encoding).

![Platformer](https://i.imgur.com/uuK9ubI.png)

This project aims to strengthen understanding of object-oriented programming (OOP) concepts such as encapsulation and data abstraction. It also introduces practical applications of exception handling for robust error management dealing with file input and output. It can also serve as testing grounds for concepts such as inheritance and polymorphism.

To get started, clone the repository to get the starter code. You are encouraged to modify and refactor it as needed to meet the project requirements.

Be inventive in applying OOP principles to redesign the game, ensuring to implement the following critical modifications:

## Project Requirements

### Refactoring to Object-Oriented Design:

- **Level Class**: Convert the game level variables and related functions into a `level` class. This class should manage all aspects related to game levels, including storing level layout, and managing level state.

- **Player Class**: Transform player-related variables and functions into a `player` class. This class should encapsulate all player-related functionalities, such as movement, interaction with the game world, and player state management.

- **Enemy Class**: Similar to the `player` class, transform enemy-related variables and functions into an `enemy` class. This class should encapsulate all enemy-related functionalities, such as movement and interaction with the game world.

### Dynamic Level Loading:

- Implement functionality to parse and load levels from the `levels.rll` file located in the `data` directory. This file contains level designs encoded using a specific [run-length encoding scheme](#run-length-encoding). Your implementation must accurately parse these files, correctly interpret the run-length encoding, and construct playable levels from the data.

- Integrate robust error handling using exceptions to manage potential parsing and file-reading errors gracefully. This approach should ensure the game remains stable and provides informative feedback in the event of encountered errors.

## Additional Requirements:

- **Code Organization**: Maintain a clean and organized codebase. Structure your project into appropriate directories and files, ensuring a logical separation of classes, game assets, and other resources.

- **Readme.md**: Create a `Game.md` file detailing your implementation, including a description of how you approached the refactoring process, any challenges faced, and how you overcame them. Also, document the steps required to set up and run your version of the game.

- **Commit History**: Develop your project with a detailed commit history in a version control system. Regular commits with descriptive messages are essential to demonstrate the progression and milestones of your project work.

![Death screen](https://i.imgur.com/kCLXsJk.png)

## Deliverables

* Develop the game within the `<repository>` directory. This directory should include a `data` subdirectory containing all game assets (images, fonts, sounds, etc.). Additionally, ensure the `<repository>` directory contains a `Game.md` file detailing your game and listing the implemented features.

* Organize your code into a set of header files and classes. Each header file should contain related function definitions. Please, add forward declarations for functions __that are outside of classes__ in `globals.h` to prevent circular dependencies. If a function from a class is necessary, include its header file.

* Place your main game loop in the `platformer.cpp` file, which should include `globals.h` and all other necessary header files.

Upon completion, your Project directory should have an organized structure similar to the following example:

```
.
└── <repository>
    ├── data
    │   ├── fonts
    │   │   └── ...
    │   ├── images
    │   │   └── ...
    │   ├── music
    │   │   └── ...
    │   └── sounds
    │       └── ...
    ├── platformer.cpp
    ├── globals.h
    ├── level.h
    ├── level.cpp
    ├── player.h
    ├── player.cpp
    ├── enemy.h
    ├── enemy.cpp
    ├── <various header files>.h
    ├── ... (.idea, .gitignore, CMakeLists.txt, other directories)
    └── Readme.md
```

Commit and push your work at regular intervals, such as daily or after significant changes. Finally, submit the URL of the last commit to Moodle before the deadline.

## Project Setup Notes

Set up the raylib project as you have done in previous labs for graphics assignments.

Before attempting to run the code for the `platformer` target, please ensure that you set the `Working directory` correctly in your CLion IDE.

The current working directory must point to the directory containing the necessary resource files (e.g., `data/images`, `data/fonts`, and `data/sound|music`). Failure to do so will result in these resources not being loaded properly, and you will not see or hear the intended visuals or sounds in the game.

To set the working directory:

1. Open the `Run` menu and find the `Edit configurations` button there in CLion.
2. Select the `platformer` target.
3. Locate the `Working directory` field.
4. Set it to the `$ProjectFileDir$` value (this ensures it points to the root project directory with the `data` folder).
5. Save your configuration and run the target again.

If you encounter any issues, double-check that the paths to the resource files are correct relative to the working directory.

## Deadline

Refer to the specific course or assignment page on Moodle for detailed information about deadlines.

<h1 align="center"> Attachments </h1>

## Prerequisites

To successfully complete this project, students are encouraged to familiarize themselves with the following theoretical ground:

- Liang, Daniel Y. _Introduction to Programming with C++_. Pearson, 2014, Chapters 9, 10, 11, and 13 (pp. 361-542);
- [Singleton](https://refactoring.guru/design-patterns/singleton), [Factory](https://refactoring.guru/design-patterns/factory-method), and [Mediator](https://refactoring.guru/design-patterns/mediator)/[Observer](https://refactoring.guru/design-patterns/observer) design patterns.

Doing so will simplify and ease the process of redesigning procedural code into object-oriented code.

## Run-Length Encoding

This project utilizes RLE to encode levels, increasing compactness and efficiency. Level elements are represented by the following elements:

| Puzzle Element | Character | ASCII code |
|:---------------|:----------|:-----------|
| Wall           | #         | 0x23       |
| Dark Wall      | =         | 0x3D       |
| Air            | -         | 0x2D       |
| Player         | @         | 0x40       |
| Coin           | *         | 0x2A       |
| Spike          | ^         | 0x5E       |
| Enemy          | &         | 0x26       |

In the RLE format, digits are used to show how many elements of the same type are following; as such:

`---- becomes 4-`.

So the following level,
```
 -------
 ---*---
 -------
 -------
 -------
 @-^^^-E
 #######
```

looks like this with RLE:

`7-|3-*3-|7-|7-|7-|@-3^-E|7#`.

The rows of the level are separated by vertical bars (`|`s).

## Implementation Tips

- In your `class.h` files, exclusively use forward declarations—do not implement functions. Furthermore, try to only include `"raylib.h"` in your `.h` files (wherever possible).
- In your `class.cpp` files, however, you may include anything that is necessary.
- Use the `inline` keyword on variables that are included in more than one `.cpp` file, like `globals.h`, to avoid variable redefinitions.
- Do not forget to initialize static member variables, especially singletons.
- Use compilation and linking errors to your advantage: _read_ them.
- Use the debug tool to resolve crashes.

## Links

### raylib

* [Wiki](https://github.com/raysan5/raylib/wiki)
* [Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)
* [Examples](https://www.raylib.com/examples.html)

### Tools

* [aseprite](https://github.com/aseprite/aseprite) / [krita](https://krita.org/en/)
* [jsfxr](https://sfxr.me/)

## Books

* Introduction to Programming with C++, 3rd Edition by Daniel Liang

## Credits

* [Luna Maltseva](https://github.com/lunamaltseva)

