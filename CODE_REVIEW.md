# PicaSim code review before 1.6.0

Static review of PicaSim's own code (about 58,000 lines, leaving out Bullet, tinyxml and the other third-party libraries). Nothing was run. Items that need a runtime check to settle are listed at the end.

Line numbers refer to main at 7d94917.

**Status:** sections 1 and 3 are fixed in PR 27 (branch `review-fixes`). From section 2, these are fixed in the same PR:
- the double aspect ratio correction
- flutter drag
- tug propwash
- the jet thrust clamp
- `numRings`

The plane's own prop wash is deliberately unchanged (see section 2). Section 4 (performance) is untouched.

## Summary

No serious crash or physics bug turned up in normal use with the shipped data. The flight model, the physics loop and the AI are in good shape, and most of what follows is robustness against unusual data or edge cases.

The items most worth doing before 1.6.0 are small and low risk:

1. **Lost clicks and taps on the in-flight buttons.** This is the same pattern as the Android back-button bug fixed in PR 25, but for the mouse and touch.
2. **Safer saving of settings.xml**, so an interrupted save can't wipe a user's settings, controller setup and high scores.
3. **A missing panorama crashes the app.** A scenery that refers to a panorama that isn't there corrupts memory, and because the choice is saved, it would crash on every launch.
4. **A handful of one-line guards** against divide-by-zero and empty arrays when loading custom aircraft, settings or races.
5. **The aerotow tug hook uses the glider's size scale.** It's a one-word fix.

Four physics findings (flutter drag, a double aspect-ratio correction, tug propwash and jet thrust shape) are real, but the shipped aircraft are tuned around the current behaviour. I'd suggest documenting them rather than changing them for this release.

## 1. Worth fixing before 1.6.0

### 1.1 Mouse clicks and touch taps can be lost (bug)
`source/Platform/Input.cpp`

**Mouse.** `UpdateMouseStates()` (about line 431) compares "down now" with "down last frame". If a button goes down and up within one frame, both frames read as up and the click disappears.

**Touch.** On `SDL_FINGERUP` (about line 311), the finger is marked inactive straight away. As a result, a tap shorter than a frame never shows up as `PRESSED`.

**Who is affected.** Every in-flight on-screen button goes through `ButtonOverlay::IsPressed()` and `s3ePointerGetTouchState()`: pause, resume, relaunch, settings, change view and walkabout. Laptop touchpads with tap-to-click send the down and up a few milliseconds apart, so the click can be lost there. On phones, it gets more likely as the frame rate drops. The ImGui menus are not affected, because they keep their own event queue.

**Fix.** Latch a "pressed this frame" flag, the same way as `mKeyPressedThisFrame` from PR 25, and keep a released touch visible for one frame.

### 1.2 settings.xml is overwritten in place (robustness)
`source/PicaSim/GameSettings.cpp:28` (`Settings::SaveToFile`)

**The problem.** Settings are saved when returning to the start menu, and shortly after each flight starts. If that write is interrupted, the file is left truncated: Android killing the app, a crash or a power cut can all do this. On the next launch, loading fails quietly, defaults are used, and they are then saved over the top. The user loses controller calibration, options, statistics and high scores.

**Fix.** Write to `settings.xml.tmp`, then rename it over the original.

### 1.3 A missing panorama crashes at load (crash risk)
`source/PicaSim/Terrain.cpp:610-650`

**The problem.** If `Panorama.xml` fails to load, the only check is `IwAssert`, which compiles to nothing in release builds. `nx` stays 0, `new Heightfield::Vertex[0]` is allocated, and `LoadHeightfield` writes into it, corrupting the heap.

**When it happens.** The cause would be a custom scenery, or a saved scenery that points at a folder that has been removed or renamed. The selection is stored in settings.xml, so the crash repeats on every launch until the user deletes their settings. `Environment.cpp:51` uses the same assert-only pattern.

**Fix.** Check the load result, and fall back to a default scenery with a message.

### 1.4 Cheap guards against bad data (crash risk, custom content)

None of these can happen with the shipped data. Each is a one- or two-line fix.

