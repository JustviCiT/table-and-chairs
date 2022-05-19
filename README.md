# Table And Chairs

Template used: C++ Archviz procedural geometry demo in Unreal Engine 4

## Controls

- Left click on corner, resize table
- Right click on table or chairs, remove 

#### Camera
- Wheel up/down , zoomiN / zoomOut
- W , up
- A , left
- S , down
- D , right

## Changelog

- Fixed editing of corners when not in center
- Fixed table overflow in the opposite direction
- Added Camera Pawn that can move with wasd + mouse control
- New functionality to spawn tables + chairs
- New functionality to remove tables + chairs
- Split Table into another Actor class 
- Chair spawn performance improved and updated spawn strategy
- Added code to use the corner material for the selected state 
- Clean of all superfluous content
- Converted project to 4.25, removed all old includes, renamed log category
- Forked template