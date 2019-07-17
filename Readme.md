# Addle

Addle is an image editor and drawing program that prioritizes speed, simplicity,and effortlessness over making really nice pictures or taking advanced control over image data. Think more "Microsoft Paint" than "Adobe Photoshop".

However, Addle is more than just another open source Paint clone. There is room there for improvement and modernization. Addle intends to up the ante, being more powerful and versatile than Paint and solving old gripes its users have had these last 30 years -- while at the same time being even more convenient and straightforward.

Addle is free and permissively open source, (MIT licensed). It is written in C++ using the Qt Framework. It is currently very early in development. Its developer is Eleanor Hawk.

## Features

The full list of features Addle will support is likely to fluctuate throughout early development. I recognize the danger of feature creep, so I will continue to test the waters of what's practical to implement or support. Hopefully I will be willing to kill my darlings for the good of the project's core goals.

Addle's features are not meant to be worse versions of those of more advanced image editors, but rather the complete set covering a different and not-much-overlapping set of use cases.

I have not been subtle in comparing Addle to Paint, certainly I want Addle to at least support every useful feature Paint has. The following are notable additions or alterations:

### Improved viewport

The viewport will be fully independent of the window, i.e., the canvas is not fixed to the top-left corner of the display area. The viewport will also be able to be zoomed in and out, and arbitrarily rotated (i.e., not fixed to the cardinal directions). All tools that have an orientation (e.g., drawing a rectangle) will be oriented relative to the view and not to the canvas.

### Canvas sizing / infinite canvas

A finite canvas will be able to be resized by dragging anchors on the screen. Paint supports this actually; though in Addle it will also be possilbe to do this on the top and left side of the cavnas, and the anchors will be bars spanning the entire border rather than very small squares (inspired by KolourPaint). The starting canvas size will be remembered from one session to another, also Paint-style. This is more of an improvement on other drawing programs which almost universally use a dialog to resize the canvas.

But I think we can do better than the Paint approach. Thus, Addle will support (and offer by default on new images) an infinite canvas. There will be a virtual infinite area that accepts drawing input and displays no edges. The actual image data will be handled automatically under the hood.

### Layering and transparency

I was reluctant to support layering. Layering adds a level of use complexity that I worried would cut into the effortlessness I want for Addle. However, it's very useful, and a layer-based mindset lends itself to an improved model for transparency. So, Addle will support multiple layers, but without many of the bells or whistles you might see in other image editors:

- No blending modes
- No mask layers
- No layer groups
- No layer locking
- No alpha locking

In addition, selection and clipboard operations will follow a more Paint-style behavior than other image editors, layers notwithstanding. More on that under "Selection and transformation".

Addle will fully support alpha and transparency on images. Transparency will be the default background color of all layers, and a solid color of the user's choice (white by default, surely) will be displayed under the bottom layer. This should greatly simplify and improve the quality of operations that make use of transparency like moving a selection.

### Selection and transformation

I intend to follow a very Paint-like behavior for selection. It's just so easy. That means that selections will not be persistent and will certainly not be used to mask other tools. A floating selection will be immediately merged with the current layer when another tool or layer is selected, or when the selection is canceled. Sections of images can be moved between layers by means of the clipboard.

A selection can be drawn in the familiar ways with a rectangle and a lasso (i.e., free-form). I question the utility of also offering an ellipse or any form of selection construction or discontiguous selections. However I think a magic selection (i.e., select continuous area of similar colors) would be a good addition.

Selections will be able to be transformed. Translation, flipping, resizing, and arbitrary rotation should be able to be accomplished directly on the editor by clicking and dragging the mouse or pushing buttons, and without use of a dialog. These transformations will be anti-aliased and interpolated. Other transformations like shearing and perspective may also be useful though I would not put them front and center.

### Fill bucket

The fill bucket will have a few improvements:

