# PicaSim Coding Style Guide

This style guide applies to code in `PicaSim`, `Platform`, and `Framework` directories. Leave code from external sources (bullet, tinyxml, etc.) intact.

## Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Member variables | Prefix with `m` | `mMemberVar` |
| Static variables | Prefix with `s` | `sStaticVar` |
| Global variables | Prefix with `g` | `gGlobalVar` |
| Classes | PascalCase | `MyClass` |
| Local variables | camelCase | `localVariable` |
| Macros | ALL_CAPS | `MY_MACRO` |

## Formatting

### Line Length

Maximum line length is **120 characters** (unless there are exceptional reasons).

### Indentation

- Use **spaces**, not tabs
- Indent block is **4 spaces**

### Braces

Opening braces go on a **new line**:

```cpp
for (int i = 0 ; i != 2 ; ++i)
{
    Display(i);
}
```

### Function Separators

Function definitions (and significant blocks at file scope) should be preceded by this separator, marking the 120 character limit:

```cpp
//======================================================================================================================
void MyFunction()
{
}
```

Most class definitions should be preceded by this separator too.

## Functions

### Single-Line Declarations

Declare/define functions on a single line where possible (up to 120 characters):

```cpp
void ProcessInput(float x, float y, bool pressed);
```

### Two-Line Declarations

If parameters don't fit on one line but are simple, split into two lines:

```cpp
void Fn(float a, float b,
        float c, float d)
```

### Multi-Line Declarations

For complex parameters or many lines, use one parameter per line with aligned types:

```cpp
void Fn(float        a,
        ComplexType  bIsComplex,
        BlahBlah     cIsAsWell)
```

### Constructor Initializer Lists

```cpp
//======================================================================================================================
LoadingScreen::LoadingScreen(const char* initialText, struct GameSettings& gameSettings,
                             bool showTips, bool clearOnExit, bool showProgress)
  : mFraction(0.0f)
  , mGameSettings(gameSettings)
  , mClearOnExit(clearOnExit)
  , mShowProgress(showProgress)
  , mProgressDisabled(false)
  , mBackgroundTexture(nullptr)
  , mProgressTexture(nullptr)
  , mProgressImageWidth(0)
  , mProgressImageHeight(0)
{
}
```

## Pointers and Const

- Pointer style: `Type* ptr` (asterisk with type)
- Be const correct where possible
- Don't be overly pedantic: avoid `const Type* const t`

## Declarations

One declaration per line unless they're super-simple:

```cpp
// OK - simple
int i, j, k;

// Required - has initialization or pointers
int count = 0;
float* data = nullptr;
```

## Include Guards

Use the style:

```cpp
#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H
// ...
#endif
```

## Include Order

1. The `.cpp` file's own header first
2. Blank line, then local files in alphabetical order
3. Blank line, then wider project files in alphabetical order
4. Blank line, then system includes in alphabetical order

## Comments

Be fairly liberal with comments. They serve to:

- Help readers understand unfamiliar code
- Guide the eye through the structure
- Explain **why** the code is as it is
- Summarize the intent of larger blocks
- Embed notes about TODOs, gotchas, quirks, sources (e.g. for physics)

Comments in headers should explain intent and clarify complexity. Not all parameters need comments when the purpose is clear.

## Whitespace

- Single blank line between functions in `.cpp` files
- Blank line at the end of each file
