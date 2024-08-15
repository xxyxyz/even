![Screenshot](https://raw.github.com/xxyxyz/even/master/screenshot.png)


#### Even &mdash; 1D + 1D = 2D

Even is an application which integrates a viewer of graphic formats and a simple editor for Python. It comes with [Flat](https://github.com/xxyxyz/flat) library pre-installed, to conveniently make use of the two.

It is distributed under open source license and is intended to run cross-platform.


#### Usage

The workflow is quite simple &mdash; open or write a program which can generate the output in one of the supported formats and display the result.

The default location for searching for files a program might try to use (images, fonts, ...) is the desktop if the program was not yet saved on disk, otherwise it is its parent folder.

For example, provided the font has been downloaded, copy-pasting the following snippet and triggering Menu <i class="fa fa-angle-right"></i> Script <i class="fa fa-angle-right"></i> Run will show the above screenshot:

```python
from flat import font, text, strike, document, view

def layout(author, title, *paragraphs):
    # Vollkorn by Friedrich Althausen
    # http://friedrichalthausen.de/vollkorn/
    regular = font.open('Vollkorn-Regular.ttf')
    bold = font.open('Vollkorn-Bold.ttf')
    body = strike(regular).size(12, 16)
    headline = strike(bold).size(12, 16)
    pieces = [
        body.paragraph(author),
        headline.paragraph(title),
        body.paragraph('')]
    pieces.extend(body.paragraph(p) for p in paragraphs)
    doc = document(148, 210, 'mm')
    page = doc.addpage()
    block = page.place(text(pieces))
    while block.frame(18, 21, 114, 167).overflow():
        page = doc.addpage()
        block = page.chain(block)
    return doc

doc = layout('Vitruvius', 'Ten Books on Architecture',
    u'Chapter I \u2014 The Education of the Architect', '',
    ('1. The architect should be equipped with knowledge of many branches of '
        'study and varied kinds of learning, for it is by his judgement that all '
        'work done by the other arts is put to test. This knowledge is the child '
        'of practice and theory. Practice is the continuous and regular exercise '
        'of employment where manual work is done with any necessary material '
        'according to the design of a drawing. Theory, on the other hand, is the '
        'ability to demonstrate and explain the productions of dexterity on the '
        'principles of proportion.'), '',
    ('2. It follows, therefore, that architects who have aimed at acquiring '
        'manual skill without scholarship have never been able to reach a '
        'position of authority to correspond to their pains, while those who '
        'relied only upon theories and scholarship were obviously hunting the '
        'shadow, not the substance. But those who have a thorough knowledge of '
        'both, like men armed at all points, have the sooner attained their '
        'object and carried authority with them.'), '',
    ('3. In all matters, but particularly in architecture, there are these two '
        u'points:\u2014the thing signified, and that which gives it its significance. '
        'That which is signified is the subject of which we may be speaking; and '
        'that which gives significance is a demonstration on scientific '
        'principles. It appears, then, that one who professes himself an '
        'architect should be well versed in both directions. He ought, therefore, '
        'to be both naturally gifted and amenable to instruction. Neither natural '
        'ability without instruction nor instruction without natural ability can '
        'make the perfect artist. Let him be educated, skilful with the pencil, '
        'instructed in geometry, know much history, have followed the '
        'philosophers with attention, understand music, have some knowledge of '
        'medicine, know the opinions of the jurists, and be acquainted with '
        'astronomy and the theory of the heavens.'))

view(doc.pdf())
```

The available objects of the library are decribed at [API reference](https://github.com/xxyxyz/flat#reference).

It is also possible to generate the output with libraries other than Flat, as long as they can export PNG, JPEG or PDF format.

To view the result include `from flat import view` and call `view(<data>)`. Optionally, to sidestep Flat completely, one can write custom presenting function, too. For details refer to the source code of `even.py` of Flat.


#### Download

Binaries of version 0.2 for [MacOS](https://github.com/xxyxyz/even/releases/download/v0.2/even-0.2-mac.dmg), [Ubuntu 16.04](https://github.com/xxyxyz/even/releases/download/v0.2/even-0.2-linux.zip).

For Ubuntu, please install both Qt and PyPy: `sudo apt-get install qt5-default pypy`.


#### Source code

[github.com/xxyxyz/even](https://github.com/xxyxyz/even)

Even uses [PyPy](https://pypy.org/) JIT compiler, [Qt](https://www.qt.io/) framework and [MuPDF](https://www.mupdf.com/) library. Thanks!


#### Archive

The original content of [xxyxyz.org](https://web.archive.org/web/https://xxyxyz.org/) can be accessed via Wayback Machine.


#### Contact

Juraj Sukop, [contact@xxyxyz.org](mailto:contact@xxyxyz.org)