- Threshold: similar but not identical colors within a chosen tolerance will be filled as a continuous area. (Paint is pretty much the only raster image editor anymore that doesn't support this.)
- Live preview: an outline will be displayed on the editor indicating the area that will be filled if a given point is clicked with the fill bucket. (This will not have to be run at 60fps.)
- Gap-closed: an area whose border has small gaps (within a chosen tolerance) will be filled as though the gaps were closed. This is a very useful feature that I quite want Addle to have, but there does not appear to be an efficient algorithm for it. So it's experimental for the time being.

I anticipate the fill bucket will replace underlying color rather than mix with it. This will mean that setting the fill color to transparent will erase sections of similar color.

### Other ideas I've had, of various merit

- **Shapes and text** will float above the layer as vector items for a period, sort of like selections. In this state, they can be recolored and transformed freely (without destruction). Drawing another shape/text box, or changing tools or layers will cause the shape/text to merge onto the layer. Paint has supported this with text for a while and with shapes since Windows 7.
- **Text:** I want as much rich text formatting as is practical, probably no indentation stuff but definitely inline editing of font, size, style, color, applied to sections of text and not just the whole box uniformly. Maybe text alignment too. Also, text outlining and emoji support via Blobmoji (and probably Twemoji too). Not 100% sure how text will interact with view rotation. Perhaps text should not be merged with the layer unless the user expressly commands it.
- **Stickers:** not married to this idea, but a lot of art programs when I was a kid had reusable art assets called stamps. Most of the time, these would be merged directly onto the canvas, but in one program (StoryBook Weaver) they could be rearranged, transformed, and even recolored. I think stickers would be a better name for this kind of thing, and I think they'd be useful and fun. Especially since a sticker book would only need to be some .png files and a manifest -- it'd be very open to the users to hack and share.
- **Pallet:** I want the color pallet to be really big by default. Like 200 colors. I wonder if it might be harder to use if the pallet isn't fixed-width though. Worth investigating. I also think being able to define custom color pallets, name them, name the colors, save and load them would be a neat feature even if an advanced one.
- **Screenshots:** one of the most practical use cases for Paint I find is when I need to annotate a screenshot. So how about we cut out the middle-man. This might require a daemon for system integration or just some command line flags and keybindings. I also think it'd be better if Addle captures the whole screen and lets the user crop it in the editor (where they can zoom, and where the image isn't changing), rather than selecting a portion of the screen. This could be aided by also noting the positions of windows on the screen and using that data to allow for smart cropping. If all that's even possible.
- **Filters:** things like blur, emboss, brightness/contrast, HLS adjust.
- **Import animation frames:** Addle won't support animation, but it could take frames from an animation (certainly from a .gif or .webm, but perhaps also a proper video file) and aggregate them somehow or open new windows for multiple frames.
- **Extension framework:** The extension I had in mind would be novelty brushes and filters like KidPix Studio, specifically intended for kids to play with. This isn't something I would want cluttering the UI by default. Some others of these features might more logically work as extensions, even if they are included by default. Qt offers some plugin-related features, and also Python is a popular tool for this kind of thing (so plugins don't have to be built for different targets). That sounds like a lot of work so if it happens it's *way* down the line, possibly 2.0 kind of stuff.
- **Image viewer**, there seems to be a bit of a vacuum for a good desktop-agnostic image viewer on Linux. An image editor is most of the way there, so maybe Addle could be extended into an image viewer too. Selection and viewport rotation could be useful features on a viewer.
- **Patterns and bitmaps** as fill contents, kind of vector-like. Not loving this tbh.
- **Drawpile integration**/compatibility. I don't know, it sounded like a good idea in my head.
- **Improved clipboard data:** the clipboard can often be used to upload images to social media or web chat clients, so Addle could probably be a useful tool for taking clips. However, depending on the application it might be inconvenient for lots of images to be uploaded with the name "unknown.png" or "image0.png". Addle might be able to do something about this.

### File formats

Many image editors distinguish between the operations Open/Save and Import/Export, where Open/Save deals with an editor-specific document format and Import/Export deals with all other formats.

Addle will not do that.

In the spirit of simplifying the user experience, I'm inclined to make .png the primary file format of Addle. Since the purpose of Addle is *not* advanced image editing functionality I'm inclined to think saving by default to a format useful to many, many other applications is more important than encapsulating editor state. However, this will mean that by default, information about layering and transparency will be lost when saving. I think this would be accepable for the general use of Addle, but it is something that would need to be communicated to the user, and I will have to consider the best way to do that.

Naturally, Addle will support the other web mainstays .jpg and .gif -- though as mentioned above I feel Addle should be a bit more useful in handling .gif animations. 

Addle will support the Open Raster format, with a modest amount of extension, as the file format for saving complete document information when desired. This will include layering, any and all vector-like objects (like stickers and text), and metadata. It will also take reasonable measures to support .ora files made in other applications. Again, some communication with the user will be necessary to convey the role of this format, and in particular not to make the user afraid to use .png when it'd be fine.

Addle will, to a limited extent, support the Photoshop Document format for communicating multi-layer image data to a wider set of other image editors. Vector-like data will be merged into raster layers for this, and it will only support reading .psd files well enough to be able to open its own files.

There exist a number of older or uncommon single-layer raster formats (e.g., .bmp, .ppm, .webp), which with the help of FFmpeg it would be mostly trivial to support. However, I have no interest in cluttering the user interface with a dozen obscure file formats, so outside of a handful of favorites the user will have to go out of their way to access these.

Additionally, there are a few specialty file formats that might be useful to support reading but not writing. For example, reading .svg will be necessary as a part of supporting .ora, but this will take the form of flattening the .svg onto a raster, and even though you *could* save that data back into a valid .svg it'd be weird and dumb. One could conceive doing something similar for other vector formats. Also, the previously mentioned ideas about importing video frames would fall into this category.

Now, I did say that using .png as the default file format would result in loss of transparency information. While .png supports alpha, Addle uses a solid background color, so all transparency would get flattened down onto this and unless the background itself is pure transparency this will result in some information loss. However, the .png file container can be extended with chunks of arbitrary text, which means it's feasable that a .png file could be augmented with a little metadata and an additional 8-bit pixmap (compressed with DEFLATE and encoded in Base64) to "unmerge" the background color from the image data. The size increase would probably not be unreasonable, especially if the alpha channel is otherwise unused in the .png file. The image would display correctly in other applications while behaving more correctly in Addle. This is... a weird thing to do, but could be something worth trying out. This could be extended to include all the .ora information, but that's probably taking things too far.

## Targeted platforms

Qt supports numerous platforms, so Addle can too without compounding in complexity. My first priority is targeting modern desktop platforms, i.e., Windows 10 and Linux. Naturally OS X is a desired desktop target as well, but as I do not have a Mac this will probably have to wait until we're much closer to deployment.

I could see Addle being useful as a mobile app as well, but this is not a priority. If a mobile port is made it will probably be a ways down the road.

I don't know if I will officially target Windows XP with any version, but as Addle could probably work on it I am toying with the idea of creating an unofficial Windows XP build of version 1.0.

## Dependencies
- Qt 5 (Sub version?)
- C++11
- (Anticiapted) Sqlite3, for user preferences
- (Anticipated) FFmpeg, for file formatting.
- (Maybe) Python3, for extension framework (see above)