| Where | What goes wrong |
|---|---|
| `AeroplanePhysics.cpp:366` | `&masses[0]` when an aircraft has no wings, fuselages or shapes |
| `AeroplanePhysics.cpp:409` | `&collidingPoints[0]` when no part has `collide="true"` |
| `GameSettings.h:127-128`, `PicaSim.cpp:852` | `% mNumAltSettings` is read unchecked from settings.xml, so a value of 0 is an integer divide by zero |
| `PropellerEngine.cpp:539, 649` | `maxRPM` omitted means `mMaxW` is 0, then a divide by zero (NaN throttle and audio) |
| `PropellerEngine.cpp:137, 186` | If `CLPerDegree` is omitted, the default is converted to per-radian twice (328 per radian instead of 5.7) |
| `AerofoilDefinition.cpp:137-160` | Members such as `mCDFlying`, `mCM0` and `mRefRe` have no defaults, so a missing attribute or file gives garbage forces |
| `ChallengeRace.cpp:190` | Not a problem after all: `ChallengeSettings` only keeps checkpoints whose gate exists when it loads the challenge |
| `Menus/FileMenu.cpp:794` | `std::filesystem::remove` throws if the delete fails (permissions, locked file), and nothing catches it, so the app terminates. Use the `error_code` overload. |
| `Framework/AudioManager.cpp:321-323` | For a `.raw` file with an odd byte count, `fread` writes one byte past the buffer |

### 1.5 Aerotow tug hook uses the glider's scale (bug, minor)
`source/PicaSim/AeroplanePhysics.cpp:1458`

**What's wrong.** `AttachToTug` works out the tug's tail hook with the glider's `DimensionalScaling`. The glider's nose hook is correct everywhere. Launch placement (`Aeroplane.cpp:446`) and rendering (`AeroplanePhysics.cpp:896`) both use the tug's own scale for its hook.

**Why it lasts the whole tow.** Bullet's `appendAnchor` fixes the anchor in the tug's frame from the rope node's position when the rope is created. So the wrong point persists for the whole tow, and the rope's rest length is off by the same amount.

**How far off it is.** The tug's scale is the tug preset's size times the glider preset's "tug size scale". Shipped presets that differ include:

| Preset | Tug scale | Hook anchored |
|---|---|---|
| Moyes hang glider | ×3 | 1.8 m too far forward |
| Discus | ×1.5 | 0.45 m forward |
| SpitfireIndoor (glider scale 0.4) | | 0.54 m forward |
| Magpie, MiniDLG, Sherpa2 | ×0.8 | 0.18 m behind |

**Fix.** Use `tds` built from the tug's settings.

### 1.6 ASW15's tug name has the wrong case, and a failed tug load crashes (crash risk, Linux and Android)

**The name.** `SystemSettings/Aeroplane/ASW15.xml` names its tug `Acrobat.xml`, but the file is `AcroBat.xml`. Windows and macOS don't mind. Linux and Android have case-sensitive file systems, and I believe iOS devices do too.

**The crash.** If aerotow is chosen for the ASW15 on those platforms, `AIControllerTug::Init` fails ("Failed to load AI controller"). `Aeroplane.cpp:155-157` ignores the result and dereferences the null tug aeroplane.

**Fix.** Correct the name, and check `Init`'s result so a missing tug falls back to a normal launch.

**A second case mismatch.** A scan of all data references found one more. The Genie's "Blue" scheme refers to `Genie25.3ds`, but the file is `genie25.3ds`. I haven't traced whether that shows no model or fails harder.

### 1.7 Found while fixing section 1

**AI aircraft whose file is missing.** An AI controller registered itself for updates before loading its aircraft. When the load failed, the controller was deleted but stayed registered, so the next frame used freed memory. The same happened to the aerotow tug. Controllers now register only after loading succeeds.

**Touches.** Touch presses were turned into "held" before the game read them, so a press was never seen as a press. The in-flight buttons only worked through SDL's emulated mouse, which follows the first finger, so a second finger could never tap a button.

**Dialogs shown while loading.** A dialog shown before the game's view exists, such as "failed to load environment", left a trail of copies as it slid in, because nothing cleared the screen behind it.

## 2. Physics findings (decide: fix, or document as intended)

These are long-standing and the aircraft are tuned with them in place. Changing any of them changes handling.

**Flutter drag isn't speed-dependent** (`AeroplanePhysics.cpp:1080-1087`).
- **What the code does.** `CDFlutter = v²/vflutter²` is computed but never used, so the "flutter drag" is a plain extra drag in proportion to dynamic pressure. `flutterDragSpeed` only acts as an on/off switch.
- **Who uses it.** Ten aircraft: ASW15, Banana, Discus, Jart, MiniDLG, Minimoa, Quark, Quartz, Spirit26 and Tigger.
- **What I'd do.** Leave it, and add a comment.

**Aspect ratio correction is applied twice** (`AerofoilDefinition.cpp:254-260` and `366-368`).
- **What the code does.** `GetGraphData` applies the usual finite-wing slope correction a0/(1 + a0/(π·AR)). `GetFlightData` then applies a second one, CL/(1 + |CL|/(π·AR)).
- **Size of the effect.** About 4% less lift at CL 1 with AR 8.
- **A related inconsistency.** Induced drag is worked out from CL before the second correction.

