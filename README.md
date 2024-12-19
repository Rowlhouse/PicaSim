# PicaSim

This contains the complete source, including dependencies, for PicaSim flight simulator: https://rowlhouse.co.uk/PicaSim/

Note that in order to build it, you will need Marmalade SDK (and a functional licence), which you won't have!

If you want/are able to port PicaSim to a different framework, then feel free to branch the project, but I suggest contacting me in case there are others wanting to do ths same.

Danny Chapman - picasimulator@gmail.com

# Notes on the licence 

PicaSim's licence only covers PicaSim source code and some of the data it uses.

The third party packages and assets will have their own licences which need to be abided by.

These are covered by PicaSim's licence:

Under Source
- Framework: Contains fairly generic code on which PicaSim is built
- Heightfield: Runtime refinement for rendering a heightfield, based on a paper by Lindstrom + Pascucci. It was good in its day, but I would not recommend it now!
- Joystick: Marmalade extension for reading joysticks under Windows
- MapTrace: Stand-alone helper application for creating a heightfield by tracing contours (very old!)
- PicaSim: All the application code
- ProcessUI: Stand-alone helper application for processing the UI bitmaps

Under Data
- Menus and iwui_style contain UI resources
- SystemSettings contains the "front end" for data - so typically things that are presented to the user, and perhaps modifyable
- SystemData contains the "back end" for data - typically the low-level representations.
-- All text and XML files here (but not .ac and .png etc) can be used under PicaSim's licence.

The following need to be treated differently:

Under Data
- Images and model files come from various sources (ranging from having been created by me, to provided by others) have been authorised for use with PicaSim
- They can therefore be used in direct derivatives of PicaSim
- You will need to request permissions directly from me/the original author to use them in another project.

These are not covered by PicaSim's licence - they have their own:

Under Source
- Gamepad: Marmalade extension written by Gleb Lebedev for gamepads on Android
- bullet-2.81: Bullet physics library (source is under the zlib licence). THere may be some modifications from the original.
- dpi: Marmalade extension for handling screen resolution/DPI
- tinyxml: Under the zlib licence

