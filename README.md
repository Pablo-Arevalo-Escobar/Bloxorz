# Bloxorz
A modern reimagining of the web classic Bloxorz. 
The game is made using Unreal Engine, the programming is done almost entirely in Unreal's flavour of C++ apart from certain UI related functionality.

## Contents:
- [Videos](#videos)

- [Controls](#controls)

- [Rules](#rules)

## Videos

### Game Footage

#### Snow Effects 

https://github.com/user-attachments/assets/a7e45e8b-30cc-44f2-b51b-4bfae915a49f



https://github.com/user-attachments/assets/c14efa24-0d0a-4df6-ae18-8e91db46c0e6

#### Base Game

https://github.com/user-attachments/assets/3c99aa8a-fd3e-471a-b495-94c91cda2486


https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/30a90905-79e0-418d-bc82-54e21d781167


https://github.com/user-attachments/assets/7201f9f7-24b4-4714-b50d-1a1d08d79b85



https://github.com/Pablo-Arevalo-Escobar/Bloxorz/assets/63361048/bb1679ac-1ba1-4b85-b69b-e55a759f361e



### Level Editor Footage
#### Tile viewer


https://github.com/user-attachments/assets/8d5f750d-7444-4d5f-90f7-8d61016a290a


#### Linking buttons to bridges/tiles

https://github.com/user-attachments/assets/fde1d361-5323-48c8-920c-29b1d175bb80


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