**Tug propwash on the towed glider has no distance falloff** (`AerofoilDefinition.cpp:604-610`).
- **What the code does.** The "distance drop-off" factor is a constant 0.25. The glider gets a quarter of the tug's slipstream at any range, as long as it's within two prop radii of the axis.
- **Comments.** The comment about rotating for lag has no code behind it.

**Jet thrust curve** (`JetEngine.cpp:160-170`).
- **What the code does.** Thrust is maxForce·(1 - s/max)·(s - min)/(max - min). It peaks at about 0.4 × maxForce in the middle of the speed range.
- **The risk.** The default `minSpeed` of 1.0 gives reverse thrust at rest for any custom jet that doesn't set it. F18Jet and Trainer both set a negative `minSpeed`, so they are fine.

**Propeller swirl ignores rotation direction** (`PropellerEngine.cpp:529`).
- **What the code does.** `mLastWashAngVel` is missing `mDirectionMultiplier`.
- **Current impact.** None, because only the Spitfire and the Extra 3D use swirl, and both turn the normal way.

**Physics step size depends on frame rate above 240 fps** (`Framework/EntityManager.cpp:143-147`).
- **What the code does.** The fixed step is 1/(30 × substeps), which is 1/240 s by default. When less than one step of time has built up, the loop runs one short step of exactly that length, and the left-over time is dropped.
- **When it happens.** On monitors faster than 30 × substeps (240 Hz at the default, 120 Hz at accuracy 4), and in slow motion, the step size then follows the frame time.
- **Likely impact.** Probably small, but it makes flight behaviour depend on frame rate. See the runtime experiments in section 6.

**Other small physics notes:**
- The accelerometer starts from zero velocity at launch, so a launch with speed gives one large spike (`Accelerometer.cpp:56-63`).
- Wheel drag uses the centre-of-mass velocity, not the velocity at the wheel (`Wheel.cpp:199`).
- The tow-rope ground clearance (0.04 m) isn't size-scaled (`AeroplanePhysics.cpp:874`), while the tether's is (`AeroplanePhysics.cpp:822`).
- The tow rope's density isn't mass-scaled (`AeroplanePhysics.cpp:1469`).
- `numRings` from the XML is ignored, because a local `int numRings = 4` shadows it (`PropellerEngine.cpp:424`). All the shipped aircraft use 4 anyway. (Fixed on `review-fixes`.)
- Servo rate while paused is handled three different ways: Wing uses the frame time, JetEngine is instant, and PropellerEngine freezes.

## 3. Smaller bugs

All of these are fixed on the `review-fixes` branch. The touch dropout was fixed as a side effect of the input changes for 1.1.

**Brief NaN in AI controls** (`AIControllerGlider.cpp:323, 340`, `AIControllerPowered.cpp:223, 231`, `AIControllerTug.cpp:211, 219`).
- **What goes wrong.** `asinf(tm.RowY().z)` can give NaN when rounding pushes the argument just past 1. Separately, `velRelWind.z / fwdSpeedRelAir` is 0/0 when the tug starts from rest in zero wind.
- **Effect.** The NaN reaches the control outputs for one frame, and the servo code moves one step the wrong way.
- **Recovery.** It recovers by itself, because `ExponentialApproach` treats a NaN error as zero on the next frame.
- **Fix.** Clamp the `asinf` argument, and use max(speed, small) as the divisor.

**Tug camera start point** (`Aeroplane.cpp:352`). `LaunchAtPosition` scales the world position by the size scale, rather than the local offset as line 257 does. The camera smoothing for the tug starts from the wrong point when its scale isn't 1.

**Zoom view** (`PicaSim.cpp:359-361`). With "only plane in zoom view" on, switching the camera aeroplane adds the new one but never removes the old one, so both appear in the zoom view.

**Touch index 0 and mouse emulation** (`Input.cpp:798-830`).
- **The setup.** Touch index 0 also reports SDL's emulated mouse.
- **What goes wrong.** When the finger driving the emulated mouse lifts while another stays down, index 0 reads "released" for a frame.
- **Effect.** One stick drops out for one frame.

**Timer overflow** (`PlatformSDL.cpp:285`). `Timer::GetMicroseconds` overflows after about 21 days of uptime. Nothing uses it today.

**After the 1.1 input changes**, a lifted touch keeps its place for one more frame, reported as released, so the remaining finger keeps its index and its stick doesn't drop out.

## 4. Performance

Nothing here looks urgent at default settings. The first two items are where time would go on a slow phone, so they're worth profiling before changing anything.

