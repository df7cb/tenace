tenace
======

Tenace is a Bridge hand viewer and editor.

* https://www.df7cb.de/bridge/tenace/

Screen Shots
------------

Main window

![Tenace main window](screenshots/board_cards.png)

Main window with textual cards

![Main window with textual cards](screenshots/board_text.png)

Board editor

![Board editor](screenshots/board_editor.png)

Requirements
------------

Tenace was written on Debian GNU/Linux, but should run on any platform
that features GTK+. Recommended minimum version is 2.12 due to the usage of
GtkTooltip. Tenace will still compile with older versions, but some features
will be disabled, notably the recently-used boards menu and tooltip popups for
alert explanations. To render images of cards, librsvg is used (other image
formats to display cards work as well). The GUI is rendered using libglade.

* http://www.gtk.org/
* http://library.gnome.org/devel/rsvg/
  http://ftp.gnome.org/pub/GNOME/sources/librsvg/

Tenace uses Bo Haglund's excellent DDS double dummy solver library for
discovering optimal leads and computing parscores.

* http://privat.bahnhof.se/wb758135/bridge/index.html

Installation
------------

Get and unpack the tarball.

    $ ./configure --help
    $ ./configure
    $ make
    # make install

If you are running Debian, get packages from the next Debian mirror.

The contrib sub-directory contains some useful supplementary files:

 * mailcap, mime.types: sample MIME configs to launch Tenace from web browsers
   and mail clients. On Debian systems, mailcap will automatically be installed.
   mime.types can be appended to your ~/.mime.types.
 * bonded.svg: SVG data to render cards. Most probably already installed in
   /usr/share/gnome-games-common/cards/ or the like by your Gnome installation.

Translations
------------

* German (de)
* Norwegian Nynorsk (nn) by Yngve Spjeld Landro

More translations welcome! (For technical reason, there's also an English
translation that only contains four strings, though real `en_US`/`en_GB`
translations would be welcome as well.)

Development
-----------

The Tenace source code is maintained on GitHub:

* https://github.com/df7cb/tenace

Feedback and Bugs
-----------------

I'd be happy to hear back from you, either by mail, or using the Debian bug
tracking system. I'm Myon on BBO, feel free to say hello there.

Credits
-------

* Mark Voorhies spotted a problem with greying out already-played cards.
* Christos Trochalakis suggested the "Open in web" feature.

I probably forgot to list others here. Please tell me when this is the case.

 -- Christoph Berg <cb@df7cb.de>   Thu, 10 Feb 2011 00:21:34 +0100
