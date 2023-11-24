# Bloxorz
A modern reimagining of the web classic Bloxorz. 
The game is made using Unreal Engine, the programming is done almost entirely in Unreal's flavour of C++ apart from certain UI related functionality.

## Contents:
- [Videos](#videos)

- [Images](#images)

- [Controls](#controls)

- [Rules](#rules)

## Videos

https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/37236d8a-9e62-4840-995d-06bab127583f



## Images

### Level Examples

![BloxSplash](https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/938df394-d516-4d10-a065-95dfa4d4aaa0)

![BoxSplash2](https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/a537541d-39fd-40d9-9731-2c89e095232a)

### SFX

![BloxLighting](https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/10e15b0b-0f12-4d81-8024-08c27383cecc)


## Controls
Movement:

W/UP_ARROW    = Move up

S/DOWN_ARROW  = Move down

A/LEFT_ARROW  = Move left

D/RIGHT_ARROW = Move right

Special:

SPACE = Toggle between the split cubes 

## Rules


### Tile Types

Black Tile:

  - The black tile is the end point of the level. It is the player's goal to make it here.

Bridge Tile:

  - The bridge tile can open new paths but must be activated by a switch!

Fall/Red Tile:

  - These tiles will fall whenever the player character is standing on top of them.

Torus Tile:
  - The torus tile splits your character into two at puts them at different points of the map.

Circle switches:

  - Circle switches are activated whenever the character is over them regardless if the character is flat or standing.

Cross switches:

  - Cross switches are activated whenever the character is standing over them. If the character is flat, the switch does 
    not react.

### Movement Counter

The amount of moves the player makes in each level is stored and used to calculate their overall score at the end (score feature is not yet implemented).

If the player falls, then the move count is reset to whatever it was at the beginning of the level.
