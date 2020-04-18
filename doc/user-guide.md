# User Guide

- [Configuration Flags](#configuration-flags)
  - [Setting Configuration Flags](#setting-configuration-flags)
  - [`ActiveTabHasCloseButton`](#activetabhasclosebutton)
  - [`DockAreaHasCloseButton`](#dockareahasclosebutton)
  - [`DockAreaCloseButtonClosesTab`](#dockareaclosebuttonclosestab)
  - [`OpaqueSplitterResize`](#opaquesplitterresize)
  - [`XmlAutoFormattingEnabled`](#xmlautoformattingenabled)
  - [`XmlCompressionEnabled`](#xmlcompressionenabled)
  - [`TabCloseButtonIsToolButton`](#tabclosebuttonistoolbutton)
  - [`AllTabsHaveCloseButton`](#alltabshaveclosebutton)
  - [`RetainTabSizeWhenCloseButtonHidden`](#retaintabsizewhenclosebuttonhidden)
  - [`OpaqueUndocking`](#opaqueundocking)
  - [`DragPreviewIsDynamic`](#dragpreviewisdynamic)
  - [`DragPreviewShowsContentPixmap`](#dragpreviewshowscontentpixmap)
  - [`DragPreviewHasWindowFrame`](#dragpreviewhaswindowframe)

## Configuration Flags

The Advanced Docking System has a number of global configuration options to
configure the design and the functionality of the docking system. Each
configuration will be explained in detail in the following sections.

### Setting Configuration Flags

You should set the configuration flags before you create the dock manager
instance. That means, setting the configurations flags is the first thing
you do, if you use the library.

```c++
CDockManager::setConfigFlags(CDockManager::DefaultOpaqueConfig);
CDockManager::setConfigFlag(CDockManager::RetainTabSizeWhenCloseButtonHidden, true);
...
d->DockManager = new CDockManager(this);
```

If you set the configurations flags, you can set individual flags using the
function `CDockManager::setConfigFlag` or you can set all flags using
the function `CDockManager::setConfigFlags`. Instead of settings all
flags individualy, it is better to pick a predefined set of configuration
flags and then modify individual flags. The following predefined
configurations are avilable

- `DefaultNonOpaqueConfig` - uses non opaque splitter resizing and non opaque docking
- `DefaultOpaqueConfig` - uses opaque splitter resizing and opaque docking

Pick one of those predefined configurations and then modify the following
configurations flags to adjust the docking system to your needs.

### `ActiveTabHasCloseButton`

If this flag is set (default configuration), the active tab in a tab area has
a close button.

![ActiveTabHasCloseButton true](cfg_flag_ActiveTabHasCloseButton_true.png)

If this flag is cleared, the active tab has no close button. You can combine
this with the flag `DockAreaCloseButtonClosesTab` to use the close button
of the dock are to close the single tabs.

![ActiveTabHasCloseButton true](cfg_flag_ActiveTabHasCloseButton_false.png)

### `DockAreaHasCloseButton`

If the flag is set (default configuration) each dock area has a close button.

![DockAreaHasCloseButton true](cfg_flag_DockAreaHasCloseButton_true.png)

If this flag is cleared, dock areas do not have a close button.

![DockAreaHasCloseButton true](cfg_flag_DockAreaHasCloseButton_false.png)

### `DockAreaCloseButtonClosesTab`

If the flag is set, the dock area close button closes the active tab,
if not set, it closes the complete dock area (default).

### `OpaqueSplitterResize`

The advanced docking system uses standard `QSplitters` as resize separators and thus supports opaque and non-opaque resizing functionality of `QSplitter`. In some rare cases, for very complex widgets or on slow machines resizing via separator on the fly may cause flicking and glaring of rendered content inside a widget. This global dock manager flag configures the resizing behaviour of the splitters. If this flag is set, then widgets are resized dynamically (opaquely) while interactively moving the splitters. If you select the predefined configuration `DefaultOpaqueConfig`, then this is the configured behaviour.

![Opaque resizing](opaque_resizing.gif)

If this flag is cleared, the widget resizing is deferred until the mouse button is released - this is some kind of lazy resizing separator. If you select the predefined
configuration `DefaultNonOpaqueConfig`, then this is the configured behaviour.

![Non-opaque resizing](non_opaque_resizing.gif)

### `XmlAutoFormattingEnabled`

If enabled, the XML writer automatically adds line-breaks and indentation to
empty sections between elements (ignorable whitespace). This is used, when
the current state or perspective is saved. It is disabled by default.

### `XmlCompressionEnabled`

If enabled, the XML output will be compressed and is not human readable anymore.
This ie enabled by default to minimize the size of the saved data.

### `TabCloseButtonIsToolButton`

If enabled the tab close buttons will be `QToolButtons` instead of `QPushButtons` - 
disabled by default. Normally the default configuration should be ok but if your
application requires `QToolButtons` instead of `QPushButtons` for styling reasons
or for any other reasons, then you can enable this flag.

### `AllTabsHaveCloseButton`

If this flag is set, then all tabs that are closable show a close button. The
advantage of this setting is that the size of the tabs does not change and the
user can immediately close each tab. The disadvantage is that all tabs take up
more space.

![AllTabsHaveCloseButton true](cfg_flag_AllTabsHaveCloseButton_true.png)

If this flas is cleared, then only the active tab has a close button (default)
and therefore the tabs need less space.

![AllTabsHaveCloseButton false](cfg_flag_ActiveTabHasCloseButton_true.png)

### `RetainTabSizeWhenCloseButtonHidden`

If this flag is set, the space for the close button is reserved even if the
close button is not visible. This flag is disabled by default. If this flag
is disabled, the tab size dynamically changes if the close button is
visible / hidden in a tab. If this flag is enabled, the tab size always remains
constant, that means, if enabled, the tabs need more space.

![AllTabsHaveCloseButton false](cfg_flag_RetainTabSizeWhenCloseButtonHidden_true.png)

### `OpaqueUndocking`

If this flag is set, opaque undocking is active. That means, as soon as you drag a dock widget or a dock area with a number of dock widgets it will be undocked and moved into a floating widget and then the floating widget will be dragged around. That means undocking will take place immediatelly. You can compare this with opaque splitter resizing.

![OpaqueUndocking true](opaque_undocking.gif)

If you would like to test opaque undocking, you should set the pedefined config
flags `CDockManager::DefaultOpaqueConfig`.

```c++
CDockManager::setConfigFlags(CDockManager::DefaultOpaqueConfig);
```

If this flag is cleared (default), then non-opaque undocking is active. In this mode, undocking is more like a standard drag and drop operation. That means, the dragged dock widget or dock area is not undocked immediatelly. Instead, a drag preview widget is created and dragged around to indicate the future position of the dock widget or dock area. The actual dock operation is only executed when the mouse button is released. That makes it possible, to cancel an active drag operation with the escape key.

![OpaqueUndocking true](non_opaque_undocking.gif)

The drag preview widget can be configured by a number of global dock manager flags:

- `DragPreviewIsDynamic`
- `DragPreviewShowsContentPixmap`
- `DragPreviewHasWindowFrame`

Non-opaque undocking is enabled by default. If you would like to enable it
explicitely, you can do this by setting the predefined configuration flags
`CDockManager::DefaultNonOpaqueConfig`.

```c++
CDockManager::setConfigFlags(CDockManager::DefaultNonOpaqueConfig);
```

### `DragPreviewIsDynamic`

If non-opaque undocking is enabled, this flag defines the behavior of the drag 
preview window. If this flag is enabled, then it will give the user the
impression, that the floating drag preview is dynamically adjusted to the drop
area. In order to give the perfect impression, you should disable the flags
`DragPreviewShowsContentPixmap` and `DragPreviewHasWindowFrame`.

```c++
CDockManager::setConfigFlag(CDockManager::DragPreviewIsDynamic, true);
CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, false);
CDockManager::setConfigFlag(CDockManager::DragPreviewHasWindowFrame, false);
```

![DragPreviewIsDynamic true](dynamic_drag_preview.gif)

### `DragPreviewShowsContentPixmap`

If non-opaque undocking is enabled, the created drag preview window shows a 
copy of the content of the dock widget / dock are that is dragged, if this
flag is enabled (default).

![DragPreviewShowsContentPixmap true](cfg_flag_DragPreviewShowsContentPixmap_true.png)

If this flag is disabled, the drag preview is only a transparent `QRubberBand`
like window without any content.

![DragPreviewShowsContentPixmap true](cfg_flag_DragPreviewShowsContentPixmap_false.png)

### `DragPreviewHasWindowFrame`

If non-opaque undocking is enabled, then this flag configures if the drag 
preview is frameless (default) or looks like a real window. If it is enabled,
then the drag preview is a transparent window with a system window frame.

![DragPreviewHasWindowFrame true](cfg_flag_DragPreviewHasWindowFrame_true.png)