**Wind sampling per aerofoil per substep.**
- **What it costs.** Every aerofoil, fuselage, engine and wheel calls `GetWindAtPosition` on every physics substep. Each call does about six heightfield lookups (`Environment.cpp:598-650`) plus the thermal loop.
- **Scale.** A glider with many wing sections at the default 8 substeps makes tens of thousands of calls a second.

**Thermal lookup is linear in the number of thermals** (`ThermalManager.cpp:317-327`).
- **What it costs.** The count is density × range². At the slider maximums (50 per km², 10 km range), that's about 5,000 thermals checked on every wind sample above.
- **Fix.** A coarse spatial grid would remove the cost.
- **At default settings.** There are only tens of thermals.

**VR panorama depth texture** (`RenderManager.cpp:921-1019`, Windows VR only).
- **What it costs.** An eye-sized depth texture is created, filled and deleted for each eye on every frame. It should be created once and reused.
- **A related mismatch.** The `glTexImage2D` call there pairs `GL_DEPTH_COMPONENT24` with `GL_FLOAT`. This is harmless, because the call that follows defines the texture again.

**Per-frame GL queries.** `glGetIntegerv(GL_DEPTH_BITS)` (`PicaSim.cpp:1139`) and `glGetError()` (`RenderManager.cpp:309`) run every frame. On some mobile drivers they can force a sync. Cache the first, and make the second debug-only.

**Millisecond frame timing** (`Main.cpp:661`, `PlatformSDL.cpp:274`). At 144 to 240 Hz, frame deltas alternate between values such as 6 and 7 ms, up to 20% jitter. The physics accumulator absorbs this, but camera smoothing sees it. Using `SDL_GetPerformanceCounter` would be smoother.

**Other notes.** Most dynamic geometry is drawn from client-side arrays, and entity unregistering is a linear scan. Neither matters at current scale.

## 5. Checked and found OK

**Physics:**
- The aeroplane's rigid body can never go to sleep, because its sleeping thresholds are 0. Otherwise Bullet would zero its velocity and ignore thrust.
- Crash detection uses the change in velocity per step. That is the right measure for contact impulses and doesn't depend much on the step size.
- `Vector3::Normalise` is safe on zero vectors, so the tether twist can't go NaN.
- All 35 aerofoils referenced by the shipped aircraft exist and define every value the code reads.
- Every shipped propeller engine sets `maxRPM` and `CLPerDegree`, either directly or through `copy`.
- Terrain lookups clamp their indices, and every caller of the turbulent wind type passes turbulence data.

**Audio:** channel handles are bounds-checked, and the callers release channels before unloading sounds.

**Text buffers:** the `sprintf`-to-fixed-buffer cases in the challenge and HUD text are bounded.

**Networking:** the socket controller lifetime was fixed in PR 24.

## 6. Candidates for runtime experiments

1. **Tap-to-click on a laptop touchpad.** Use it on the pause, resume and relaunch buttons to confirm 1.1, then check the fix.
2. **Frame-rate dependence.** Fly the same glider and powered aircraft at 60 Hz and at 240 Hz or more (vsync off, or a fast monitor), then compare glide and climb rates and crash sensitivity.
3. **Profile a heavy case on Android.** Use a many-section glider with thermals at high density and range, to see whether wind and thermal sampling dominate.
4. **Aerotow with mismatched scales.** Use glider and tug size scales that differ (for example 1.0 and 1.5) to see the hook offset from 1.5.
5. **Flutter drag.** Compare the ten affected aircraft with the drag computed as intended, if we want to know how much retuning a fix would need.

## Coverage

**Read in full or close to it:**
- **Flight physics:** AeroplanePhysics, Wing, Aerofoil, AerofoilDefinition, Fuselage, PropellerEngine, JetEngine, Wheel, Gyro and Accelerometer.
- **Simulation loop:** the EntityManager step loop, `PicaSim::Update` and `HandleMode`, and Aeroplane initialisation and launch.
- **AI:** the AI glider and tug.
- **Environment:** the wind and thermal code, and terrain lookup and loading.
- **Audio:** AudioManager.
- **Input:** mouse and touch handling and the compatibility layer, plus button overlay hit testing.
- **Settings and menus:** settings load and save, and file deletion in the file menu.

**Covered by pattern searches only:**
- **What I searched for:** fixed buffers, `strcpy`/`sprintf`, `std::filesystem` calls, GL create/delete balance, XML load checks, and reliance on `IwAssert`, which is a no-op in release.
- **Where I searched:** everything else, including SettingsMenu (2,700 lines), the other menus, rendering (ShaderManager, RenderModel, the ac3d loader), the heightfield generator, the other challenge types, camera, particles and VR/OpenXR.

Those areas haven't had a line-by-line read, and a second pass could cover them if wanted.
